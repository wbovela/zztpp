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

#ifndef _BM_H
#define _BM_H

#include <conio.h>

// MS Visual C++
#ifdef _MSC_VER
#include <graph.h>
#endif

#include <dos.h> // MK_FP macro
#include <malloc.h> // malloc()
#include <stdlib.h> // itoa()
#include <stdio.h>

// MS Visual C++
#ifdef _MSC_VER
#include <memory.h>
#endif

// Borland C++
#ifdef __BORLANDC__
#include <mem.h>
#endif

#include <string.h>
#include <ctype.h>

// this struct is the same for ZZT and SAV files
struct GameInfo
{
	int dummy1; // FFFF
	int NoBoards;
	int NoAmmo;
	int NoGems;
	unsigned char BlueKey;
	unsigned char GreenKey;
	unsigned char CyanKey;
	unsigned char RedKey;
	unsigned char PurpleKey;
	unsigned char YellowKey;
	unsigned char WhiteKey;
	int Health;
	int StartingBoard;//starting board no!!
	int NoTorches;
	int NoTorchCycles;
	int NoEnergizerCycles;
	int dummy3; // 0000
	int Score;
	unsigned char TitleLength;
	char Title[20]; // 20 positions
	unsigned char Flag1Length;
	char Flag1Name[20]; // 20 positions
	unsigned char Flag2Length;
	char Flag2Name[20]; // 20 positions
	unsigned char Flag3Length;
	char Flag3Name[20]; // 20 positions
	unsigned char Flag4Length;
	char Flag4Name[20]; // 20 positions
	unsigned char Flag5Length;
	char Flag5Name[20]; // 20 positions
	unsigned char Flag6Length;
	char Flag6Name[20]; // 20 positions
	unsigned char Flag7Length;
	char Flag7Name[20]; // 20 positions
	unsigned char Flag8Length;
	char Flag8Name[20]; // 20 positions
	unsigned char Flag9Length;
	char Flag9Name[20]; // 20 positions
	unsigned char Flag10Length;
	char Flag10Name[20]; // 20 positions
	int TimeElapsed; // 0=nolimit, else=seconds elapsed
};

struct Board
{
	unsigned int BoardSize;
	unsigned char TitleLength;
	char Title[34]; // Max length is 34 bytes
	unsigned char dummy[16]; // 16 bytes of mysterious functionality
	// Usually 06 0e 00 90 00 00 AF 4C 04 0F 02 12 B4 00 00 55

	// Blocks follow here. The number of blocks is to be determined
	// dynamically (i.e. until sum(Block.NoChars)==1500)
};

// Screens are compressed by replacing blocks of repeating characters by
// structs like these. Single characters are also stored this way. Since
// boards usually contain repeating consecutive sections, this way of
// storing screens will result in saving memory space.
struct Block
{
	// A screen consists of a maximum of 1500 (0x5dc) characters.
	// The sum of NoChars should be 1500.
	unsigned char NoChars;
	unsigned char CharCode;
	unsigned char ColCode;
};

// After the Board struct and the screen information, the rest of
// the board is described by this boardinfo struct. It's a prelude
// to the objects of the board.
struct BoardInfo
{
	unsigned char MaxShots;
	unsigned char DarkBoard;
	unsigned char BoardNorth;
	unsigned char BoardSouth;
	unsigned char BoardWest;
	unsigned char BoardEast;
	unsigned char ReEnter;
	unsigned char MessageLength;
	char Message[58];
	int dummy1; // 0x71b
	unsigned int TimeLimit;
	char dummy2[16]; // zeroes
	unsigned int NoObjects;

	// Board objects follow here. During runtime each object will be
	// represented by an instance of (a subclass of) the class Object.
};

// This struct describes the objects on the board. The first object
// is always the player object. So if BoardInfo.NoObjects equals 0
// it means that there is only 1 object, namely the player object.
// ATTN: ZZT places an object in the file for every creature. The term
// object used here is not a ZZT-OOP object. Depending on the code in
// the screen section we can tell whether the object is a ZZT-OOP object
// or some other creature.
struct BoardObject
{
	unsigned char XPos;
	unsigned char YPos;
	int XMovementStep;
	int YMovementStep;
	unsigned int CycleSpeed;
	unsigned char Magic1; // Too much functionality to fit into symbol
	unsigned char Magic2; // description.
	unsigned char DestinationBoardNo; // Ony for passages, otherwise 00
	int dummy1; // always FF FF
	int dummy2; // always FF FF
	unsigned char CharCodeUnder;
	unsigned char ColCodeUnder;
	char *ProgramTextPtr; // Pointer to programs in memory ???
	unsigned int ProgramIP; // Instruction Pointer
	// 10 bytes from here!
	// we use a signed integer because program length ff ff has
	// a special meaning when object use a #bind command.
	int ProgramLength;
	int d1, d2, d3, d4;
};

// This structure is an element in queues used for slime handling and
// the fillarea() routine. The queues will be dynamic linked lists.
struct xypos
{
	xypos *next;
	unsigned char x; // horizontal position (0-59)
	unsigned char y; // vertical position (0-24)
};

// This is the BM class holding the data and messages for the Board Manager
class BoardManager
{
	private:

	// A pointer to the video memory
	unsigned char far *vidbase;

	// a pointer to where the original board information is stored
	unsigned char far *vidbase_page2;

	// Queue space for FillArea and Slime handling.
	// Memory is requested during run time.
	// This Queue is implemented as a dynamic linked list.
	xypos *FillQHead;
	xypos *FillQTail;
	int FillNoElements, MaxElms;

	// Current board during game play
	char CurrentBoard;

	// Some private functions for printing information about the boards.
	void PrintGameInfo(FILE *fout);
	void PrintBoardInfo(int boardcounter, FILE *fout);
	void PrintBoard(int boardcounter, FILE *fout);
	void PrintObjectInfo(int boardcounter, int objectcounter, FILE *fout);

	// Internal stack manipulation functions.
	int FillRemove(xypos *position);
	void FillAdd(xypos position);
	unsigned char FillQIsEmpty(void);

	public:
	// These are public to allow easy access by GE

	// One struct for GameInfo
	GameInfo GI;

	// An array of Boards
	// Example: BRD[0].BoardSize
	Board *BRD;

	// An array of blocks per board.
	// Example: BLCK[0][0].NoChars is block# 0 of board# 0
	Block **BLCK;

	// An array of BoardInfo structs
	// Example: BRDINF[0] is the info for board 0.
	BoardInfo *BRDINF;

	// An array of objects per board.
	// Example: BRDOBJ[0][1] is the second object for board# 0
	BoardObject **BRDOBJ;

	// An array of Program texts per object.
	char ***OBJPRG;

	// Saved game indicator at position 264
	unsigned char GameSaved;

	int DrawChar(int x, int y, int c, int col, int board);
	int DrawChar_Direct(int x, int y, int c, int col);
	int GetChar(int x, int y);
	int GetChar_Coded(int x, int y);
	int GetCol(int x, int y);
	int DrawBoundBox(int UpLeftx, int UpLefty, int BotRightx, int BotRighty, int c, int col);
	int DrawFilledBox(int UpLeftx, int UpLefty, int BotRightx, int BotRighty, int c, int col);
	void FillArea(char x, char y, char c, char col, char FillOverChar, char FillOverCol);
	int TypeString(int x, int y, char *s, int col);
	int TypeString(int x, int y, int i, int col);
	void AsciiChart(void);
	void ColorList(void);
	void Fade(void);
	char Try(char x, char y);
	void MovePlayer(char x, char y);

	// Board functions
	int ReadWorld(char *file, int report, int screen);
	int WriteWorld(char *file);
	void DrawBoard(int board);
	void Screen2Board(int board);
	char GetCurrentBoard(void);
	void SetCurrentBoard(char board);

	// Physical screen functions
	void SetScan350(void);
	void SetColorMode(void);
	void SetMonoMode(void);
	void EnableCursorBig(void);
	void EnableCursorSmall(void);
	void DisableCursor(void);
	void Cls(void);

	// Constructor
	BoardManager(void)
	{
		// Point to video memory.
		// We assume a color monitor right at the start.
		// MS Visual C++
		#ifdef _MSC_VER
			// video memory
			vidbase = (unsigned char*)(_MK_FP(0xb000, 0x8000));

			// page 2, used for storing zzt coded boards
			vidbase_page2 = (unsigned char*)(_MK_FP(0xb000, 0x9000));
		#endif

		// Borland C++
		#ifdef __BORLANDC__
			// video memory
			vidbase = (unsigned char*)(MK_FP(0xb000, 0x8000));

			// page 2, used for storing zzt coded boards
			vidbase_page2 = (unsigned char*)(MK_FP(0xb000, 0x9000));
		#endif

		// Initialize Fill Stack.
		FillQHead=FillQTail=NULL;
		FillNoElements=MaxElms=0;
	}

	// Destructor
	~BoardManager(void)
	{
		// Free used memory at destruction of BM
		free(BRD);
		free(BRDINF);
		free(BLCK);
		free(BRDOBJ);
		free(OBJPRG);
	}
};

#endif


