// ZZT++
//
// Copyright (C) 1998-2001 Wouter E.P. Bovelander
//	wep@planetzztpp.com http://www.planetzztpp.com
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License as
// published by the Free Software Foundation; either version 2 of
// the License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

#ifndef _OE_H
#define _OE_H

// MS Visual C++
#ifdef _MSC_VER
#include <graph.h>
#endif

// MS Visual C++
#ifdef _MSC_VER
#include <memory.h>
#endif

// Borland C++
#ifdef __BORLANDC__
#include <mem.h>
#endif

#include <conio.h>		// getch()
#include <dos.h>			// ?
#include <stdio.h>		// printf()
#include <alloc.h>		// calloc()
#include "zconst.hpp"	// keyboard character values

// Screen items
#define	MAXLINELEN	42
#define	MAXLINES		15
#define	MAXTEXTLEN	0xFFFF
#define	EOL			0x0d 		// end of line

// Some graphics for the border
#define	LEFTSCROLL	198
#define	RIGHTSCROLL	181
#define	DBLHORBOT	207
#define	DBLHORTOP	209
#define	SNGLVERT		179
#define	DBLHOR		205

// Some absolute text positions of the border
#define	BRDR_TL_X	6		// top left
#define	BRDR_TL_Y	4
#define	BRDR_TR_X	54		// top right
#define	BRDR_TR_Y	4
#define	BRDR_ML_X	7		// middle left
#define	BRDR_ML_Y	6
#define	BRDR_MR_X   53		// middle right
#define	BRDR_MR_Y	6
#define	BRDR_BL_X	6		// bottom left
#define	BRDR_BL_Y	22
#define	BRDR_BR_X	54		// bottom right
#define	BRDR_BR_Y	22

// these are used to mark the left and right edges of the current line.
#define	LEFTMARKER	0xaf
#define	RIGHTMARKER	0xae

// these are the absolute positions of the ope text editor.
#define	LB_TEXT_X	10						// upper left x
#define	LB_TEXT_Y	7						// upper left y (links boven)
#define	RO_TEXT_X	(LB_TEXT_X + 42)	// right bottom x
#define	RO_TEXT_Y	(LB_TEXT_Y + 14)	// right bottom y (rechts onder)

// Macros to translate absolute x coordinate to relative x coordinate
// and vice versa. These are needed when comparing relative to absolute
// horizontal or vertical screen coordinates.
#define	HABS2REL(x)	(x-(LB_TEXT_X-1))
#define	HREL2ABS(x)	(x+(LB_TEXT_X-1))
#define	VABS2REL(y)	(y-(LB_TEXT_Y-1))
#define	VREL2ABS(y)	(y+(LB_TEXT_Y-1))

// declare the dotted line drawn in the editor here.
// there's no pretty way to do this economically.
const char DottedLine[]={32,32,32,32,7,32,32,32,32,7,32,32,32,32,7,32,32,
				32,32,7,32,32,32,32,7,32,32,32,32,7,32,32,32,32,7,32,32,32,32,7,
				32,32,32,32,7};
//
// This class defines the Object Program Editor which is used to edit
// text contained in the Object's oop program.
//
class ObjectProgramEditor
{
	private:
		// general editor information
		unsigned int textlen;	// current total length of our text
		unsigned int memlen;		// current number of bytes allocated
		int curlin;					// current line number;
		int totallines;			// total number of lines;
		int insert_mode; 			//	0=overwrite, otherwise insert

		// canvas cursor pointers
		char *p_used;	// pointer to EOT character in canvas
		char *p_endmem;// pointer to end of allocated memory.
		char *p_edpos;	// pointer to current editing position in canvas
		char *p_curpag;// pointer to first char of current page in canvas
		char *p_curlin;// pointer to first char of current line in canvas

		// screen oriented variables
		int	screenx;				// absolute(!) x position of screen cursor
		int	screeny;				// absolute(!) y position of screen cursor

		// our canvas is the space that contains the entire text
		// despite the size that is shown on screen. memory is
		// dynamically allocated.
		char *Canvas;

		void ObjectProgramEditor::DrawDottedLine(int line);
		void ObjectProgramEditor::DrawCanvas(char *Title);
		void ObjectProgramEditor::DrawCanvasBorder(char *Title);
		void ObjectProgramEditor::ScootRight(void);
		void ObjectProgramEditor::ScootLeft(void);
		void ObjectProgramEditor::ClearToEndOfLine(int fromx);
		void ObjectProgramEditor::PerformBackspace(void);

	public:
		void ObjectProgramEditor::EditObjectProgram(struct BoardObject *Object,
																	char *Title);

		ObjectProgramEditor(void)
		{
			// allocate the maximum possible canvas size.
			Canvas=(char *)calloc(MAXTEXTLEN, sizeof(char));
			if (Canvas==NULL)
			{
				fprintf(stderr, "zztpp: out of memory while creating ope./n");
				return;
			}

			// set memory length to the last byte allocated.
			memlen=MAXTEXTLEN-1;

			p_used=Canvas;				// set p_used to the end position
			p_endmem=Canvas+memlen; // set end of allocated memory pointer
			p_edpos=Canvas;			// set p_edpos to the start position
			p_curpag=Canvas;			// set current page to start position
			p_curlin=Canvas; 			// set current line to start position

			screenx=LB_TEXT_X;// set x to left side of edit window
			screeny=8;			// line 8 is the center line
			textlen=1;		  	// total length of our text, including EOL's
			curlin=1;		  	// set current line number to 1
			totallines=1;		// set total number of lines to 1
			insert_mode=1;		// set insert mode to ON
		}

		~ObjectProgramEditor(void)
		{
			free(Canvas);
		}
};

#endif

