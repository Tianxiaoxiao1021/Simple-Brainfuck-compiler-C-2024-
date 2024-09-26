
# BrainFuck Simple Compiler
## Synopsis
This is a simple compiler for the BrainFuck language, developed by tianxiaoxiao using C++.
**Note:** This is a basic compiler and may have limitations.
### BrainFuck
BrainFuck is a minimalist programming language (with a terrible name) created by Urban Müller in 1993 that has a pointer to an array of 30,000 elements and 8 commands to control the pointer and the elements it points to.
- >: move the pointer one place to the right
- <: shift left one pointer
- , : reads an integer from the input and stores it at the pointer's location.
- ‘ .’ : Prints the current pointer position
- ‘[’, ‘]’: loop (not an emoji) that executes the contents of the pointer when it points to a non-zero element
- '+' , '-' : add or subtract 1 to the element pointed to by the pointer, respectively
For this compiler, for convenience, I've added a command that makes it possible to manipulate two pointers: the ‘@’ command switches the pointer manipulated by the previous command, but of course, you can leave it out (just don't write it)
### Principle
The principle of this compiler is to convert your input BrainFuck source code to C++ source code, and then compile the C++ file using G++.
## Installation
No need to clone any repositories or install dependencies.
## Use
### Before Starting
Please **set the path to the G++ compiler**:
- For the Chinese edition, 点击 配置-G++编译器路径, 选择内置的BrainFuck compiler/bin/g++.exe路径或您PC上的任何g++编译器路径。
- For the English edition, click Configuration - G++ Compiler Path, and select the path to the built-in BrainFuck compiler/bin/g++.exe or any G++ compiler path on your PC.
### Edit an Existing BrainFuck Code
- If you have an existing BrainFuck code (.bf file):
  - For the Chinese edition, 点击 文件-导入, 然后选择您的BrainFuck代码路径, 其中的内容会加载到工作区，然后您可以点击文件-导出工作区内容为-.exe (Windows可执行文件) 和 .cpp (C++源文件)，选择您将要保存的路径（无文件后缀名）来编译您的BrainFuck代码到 .exe 文件。
  - For the English edition, click File - Import, then select the path of your BrainFuck code. The content will be loaded into the workspace. You can then click File - Export workspace content to -.exe (Windows executable) and .cpp (C++ source file), select the path you want to save (without file extension) to compile your BrainFuck code to a .exe file.
### Edit and Create New BrainFuck Code
- If you don't have any BrainFuck code, you can write your BrainFuck code directly in the workspace and click File - Export workspace content to - .bf (BrainFuck source file) to save your BrainFuck code to your PC.
## Functionality
- Check your BrainFuck code.
- Compile your BrainFuck code to .exe file.
- Compile C++ code.
## Reporting Issues
If you encounter any issues with the program, please report them to "Bf_bug_Feedback@126.com" with the following information:
- Your PC system
- Description of the problem or ideas for improvement