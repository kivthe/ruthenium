#include "allocators/linear_allocator.h"

#include <cstdlib>
#include <cstring>
#include <stdexcept>

namespace ruthen
{

namespace memory
{

LinearAllocator::LinearAllocator(std::size_t size_bytes) :
  size_{0},
  used_{0},
  data_{nullptr}
{
  void* pointer = nullptr;
  pointer = std::malloc(size_bytes);
  if(pointer == nullptr) throw std::bad_alloc{};
  data_ = pointer;
  size_ = size_bytes;
  used_ = 0;
}

LinearAllocator::~LinearAllocator() {
  if(data_ != nullptr) std::free(data_);
}

void* LinearAllocator::Allocate(std::size_t size_bytes) {
  if(used_ + size_bytes > size_) throw std::bad_alloc{};
  std::size_t used_before = used_;
  used_ += size_bytes;
  return data_ + used_before;
}

void LinearAllocator::Reset() {
  used_ = 0;
}

std::size_t LinearAllocator::GetSize() const {
  return used_;
}

bool LinearAllocator::IsEmpty() const {
  return used_ == 0;
}

}

}