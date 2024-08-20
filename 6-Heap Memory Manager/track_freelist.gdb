# Set a breakpoint at the start of your function(s) that modify the free list
break Helper_FreeOperationMiddleNode
break Helper_FreeOperationBeforeHead
break Helper_FreeOperationAfterTail

# Command to run when hitting the breakpoint
commands
    # Print the current state of the free list before any modifications
    echo \n--- Free List State Before Modification ---\n
    set $i = Head
    while ($i != SYMBOL_OF_HEAP_NULL)
        printf "Block at index %d: size = %d, next = %d, prev = %d\n", $i, SimHeap[$i], SimHeap[$i + NEXT_FREE_BLOCK_SHIFT], SimHeap[$i + PREVIOUS_FREE_BLOCK_SHIFT]
        set $i = SimHeap[$i + NEXT_FREE_BLOCK_SHIFT]
    end
    echo \n--- End of Free List ---\n

    # Continue to the next line of code after the breakpoint
    continue
end

# Watch the SimHeap array's critical elements (e.g., head, tail, next, previous pointers)
watch SimHeap[Head]
watch SimHeap[Tail]
watch SimHeap[Head + NEXT_FREE_BLOCK_SHIFT]
watch SimHeap[Tail + PREVIOUS_FREE_BLOCK_SHIFT]

# Command to run when a watchpoint is hit
commands
    echo \n--- Watchpoint Hit: Free List Modified ---\n
    # Print the current state of the free list after the modification
    set $i = Head
    while ($i != SYMBOL_OF_HEAP_NULL)
        printf "Block at index %d: size = %d, next = %d, prev = %d\n", $i, SimHeap[$i], SimHeap[$i + NEXT_FREE_BLOCK_SHIFT], SimHeap[$i + PREVIOUS_FREE_BLOCK_SHIFT]
        set $i = SimHeap[$i + NEXT_FREE_BLOCK_SHIFT]
    end
    echo \n--- End of Free List ---\n

    # Continue execution
    continue
end

