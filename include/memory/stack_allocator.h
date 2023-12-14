#ifndef RUTHEN_STACK_ALLOCATOR
#define RUTHEN_STACK_ALLOCATOR

namespace ruthen
{

namespace memory
{

struct MemoryBlock
{
    unsigned long int id;
    unsigned long int size;
    const void* pointer;
};

class StackAllocator
{
public:
    typedef unsigned long int MemoryBlockID;
    typedef unsigned long int Size;

public:
    StackAllocator(Size size_bytes);
    ~StackAllocator();

public:
    MemoryBlock Allocate(Size size_bytes);
    void Deallocate(MemoryBlockID block_id);
    void Deallocate(MemoryBlock block);

private:
    Size stack_size_;
    Size block_count_;
    Size block_arr_cap_;
    MemoryBlock* blocks_;
    void *stack_;
    const void* top_;
};

}

}

#endif