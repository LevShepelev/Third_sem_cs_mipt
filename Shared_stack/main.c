#include "Shared_stack.h"

int main()
    {
    Stack_t* stack = attach_stack(10, 5);
    int* b_ptr;
    stack_dump(stack);
    pop(stack, (void*) &b_ptr);
    printf("b = %d", *b_ptr);
    sleep(4);
    detach_stack(stack);
    return 0;
    }