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

#ifndef _GE_H
#define _GE_H

#include "bm.hpp"

// declare global BM here
extern BoardManager BM;

class GameEngine
{
	private:

		// holds number of the current board.
		char GameSpeed;

		// menu options for game menu.
		int Quit(void);

	public:

		GameEngine(void)
		{
			// set title screen and game speed when GE is made.
			// even before a game is started we would like to know the game
			// speed.
			InitGame();
		}

		void InitGame(void);
		void RunGame(void);

		char SetSpeed(char speed)
		{
			if (speed>0 && speed<10)
				GameSpeed=speed;

			return GameSpeed;
		}

		char GetSpeed()
		{
			return GameSpeed;
		}

	protected:
};

//---------------------------------------------------------
//
// Base class for all 'things' in ZZT++
//
class Object
{
	private:

	public:
		Object(void)
		{}

	protected:
};

class Player : public Object
{
	private:

	public:
		Player(void)
		{}

	protected:
};

class Enemy : public Object
{
	private:
		BoardObject **Info;
		char ***Program;

	public:
		Enemy(void)
		{}

	protected:
};

class Puzzle : public Enemy
{
	private:

	public:
		Puzzle(void)
		{}

	protected:
};

class Monster : public Enemy
{
	private:

	public:
		Monster(void)
		{}

	protected:
};

class Pusher : public Puzzle
{
	private:

	public:
		Pusher(void)
		{}

	protected:
};

class Ruffian : public Monster
{
	private:

	public:
		Ruffian(void)
		{}

	protected:
};

#endif



