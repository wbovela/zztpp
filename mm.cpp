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

#pragma warn -use

#include <bios.h>
#include <ctype.h>
#include <conio.h>
#include <dos.h>

#include "mm.hpp"
#include "ge.hpp"
#include "ope.hpp"
#include "zconst.hpp"
#include "timerisr.hpp"

// declare this here.
extern GameEngine GE;

//////////////////////////////////////////////////////////////////////////
// Function: DrawMenu
//		Set the active menu and display menu number MenuNumber.
// Parameters:
//		MenuNumber =	MAINMENU (0x01), GAMEMENU (0x02), EDITMENU (0x03),
//							ITEMMENU (0x04), CREAMENU (0x05) or TERRMENU(0x06)
//		Illegal menunumbers result in no change.
// Returns:
//		Nothing.
//////////////////////////////////////////////////////////////////////////

void MenuManager::DrawMenu(char MenuNumber)
{
	// Define some standard colors for easy access.
	char
	C1=m_color(R_BLACK,R_GREY,0,1), // white on black=15
	C2=m_color(R_BLUE,R_BROWN,0,1), // yellow on blue=30
	C3=m_color(R_BLUE,R_GREY,0,1), // white on blue=31
	C4=m_color(R_CYAN,R_BLACK,0,0), // black on cyan=48
	C5=m_color(R_GREY,R_BLACK,0,0); // black on grey=112

	// Tell the MenuManager object that the current menu has changed.
	// Illegal menunumbers result in no change.
	CurrentMenu=(TERRMENU>=MenuNumber>=MAINMENU)?MenuNumber:CurrentMenu;

	// Draw the menu background.
	BM.DrawFilledBox(61, 1, 80, 25, I_SPACE,m_color(R_BLUE, R_GREY, 0, 1));
//	BM.FillArea(61,1,I_SOLID,17,BM.GetChar_Coded(61,1),BM.GetCol(61,1));

	// Draw the menu based on the menunumber given.
	switch (MenuNumber)
	{
		case GAMEMENU:
			{
				BM.TypeString(66,1,"- - - - -\0",C3);
				BM.TypeString(63,2,"     ZZT++      \0",C5);
				BM.TypeString(66,3,"- - - - -\0",C3);
				BM.DrawChar(63,8,I_PLAYER,C3,0);
				BM.TypeString(66,8,"Health :",C2);
				BM.DrawChar(63,9,I_AMMO,m_color(R_BLUE,R_CYAN,0,1),0);
				BM.TypeString(68,9,"Ammo : ",C2);
				BM.DrawChar(63,10,I_TORCH,m_color(R_BLUE,R_BROWN,0,0),0);
				BM.TypeString(65,10,"Torches :",C2);
				BM.DrawChar(63,11,I_GEM,m_color(R_BLUE,R_CYAN,0,1),0);
				BM.TypeString(68,11,"Gems :",C2);
				BM.TypeString(67,12,"Score :",C2);
				BM.TypeString(68,13,"Keys :",C2);

				BM.TypeString(63,15," T \0",C5);
				BM.TypeString(67,15,"Torch\0",C3);
				BM.TypeString(63,16," B \0",C4);
				BM.TypeString(67,16,"Be quiet\0",C3);
				BM.TypeString(63,17," H \0",C5);
				BM.TypeString(67,17,"Help\0",C3);

				BM.DrawChar(68,19,I_SOLID,m_color(R_BLUE,R_CYAN,0,0),0);
				BM.DrawChar(69,19,I_SPINNING_GUN,C4,0);
				BM.DrawChar(70,19,I_DOWN_ARROW,C4,0);
				BM.DrawChar(71,19,I_RIGHT_ARROW,C4,0);
				BM.DrawChar(72,19,I_LEFT_ARROW,C4,0);
				BM.TypeString(74,19,"Move",C3);

				BM.TypeString(62,20," Shift ",C5);
				BM.DrawChar(69,20,I_SPINNING_GUN,C5,0);
				BM.DrawChar(70,20,I_DOWN_ARROW,C5,0);
				BM.DrawChar(71,20,I_RIGHT_ARROW,C5,0);
				BM.DrawChar(72,20,I_LEFT_ARROW,C5,0);
				BM.TypeString(74,20,"Shoot",C3);

				BM.TypeString(63,22," S \0",C5);
				BM.TypeString(67,22,"Save game\0",C3);
				BM.TypeString(63,23," P \0",C4);
				BM.TypeString(67,23,"Pause\0",C3);
				BM.TypeString(63,24," Q \0",C5);
				BM.TypeString(67,24,"Quit\0",C3);

				break;
			}

		case EDITMENU:
			{
				BM.TypeString(66,1,"- - - - -\0",C3);
				BM.TypeString(63,2," ZZT++ Editor  \0",C5);
				BM.TypeString(66,3,"- - - - -\0",C3);
				BM.TypeString(62,5," L \0",C5); BM.TypeString(71,5," H \0",C5);
				BM.TypeString(66,5,"Load\0",C3); BM.TypeString(75,5,"Help\0",C2);
				BM.TypeString(62,6," S \0",C4);  BM.TypeString(71,6," Q \0",C4);
				BM.TypeString(66,6,"Save\0",C3); BM.TypeString(75,6,"Quit\0",C3);
				BM.TypeString(62,8," B \0",C4);
				BM.TypeString(67,8,"Switch boards\0",C3);
				BM.TypeString(62,9," I \0",C5);
				BM.TypeString(67,9,"Board info\0",C3);
				BM.TypeString(62,11,"  f1   \0",C5);
				BM.TypeString(70,11,"Item\0",C3);
				BM.TypeString(62,12,"  f2   \0",C4);
				BM.TypeString(70,12,"Creature\0",C3);
				BM.TypeString(62,13,"  f3   \0",C5);
				BM.TypeString(70,13,"Terrain\0",C3);
				BM.TypeString(62,14,"  f4   \0",C4);
				BM.TypeString(70,14,"Enter text\0",C3);
				BM.TypeString(62,16," Space \0",C5);
				BM.TypeString(70,16,"Plot\0",C3);
				BM.TypeString(62,17,"  Tab  \0",C4);
				BM.TypeString(70,17,"Draw mode\0",C3);
				BM.TypeString(62,19," P \0",C4);
				BM.TypeString(66,19,"Pattern\0",C3);
				BM.TypeString(62,20," C \0",C5);
				BM.TypeString(66,20,"Color:\0",C3);

				// Patterns
				BM.DrawChar(63,23,I_SOLID,C1,0);
				BM.DrawChar(64,23,I_NORMAL,C1,0);
				BM.DrawChar(65,23,I_BREAKABLE,C1,0);
				BM.DrawChar(66,23,I_SPACE,C1,0);
				BM.DrawChar(67,23,I_DOUBLE_CROSS,C1,0);
				BM.DrawChar(68,23,I_SPACE,C1,0);

				// Colors
				BM.DrawChar(71,23,I_SOLID,m_color(R_BLACK,R_BLUE,0,1),0);
				BM.DrawChar(72,23,I_SOLID,m_color(R_BLACK,R_GREEN,0,1),0);
				BM.DrawChar(73,23,I_SOLID,m_color(R_BLACK,R_CYAN,0,1),0);
				BM.DrawChar(74,23,I_SOLID,m_color(R_BLACK,R_RED,0,1),0);
				BM.DrawChar(75,23,I_SOLID,m_color(R_BLACK,R_MAGENTA,0,1),0);
				BM.DrawChar(76,23,I_SOLID,m_color(R_BLACK,R_BROWN,0,1),0);
				BM.DrawChar(77,23,I_SOLID,m_color(R_BLACK,R_GREY,0,1),0);

				BM.TypeString(63,25,"Mode:\0",C3);

				break;
			}

		case ITEMMENU:
			{
				BM.TypeString(66,1,"- - - - -\0",C3);
				BM.TypeString(63,2," ZZT++ Editor  \0",C5);
				BM.TypeString(66,3,"- - - - -\0",C3);
				BM.TypeString(66,5,"Items:\0", C2);
				BM.TypeString(62,6," Z \0",C5);
				BM.TypeString(66,6,"Player\0",C3);
				BM.DrawChar(79,6,I_PLAYER,C3,0);
				BM.TypeString(62,7," A \0",C4);
				BM.TypeString(66,7,"Ammo\0",C3);
				BM.DrawChar(79,7,I_AMMO,m_color(R_BLUE,R_BLUE,0,1),0);
				BM.TypeString(62,8," T \0",C5);
				BM.TypeString(66,8,"Torch\0",C3);
				BM.DrawChar(79,8,I_TORCH,m_color(R_BLUE,R_BROWN,0,0),0);
				BM.TypeString(62,9," G \0",C4);
				BM.TypeString(66,9,"Gem\0",C3);
				BM.DrawChar(79,9,I_GEM,m_color(R_BLUE,CurrentColor,0,1),0);
				BM.TypeString(62,10," K \0",C5);
				BM.TypeString(66,10,"Key\0",C3);
				BM.DrawChar(79,10,I_KEY,m_color(R_BLUE,CurrentColor,0,1),0);
				BM.TypeString(62,11," D \0",C4);
				BM.TypeString(66,11,"Door\0",C3);
				BM.DrawChar(79,11,I_DOOR,m_color(CurrentColor,R_GREY,0,1),0);
				BM.TypeString(62,12," S \0",C5);
				BM.TypeString(66,12,"Scroll\0",C3);
				BM.DrawChar(79,12,I_SCROLL,C3,0);
				BM.TypeString(62,13," P \0",C4);
				BM.TypeString(66,13,"Passage\0",C3);
				BM.DrawChar(79,13,I_PASSAGE,m_color(R_BLUE,CurrentColor,0,1),0);
				BM.TypeString(62,14," U \0",C5);
				BM.TypeString(66,14,"Duplicator\0",C3);
				BM.DrawChar(79,14,I_DUPLICATOR,C3,0);
				BM.TypeString(62,15," B \0",C4);
				BM.TypeString(66,15,"Bomb\0",C3);
				BM.DrawChar(79,15,I_BOMB,m_color(R_BLUE,CurrentColor,0,1),0);
				BM.TypeString(62,16," E \0",C5);
				BM.TypeString(66,16,"Energizer\0",C3);
				BM.DrawChar(79,16,I_ENERGIZER,m_color(R_BLUE,R_MAGENTA,0,0),0);

				BM.TypeString(66,18,"Conveyors:\0",C2);
				BM.TypeString(62,19," 1 \0",C4);
				BM.TypeString(66,19,"Clockwise\0",C3);
				BM.DrawChar(79,19,I_CONVEYOR_C,m_color(R_BLUE,CurrentColor,0,1),0);
				BM.TypeString(62,20," 2 \0",C5);
				BM.TypeString(66,20,"Counter\0",C3);
				BM.DrawChar(79,20,I_CONVEYOR_CC,m_color(R_BLUE,CurrentColor,0,1),0);

				// Patterns
				BM.DrawChar(63,23,I_SOLID,C1,0);
				BM.DrawChar(64,23,I_NORMAL,C1,0);
				BM.DrawChar(65,23,I_BREAKABLE,C1,0);
				BM.DrawChar(66,23,I_SPACE,C1,0);
				BM.DrawChar(67,23,I_DOUBLE_CROSS,C1,0);
				BM.DrawChar(68,23,I_SPACE,C1,0);

				// Colors
				BM.DrawChar(71,23,I_SOLID,m_color(R_BLACK,R_BLUE,0,1),0);
				BM.DrawChar(72,23,I_SOLID,m_color(R_BLACK,R_GREEN,0,1),0);
				BM.DrawChar(73,23,I_SOLID,m_color(R_BLACK,R_CYAN,0,1),0);
				BM.DrawChar(74,23,I_SOLID,m_color(R_BLACK,R_RED,0,1),0);
				BM.DrawChar(75,23,I_SOLID,m_color(R_BLACK,R_MAGENTA,0,1),0);
				BM.DrawChar(76,23,I_SOLID,m_color(R_BLACK,R_BROWN,0,1),0);
				BM.DrawChar(77,23,I_SOLID,m_color(R_BLACK,R_GREY,0,1),0);

				BM.TypeString(63,25,"Mode:\0",C3);

				break;
			}

		case CREAMENU:
			{
				BM.TypeString(66,1,"- - - - -\0",C3);
				BM.TypeString(63,2," ZZT++ Editor  \0",C5);
				BM.TypeString(66,3,"- - - - -\0",C3);
				BM.TypeString(66,5,"Creatures:\0", C2);
				BM.TypeString(62,6," B \0",C5);
				BM.TypeString(66,6,"Bear\0",C3);
				BM.DrawChar(79,6,I_BEAR,m_color(R_BLUE,R_BROWN,0,0),0);
				BM.TypeString(62,7," R \0",C4);
				BM.TypeString(66,7,"Ruffian\0",C3);
				BM.DrawChar(79,7,I_RUFFIAN,m_color(R_BLUE,R_MAGENTA,0,1),0);
				BM.TypeString(62,8," O \0",C5);
				BM.TypeString(66,8,"Object\0",C3);
				BM.DrawChar(79,8,I_PLAYER,m_color(R_BLUE,CurrentColor,0,1),0);
				BM.TypeString(62,9," V \0",C4);
				BM.TypeString(66,9,"Slime\0",C3);
				BM.DrawChar(79,9,I_SLIME,m_color(R_BLUE,CurrentColor,0,1),0);
				BM.TypeString(62,10," Y \0",C5);
				BM.TypeString(66,10,"Shark\0",C3);
				BM.DrawChar(79,10,I_SHARK,m_color(R_BLUE,R_GREY,0,0),0);
				BM.TypeString(62,11," G \0",C4);
				BM.TypeString(66,11,"Spinning gun\0",C3);
				BM.DrawChar(79,11,I_SPINNING_GUN,m_color(R_BLUE,CurrentColor,0,1),0);
				BM.TypeString(62,12," P \0",C5);
				BM.TypeString(66,12,"Pusher\0",C3);
				BM.DrawChar(79,12,I_PUSHER,m_color(R_BLUE,CurrentColor,0,1),0);

				BM.TypeString(66,14,"Beasts:\0",C2);
				BM.TypeString(62,15," L \0",C4);
				BM.TypeString(66,15,"Lion\0",C3);
				BM.DrawChar(79,15,I_LION,m_color(R_BLUE,R_RED,0,1),0);
				BM.TypeString(62,16," T \0",C5);
				BM.TypeString(66,16,"Tiger\0",C3);
				BM.DrawChar(79,16,I_TIGER,m_color(R_BLUE,R_CYAN,0,1),0);

				BM.TypeString(66,18,"Centipedes:\0",C2);
				BM.TypeString(62,19," H \0",C4);
				BM.TypeString(66,19,"Head\0",C3);
				BM.DrawChar(79,19,I_HEAD,m_color(R_BLUE,CurrentColor,0,1),0);
				BM.TypeString(62,20," S \0",C5);
				BM.TypeString(66,20,"Segment\0",C3);
				BM.DrawChar(79,20,I_SEGMENT,m_color(R_BLUE,CurrentColor,0,1),0);

				// Patterns
				BM.DrawChar(63,23,I_SOLID,C1,0);
				BM.DrawChar(64,23,I_NORMAL,C1,0);
				BM.DrawChar(65,23,I_BREAKABLE,C1,0);
				BM.DrawChar(66,23,I_SPACE,C1,0);
				BM.DrawChar(67,23,I_DOUBLE_CROSS,C1,0);
				BM.DrawChar(68,23,I_DOWN_ARROW,C1,0);

				// Colors
				BM.DrawChar(71,23,I_SOLID,m_color(R_BLACK,R_BLUE,0,1),0);
				BM.DrawChar(72,23,I_SOLID,m_color(R_BLACK,R_GREEN,0,1),0);
				BM.DrawChar(73,23,I_SOLID,m_color(R_BLACK,R_CYAN,0,1),0);
				BM.DrawChar(74,23,I_SOLID,m_color(R_BLACK,R_RED,0,1),0);
				BM.DrawChar(75,23,I_SOLID,m_color(R_BLACK,R_MAGENTA,0,1),0);
				BM.DrawChar(76,23,I_SOLID,m_color(R_BLACK,R_BROWN,0,1),0);
				BM.DrawChar(77,23,I_SOLID,m_color(R_BLACK,R_GREY,0,1),0);

				BM.TypeString(63,25,"Mode:\0",C3);

				break;
			}

		case TERRMENU:
			{
				BM.TypeString(66,1,"- - - - -\0",C3);
				BM.TypeString(63,2," ZZT++ Editor  \0",C5);
				BM.TypeString(66,3,"- - - - -\0",C3);
				BM.TypeString(66,5,"Terrains:\0", C2);
				BM.TypeString(62,6," W \0",C5);
				BM.TypeString(66,6,"Water\0",C3);
				BM.DrawChar(79,6,I_WATER,m_color(R_GREY,R_BLUE,1,0),0);
				BM.TypeString(62,7," F \0",C4);
				BM.TypeString(66,7,"Forest\0",C3);
				BM.DrawChar(79,7,I_FOREST,m_color(R_GREEN,R_BLACK,0,1),0);
				BM.TypeString(66,9,"Walls:\0",C2);
				BM.TypeString(62,10," S \0",C5);
				BM.TypeString(66,10,"Solid\0",C3);
				BM.DrawChar(79,10,I_SOLID,m_color(R_BLUE,CurrentColor,0,1),0);
				BM.TypeString(62,11," N \0",C4);
				BM.TypeString(66,11,"Normal\0",C3);
				BM.DrawChar(79,11,I_NORMAL,m_color(R_BLUE,CurrentColor,0,1),0);
				BM.TypeString(62,12," B \0",C5);
				BM.TypeString(66,12,"Breakable\0",C3);
				BM.DrawChar(79,12,I_BREAKABLE,m_color(R_BLUE,CurrentColor,0,1),0);
				BM.TypeString(62,13," O \0",C4);
				BM.TypeString(66,13,"Boulder\0",C3);
				BM.DrawChar(79,13,I_BOULDER,m_color(R_BLUE,CurrentColor,0,1),0);
				BM.TypeString(62,14," 1 \0",C5);
				BM.TypeString(66,14,"Slider NS\0",C3);
				BM.DrawChar(79,14,I_SLIDER_NS,m_color(R_BLUE,CurrentColor,0,1),0);
				BM.TypeString(62,15," 2 \0",C4);
				BM.TypeString(66,15,"Slider EW\0",C3);
				BM.DrawChar(79,15,I_SLIDER_EW,m_color(R_BLUE,CurrentColor,0,1),0);
				BM.TypeString(62,16," A \0",C5);
				BM.TypeString(66,16,"Fake\0",C3);
				BM.DrawChar(79,16,I_FAKE,m_color(R_BLUE,CurrentColor,0,1),0);
				BM.TypeString(62,17," I \0",C5);
				BM.TypeString(66,17,"Invisible\0",C3);
				BM.DrawChar(79,17,I_INVISIBLE,m_color(R_BLUE,CurrentColor,0,1),0);
				BM.TypeString(62,18," L \0",C4);
				BM.TypeString(66,18,"Blink wall\0",C3);
				BM.DrawChar(79,18,I_BLINK_WALL,m_color(R_BLUE,CurrentColor,0,1),0);
				BM.TypeString(62,19," T \0",C5);
				BM.TypeString(66,19,"Transporter\0",C3);
				BM.DrawChar(79,19,I_TRANSPORTER,m_color(R_BLUE,CurrentColor,0,1),0);
				BM.TypeString(62,20," R \0",C4);
				BM.TypeString(66,20,"Ricochet\0",C3);
				BM.DrawChar(79,20,I_RICOCHET,m_color(R_BLUE,R_GREEN,0,1),0);

				// Patterns
				BM.DrawChar(63,23,I_SOLID,C1,0);
				BM.DrawChar(64,23,I_NORMAL,C1,0);
				BM.DrawChar(65,23,I_BREAKABLE,C1,0);
				BM.DrawChar(66,23,I_SPACE,C1,0);
				BM.DrawChar(67,23,I_DOUBLE_CROSS,C1,0);
				BM.DrawChar(68,23,I_SPACE,C1,0);

				// Colors
				BM.DrawChar(71,23,I_SOLID,m_color(R_BLACK,R_BLUE,0,1),0);
				BM.DrawChar(72,23,I_SOLID,m_color(R_BLACK,R_GREEN,0,1),0);
				BM.DrawChar(73,23,I_SOLID,m_color(R_BLACK,R_CYAN,0,1),0);
				BM.DrawChar(74,23,I_SOLID,m_color(R_BLACK,R_RED,0,1),0);
				BM.DrawChar(75,23,I_SOLID,m_color(R_BLACK,R_MAGENTA,0,1),0);
				BM.DrawChar(76,23,I_SOLID,m_color(R_BLACK,R_BROWN,0,1),0);
				BM.DrawChar(77,23,I_SOLID,m_color(R_BLACK,R_GREY,0,1),0);

				BM.TypeString(63,25,"Mode:\0",C3);

				break;
			}

		case MAINMENU:
			{
				BM.TypeString(66,1,"- - - - -\0",C3);
				BM.TypeString(63,2,"     ZZT++      \0",C5);
				BM.TypeString(66,3,"- - - - -\0",C3);
				BM.TypeString(63,6,"Pick a command:\0", 27);
				BM.TypeString(63,8," W \0",C4);
				BM.TypeString(67,8,"World:\0",C2);
				BM.TypeString(63,12," P \0",C5);
				BM.TypeString(67,12,"Play\0",C3);
				BM.TypeString(63,13," R \0",C4);
				BM.TypeString(67,13,"Restore game\0",C2);
				BM.TypeString(63,14," Q \0",C5);
				BM.TypeString(67,14,"Quit\0",C2);
				BM.TypeString(63,17," A \0",C4);
				BM.TypeString(67,17,"About ZZT!\0",C3);
				BM.TypeString(63,18," H \0",C5);
				BM.TypeString(67,18,"High Scores\0",C2);
				BM.TypeString(63,19," E \0",C4);
				BM.TypeString(67,19,"Board Editor\0",C2);
				BM.TypeString(63,22," S \0",C5);
				BM.TypeString(67,22,"Game speed:\0",C2);
				BM.DrawChar(67+GE.GetSpeed(), 23, I_DOWN_TRIAN, m_color(R_BLUE, R_GREY, 0,1),0);
				BM.TypeString(67,24,"F....:....S\0",C2);
				break;
			}

		default: break;
	};
}

//////////////////////////////////////////////////////////////////////////
// Function: MainLoop
//		Run main game loop. This called from main() to start things rolling.
// Parameters:
//		none.
// Returns:
//		Nothing.
//////////////////////////////////////////////////////////////////////////

void MenuManager::MainLoop(void)
{
	char xit=0;			// exit game, false by default.
	char x,y;			// temporary storage for player coordinates
	char currentboard;// current boad number during play
	int c;				// use to store character read from keyboard.

	// Draw main menu
	DrawMenu(MAINMENU);

	// Draw title screen.
	currentboard=0;
	BM.SetCurrentBoard(currentboard);
	BM.DrawBoard(0);

	// Get current board number, make sure it's good
	currentboard=BM.GetCurrentBoard();

	// start the main loop
	while (!xit)
	{
		// Store current player position on current (title screen) board
		x=BM.BRDOBJ[currentboard][0].XPos;
		y=BM.BRDOBJ[currentboard][0].YPos;

		// draw a space over the player
		BM.DrawChar_Direct(x,y,32,m_color(R_BLACK, R_BLACK, 0, 0));

		// Read a character from the keyboard buffer.
		// This method of MM does not wait for user input.
		c=ReadKeyboard();

		// now, interpret the command.
		switch (c)
		{
			// Escape or Quit was pressed.
			case 'q':
			case 'Q':
			case ESC: xit=Quit(); break;

			//	case 'P':
			case 'p': Play();
				// Call Game Engine to start the game.
				// Return for game over and user abort of the game.
				GE.RunGame();

				// when we return, redraw main menu and title screen.
				currentboard=0;
				BM.SetCurrentBoard(0);
				BM.DrawBoard(currentboard);
				DrawMenu(MAINMENU);

				// make sure we don't exit the game yet.
				xit=0;
				break;

			case 'w':
			case 'W': World(); break;

			case 'r':
			case 'R': Restore(); break;

			case 'a':
			case 'A': About(); break;

			//	case 'H':
			case 'h': Help(); break;

			case 'e':
			case 'E': Edit(); break;

			case 's':
			case 'S': Speed(); break;

			default: break;
		}; // end of switch(c)
	}// end of while(!xit)
}

/*
** Function: GetChar
**		Read character from keyboard. Halt program until character pressed.
** Parameters:
**		None.
** Returns:
**		The character's entire scancode (return code in AX after INT16).
*/

int MenuManager::GetChar(void)
{
	union REGS regs;

	regs.x.ax=0x0000;

	// MS Visual C++
	#ifdef _MSC_VER
		_int86(0x16,&regs,&regs);
	#endif

	// Borland C++
	#ifdef __BORLANDC__
		int86(0x16,&regs,&regs);
	#endif

	return regs.x.ax;
}

//=============================================================================
// Function: ReadKeyboard
//		If the keyboard was hit, read the character from the keyboard buffer.
//		Even if it was a special key. Does not block program.
// Parameters:
//		none.
// Returns:
//		The ASCII code of the character pressed.
//		If nothing was pressed, the null character is returned.
//=============================================================================

int MenuManager::ReadKeyboard(void)
{
	// A temporary storage area.
	// it defaults to a NULL character.
	int c='\0';

	// This returns the character that was last pressed and is still on the
	// keyboard buffer.
	c=_bios_keybrd(_KEYBRD_READ);

	// if the lower 8 bits of c are non-zero, return the ascii code read.
	//	if the lower 8 bits are zero, the higher 8 bits contain the special char.
	// this discriminates between ESC (lower 8) and LEFT, RIGHT, UP and DOWN
	// (higher 8).
	if (c & 0xFF)
		return (c & 0xFF);
	else
		return (c >> 8);
}

//=============================================================================
// Function: ReadKeyboardModifiers
//		Returns the status of the keyboard modifiers like SHIFT, CTRL, ALT, etc.
// Parameters:
//		none.
// Returns:
//		An integer of which the lower byte holds the modifier status:
// 	Bit 7	0x80	Insert on
//		Bit 6	0x40	Caps on
//		Bit 5	0x20	Num Lock on
//		Bit 4	0x10	Scroll Lock on
//		Bit 3	0x08	Alt pressed
//		Bit 2	0x04	Ctrl pressed
//		Bit 1	0x02	Left Shift pressed
//		Bit 0	0x01	Right Shift pressed
//	Example:
//		modifiers=ReadKeyboardModifiers();
//		if (mod & 0x04) printf("CTRL");
//=============================================================================

int MenuManager::ReadKeyboardModifiers(void)
{
	// This returns the status of the keyboard modifiers
	return (_bios_keybrd(_KEYBRD_SHIFTSTATUS));
}


//=============================================================================
// MenuManager::InitGame
//
//=============================================================================
int MenuManager::World(void)
{
	return 0;
}

//=============================================================================
// MenuManager::Play
//
//=============================================================================
int MenuManager::Play(void)
{
	return 0;
}

//=============================================================================
// MenuManager::Restore
//
//=============================================================================
int MenuManager::Restore(void)
{
	return 0;
}

//=============================================================================
// MenuManager::Quit
//
//=============================================================================
int MenuManager::Quit(void)
{
	char c='\0';

	BM.TypeString(63, 6, "               \0", m_color(R_BLUE,R_GREY,0,1));
	BM.TypeString(76, 6, "_", m_color(R_BLUE, R_BROWN, 1,1));
	BM.TypeString(64, 6, "Quit ZZT++?\0", m_color(R_BLUE,R_GREY,0,1));

	// check answer
	while ( (c!='y') && (c!='Y') && (c!='n') && (c!='N') )
	{
		// read a key from the keyboard buffer.
		c=GetChar();
	}

	// if yes was typed, pass the exit code to the calling loop.
	if (c=='y' || c=='Y')
		return 1;
	else
	{
		// otherwise, redraw the MAINMENU and return the exit code.
		DrawMenu(MAINMENU);
		return 0;
	}
}


//=============================================================================
// MenuManager::About
//
//=============================================================================
int MenuManager::About(void)
{
	return 0;
}

//=============================================================================
// MenuManager::Help
//
//=============================================================================
int MenuManager::Help(void)
{
	return 0;
}

//=============================================================================
// MenuManager::Edit
//
//=============================================================================
int MenuManager::Edit(void)
{
	// Draw the edit menu
	DrawMenu(EDITMENU);

	//
	// test code for OPE
	//
	ObjectProgramEditor	OPE;
	struct BoardObject Obj;

	OPE.EditObjectProgram(&Obj, "Edit Program\0");
	//
	// end test code for OPE
	//

	BM.DrawBoard(0);

	// Draw the main menu
	DrawMenu(MAINMENU);

	// reset (invisible cursor) to origin.
	gotoxy(1,1);

	return 0;
}

//=============================================================================
// MenuManager::Speed
//
//=============================================================================
int MenuManager::Speed(void)
{
	char xit=0;
	int  c;
	char speed;

	// Get the current game speed;
	speed=GE.GetSpeed();

	// Set the speed indicator arrow.
	// Color the indicator blinking grey on bright blue.
	BM.DrawChar(67+speed, 23, I_DOWN_TRIAN, m_color(R_BLUE, R_GREY, 1,1),0);

	// While Enter wasn't pressed, loop the loop.
	while (!xit)
	{
		// Read the keyboard
		c=ReadKeyboard();

		// And interpret the command.
		switch(c)
		{
			// going left?
			case LEFT:
					// only go left as long as we can.
					if (speed>0)
					{
						// draw a space over the current setting.
						BM.DrawChar(67+speed, 23, I_SPACE, m_color(R_BLUE, R_GREY, 0,1),0);
						// decrease the speed
						speed=GE.SetSpeed(--speed);
						// and draw a new triangle.
						BM.DrawChar(67+speed, 23, I_DOWN_TRIAN, m_color(R_BLUE, R_GREY, 1,1),0);
					}
					break;

			case RIGHT:
					if (speed<10)
					{
						BM.DrawChar(67+speed, 23, I_SPACE, m_color(R_BLUE, R_GREY, 0,1),0);
						speed=GE.SetSpeed(++speed);
						BM.DrawChar(67+speed, 23, I_DOWN_TRIAN, m_color(R_BLUE, R_GREY, 1,1),0);
					}
					break;

			case ENTER: xit=1; break;
		}
	}

	// now give the indicator back its original color.
	BM.DrawChar(67+speed, 23, I_DOWN_TRIAN, m_color(R_BLUE, R_GREY, 0,1),0);

	return 0;
}





