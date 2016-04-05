# asIDE
An IDE for writing E100 Assembly Code

#### What in the World is E100?

The E100 is part of the "Microprocessors and Toys" section of "Engineering 100," the first-year engineering course at the Universty of Michigan. In this section, students learn the basics of computer engineering and implement a simple microprocessor (called the E100) on the [Altera DE2 FPGA Development Board](http://wl.altera.com/education/univ/materials/boards/de2/unv-de2-board.html).

The E100 has a simple Instruction Set Architecture. Using it, the students write E100 assembly code (E100 asm) programs to make their toys functional.

## About asIDE

asIDE is a cross-platform application for developing E100 assembly language programs. The IDE (Integrated Development Environment) is integrated with **ase100**, the assembler developed by Professor Peter Chen at U of M. This allows for a rapid, fluid workflow for E100 Assembly programmers.

![A nastyass screenshot](http://i.imgur.com/1VjdW2T.png)

## Features

### Cross-Platform

asIDE works on Windows, Mac OS X, and Linux!

### Tabbed Multiple File Editing

![Three files being edited at once](http://i.imgur.com/YIQrmgs.png)

### Beautiful Syntax Highlighting

![Bootiful](http://i.imgur.com/ptMDfCZ.png)

![Such highlight](http://i.imgur.com/NsgXNMV.png)

### Integration with ase100 (for Linux and Windows only)

![Green means go](http://i.imgur.com/0KtmgMf.png)

![You did it!](http://i.imgur.com/m3jVtjn.png)

![You're a terrible person](http://i.imgur.com/g1wRrRI.png)

### Introspection into Compiled Files

![You might have to squint](http://i.imgur.com/R3fclp4.png)

![But wow look at that](http://i.imgur.com/hOwc5Ja.png)

## Planned Features

* Code Introspection
  * Detection of Undefined/Duplicated Labels
  * Detection of Syntax Errors
  * File Not Found Errors
