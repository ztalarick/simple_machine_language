# simple_machine_language
An basic interprter for Simple Machine Language written in C.

Simple Machine Language
  push <num> - push <num> to stack
  pop - pop top number from stack
  add - pop top 2 numbers off the stack, add them, push the sum on the stack
  ifeq <line number> - pop the top of stack if its eq 0 jump to <line number>
  jump <line number> - jump to <line number>
  print - print the stack
  dup  - pushes a copy of whats on top of the stack
  
 To Run:
 1. Write SML code in a file with <filename>
 2. ./interp filename
