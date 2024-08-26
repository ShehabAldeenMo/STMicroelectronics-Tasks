# Define a custom GDB command to print the free list from the head
define PrintFreeListFromHead 
  set $current = ptrHead
  printf "Free List From Head:\n"
  printf "------------------------------------------------------------\n"
  printf "| Address        | Block Size | Prev Free Block | Next Free Block |\n"
  printf "------------------------------------------------------------\n"

  while ($current != 0)
    set $size = $current->BlockSize
    set $prev = $current->PreviousFreeBlock
    set $next = $current->NextFreeBlock

    printf "| %p | %10d | %15p | %15p |\n", $current, $size, $prev, $next
    set $current = $next
  end

  printf "------------------------------------------------------------\n"
end

# Define a custom GDB command to print the free list from the tail
define PrintFreeListFromTail
  set $current = ptrTail
  printf "Free List From Tail:\n"
  printf "------------------------------------------------------------\n"
  printf "| Address        | Block Size | Prev Free Block | Next Free Block |\n"
  printf "------------------------------------------------------------\n"

  while ($current != 0)
    set $size = $current->BlockSize
    set $prev = $current->PreviousFreeBlock
    set $next = $current->NextFreeBlock

    printf "| %p | %10d | %15p | %15p |\n", $current, $size, $prev, $next
    set $current = $prev
  end

  printf "------------------------------------------------------------\n"
end

