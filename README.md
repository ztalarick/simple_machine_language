# simple_machine_language
An basic interprter for Simple Machine Language written in C.

Simple Machine Language <br />
  push **num** - push **num** to stack <br />
  pop - pop top number from stack <br />
  add - pop top 2 numbers off the stack, add them, push the sum on the stack <br />
  ifeq **line number** - pop the top of stack if its eq 0 jump to **line number** <br />
  jump **line number** - jump to **line number** <br />
  print - print the stack <br />
  dup  - pushes a copy of whats on top of the stack <br />
  
 To Run:
 1. Write SML code in a file with <filename>
 2. ./interp filename
