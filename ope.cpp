// ZZT++
//
// Copyright (C) 1998-2014 Wouter E.P. Bovelander
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

#pragma warn -use
#pragma warn -par	// parameter not used in function (EditObectProgram *Object)

#include <string.h>		// strlen()
#include "zconst.hpp"
#include "ope.hpp"
#include "bm.hpp"

// declared global in zzt.cpp and used here
extern BoardManager BM;

// ============================================================================
// Function: ClearToEndOfLine
//		Fills screen (only!) with 0x00 from given x position to MAXLINELEN.
//		Uses but does not alter screeny from Object.
// Parameters:
//		none
// Returns:
//		nothing.
// ============================================================================
void ObjectProgramEditor::ClearToEndOfLine(int fromx)
{
	// start at given x position, which can be anywhere on the current line.
	// we are talking about absolute positions here.
	// the end of the line is expressed in terms of MAXLINELEN, which is a
	// relative value. We add 4 to the length of the line in order to
	// clear beyond the actual text area. 
	// just draw directly to the screen and write 0x00's in the correct color.
	for (fromx; fromx<=(VREL2ABS(MAXLINELEN+4)); fromx++)
		BM.DrawChar_Direct(fromx, screeny, 0x00, m_color(R_BLUE, R_GREY, 0, 0));
}

// ============================================================================
// Function: DrawDottedLine
//		Draw a dotted line.
//		Does not alter Canvas. Only the screen.
// Parameters:
//		int line, current line number
// Returns:
//		nothing.
// ============================================================================
void ObjectProgramEditor::DrawDottedLine(int line)
{
	int i, x;

	// set x to left-most x position
	x=LB_TEXT_X-2;

	// loop through DottedLine and draw the character
	for (i=0; i<45; i++)
		BM.DrawChar_Direct(x++, line, DottedLine[i],
						m_color(R_BLUE, R_BROWN, 0, 1));
}

// ============================================================================
// Function: PerformBackspace
//		Deletes current character and scoots everything left one position.
//		This is called in case of backspace and delete line.
//		Alters Canvas not the screen.
// Parameters:
//		none.
// Returns:
//		nothing.
// ============================================================================
void ObjectProgramEditor::PerformBackspace(void)
{
	ScootLeft();	// scoot characters left
	screenx--;		// retreat screen x position
	p_edpos--;		// retreat edit position
	p_used--;		// we added a space at the end, but it doesn't count.
	textlen--;		// decrease text length

	return;
}


// ============================================================================
// Function: DrawCanvasBorder
//		Draw a fancy border around the canvas on screen.
//		This border is painted at a fixed position.
// Parameters:
//		none.
// Returns:
//		nothing.
// Note:
//		In real zzt the border animates into life. On faster computers, the
//		animation is hardly seen. Our border does not animate.
// ============================================================================
void ObjectProgramEditor::DrawCanvasBorder(char *Title)
{
	int x,y;

	// initialise temporary coordinates
	x=y=0;

	// First, draw all the edges and intersections

	// the top horizontal bar edges
	BM.DrawChar_Direct(BRDR_TL_X, BRDR_TL_Y, LEFTSCROLL,
			m_color(R_BLACK, R_GREY,0,1));

	BM.DrawChar_Direct(BRDR_TL_X+1, BRDR_TL_Y, DBLHORTOP,
			m_color(R_BLACK, R_GREY,0,1));

	BM.DrawChar_Direct(BRDR_TR_X, BRDR_TR_Y, RIGHTSCROLL,
			m_color(R_BLACK, R_GREY,0,1));

	BM.DrawChar_Direct(BRDR_TR_X-1, BRDR_TR_Y, DBLHORTOP,
			m_color(R_BLACK, R_GREY,0,1));

	// top horizontal bar
	for (x=BRDR_TL_X+2; x<BRDR_TR_X-1; x++)
		BM.DrawChar_Direct(x, BRDR_TL_Y, DBLHOR, m_color(R_BLACK, R_GREY,0,1));

	// the two vertical lines beside the title
	BM.DrawChar_Direct(BRDR_TL_X+1, BRDR_TR_Y+1, SNGLVERT,
			m_color(R_BLACK, R_GREY,0,1));

	BM.DrawChar_Direct(BRDR_TR_X-1, BRDR_TR_Y+1, SNGLVERT,
			m_color(R_BLACK, R_GREY,0,1));

	// the two middle scroll ends
	BM.DrawChar_Direct(BRDR_TL_X+1, BRDR_TR_Y+2, LEFTSCROLL,
			m_color(R_BLACK, R_GREY,0,1));

	BM.DrawChar_Direct(BRDR_TR_X-1, BRDR_TR_Y+2, RIGHTSCROLL,
			m_color(R_BLACK, R_GREY,0,1));

	// middle horizontal line
	for (x=BRDR_TL_X+2; x<BRDR_TR_X-1; x++)
		BM.DrawChar_Direct(x, BRDR_TL_Y+2, DBLHOR, m_color(R_BLACK, R_GREY,0,1));

	// both vertical lines
	for (y=BRDR_TL_Y+3; y<BRDR_BL_Y; y++)
		BM.DrawChar_Direct(BRDR_TL_X+1, y, SNGLVERT,
				m_color(R_BLACK, R_GREY,0,1));

	for (y=BRDR_TL_Y+3; y<BRDR_BL_Y; y++)
		BM.DrawChar_Direct(BRDR_TR_X-1, y, SNGLVERT,
				m_color(R_BLACK, R_GREY,0,1));

	// the bottom horizontal bar edges
	BM.DrawChar_Direct(BRDR_BL_X, BRDR_BL_Y, LEFTSCROLL,
			m_color(R_BLACK, R_GREY,0,1));

	BM.DrawChar_Direct(BRDR_BL_X+1, BRDR_BL_Y, DBLHORBOT,
			m_color(R_BLACK, R_GREY,0,1));

	BM.DrawChar_Direct(BRDR_BR_X, BRDR_BR_Y, RIGHTSCROLL,
			m_color(R_BLACK, R_GREY,0,1));

	BM.DrawChar_Direct(BRDR_BR_X-1, BRDR_BR_Y, DBLHORBOT,
			m_color(R_BLACK, R_GREY,0,1));

	// bottom horizontal bar
	for (x=BRDR_BL_X+2; x<BRDR_BR_X-1; x++)
		BM.DrawChar_Direct(x, BRDR_BL_Y, DBLHOR, m_color(R_BLACK, R_GREY,0,1));

	// title
	for (x=BRDR_TL_X+2; x<BRDR_TR_X-1; x++)
		BM.DrawChar_Direct(x, BRDR_TL_Y+1, SPACE, m_color(R_BLUE, R_BROWN,0,1));

	// calculate starting position of title string
	// there are 45 positions in the title bar. 45/2=22 and a half
	x=HREL2ABS( (45/2) - (strlen(Title)/2) );
	BM.TypeString(x, BRDR_TL_Y+1, Title, m_color(R_BLUE, R_BROWN,0,1));

	return;
}


// ============================================================================
// Function: DrawCanvas
//		Draw the contents of the canvas on the screen and set screen cursor.
//		Uses then resets screeny from Object to fixed edit line.
// Parameters:
//		none.
// Returns:
//		nothing.
// ============================================================================
void ObjectProgramEditor::DrawCanvas(char *Title)
{
	int x,y;		// absolute coordinates for drawing
	int count;	// general counter variable.
	char *walk;	// dummy pointer var

	walk=p_edpos;		// set walk to current editing position
	count=0;				// set number of eol counter to zero

	// first, draw our fancy border.
	DrawCanvasBorder(Title);

	// try to walk back 8 lines and stop if we hit the beginning of canvas.
	// 8 lines fit between the start of screen area and the fixed edit line.
	while (walk != Canvas && count<8)
	{
		walk--;				// decrement walk
		if (*walk==EOL)	// count eacht eol encountered
			count++;
	}

	// set p_curpag to the correct value. either Canvas or the first
	// character of the next line. remember that we ended up on the eol
	// of the line previous to the one we wanted.
	if (walk==Canvas)
		p_curpag=Canvas;
	else
		p_curpag=walk+1;

	// set x coordinate to upper leftmost x
	// set y coordinate to upper leftmost y
	// set the screen y to the first drawing position.
	x=LB_TEXT_X;
	y=LB_TEXT_Y;
	screeny=y;

	// if the current line is less than 8, draw some blank lines first.
	// we may have had less than 8 lines in the program.
	if (curlin<8)
	{
		// we need to draw 8 - curlin lines.
		count=8-curlin;

		// we --count because we want to leave one line for the dotted line
		while (--count)
		{
			ClearToEndOfLine(LB_TEXT_X-2);	// draw a blank line
			y++;									// next line
			screeny++;
		}

		// now, draw the dotted line at the top and increase the line number
		// don't draw the dotted line if we're on line 8 or higher.
		DrawDottedLine(y++);
	}

	// set walk to current page in the text
	walk=p_curpag;

	// keep going until we are at the end of the text or we have
	// reached the end of the screen.
	count=0;
	while (walk <= p_used && y<=RO_TEXT_Y)
	{
		// at the end of line, position screen cursor on the next line.
		if (*walk==EOL)
		{
			// clear to end of line
			while (count<=MAXLINELEN)
			{
				// Draw a space
				BM.DrawChar_Direct(x++, y ,0 ,m_color(R_BLUE, R_GREY,0,0));
				// increase count
				count++;
			}
			x=LB_TEXT_X;		// reset x to start of line
			y++;					// set y to next line
			walk++;

			// reset line character counter
			count=0;
		}
		else
		{
			// Count another character and draw it.
			count++;
			BM.DrawChar_Direct(x++, y, *walk++, m_color(R_BLUE, R_BROWN,0,1));
		}
	}

	// if we're not at the end of the text box yet, start drawing
	// a dotted line and fill the rest with blank lines.
	if (y<RO_TEXT_Y)
	{
		DrawDottedLine(y++);
		screeny=y;

		//	Make sure we fill the rest with blank lines if necessary
		while (y<=RO_TEXT_Y)
		{
			ClearToEndOfLine(LB_TEXT_X-2);
			y++;
			screeny=y;
		}
	}

	// set the cursor on the correct line
	// we don't gotoxy() here. it's done elsewhere.
	screeny=VREL2ABS(8);

	// draw the markers.
	BM.DrawChar_Direct(LB_TEXT_X-2, screeny, LEFTMARKER,
				m_color(R_BLUE, R_RED, 0, 1));

	BM.DrawChar_Direct(LB_TEXT_X-1, screeny, 0x00,
				m_color(R_BLUE, R_RED, 0, 1));

	BM.DrawChar_Direct(RO_TEXT_X, screeny, RIGHTMARKER,
				m_color(R_BLUE, R_RED, 0, 1));

}

// ============================================================================
// Function: ScootRight
//		Move all characters one position to the right,
//		starting from the current editing position. Adjusts screen.
// Parameters:
//		none.
// Returns:
//		nothing.
// ============================================================================
void ObjectProgramEditor::ScootRight(void)
{
	char *walk;
	int x=screenx;		// put x on the current absolute screen position

	// remember only to insert if we're not already at the end of the line.
	// also, don't insert if we have reached the maximum number of characters.
	// this test should not be necessary..just a sanity check.
	if (x<=RO_TEXT_X && textlen<MAXTEXTLEN)
	{
		// start from the last position. we can do so because we know there's
		// still room in Canvas (we've checked textlen)
		walk=p_used;

		// walk back to our current editing position
		while (walk >= p_edpos)
		{
			*(walk+1)=*walk;	// copy the current char to the next position,
			walk--;				// and walk back one char.
		}

		// now, we want to redraw the current line.
		// we could call DrawCanvas() but this is faster.
		// start at the current editing position
		walk=p_edpos;

		// walk to end of line
		while (*walk != EOL)
		{
			 walk++;	// next character in Canvas
			 x++;		// next position on screen.
		}

		// now, walk back while moving characters
		// we use the screen cursor here because it's easy.
		while (walk >= p_edpos)
		{
			// don't actually draw EOL on the screen please.
			if (*walk != EOL)
				BM.DrawChar_Direct(x, screeny, *walk,
									m_color(R_BLUE, R_BROWN, 0, 1));
			walk--;
			x--;
		}
	}
}

// ============================================================================
// Function: ScootLeft
//		Move all characters one position to the left,
//		starting from the current editing position to the end of text.
//		Character to the left of editing position is deleted and lost.
//		p_used is not decremented here. Adjusts screen.
// Parameters:
//		none.
// Returns:
//		nothing.
// ============================================================================
void ObjectProgramEditor::ScootLeft(void)
{
	char *walk;
	int x;			// interpreted as an absolute x coordinate

	walk=p_edpos;	// set walk to current editing position
	x=screenx;		// set x to current x position

	// while walking to the eol character, move character left.
	// since we have already checked that we're not already at the
	// first character on the line, we don't do it here.
	while (walk <= p_used)
	{
		*(walk-1)=*walk;	// previous character becomes current
		walk++;
	}
	// clear last position
	*(walk-1)=SPACE;

	walk=p_edpos-1;
	x--;
	while (*walk != EOL)
	{
		BM.DrawChar_Direct(x, screeny, *walk,
						m_color(R_BLUE, R_BROWN, 0, 1));

		walk++;				// next character
		x++;					// next screen position
	}

	// don't forget to clear the last character.
	BM.DrawChar_Direct(x, screeny, SPACE, m_color(R_BLUE, R_BROWN, 0, 1));
}

// ============================================================================
// Function: EditObjectProgram
//		Edit the given object's program.
// Parameters:
//		struct BoardObject *Object, the object to be edited
// Returns:
//		nothing.
// ============================================================================

void ObjectProgramEditor::EditObjectProgram(struct BoardObject *Object,
															char *Title)
{
	char c;				// we'll put the character we read in this variable
	char special;  	// was a special character read?
	char *walk;			// character pointer for general use
	int count;			// integer for general use
	int old_screenx;	// a temporary store for the delete line action.

	// init
	Canvas[0]=EOL;		// insert an EOL character first.

	// enable cursor
	BM.EnableCursorBig();

	// draw text area on the screen.
	DrawCanvas(Title);

	c=0;										// initialise c to 0
	special=0;								//	set special char to not special
	gotoxy(screenx, screeny);			// place the cursor.

	// start our main editor loop.
	// continue untill escape is pressed
	while (c!=ESC)
	{
		// get a character from the keyboard
		c=getch();

		// watch for special characters. they result in an extra leading 0.
		if (c==0)
		{
			special=1;
			c=getch();
		}

		// interpret the character we just read
		// first, handle special characters. we do this separately from normal
		// characters because LEFT can be confused with 'K'. inside the game
		// loop, this doesn't matter, because we're not typing text. inside the
		// editor we are so we need to make a difference between specials and
		// normals.
		if (special)
		{
			// reset special to not special
			special=0;

			switch (c)
			{
				// =========================================================
				case UP:
				{
					// make sure there is a line to move up to.
					if (curlin>1)
					{
						walk=p_edpos;		// set walk to current editing position
						count=0;				// count number of eols encountered

						// now, keep walking back until we have gone back a full line:
						// that's two eols(!) or until we hit Canvas
						while (walk>Canvas && count<2)
						{
							walk--;				// walk back one character

							if (*walk==EOL)	// count each eol
								count++;
						}

						// if we hit canvas we're there, otherwise advance to the
						// first character of the next line.
						// remember that both could be true as well: we're at
						// Canvas AND we have encountered 2 eols.
						if (walk!=Canvas || count==2)
							walk++;

						// move the cursor forward as far as we were on the previous
						// line, or to the end of the current line, whichever comes
						// first.
						count=1;

						while (count < HABS2REL(screenx) && *walk!=EOL)
						{
							count++;
							walk++;
						}

						p_edpos=walk;					// set new editing position
						screenx=HREL2ABS(count); 	// set new screen position
						curlin--;						// set current line number

						DrawCanvas(Title);		// redraw the canvas
					}
					break;
				}

				// =========================================================
				case DOWN:
				{
					// only go down if we are not at the last line.
					if (curlin<totallines)
					{
						walk=p_edpos;		// set walk to current editing position

						while (*walk!=EOL)	// walk to end of current line
							walk++;

						// only act if there's actually a line to move to.
						if (walk!=p_used)
						{
							walk++;		// go to the start of the next line

							count=1;		// walk over the new line until we're at the
											// previous x position or an eol.
							while (count < HABS2REL(screenx) && *walk!=EOL)
							{
								count++;
								walk++;
							}

							screenx=HREL2ABS(count);// now set new screen position
							p_edpos=walk;				// set editing position
							curlin++;					// set new current line

							// redraw the canvas
							DrawCanvas(Title);
						}
					}
					break;
				}

				// =========================================================
				case LEFT:
				{
					// going left is not as easy as it seems.
					// moving the cursor to the left should also mean moving
					// to the previous line at the first character of the current
					// line. when on the first line, the cursor should move to the
					// eol of that line when the first character is reached.

					// are we not already at the beginning of the line?
					if (screenx>LB_TEXT_X)
					{
						screenx--;
						p_edpos--;
					}
					else
					{
						walk=p_edpos;		// set walk to current editing position
						count=0;				// count number of eols encountered

						// now, keep walking back until we have gone back a full line:
						// that's two eols(!) or until we hit Canvas
						while (walk>Canvas && count<2)
						{
							walk--;				// walk back one character

							if (*walk==EOL)	// count each eol
								count++;
						}

						// if we hit canvas we're there, otherwise advance to the
						// first character of the next line.
						// remember that both could be true as well: we're at
						// Canvas AND we have encountered 2 eols.
						if (walk!=Canvas || count==2)
							walk++;

						// move the cursor forward to the eol
						count=1;
						while (*walk != EOL)
						{
							count++;
							walk++;
						}

						p_edpos=walk;					// set new editing position
						screenx=HREL2ABS(count); 	// set new screen position

						// we stay on the first line.
						if (curlin>1)
						{
							curlin--;	 	// set current line number
							DrawCanvas(Title);	// redraw the canvas
						}
					}
					break;
				}

				// =========================================================
				case RIGHT:
				{
					// going right is not as easy as it seems.
					// moving the cursor to the right should also mean moving
					// to the next line at the eol of the current line. when on
					// the last line, the cursor should move to the first character
					// of that line when the eol is reached.

					// if we're somewhere in text, just move right
					if (*p_edpos != EOL)
					{
						screenx++;
						p_edpos++;
					}
					else
						// at an EOL, but not at the end of the text.
						// move to next line.
						if (p_edpos<p_used)
						{
							curlin++;				// move to the next line
							screenx=LB_TEXT_X;   // set the cursor to the beginning
							p_edpos++;				// new editing position
							DrawCanvas(Title); 	// redraw the canvas
						}
						else
						{
							// move the screen cursor to the start of the line.
							screenx=LB_TEXT_X;

							// if we are at Canvas, then do nothing because
							// we are also at p_used (end of text).
							// otherwise, move to the start of the current line,
							// which must be the last as well.
							if (p_edpos>Canvas)
							{
								// if the last line happens to be empty, do nothing
								if (*(p_edpos-1) != EOL)
								{
									// now, move to the previous EOL or to Canvas
									// whichever comes first.
									p_edpos--;

									// walk back
									while (*p_edpos != EOL && p_edpos > Canvas)
										p_edpos--;

									// we're at the end of a previous line,
									// move to the firs character of the last line.
									if (*p_edpos == EOL)
										p_edpos++;
								}
							}
						}


					break;
				}

				// =========================================================
				case DELETE:
				{
					// delete current character, except eols
					// works like backspace, but we first advance one character
					// backspace and return to our original position.
					if (*p_edpos != EOL)
					{
						p_edpos++;
						screenx++;
						ScootLeft();
						p_edpos--;
						screenx--;
						
						textlen--;
						p_used--;
					}
					break;
				}

				// =========================================================
				case INSERT:
				{
					// invert insert mode.
					insert_mode=!insert_mode;

					// MM.InsertToggleNotify(insert_mode);
					break;
				}


				// =========================================================
				case 'I':
				{
					DrawCanvas(Title);
					break;		// PgUp redraws
				}

				// =========================================================
				case 'Q':
				{
					clrscr();
					break;		// PgDown clears screen
				}


				// =========================================================
				default:
					break;

			}// end switch(c)
		}// end if(special)
		else
		{
			// the key was not a special key, so start handling normal keys.
			switch(c)
			{
				// =========================================================
				case ENTER:
				{
					// return places the cursor on the next line if possible.
					// the cursor is returned to the left-most position on the
					// new line. actually, return should result in the
					// creation of a new line.

					// move everything to the right in order to insert an eol
					ScootRight();
					*p_edpos=EOL;
					ClearToEndOfLine(screenx);

					//screeny=screeny>=RO_TEXT_Y?screeny:screeny+1;
					screenx=LB_TEXT_X;

					// add 1 to p_used as well as to textlen.
					// and move to next editing position.
					p_used++;		// one extra character used
					textlen++;		// text length increases
					p_edpos++;		// next editing position
					curlin++;		// move to new current line
					totallines++;	// add a line

					DrawCanvas(Title);
					break;
				}

				// =========================================================
				case BACKSP:
				{
					// if backspace is pressed, retreat one if possible,
					// go to the new position and draw a space there. also
					// apply this change in text area. the character to the
					// left of the original position has been removed.
					if (screenx>LB_TEXT_X)
						PerformBackspace();
					break;
				}

				// =========================================================
				case CTRL_Y:
				{
					// delete current line.
					// deleting a line is not as easy as it may appear.

					// old_screenx is used a the previous value of the screenx
					// parameter.
					old_screenx=screenx;

					// first walk to the end of the current line,
					// regardless of what is looks like (empty, first, last, etc).
					while (*p_edpos != EOL)
					{
						screenx++;	// just let screenx tag along to keep them
						p_edpos++;	// synchronised.
					}

					// now keep backspacing until the current line is empty.
					// remember that the current position is always EOL.
					// the current line is empty when the character before the
					// current is also EOL or Canvas when we are deleting the
					// first line.
					while (*(p_edpos-1) != EOL && p_edpos != Canvas)
						PerformBackspace();

					// if we were not on the first line we are now not at
					// Canvas. In that case we remove the EOL of the previous
					// line, replacing it with the current EOL we are (still)
					// standing on.
					if (p_edpos != Canvas)
						PerformBackspace();

					// there's just one special case left: we are deleting
					// the first line and it is empty.
					if (curlin==1)
					{
						// if there was just one line in total, leave the
						// EOL.
						if (totallines==1)
							;
						else
						{
							// otherwise, remove the EOL the first line
							// consists of to make the second line the first line.
							p_edpos++;
							screenx++; // synchronise screenx with p_edpos again.
							PerformBackspace();
						}
					}

					// now we begin repositioning the internal and screen
					// cursors. remember that totallines and curlin still
					// reflect the situation from before CTRL-Y was pressed.

					// place the screen cursor at the left most position.
					screenx=LB_TEXT_X;

					// if we deleted the first line, simply move to the end
					// or the previous x position.
					if (curlin==1)
					{
						// just walk forward until eol is encountered or the
						// previous x position.
						while (*p_edpos!=EOL && old_screenx>screenx)
						{
							p_edpos++;
							screenx++;
						}
					}
					else
					{
						// if we're on the last line, but the last line is
						// not the first...
						if (curlin==totallines && curlin>1)
						{
							// we have to move to the start of the current line.
							// since a check for EOL would immediately end,
							// we move back one position, but only if we're not
							// at Canvas after having removed the line. curlin still
							// has its previous value!
							if (p_edpos!=Canvas)
								p_edpos--;

							// now, move the the beginning of the line.
							while (*p_edpos!=EOL && p_edpos!=Canvas)
								p_edpos--;

							// we ended up on the previous line's eol or Canvas,
							// so move forward if necessary.
							if (p_edpos!=Canvas)
								p_edpos++;

							// we've deleted the last line, so the current line
							// decreases by one.
							curlin--;
						}
						else
						{
							// if we deleted an arbitrary line we are now at
							// the eol of a previous line. move one ahead to
							// end up on the first position of the new current line.
							p_edpos++;
						}

						// all preparation are done. just move forward until we
						// hit the eol or the previous x position.
						while (*p_edpos!=EOL && old_screenx>screenx)
						{
							p_edpos++;
							screenx++;
						}
					}

					// decrement the total number of lines and redraw.
					totallines--;
					DrawCanvas(Title);

					break;
				}

				// =========================================================
				default:
				{
					// if we're not at the end of a line and
					// the character is a letter, draw it on the screen
					// and save it in text area
					if (screenx<RO_TEXT_X && c>=32 && c<=122)
					{
						// check if we're inserting.
						if (insert_mode)
							ScootRight();

						// draw the new character on the screen.
						BM.DrawChar_Direct(screenx, screeny, c,
								m_color(R_BLUE, R_BROWN, 0, 1));

						// store c in the canvas
						*p_edpos=c;

						// only increase p_used if we are in inser mode or
						// if the edit position is the last text position
						if (insert_mode || p_edpos==p_used)
						{
							p_used++;
							textlen++;
						}

						// move to the next editing position
						p_edpos++;
						screenx=screenx>=RO_TEXT_X?screenx:screenx+1;
						break;
					}// end if
				} // end default:
			} // end switch(c)
		}// end if-else

		// move the cursor in case the coordinates were changed
		gotoxy(screenx,screeny);
	}// end while

	// disable cursor
	BM.DisableCursor();
}


