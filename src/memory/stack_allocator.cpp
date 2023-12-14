#include <cstdlib>
#include <cstring>
#include <stdexcept>

#include "memory/stack_allocator.h"

namespace ruthen
{

namespace memory
{

StackAllocator::StackAllocator(Size size_bytes) :
    stack_size_{0},
    block_count_{0},
    block_arr_cap_{0},
    blocks_{nullptr},
    stack_{nullptr},
    top_{nullptr}
{
    void* pointer = malloc(size_bytes);
    if (pointer == nullptr) throw std::bad_alloc{};
    stack_size_ = size_bytes;
    stack_ = pointer;
    top_ = pointer;
}

StackAllocator::~StackAllocator()
{
    if(stack_ != nullptr) free(stack_);
    if(blocks_ != nullptr) free(blocks_);
}

MemoryBlock StackAllocator::Allocate(Size size_bytes)
{
    
}

void StackAllocator::Deallocate(MemoryBlock block)
{

}

}

}

