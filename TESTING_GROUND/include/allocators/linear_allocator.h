#ifndef RUTHEN_LINEAR_ALLOCATOR_H
#define RUTHEN_LINEAR_ALLOCATOR_H

#include <cstddef>

namespace ruthen
{

namespace memory
{

class LinearAllocator
{
public:
  LinearAllocator(std::size_t size_bytes);
  LinearAllocator(const LinearAllocator& src) = delete;
  LinearAllocator(LinearAllocator&& src) noexcept = default;
  ~LinearAllocator();
  
  LinearAllocator& operator=(const LinearAllocator& rhs) = delete;
  LinearAllocator& operator=(LinearAllocator&& rhs) noexcept = default;

public:
  [[nodiscard]] void* Allocate(std::size_t size_bytes);
  void Reset();

public:
  std::size_t GetSize() const;
  bool IsEmpty() const;

private:
  std::size_t size_;
  std::size_t used_;
  void* data_;
};

}

}


#endif