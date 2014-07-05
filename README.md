					=====
					ZZT++
					=====

	By Wouter Bovelander
	Copyright 1998-2014
	published under GNU General Public License
	wouter@thehilander.nl
	http://www.thehilander.nl

	July 5 2014

Notes
=====

You can read the changes made to this version in the "changelo.txt" file, which is part of the source code as well.

Only the title screen is shown since this is only for demonstration purposes. 
Thanks to JacobH, creator of ZIG for supplying the necessary file format information.

Where can I get zzt++?
======================

The latest executables, sources and documentation should be available from github or thehilander.nl

Installing ZZT++
================

Simply unzip zzt.zip in any directory and place a .zzt file in the same directory. Run zzt++ using the following command line:

	zzt [-options] [file[.zzt]]

The following options are available:
	r=report on the contents to a file called REPORT.TXT, overwriting the previous file.
	s=report including screen information. WARNING: report files with screen information can generate very large files!
	w=write the output to OUTWORLD.ZZT

These options can be used in any order, so -rws is equal to -wrs.

Supplying a file name is optional. If no filename is given, zzt++ will open the file TOWN.ZZT. You may omit the .zzt extension. zzt++ will add it for you if no extension was given.

Running zzt++ as file format analyser
=====================================

Use the following command to analyse the file CWARS10.ZZT:

	zzt -rs CWARS10

You can open REPORT.TXT using an editor like wordpad, notepad+ or vi to read zzt++'s output. Since report files can become very large, notepad will often be unable to open your report file. Remember that each time you create a REPORT.TXT file, the previous version (if any) is overwritten automatically by zzt++.

If you want to leave out the screen information use:

	zzt -r CWARS10

Compiling ZZT++
===============

It has never been my intention to give support for compiling ZZT++. Not on Borland, nor on other compilers. However, I have added support for compiling ZZT++ under MS Visual 1.52. In adapting the source code from Borland to MSVC I found that most of the effort goes into setting up the compiler environment, instead of adapting te source code. So, if you want to compile ZZT++ with your compiler, make sure the source is adapted to your specific compiler first (e.g.: DJGPP does not support the 'far' keyword and uses a Dos Protected Mode Interface for just about everything). Next, make sure your compiler is set up like this:

  COMPILER SETTINGS
  -make sure you are using a 16-bit compiler.
  -create an MS-DOS executable.
  -use the 8086/8088 instruction set.
  -set the compiler to generate a LARGE or HUGE memory model.
  -tell it to use single byte alignment, or no alignment at all.
  -tell the compiler that the stack segment (SS) should never equal the data segment (DS).
  -do not enable run-time pointer checking. This causes bogus errors on some compilers.
  -set the far data threshold to 32767.
  -tell the linker to use a 8192 byte stack size.
  -tell the linker to pack the code segment using 8192 as pack size.

  CODE CHANGES MADE TO COMPLY WITH MSVC
  -change the call to MK_FP to _MK_FP for making far pointers.
  -also add underscores to calls to int86(), dosgetvect(), dossetvect(), enable() and disable().
  -change the REGS union to _REGS.
  -add newlines to the end of each header file and change their filenames to .hpp instead of .h

  KNOWN ISSUES
  -on MSVC the ISR routine causes a stack overflow if a function is called within it.
  -conio.h is incomplete for MSVC causing the need for extra clrscr() and gotoxy() functions, unless GRAPH.H is used.
  -every call to a GRAPH.H function causes ZZT++ to switch automatically to full screen mode.
  -the first two times that BM.TypeString() is called, the strings are not completed correctly displayed. The function completes without error.

Compiling zzt++ on a PII300 with 96MB will take only a few seconds and generate an executable with debugging information smaller than 200Kb on a DOS/Windows platform. If debugging information is left out, the executable should drop dramatically in size.

I will probably be adding some support for other compilers in the future, but please do not ask me to create a version for your specific compiler. Converting a source and setting up a compiler is painstaking work and would take too much time out of the development of zzt++. If you want to be able to compile zzt++ on your compiler, please give it a try yourself. You are free to do so as long as you heed the GNU license agreement. I am always available for help of course and will gladly accept any conversion you may have accomplished.

Known Issues
============

- The thing is not complete yet. Duh.


