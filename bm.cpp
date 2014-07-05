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

// Board Manager code.

#pragma warn -use

#include "bm.hpp"
#include "ge.hpp"
#include "sm.hpp"
#include "zconst.hpp"

#include <stdio.h>

// declare GE here.
extern GameEngine GE;

// declar SM here.
extern SoundManager SM;

// ============================================================================
// Function: FillRemove
//		Remove top element from the Queue.
// Parameters:
//		None.
// Returns:
//		The number of element taken from the queue (0 or 1).
// ============================================================================

int BoardManager::FillRemove(xypos *position)
{
	// a pointer to a queue element.
	xypos *ti;

	// can't remove from empty queue.
	if (!FillNoElements) return 0;

	// set ti to the front of the queue. advance the
	// queue header pointer and subtract 1 off the number
	// of elements. ti now points to the element to be removed
	// from the queue.
	ti=FillQHead;
	FillQHead=FillQHead->next;
	FillNoElements--;

	// if no elements left, set the tail to NULL.
	if (!FillNoElements)
		FillQTail=NULL;

	// now copy the contents of ti to the memory pointed to
	// by position. THERE MUST BE MEMORY ALLOCATED!
	// finally, free the memory for ti and return.
	memcpy(position, ti, sizeof(xypos));
	free(ti);
	return 1;
}

// ============================================================================
// Function: FillAdd
//		Add one element to the end of the Fill Queue.
// Parameters:
//		The xypos structure to be pushed.
// Returns:
//		nothing.
// ============================================================================

void BoardManager::FillAdd(xypos position)
{
	// a pointer to a queue element
	xypos *t;

	// allocate memory and return if unsuccessful.
	t=(xypos *)malloc(sizeof(xypos));
	if (!t) return;

	// copy the contents of the element to be added to our
	// newly acquired memory. it's next pointer is NULL by default.
	memcpy(t, &position, sizeof(xypos));
	t->next=NULL;

	// if the queue was empty, the head is the tail is the
	// new element. the number of elements is 1. so is the counter
	// of the max number of elements in the queue.
	if (!FillNoElements)
	{
		FillQHead=FillQTail=t;
		FillNoElements=MaxElms=1;
	}
	// if the queue wasn't empty, add it to the back of the queue
	// by making the next pointer of the current last element point
	// to the new element. then, have the tail point to our new element.
	// update maxelements accordingly.
	else
	{
		FillQTail->next=t;
		FillQTail=t;
		FillNoElements++;
		if (FillNoElements>MaxElms) MaxElms=FillNoElements;
	}
}

// ============================================================================
// Function: FillQIsEmpty
//		Check whether FillAreaArray is empty
// Parameters:
//		None.
// Returns:
//		0 if there is 1 or more elements on the stack.
//		non nul if the stack contains 0 elements
// ============================================================================

unsigned char BoardManager::FillQIsEmpty(void)
{
	// if the element counter is 0, the queue must be empty.
	return FillNoElements==0;
}

// ============================================================================
// Function: DrawChar
//		Draw a zzt coded character ch at position x,y using color cl. This
//		function translates the zzt coded character to ascii code first. To do
//		this, we may need access to the board in order to find out what an
//		object's ascii character is.
// Parameters:
//		int x=horizontal position 0-79
//		int y=vertical position 0-24
//		int ch=zzt code for character to print
//		int cl=color code for character
//		int board=board we're drawing on. we may need to access its objects.
// Returns:
//		1=x or y values out of range
// Note:
//		The top left position of the screen is (0,0). zzt
//		starts counting at (1,1). Sorry about that...
// ============================================================================

int BoardManager::DrawChar(int x, int y, int ch, int cl, int board)
{
	// however, we keep the originals for the zzt-coded board information.
	// this allows us to differentiate between a fake and a normal wall for
	// instance.
	unsigned char ch_org, cl_org;
	int i;

	// save the original zzt codes. we want to save them in order to store
	// them in the zzt-code memory area.
	ch_org=ch;
	cl_org=cl;

	// Check if values out of range.
	if ( x>80 || x<1 || y>25 || y<1 )
		return 1;

	// We start our translation process here.
	// first check if we're not dealing with thext.
	if (ch < TEXT)
	{
		// So, no text. It might be an object.
		// For objects, take the char code from the BRDOBJ array.
		if (ch==0x24)
		{
			// Scan through the creatures and match on the x,y coordinates
			// of the object. Use the current drawing position and the x,y
			// position of the creatures.
			for (i=1; i<=BRDINF[board].NoObjects; i++)
				if (BRDOBJ[board][i].XPos==x && BRDOBJ[board][i].YPos==y)
					ch=BRDOBJ[board][i].Magic1;
		}
		else
			// Look up the correct ascii value in the translation table.
			// We might as well use ch here as the index into the Table, but
			// lets be consistent and use ch's copy ch_org.
			ch=Table[ch_org].AsciiValue;

		// If the translation table has a color code filled in use it.
		// Otherwise use the color from the world file.
		if (Table[ch_org].ColorValue !=0)
			cl=Table[ch_org].ColorValue;

		// Character code empty should not show up, so set color to 0.
		// This is also a nice way...cl=ch?cl:0;
		if (ch==0)
			cl=0;
	}
	// now what if we received text...
	else	if (ch <= TABLE_SIZE)
			{
				// The AsciiValue holds the color code in case of text.
				cl=Table[ch_org].AsciiValue;
				// The ColCode holds the ascii value of text.
				ch=cl_org;
			}

	// Increase both x and y coordinates to solve the (0,0) vs (1,1) problem.
	// (1,1) in zzt is (0,0) in zzt++, so both need to decrease by one.
	x--; y--;

	// This way, we only have to calculate once.
	// Use shifting in stead of multiplication to speed things up.
	// 2*x + 160*y = 2*x + 128*y + 32*y = x<<1 + y<<7 + y<<5
	unsigned char far *loc=vidbase+(x<<1)+(y<<7)+(y<<5);
	*loc=ch;
	*(loc+1)=cl;

	// do the same for the original zzt codes, but save them to the
	// zzt coded puffer at vidbase_page2.
	loc=vidbase_page2+(x<<1)+(y<<7)+(y<<5);

	*loc=ch_org;
	*(loc+1)=cl_org;
	return 0;
}

// ============================================================================
// Function: DrawChar_Direct
//		Draw ascii character c at position x,y using color col to
//		visible video RAM, without translating from zzt-code to ascii value.
// Parameters:
//		int x=horizontal position 0-79
//		int y=vertical position 0-24
//		int c=ascii value of character to print
//		int col=color code for character
// Returns:
//		1=x or y values out of range
// Note:
//		The top left position of the screen is (0,0). zzt
//		starts counting at (1,1). Sorry about that...
// ============================================================================

int BoardManager::DrawChar_Direct(int x, int y, int c, int col)
{
	// Check if values out of range.
	if ( x>80 || x<1 || y>25 || y<1 ) return 1;

	// Increase both x and y coordinates to solve the (0,0) vs (1,1) problem.
	x--; y--;

	// This way, we only have to calculate once.
	// Use shifting in stead of multiplication to speed things up.
	// 2*x + 160*y = 2*x + 128*y + 32*y = x<<1 + y<<7 + y<<5
	unsigned char far *loc=vidbase+(x<<1)+(y<<7)+(y<<5);

	*loc=c;
	*(loc+1)=col;
	return 0;
}

// ============================================================================
// Function: GetChar
//		Return character position x,y. Note that the character value is also
//		returned in case we're inspecting a text position, where the ascii
//		and color values are switched.
// Parameters:
//		int x=horizontal position 1-80
//		int y=vertical position 1-25
// Returns:
//		1=x or y values out of range
//		ascii value of character at given coordinates
// ============================================================================

int BoardManager::GetChar(int x, int y)
{
	// Check if values out of range.
	if ( x>80 || x<1 || y>25 || y<1 ) return 1;

	// Adjust x and y to solve the (0,0) vs (1,1) problem
	x--; y--;

	// This way, we only have to calculate once.
	// Use shifting in stead of multiplication to speed things up.
	// 2*x + 160*y = 2*x + 128*y + 32*y = x<<1 + y<<7 + y<<5
	return *(vidbase+(x<<1)+(y<<7)+(y<<5));
}

// ============================================================================
// Function: GetChar_Coded
//		Return zzt coded character position x,y.
//		Note that the character value is also
//		returned in case we're inspecting a text position, where the ascii
//		and color values are switched.
// Parameters:
//		int x=horizontal position 1-80
//		int y=vertical position 1-25
// Returns:
//		1=x or y values out of range
//		ascii value of character at given coordinates
// ============================================================================

int BoardManager::GetChar_Coded(int x, int y)
{
	// Check if values out of range.
	if ( x>80 || x<1 || y>25 || y<1 ) return 1;

	// Adjust x and y to solve the (0,0) vs (1,1) problem
	x--; y--;

	// This way, we only have to calculate once.
	// Use shifting in stead of multiplication to speed things up.
	// 2*x + 160*y = 2*x + 128*y + 32*y = x<<1 + y<<7 + y<<5
	return *(vidbase_page2+(x<<1)+(y<<7)+(y<<5));
}

// ============================================================================
// Function: GetCol
//		Return color at position x,y
// Parameters:
//		int x=horizontal position 0-79
//		int y=vertical position 0-24
// Returns:
//		1=x or y values out of range
//		color value of given coordinates
// ============================================================================

int BoardManager::GetCol(int x, int y)
{
	// Check if values out of range.
	if ( x>80 || x<1 || y>25 || y<1 ) return 1;

	// Adjust x and y to solve the (0,0) vs (1,1) problem
	x--; y--;

	return *(vidbase+1+(x<<1)+(y<<7)+(y<<5));
}

// ============================================================================
// Function: AsciiChart
//		Create basic ascii chart
// Parameters:
//		None.
// Returns:
//		Nothing.
// ============================================================================

void BoardManager::AsciiChart(void)
{
	int x,y,c;
	char *s;
	// itoa returns a mac of 17 bytes
	if ( (s=(char *)malloc(17) )==NULL ) return;

	c=0;
	for (x=1;x<=60;x+=5)
		for (y=0;y<24;y++)
			if (c<=255)
			{
				DrawChar_Direct(x,y,c,m_color(R_CYAN,R_GREY,0,1));
				s=itoa(c,s,10);
				TypeString(x+1,y,s,15);
				c++;
			}
}

// ============================================================================
// Function: ColorList
//		Display a list of all color codes
// Parameters:
//		None.
// Returns:
//		Nothing.
// ============================================================================

void BoardManager::ColorList(void)
{
	int x,y,col;
	char *s;
	// itoa returns a mac of 17 bytes
	if ( (s=(char *)malloc(17) )==NULL ) return;

	col=0;
	for (x=1;x<=60;x+=5)
		for (y=0;y<24;y++)
			if (col<=255)
			{
				DrawChar_Direct(x,y,88,m_color(0,col,0,1));
				s=itoa(col,s,10);
				TypeString(x+1,y,s,15);
				col++;
			}
}

// ============================================================================
// Function: DrawBoundBox
//		Draw a rectangle between top left and bottom right coordinates
//		using a given character and color.
// Parameters:
//		int UpLeftx=x-coordinate of upper left corner
//		int UpLefty=y-coordinate of upper left corner
//		int BotRightx=x-coordinate of bottom right corner
//		int BotRighty=y-coordinate of bottom right corner
//		int c=character to be used
//		int col=color to be used
// Returns:
//		1=x or y values out of range.
// ============================================================================

int BoardManager::DrawBoundBox(int UpLeftx, int UpLefty, int BotRightx, int BotRighty, int c, int col)
{
	// Check for out of bounds.
	if (	UpLeftx<1 || UpLeftx>80 || BotRightx<1 || BotRightx>80 || \
			UpLefty<1 || UpLefty>25 || BotRighty<1 || BotRighty>25 ) return 1;

	int x,y;

	// Draw upper bound
	for (x=UpLeftx;x<=BotRightx;x++)
		DrawChar(x,UpLefty,c,col,0);

	// Draw right bound
	x--;
	for (y=UpLefty;y<=BotRighty;y++)
		DrawChar(x,y,c,col,0);

	// Draw lower bound
	y--;
	for (x=BotRightx;x>=UpLeftx;x--)
		DrawChar(x,y,c,col,0);

	// Draw left bound
	x++;
	for (y=BotRighty;y>=UpLefty;y--)
		DrawChar(x,y,c,col,0);

	return 0;
}

// ============================================================================
// Function: DrawFilledBox
//		Draw a rectangle between top left and bottom right coordinates
//		using a given character and color and fill it overwriting everything
//		inside.
// Parameters:
//		int UpLeftx=x-coordinate of upper left corner
//		int UpLefty=y-coordinate of upper left corner
//		int BotRightx=x-coordinate of bottom right corner
//		int BotRighty=y-coordinate of bottom right corner
//		int c=character to be used
//		int col=color to be used
// Returns:
//		1=x or y values out of range.
// ============================================================================

int BoardManager::DrawFilledBox(int UpLeftx, int UpLefty, int BotRightx, \
											 int BotRighty, int c, int col)
{
	// Check for out of bounds.
	if (	UpLeftx<1 || UpLeftx>80 || BotRightx<1 || BotRightx>80 || \
			UpLefty<1 || UpLefty>25 || BotRighty<1 || BotRighty>25 ) return 1;

	int x,y;

	// Fill the box.
	for (y=UpLefty; y<=BotRighty; y++)
		for (x=UpLeftx; x<=BotRightx; x++)
			DrawChar(x,y,c,col,0);

	return 0;
}


// ============================================================================
// Function: FillArea
//		Fills the area of joining identical characters like FillOverChar,
//    with the given character and color. This routine will fill the
//		current position with the new character if it contains the character and
//		has the same color we should overwrite. It does the same under this
//		condition for the position to its immediate north, east, west and south.
//		Except that we have to look ahead one position before filling. This way,
//		when we hit a wall the filling in that direction stops.
//		Positions to the north-east, etc are not used to avoid 'leaking'.
//		e.g.:
//		##### <- Would leak through this corner
//    #    #
//    #    #
//    ######
// Parameters:
//		int x=x-coordinate of starting position
//		int y=y-coordinate of starting position
//		int c=character to be used
//		int col=color to be used
//		int FillOverChar=character we are filling over
//		int FillOverCol=color we are filling over
// Returns:
//		Nothing.
// ============================================================================

void BoardManager::FillArea(char x, char y, char c, char col, char FillOverChar, char FillOverCol)
{
	// Temporary storage for x,y positions
	xypos xy, current, N, E, S, W;

	// Setup the current position as a struct.
	xy.x=x;
	xy.y=y;
	xy.next=NULL;

	// Add the current position to the queue.
	FillAdd(xy);

	// And immediately draw it.
	DrawChar(xy.x, xy.y, c, col, 0);

	// Keep looping until the queue is empty.
	while (!FillQIsEmpty())
	{
		// Get next element to draw.
		FillRemove(&current);

		// Fetch circling positons to North, East, South and West
		N.x=current.x; N.y=current.y-1; N.next=NULL;
		E.x=current.x+1; E.y=current.y; E.next=NULL;
		S.x=current.x; S.y=current.y+1; S.next=NULL;
		W.x=current.x-1; W.y=current.y; W.next=NULL;

		// Since xypos.x and xypos.y are unsigned chars, the checks for
		// them being greater or equal to zero are superfluous.
		// First check whether we should overwrite this character.
		// Then check whether we're still inside the screen limits.
		if (GetChar(N.x, N.y)==FillOverChar && GetCol(N.x, N.y)==FillOverCol)
			if (N.x<81 && N.y<26)
			{
				// Draw the character to the north and queue its position.
				DrawChar(N.x, N.y, c, col,0);
				FillAdd(N);
			}

		// Do the same for East, South and West
		if (GetChar(E.x, E.y)==FillOverChar && GetCol(E.x, E.y)==FillOverCol)
			if (E.x<81 && E.y<26)
			{
				DrawChar(E.x, E.y, c, col, 0);
				FillAdd(E);
			}

		if (GetChar(S.x, S.y)==FillOverChar && GetCol(S.x, S.y)==FillOverCol)
			if (S.x<81 && S.y<26)
			{
				DrawChar(S.x, S.y, c, col, 0);
				FillAdd(S);
			}

		if (GetChar(W.x, W.y)==FillOverChar && GetCol(W.x, W.y)==FillOverCol)
			if (W.x<81 && W.y<26)
			{
				DrawChar(W.x, W.y, c, col, 0);
				FillAdd(W);
			}
	}
}

// ============================================================================
// Function: TypeString
//		Displays the given string, starting from the given position.
//		This routine does no wrapping.
// Parameters:
//		int x=x-coordinate
//		int y=y-coordinate
//		char *s=string to be displayed. Must be "\0" terminated.
//		int col=color to be used
// Returns:
//		1=string was not displayed completely.
// ============================================================================

int BoardManager::TypeString(int x, int y, char *s, int col)
{
	// initiate a walk pointer to the start of the string.
	char *walk=s;

	// while there's no NULL character and we're still on the screen
	// draw the current character and then increase x and the walk pointer
	while (*walk && x<80) DrawChar_Direct(x++,y,*walk++, col);

	// if we ended because we reached the end of the screen, return with 1,
	// else zero.
	if (x==80) return 1;
	return 0;
}

// ============================================================================
// Function: TypeString (overloaded)
//		Displays the given integer as a string, starting from the given position.
//		This routine does no wrapping.
// Parameters:
//		int x=x-coordinate
//		int y=y-coordinate
//		int i=integer to be displayed.
//		int col=color to be used
// Returns:
//		1=string was not displayed completely.
// ============================================================================

int BoardManager::TypeString(int x, int y, int i, int col)
{
	char *s, *walk;

	// convert the number to a string.
	s=(char *)malloc(17);
	s=itoa(i,s,10);

	// Draw it on the screen and stop at final screen position.
	walk=s;
	while (*walk && x<80) DrawChar_Direct(x++,y,*walk++, col);

	// free up this memory.
	free(s);
	if (x==80) return 1;

	return 0;
}

// ============================================================================
// Function: ReadWorld
//		Reads a .ZZT file into memory.
// Parameters:
//		char *file=filename
//		int report=create report on world information?
//		int screen=include screen information in output?
// Returns:
//		0=success
//		1=world not found.
//		2=report.txt could not be created or opened.
//		10=error allocating memory for GameEngine
//		11=error allocating memory for array of Boards
//		12=error allocating memory for array of Blocks
//		13=error allocating memory for array of BoardInfo
//		14=error allocating memory for array of Objects
//		15=error allocating memory for a Block
//		16=error allocating memory for an Object
//		17=error allocating memory for a pointer to object program text
//		18=error allocating memory for program text
// ============================================================================

int BoardManager::ReadWorld(char *file, int report, int screen)
{
	FILE *world, *fout;
	int board, blocksum, block, object;

	// Open or create report.txt (overwrite).
	if (report)
		if ((fout = fopen("REPORT.TXT", "wt"))==NULL)
		{
			printf("File REPORT.TXT could not be created.\n", file);
			return 2;
		}

	// Open world
	if ((world = fopen(file, "rb"))== NULL)
	{
		printf("File %s was not found.\n", file);
		return 1;
	}

	// If we're reporting, start tracing now.
	if (report)
	{
		fprintf(fout,"START OF TRACE.\n===============\n");
		fprintf(fout,"Opened file %s.\n\n",file);
	}

	//
	// Read GameInfo struct
	//
	fread((void *)&GI, sizeof(GameInfo), 1, world);
	if (report)
		PrintGameInfo(fout);

	// Array of Board structs.
	if ((BRD=(Board *)malloc((GI.NoBoards+1)*sizeof(Board)))==NULL)
		return 10;

	// Array of arrays of Block structs.
	if ((BLCK=(Block **)malloc((GI.NoBoards+1)*sizeof(Block *)))==NULL)
		return 11;

	// Array of BoardInfo structs.
	if ((BRDINF=(BoardInfo *)malloc((GI.NoBoards+1)*sizeof(BoardInfo)))==NULL)
		return 12;

	// Array of BoardObject structs.
	if ((BRDOBJ=(BoardObject **)malloc((GI.NoBoards+1)*sizeof(BoardObject *)))==NULL)
		return 13;

	// Array of Object Program strings.
	if ((OBJPRG=(char ***)malloc((GI.NoBoards+1)*sizeof(char **)))==NULL)
		return 14;

	//
	// Seek to Game Saved indicator
	//
	fseek(world, 264, SEEK_SET);
	fread((void *)&GameSaved, sizeof(unsigned char), 1, world);

	if (report)
		fprintf(fout,"GameSaved=%d\n\n",GameSaved);

	//
	// Seek to Start of Boards
	//
	fseek(world, 512, SEEK_SET);

	//	Start reading boards
	// There is always one title screen board. So use <= in stead of <
	for (board=0; board<=GI.NoBoards; board++)
	{
		//
		// Read the Board struct into BRD array
		//
		fread(&(BRD[board]), sizeof(Board), 1, world);

		if (report)
			PrintBoard(board, fout);

		if (report && screen)
			fprintf(fout,"Compressed screen information.\n===========================\n");
		// realloc() requires this pointer to be NULL for the first call
		BLCK[board]=NULL;
		blocksum=block=0;
		while (blocksum<1500)
		{
			// Request more memory for this block
			if ((BLCK[board]= \
				(Block *)realloc(BLCK[board], (block+1)*sizeof(Block)))==NULL)
					return 15;

			//
			// Read the block into memory.
			//
			fread(&(BLCK[board][block]), sizeof(Block), 1, world);

			// Sum up the number of characters.
			blocksum+=BLCK[board][block].NoChars;

			// Note that the information we are reading here is coded.
			// We could start translating here, but then we would have to
			// retranslate on writing the zzt file as well. To save one
			// translation we will only translate in BM::DrawBoard().

			// Generate report.
			if (report && screen)
				fprintf(fout,"\tBlock#=%d, NoChars=%d, CharCode=0x%02x, ColCode=0x%02x, Sum=%d\n", \
			block,BLCK[board][block].NoChars, \
			BLCK[board][block].CharCode, \
			BLCK[board][block].ColCode, \
			blocksum);

			block++;
		}
		if (report && screen)
			fprintf(fout,"End Compressed screen information.\n===============================\n\n");

		//
		// Read BoardInfo struct
		//
		fread(&(BRDINF[board]), sizeof(BoardInfo), 1, world);

		if (report)
			PrintBoardInfo(board, fout);

		// realloc() requires these pointers to be NULL for the first call
		BRDOBJ[board]=NULL;
		OBJPRG[board]=NULL;

		object=0;
		// There is always one player object. So use <= in stead of <
		while (object<=BRDINF[board].NoObjects)
		{
			// Request more memory for this object
			if ((BRDOBJ[board]= \
				(BoardObject *)realloc(BRDOBJ[board], (object+1)*sizeof(BoardObject)))==NULL)
					return 16;

			//
			// Read the object into memory.
			//
			fread(&(BRDOBJ[board][object]), sizeof(BoardObject), 1, world);

			if (report)
				PrintObjectInfo(board, object, fout);

			// request memory for a pointer to program text
			// even though there might not be a program.
			if ((OBJPRG[board]= \
				(char **)realloc(OBJPRG[board], (object+1)*sizeof(char *)))==NULL)
					return 17;

			// Only if there is a program to be read.
			// Never read program of player object.
			if ((BRDOBJ[board][object].ProgramLength>0) && object>0)
			{
				// allocate memory for this text. Use length from object struct.
				if ((OBJPRG[board][object]= \
					(char *)malloc(BRDOBJ[board][object].ProgramLength))==NULL)
						return 18;

				//
				// Read program text into memory.
				//
				fread(OBJPRG[board][object], \
					BRDOBJ[board][object].ProgramLength, 1, world);

				if (report)
					fprintf(fout, "Program=\n%s\n",OBJPRG[board][object]);
			}
			// Count this object
			object++;
		} // End of Object loop
	} // End of Board loop

	// Flush and cose files.
	fflush(world);
	fflush(fout);
	fclose(world);
	fclose(fout);

	return 0;
}

// ============================================================================
// Function: WriteWorld
//		Writes a .ZZT world to a file.
// Parameters:
//		char *file=filename
// Returns:
//		0=success
//		1=error creating file
// ============================================================================

int BoardManager::WriteWorld(char *file)
{
	FILE *world;
	int board, blocksum, block, object;

	// Open or create binary file (overwrite).
	if ((world = fopen(file, "wb"))==NULL)
		return 1;

	//
	// Write GameInfo struct
	//
	fwrite((void *)&GI, sizeof(GameInfo), 1, world);

	//
	// Seek to Game Saved indicator
	//
	fseek(world, 264, SEEK_SET);
	fwrite((void *)&GameSaved, sizeof(unsigned char), 1, world);

	//
	// Seek to Start of Boards
	//
	fseek(world, 512, SEEK_SET);

	//	Start reading boards
	// There is always one title screen board. So use <= in stead of <
	for (board=0; board<=GI.NoBoards; board++)
	{
		//
		// Write the Board struct
		//
		fwrite(&(BRD[board]), sizeof(Board), 1, world);

		blocksum=block=0;
		while (blocksum<1500)
		{
			//
			// Write the block.
			//
			fwrite(&(BLCK[board][block]), sizeof(Block), 1, world);

			// Sum up the number of characters.
			blocksum+=BLCK[board][block].NoChars;
			block++;
		}

		//
		// Write BoardInfo struct
		//
		fwrite(&(BRDINF[board]), sizeof(BoardInfo), 1, world);

		object=0;
		// There is always one player object. So use <= in stead of <
		while (object<=BRDINF[board].NoObjects)
		{
			//
			// Write the object.
			//
			fwrite(&(BRDOBJ[board][object]), sizeof(BoardObject), 1, world);

			// Only if there is a program to be read.
			// Never read program of player object.
			if ((BRDOBJ[board][object].ProgramLength>0) && object>0)
				//
				// Write program text into memory.
				//
				fwrite(OBJPRG[board][object], \
					BRDOBJ[board][object].ProgramLength, 1, world);

			// Count this object
			object++;
		} // End of Object loop
	} // End of Board loop

	// Flush and cose files.
	fflush(world);
	fclose(world);
	return 0;
}

// ============================================================================
// Function: PrintObjectInfo
//		Report object information to fout
//		Accesses global variables BRDINF and BRDOBJ
// Parameters:
//		int board=board number
//		int object=object number
//		FILE *fout=pointer to output file
// Returns:
//		Nothing.
// ============================================================================

void BoardManager::PrintObjectInfo(int board, int object, FILE *fout)
{
	fprintf(fout,"\nCreature %d of %d %s\n================\n", \
		object,BRDINF[board].NoObjects, object==0?"(player)\0":"\0");
	fprintf(fout,"XPos:                  %d\n",BRDOBJ[board][object].XPos);
	fprintf(fout,"YPos:                  %d\n",BRDOBJ[board][object].YPos);
	fprintf(fout,"XMovementStep:         %d\n",BRDOBJ[board][object].XMovementStep);
	fprintf(fout,"YMovementStep:         %d\n",BRDOBJ[board][object].YMovementStep);
	fprintf(fout,"CycleSpeed:            %d\n",BRDOBJ[board][object].CycleSpeed);
	fprintf(fout,"Magic1:                %d\n",BRDOBJ[board][object].Magic1);
	fprintf(fout,"Magic2:                %d\n",BRDOBJ[board][object].Magic2);
	fprintf(fout,"DestinationBoardNo:    %d\n",BRDOBJ[board][object].DestinationBoardNo);
	fprintf(fout,"Dummy1:                %u\n",BRDOBJ[board][object].dummy1);
	fprintf(fout,"Dummy2:                %u\n",BRDOBJ[board][object].dummy2);
	fprintf(fout,"CharCodeUnder:         %d\n",BRDOBJ[board][object].CharCodeUnder);
	fprintf(fout,"ColCodeUnder:          %d\n",BRDOBJ[board][object].ColCodeUnder);
	fprintf(fout,"ProgramTextPtr:        %p\n",BRDOBJ[board][object].ProgramTextPtr);
	fprintf(fout,"ProgramIP:             %d\n",BRDOBJ[board][object].ProgramIP);
	fprintf(fout,"ProgramLength:         %d\n",BRDOBJ[board][object].ProgramLength);
	fprintf(fout,"End Object Info\n===============\n");
}

// ============================================================================
// Function: PrintBoard
//		Report board information to fout
//		Accesses global variables BRD and GI
// Parameters:
//		int board=board number
//		FILE *fout=pointer to output file
// Returns:
//		Nothing.
// ============================================================================

void BoardManager::PrintBoard(int board, FILE *fout)
{
	int i;

	fprintf(fout,"\nBoard Header %d of %d\n====================\n", \
		board, GI.NoBoards);
	fprintf(fout,"BoardSize:       %d\n",BRD[board].BoardSize);
	fprintf(fout,"Titlelength:     %d\n",BRD[board].TitleLength);
	fprintf(fout,"Title:           %.*s\nDummy=(hex) ", BRD[board].TitleLength, \
		BRD[board].Title);

	for (i=0;i<16;i++)
		fprintf(fout,"%02x ",BRD[board].dummy[i]);

	fprintf(fout,"\nEnd Board Header\n================\n\n");
}

// ============================================================================
// Function: PrintGameInfo
//		Report game information to fout
//		Accesses global variable GI
// Parameters:
//		FILE *fout=pointer to output file
// Returns:
//		Nothing.
// ============================================================================

void BoardManager::PrintGameInfo(FILE *fout)
{
	fprintf(fout,"Game Info\n=========\n");
	fprintf(fout,"dummy1:         %u\n",GI.dummy1);
	fprintf(fout,"NoBoards:       %d\n",GI.NoBoards);
	fprintf(fout,"NoAmmo:         %d\n",GI.NoAmmo);
	fprintf(fout,"NoGems:         %d\n",GI.NoGems);
	fprintf(fout,"BlueKey:        %d\n",GI.BlueKey);
	fprintf(fout,"GreenKey:       %d\n",GI.GreenKey);
	fprintf(fout,"CyanKey:        %d\n",GI.CyanKey);
	fprintf(fout,"RedKey:         %d\n",GI.RedKey);
	fprintf(fout,"PurpleKey:      %d\n",GI.PurpleKey);
	fprintf(fout,"YellowKey:      %d\n",GI.YellowKey);
	fprintf(fout,"WhiteKey:       %d\n",GI.WhiteKey);
	fprintf(fout,"Health:         %d\n",GI.Health);
	fprintf(fout,"StartingBoard:  %u\n",GI.StartingBoard);
	fprintf(fout,"NoTorches:      %d\n",GI.NoTorches);
	fprintf(fout,"NoTorchCycles:  %d\n",GI.NoTorchCycles);
	fprintf(fout,"NoEnergizerCycles:%d\n",GI.NoEnergizerCycles);
	fprintf(fout,"dummy3:         %u\n",GI.dummy3);
	fprintf(fout,"Score:          %d\n",GI.Score);
	fprintf(fout,"TitleLength:    %d\n",GI.TitleLength);
	fprintf(fout,"Title:          %.*s\n",GI.TitleLength, GI.Title);
	fprintf(fout,"Flag1Length     %d\n",GI.Flag1Length);
	fprintf(fout,"Flag1Name       %.*s\n",GI.Flag1Length, GI.Flag1Name);
	fprintf(fout,"Flag2Length     %d\n",GI.Flag2Length);
	fprintf(fout,"Flag2Name       %.*s\n",GI.Flag2Length, GI.Flag2Name);
	fprintf(fout,"Flag3Length     %d\n",GI.Flag3Length);
	fprintf(fout,"Flag3Name       %.*s\n",GI.Flag3Length, GI.Flag3Name);
	fprintf(fout,"Flag4Length     %d\n",GI.Flag4Length);
	fprintf(fout,"Flag4Name       %.*s\n",GI.Flag4Length, GI.Flag4Name);
	fprintf(fout,"Flag5Length     %d\n",GI.Flag5Length);
	fprintf(fout,"Flag5Name       %.*s\n",GI.Flag5Length, GI.Flag5Name);
	fprintf(fout,"Flag6Length     %d\n",GI.Flag6Length);
	fprintf(fout,"Flag6Name       %.*s\n",GI.Flag6Length, GI.Flag6Name);
	fprintf(fout,"Flag7Length     %d\n",GI.Flag7Length);
	fprintf(fout,"Flag7Name       %.*s\n",GI.Flag7Length, GI.Flag7Name);
	fprintf(fout,"Flag8Length     %d\n",GI.Flag8Length);
	fprintf(fout,"Flag8Name       %.*s\n",GI.Flag8Length, GI.Flag8Name);
	fprintf(fout,"Flag9Length     %d\n",GI.Flag9Length);
	fprintf(fout,"Flag9Name       %.*s\n",GI.Flag9Length, GI.Flag9Name);
	fprintf(fout,"Flag10Lengt     %d\n",GI.Flag10Length);
	fprintf(fout,"Flag10Name      %.*s\n",GI.Flag10Length, GI.Flag10Name);
	fprintf(fout,"TimeElapsed     %d\n",GI.TimeElapsed);
	fprintf(fout,"\nEnd Game Info\n=============\n");
};

// ============================================================================
// Function: PrintBoardInfo
//		Report boardinfo information to fout
//		Accesses global variables BRDINF and GI
// Parameters:
//		int board=board number
//		FILE *fout=pointer to output file
// Returns:
//		Nothing.
// ============================================================================

void BoardManager::PrintBoardInfo(int board, FILE *fout)
{
	int i;

	fprintf(fout,"\nBoard Footer %d of %d\n====================\n", \
		board, GI.NoBoards);

	fprintf(fout,"MaxNoShots:     %d\n",BRDINF[board].MaxShots);
	fprintf(fout,"DarkBoard:      %d\n",BRDINF[board].DarkBoard);
	fprintf(fout,"BoardNorth:     %d\n",BRDINF[board].BoardNorth);
	fprintf(fout,"BoardSouth:     %d\n",BRDINF[board].BoardSouth);
	fprintf(fout,"BoardWest:      %d\n",BRDINF[board].BoardWest);
	fprintf(fout,"BoardEast:      %d\n",BRDINF[board].BoardEast);
	fprintf(fout,"ReEnter:        %d\n",BRDINF[board].ReEnter);
	fprintf(fout,"MessageLength:  %d\n",BRDINF[board].MessageLength);
	fprintf(fout,"Message:        %.*s\n",BRDINF[board].MessageLength,\
		BRDINF[board].Message);
	fprintf(fout,"dummy1:         %u\n",BRDINF[board].dummy1);
	fprintf(fout,"TimeLimit:      %d\n",BRDINF[board].TimeLimit);

	fprintf(fout,"dummy2:(hex) ");
	for (i=0;i<16;i++)
		fprintf(fout,"%02x ",BRDINF[board].dummy2[i]);

	fprintf(fout,"\nNoObjects:    %d\n",BRDINF[board].NoObjects);
	fprintf(fout,"End Board Footer\n================\n\n");
}

// ============================================================================
// Function: DrawBoard
//		Paint board on screen
//		Accesses global variables BLCK
// Parameters:
//		int board=board number
// Returns:
//		Nothing.
// ============================================================================

void BoardManager::DrawBoard(int board)
{
	int maxx=60, maxy=25, x=1, y=1, sum=0, block=0, i;

	// loop as long as we haven't drawn 1500 characters in total.
	while (sum<1500)
	{
		sum+=BLCK[board][block].NoChars;

		// Draw this block on the screen.
		for (i=0; i<BLCK[board][block].NoChars; i++)
		{
			// Draw the zzt coded character.
			// Mind that each zzt coded character is translated separately!
			// This is inefficient, but at least DrawChar() does all the
			// translation work for us.
			//	DrawChar(x,y,ch,cl);
			DrawChar(x,y,BLCK[board][block].CharCode,BLCK[board][block].ColCode,board);

			// Make sure we stay inside the board area.
			if (++x > maxx)
			{
				x=1;
				if (++y > maxy)
					y=1;
			}
		}
		// Next block please
		block++;
	}// End of while. Sum<1500.
}

// ============================================================================
// Function: Fade
//		Fill screen with purple blocks and randomly replace them with black
//		so as to simulate a fade out.
// Parameters:
//		none.
// Returns:
//		nothing.
// ============================================================================

void BoardManager::Fade(void)
{
	int x,y,count,t;

	// Fill screen with purple blocks
	for (x=1; x<60; x++)
		for (y=1; y<25; y++)
			DrawChar(x,y,219,85,0); //purple blocks

	// Borland C++
	#ifdef __BORLANDC__
		randomize();
	#endif

	// MS Visual C++
	#ifdef _MSC_VER
		srand(60);
	#endif

	// while not 1500 positions done
	//	replace position
	count=0;
	while (count<3000)
	{
		// slow things down quick and dirty.
		t=0;while(t<1450)t++;

		// Borland C++
		#ifdef __BORLANDC__
			x=random(60);
			y=random(25);
		#endif

		// MS Visual C++
		#ifdef _MSC_VER
			x=rand();
			y=rand();
		#endif

		DrawChar(x,y,219,0,0);
		count++;
	}
}

// ============================================================================
// Function: Screen2Board
//		Compress the current screencontents into the board indicated by the
//		given parameter.
// Parameters:
//		int board = board number into which the screen will be put.
// Returns:
//		nothing.
// ============================================================================

void BoardManager::Screen2Board(int board)
{
	board=0;
}

// ============================================================================
// Function: SetScan350
//		Set the screen mode to 350 scan lines.
// Parameters:
//		none.
// Returns:
//		nothing.
// ============================================================================

void BoardManager::SetScan350(void)
{
	// MS Visual C++
	#ifdef _MSC_VER
		union _REGS regs;
	#endif

	// Borland C++
	#ifdef __BORLANDC__
		union REGS regs;
	#endif

	// Set screen mode to 350 scan lines.
	// Do this before setting color mode and putting anything on screen.
	regs.h.ah = 0x12;
	regs.h.al = 0x01;
	regs.h.bh = 0x00;
	regs.h.bl = 0x30;

	// MS Visual C++
	#ifdef _MSC_VER
		_int86(0x10,&regs,&regs);
	#endif

	// Borland C++
	#ifdef __BORLANDC__
		int86(0x10,&regs,&regs);
	#endif
}

// ============================================================================
// Function: SetColorMode
//		Set screen to 80x25 16 color mode.
//	Parameters:
//		none.
// Returns:
//		nothing.
// ============================================================================

void BoardManager::SetColorMode(void)
{
	// MS Visual C++
	#ifdef _MSC_VER
		union _REGS regs;
	#endif

	// Borland C++
	#ifdef __BORLANDC__
		union REGS regs;
	#endif

	// set screen to 80x25 16 color mode
	regs.h.ah = 0x00;
	regs.h.al = 0x03;

	// MS Visual C++
	#ifdef _MSC_VER
		_int86(0x10,&regs,&regs);

		// set pointer to screen memory. 0xb8000 for color.
		vidbase = (unsigned char*)(_MK_FP(0xb000, 0x8000));
	#endif

	// Borland C++
	#ifdef __BORLANDC__
		int86(0x10,&regs,&regs);

		// set pointer to screen memory. 0xb8000 for color.
		vidbase = (unsigned char*)(MK_FP(0xb000, 0x8000));
	#endif
}

// ============================================================================
// Function: SetMonoMode
//		Set screen to 80x25 monochrome mode.
//	Parameters:
//		none.
// Returns:
//		nothing.
// ============================================================================

void BoardManager::SetMonoMode(void)
{
	// MS Visual C++
	#ifdef _MSC_VER
		union _REGS regs;
	#endif

	// Borland C++
	#ifdef __BORLANDC__
		union REGS regs;
	#endif

	// set screen to 80x25 monochrome mode
	regs.h.ah = 0x00;
	regs.h.al = 0x07;

	// MS Visual C++
	#ifdef _MSC_VER
		_int86(0x10,&regs,&regs);

		// set pointer to screen memory. 0xb0000 for mono.
		vidbase = (unsigned char*)(_MK_FP(0xb000, 0x0000));
	#endif

	// Borland C++
	#ifdef __BORLANDC__
		int86(0x10,&regs,&regs);

		// set pointer to screen memory. 0xb0000 for mono.
		vidbase = (unsigned char*)(MK_FP(0xb000, 0x0000));
	#endif
}

// ============================================================================
// Function: EnableCursorBig
//		Enable the cursor and set size to full character block.
//	Parameters:
//		none.
// Returns:
//		nothing.
// ============================================================================

void BoardManager::EnableCursorBig(void)
{
	// MS Visual C++
	#ifdef _MSC_VER
		union _REGS regs;
	#endif

	// Borland C++
	#ifdef __BORLANDC__
		union REGS regs;
	#endif

	// enabling the cursor while it was already enabled does no harm.
	regs.h.ah = 0x01;
	regs.h.ch = 0x01;
	regs.h.cl = 0x07;

	// MS Visual C++
	#ifdef _MSC_VER
		_int86(0x10,&regs,&regs);
	#endif

	// Borland C++
	#ifdef __BORLANDC__
		int86(0x10,&regs,&regs);
	#endif
}

// ============================================================================
// Function: EnableCursorSmall
//		Enable the cursor and set size to bottom line of character block.
//	Parameters:
//		none.
// Returns:
//		nothing.
// ============================================================================

void BoardManager::EnableCursorSmall(void)
{
	// MS Visual C++
	#ifdef _MSC_VER
		union _REGS regs;
	#endif

	// Borland C++
	#ifdef __BORLANDC__
		union REGS regs;
	#endif

	// enabling the cursor while it was already enabled does no harm.
	regs.h.ah = 0x01;
	regs.h.ch = 0x06;
	regs.h.cl = 0x07;

	// MS Visual C++
	#ifdef _MSC_VER
		_int86(0x10,&regs,&regs);
	#endif

	// Borland C++
	#ifdef __BORLANDC__
		int86(0x10,&regs,&regs);
	#endif
}

// ============================================================================
// Function: DisableCursor
//		Disable the cursor.
//	Parameters:
//		none.
// Returns:
//		nothing.
// ============================================================================

void BoardManager::DisableCursor(void)
{
	// This function only works in full screen MS-DOS mode.
	// in a windows dos-box, the cursor defaults to scan lines 0 to 1.
	
	// MS Visual C++
	#ifdef _MSC_VER
		union _REGS regs;
	#endif

	// Borland C++
	#ifdef __BORLANDC__
		union REGS regs;
	#endif

	// bit 7=0, bit 6-5=01, bit 4-0=scan line
	regs.h.ah = 0x01;
	regs.h.ch = 0x28;
	regs.h.cl = 0x29;

	// MS Visual C++
	#ifdef _MSC_VER
		_int86(0x10,&regs,&regs);
	#endif

	// Borland C++
	#ifdef __BORLANDC__
		int86(0x10,&regs,&regs);
	#endif
}

// ============================================================================
// Function: Cls
//		Clear screen. Fill screen with spaces and color 0.
//	Parameters:
//		none.
// Returns:
//		nothing.
// ============================================================================

void BoardManager::Cls(void)
{
	// MS Visual C++
	#ifdef _MSC_VER
		_clearscreen(_GWINDOW); // this also changes to full screen mode.
	#endif

	// Borland C++
	#ifdef __BORLANDC__
		clrscr();
	#endif
}

// ============================================================================
// Function: Try
//		Try to move the player object to desired location. No actual move is
//		performed, only the check is done. Messages can be sent and flags set.
//	Parameters:
//		char x = desired x coordinate
//		char y = desired y coordinate
// Returns:
//		0 = player cannot move there
//		1 = player can move there
// ============================================================================

char BoardManager::Try(char x, char y)
{
	// for some reason, we have to use an int for testing the
	// value of the desired position.
	int t;

	// player cannot move beyond board.
	if (x<1 || x>60 || y<1 || y>25) return 0;

	// Get the character at desired location
	t=GetChar_Coded(x,y);

	// We can move over empties and fakes
	// Note that we're testing zzt coded chars here, not ascii codes!
	if (t==I_EMPTY || t==I_FAKE)
	{
		// play a footstep tune.
		// SM.AddTune(Sounds[???]);
		return 1;
	}

	if (t==I_SCROLL)
	{
		SM.AddTune(Sounds[SCROLLSOUND]);
	}

	// And over nothing else.
	return 0;
}

// ============================================================================
// Function: MovePlayer
//		Move player to te required position. We assume we may move there.
//	Parameters:
//		char x = desired x coordinate
//		char y = desired y coordinate
// Returns:
//		nothing
// ============================================================================

void BoardManager::MovePlayer(char x, char y)
{
	char board;

	// Get current board number
	board=GetCurrentBoard();

	// Restore character we're standing on.
	DrawChar(BRDOBJ[board][0].XPos, BRDOBJ[board][0].YPos, \
				BRDOBJ[board][0].CharCodeUnder, BRDOBJ[board][0].ColCodeUnder, board);

	// save the character we're about to move over.
	BRDOBJ[board][0].CharCodeUnder=GetChar_Coded(x,y);
	BRDOBJ[board][0].ColCodeUnder=GetCol(x,y);

	// save the current position in the player struct
	BRDOBJ[board][0].XPos=x;
	BRDOBJ[board][0].YPos=y;

	// move player to new position.
	DrawChar(x,y, I_PLAYER, m_color(R_BLUE, R_GREY, 0, 1), board);
}

// ============================================================================
// Function: GetCurrentBoard
//		Return current board number as char.
//	Parameters:
//		none
// Returns:
//		char with board number
// ============================================================================

char BoardManager::GetCurrentBoard(void)
{
	return CurrentBoard;
}

// ============================================================================
// Function: SetCurrentBoard
//		Set current board number during play.
//	Parameters:
//		char board = desired board number ranging 0..
// Returns:
//		nothing
// ============================================================================

void BoardManager::SetCurrentBoard(char board)
{
	if (board>=0)
		CurrentBoard=board;
}

