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

// Game Engine code comes here.

#pragma warn -use

#include "ge.hpp"
#include "bm.hpp"
#include "mm.hpp"
#include "zconst.hpp"
#include "timerisr.hpp"
#include <bios.h>

// Declared globally in zzt.cpp and used here.
extern BoardManager BM;
extern MenuManager MM;

//=============================================================================
// Function: InitGame
//		Is run when a new game is started by player. Sets current board to title
//		screen and sets game speed to 5. Draws nothing to screen.
//	Parameters:
//		none.
//	Returns:
//		nothing.
//=============================================================================
void GameEngine::InitGame(void)
{
	// set curent board to title screen.
	BM.SetCurrentBoard(0);

	// set initial game speed
	SetSpeed(5);
}

//=============================================================================
// Function: RunGame
//		gets control from the main program and runs the main zzt++ loop.
// Parameters:
//		none.
// Returns:
//    nothing
//=============================================================================
void GameEngine::RunGame(void)
{

	char xit=0;			// exit game, false by default.
	char shift='\0'; 	// shift pressed, false by default.
	char x,y;			// temporary storage for player coordinates
	char currentboard;// current boad number during play
	int c;				// used to store character read from keyboard.
	int mod;				// used to store keyboard modifier status.

	// We're playing a new game, so we go to the first board.
	BM.SetCurrentBoard(1);
	currentboard=BM.GetCurrentBoard();

	// Draw the current board. The player gets drawn automatically.
	BM.DrawBoard(BM.GetCurrentBoard());

	// Get the modifiers from the keyboard
	mod=MM.ReadKeyboardModifiers();
	// check whether left or right shift was pressed
	shift=((mod & KMOD_LFT) || (mod & KMOD_RGT));

	// Set initial game speed.
	SetSpeed(5);

	// Draw game menu
	MM.DrawMenu(GAMEMENU);

	// start the main loop
	while (!xit)
	{
		// Set counter at 0 at beginning of loop.
		// this is declared in timerisr.hpp as global.
		CycleCount=0;

		// Store current player position on current (title screen) board
		x=BM.BRDOBJ[currentboard][0].XPos;
		y=BM.BRDOBJ[currentboard][0].YPos;

		// Read a character from the keyboard buffer.
		// This method of MM does not wait for user input.
		c=MM.ReadKeyboard();

		// Get the modifiers from the keyboard
		mod=MM.ReadKeyboardModifiers();
		// check whether left or right shift was pressed
		shift=((mod & KMOD_LFT) || (mod & KMOD_RGT));

		// now, interpret the command.
		switch (c)
		{

			// Escape or Quit was pressed.
			case 'q':
			case 'Q':
			case ESC: xit=Quit(); break;

			// Player movement
			// Player goes up
			case UP:
				if (shift)
				{
					// FIRE UP
				}
				else
				{
					 y-=BM.Try(x,y-1)?1:0; BM.MovePlayer(x,y);
				}
				break;

			// Player goes down
			case DOWN:
				if (shift)
				{
					// FIRE DOWN
				}
				else
				{
					 y+=BM.Try(x,y+1)?1:0; BM.MovePlayer(x,y); break;
				}
				break;

			// Player goes left
			case LEFT:
				if (shift)
				{
					// FIRE LEFT
				}
				else
				{
					 x-=BM.Try(x-1,y)?1:0; BM.MovePlayer(x,y); break;
				}
				break;

			// Player goes right
			case RIGHT:
				if (shift)
				{
					// FIRE RIGHT
				}
				else
				{
					 x+=BM.Try(x+1,y)?1:0; BM.MovePlayer(x,y); break;
				}
				break;

			default: break;
		}; // end of switch(c)

		// wait if we were too fast.
		while (CycleCount < (GetSpeed()-1)*8) {/* do nothing here */};

	}// end of while(!xit)
}


//=============================================================================
// MenuManager::Quit
//
//=============================================================================
int GameEngine::Quit(void)
{
	char c='\0';

	BM.TypeString(63, 6, "               \0", m_color(R_BLUE,R_GREY,0,1));
	BM.TypeString(79, 6, "_", m_color(R_BLUE, R_BROWN, 1,1));
	BM.TypeString(64, 6, "End this game?\0", m_color(R_BLUE,R_GREY,0,1));

	// check answer
	while ( (c!='y') && (c!='Y') && (c!='n') && (c!='N') )
	{
		// read a key from the keyboard buffer.
		c=MM.GetChar();
	}

	// if yes was typed, pass the exit code to the calling loop.
	if (c=='y' || c=='Y')
		return 1;
	else
	{
		// otherwise, redraw the GAMNEMENU and return the exit code.
		MM.DrawMenu(GAMEMENU);
		return 0;
	}
}


