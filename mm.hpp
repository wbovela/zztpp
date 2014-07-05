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

// Menu Manager header file

#ifndef _MM_H
#define _MM_H

// BoardManager BM is declared in zzt.cpp main file as a global variable.
// Since MenuManager sends messages to BM, we need to know about it here.
#include "bm.hpp"
#include "zconst.hpp"

extern BoardManager BM;

class MenuManager
{
	private:
	char CurrentColor;
	char CurrentMenu;

	int World(void);
	int Play(void);
	int Restore(void);
	int Quit(void);
	int About(void);
	int Help(void);
	int Edit(void);
	int Speed(void);

	public:
	void DrawMenu(char MenuNumber);
	void MainLoop(void);
	int GetChar(void);
	int ReadKeyboard(void);
	int ReadKeyboardModifiers(void);	

	// Constructor
	MenuManager(void)
	{
		// set current color default to yellow (=bright brown).
		CurrentColor=R_BROWN;

		// set current menu to the main menu
		CurrentMenu=MAINMENU;
	}

	// Destructor
	~MenuManager(void)
	{
	}
};

#endif



