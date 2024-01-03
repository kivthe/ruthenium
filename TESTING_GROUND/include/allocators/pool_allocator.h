#ifndef RUTHEN_POOL_ALLOCATOR_H
#define RUTHEN_POOL_ALLOCATOR_H

#include <cstddef>
#include <stdexcept>
#include <cstdlib>
#include <climits>
#include <cstring>

#include <bitset>
#include <iostream>

namespace ruthen
{

namespace memory
{

template<std::size_t kBlockSize>
class PoolAllocator
{
public:
  typedef int Bitfield;
  constexpr static std::size_t kBitfieldBits = sizeof(Bitfield) * CHAR_BIT;

public:
  PoolAllocator(std::size_t block_count);
  PoolAllocator(const PoolAllocator& src) = delete;
  PoolAllocator(PoolAllocator&& src) noexcept = default;
  PoolAllocator& operator=(const PoolAllocator& src) = delete;
  PoolAllocator& operator=(PoolAllocator&& rhs) noexcept = default;
  ~PoolAllocator();

public:
  [[nodiscard]] void* Allocate(std::size_t size_bytes);
  inline void Free(void* block, std::size_t size_bytes);
  inline void Reset();
  inline void PrintBitfields() const {
    std::cout << bit_fileds_ << '\n';
    for(std::size_t i = 0; i < bit_fileds_; ++i) {
      std::cout << std::bitset<kBitfieldBits>(bitfield_[i]) << '\n';
    }
  }

private:
  inline void SetBlockFree(std::size_t index);
  inline void SetBlockAllocated(std::size_t index);

public:
  inline std::size_t BlockCount() const;
  inline bool IsEmpty() const;
  inline bool IsBlockFree(std::size_t index) const;
  inline bool IsBlockAllocated(std::size_t index) const;

private:
  inline bool IsPointerValid(void* pointer) const;

private:
  std::size_t bit_fileds_;
  std::size_t block_count_;
  std::size_t size_;
  void* data_;
  Bitfield* bitfield_;
};

template<std::size_t kBlockSize>
PoolAllocator<kBlockSize>::PoolAllocator(std::size_t block_count) :
  bit_fileds_{0},
  block_count_{0},
  size_{0},
  data_{nullptr},
  bitfield_{nullptr}
{
  bit_fileds_ = block_count / kBitfieldBits + (((block_count / kBitfieldBits) * kBitfieldBits) < block_count ? 1 : 0);
  block_count_ = block_count;
  size_ = block_count * kBlockSize;
  std::size_t alloc_size = block_count * kBlockSize + bit_fileds_ * sizeof(Bitfield);
  void* pointer = nullptr;
  pointer = std::malloc(alloc_size);
  if (pointer == nullptr) throw std::bad_alloc{};
  std::memset(pointer, 0, alloc_size);
  data_ = pointer;
  bitfield_ = reinterpret_cast<Bitfield*>(reinterpret_cast<unsigned char*>(data_) + size_);
}

template<std::size_t kBlockSize>
PoolAllocator<kBlockSize>::~PoolAllocator() {
  if(data_ != nullptr) std::free(data_);
}

template<std::size_t kBlockSize>
void* PoolAllocator<kBlockSize>::Allocate(std::size_t size_bytes) {
  std::size_t blocks_needed = size_bytes / kBlockSize + (((size_bytes / kBlockSize) * kBlockSize) < size_bytes ? 1 : 0);
  std::size_t free_index = -1;
  for(std::size_t i = 0; i < bit_fileds_ * kBitfieldBits; ++i) {
    if (IsBlockAllocated(i)) continue;
    if (blocks_needed == 1) {
      free_index = i;
      break;
    }
    bool match = true;
    for(std::size_t j = i + 1; j < bit_fileds_ * kBitfieldBits; ++j) {
      if (IsBlockAllocated(j)) {
        i = j;
        match = false;
        break;
      }
    }
    if(!match) continue;
    free_index = i;
    break;
  }
  if(free_index == static_cast<std::size_t>(-1)) return nullptr;
  for(std::size_t i = free_index, j = 0; j < blocks_needed; ++i, ++j) {
    SetBlockAllocated(i);
  }
  return reinterpret_cast<unsigned char*>(data_) + free_index * kBlockSize;
}

template<std::size_t kBlockSize>
void PoolAllocator<kBlockSize>::Free(void* pointer, std::size_t size_bytes) {
  if (!IsPointerValid(pointer)) return;
  std::size_t blocks = size_bytes / kBlockSize + (((size_bytes / kBlockSize) * kBlockSize) < size_bytes ? 1 : 0);
  std::size_t diff = reinterpret_cast<std::size_t>(pointer) - reinterpret_cast<std::size_t>(data_);
  std::size_t index = diff / kBlockSize;
  for (std::size_t i = 0; i <= blocks && index + i < block_count_; ++i) {
    SetBlockFree(index + i);
  }
}

template<std::size_t kBlockSize>
void PoolAllocator<kBlockSize>::Reset() {
  for (std::size_t i = 0; i < bit_fileds_; ++i) {
    bitfield_[i] = Bitfield{};
  }
}

template<std::size_t kBlockSize>
void PoolAllocator<kBlockSize>::SetBlockAllocated(std::size_t index) {
  if(index >= block_count_) return;
  bitfield_[index / kBitfieldBits] |= Bitfield{1} << (kBitfieldBits - (index % kBitfieldBits) - 1);
}

template<std::size_t kBlockSize>
void PoolAllocator<kBlockSize>::SetBlockFree(std::size_t index) {
  if(index >= block_count_) return;
  bitfield_[index / kBitfieldBits] &= ~(Bitfield{1} << (kBitfieldBits - (index % kBitfieldBits) - 1));
}

template<std::size_t kBlockSize>
std::size_t PoolAllocator<kBlockSize>::BlockCount() const {
  return block_count_;
}

template<std::size_t kBlockSize>
bool PoolAllocator<kBlockSize>::IsEmpty() const {
  for (std::size_t i = 0; i < bit_fileds_; ++i) {
    if (bitfield_[i] != Bitfield{}) return false;
  }
  return true;
}

template<std::size_t kBlockSize>
bool PoolAllocator<kBlockSize>::IsPointerValid(void* pointer) const {
  std::size_t start = reinterpret_cast<std::size_t>(data_);
  std::size_t param_pointer = reinterpret_cast<std::size_t>(pointer);
  if(param_pointer < start || param_pointer > start + size_) return false;
  return (param_pointer - start) % kBlockSize == 0;
}

template<std::size_t kBlockSize>
bool PoolAllocator<kBlockSize>::IsBlockFree(std::size_t index) const {
  return !IsBlockAllocated(index);
}

template<std::size_t kBlockSize>
bool PoolAllocator<kBlockSize>::IsBlockAllocated(std::size_t index) const {
  if(index >= block_count_) return false;
  return bitfield_[index / kBitfieldBits] & Bitfield{1} << (kBitfieldBits - index % kBitfieldBits - 1);
}

}

}

#endif