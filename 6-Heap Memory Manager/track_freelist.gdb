# Define global variables
set $SYMBOL_OF_HEAP_NULL = -1
set $PREVIOUS_FREE_BLOCK_SHIFT = 1
set $NEXT_FREE_BLOCK_SHIFT = 2

# Define a custom GDB command to print the free list from the head
define PrintFreeListFromHead 
  set $current = Head
  printf "Free List From Head:\n"
  printf "----------------------------------------------------\n"
  printf "| Index | Block Size | Prev Free Block | Next Free Block |\n"
  printf "----------------------------------------------------\n"

  while ($current != $SYMBOL_OF_HEAP_NULL && $current != SimHeap[$current + $PREVIOUS_FREE_BLOCK_SHIFT ])
    set $size = SimHeap[$current]
    set $prev = SimHeap[$current + $PREVIOUS_FREE_BLOCK_SHIFT ]
    set $next = SimHeap[$current + $NEXT_FREE_BLOCK_SHIFT]

    printf "| %5d | %10d | %15d | %15d |\n", $current, $size, $prev, $next
    set $current = $next
  end

  printf "----------------------------------------------------\n"
end

# Define a custom GDB command to print the free list from the tail
define PrintFreeListFromTail
  set $current = Tail
  printf "Free List From Tail:\n"
  printf "----------------------------------------------------\n"
  printf "| Index | Block Size | Prev Free Block | Next Free Block |\n"
  printf "----------------------------------------------------\n"

  while ($current != $SYMBOL_OF_HEAP_NULL && $current != SimHeap[$current + $NEXT_FREE_BLOCK_SHIFT])
    set $size = SimHeap[$current]
    set $prev = SimHeap[$current + $PREVIOUS_FREE_BLOCK_SHIFT]
    set $next = SimHeap[$current + $NEXT_FREE_BLOCK_SHIFT]

    printf "| %5d | %10d | %15d | %15d |\n", $current, $size, $prev, $next
    set $current = $prev
  end

  printf "----------------------------------------------------\n"
end
