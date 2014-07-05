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

//Timer ISR prototypes.

#ifndef _TIMER_H
#define _TIMER_H

#include "bm.hpp"

void InstallISR(void);
void UninstallISR(void);

#define TIMER_INT		0x08		// this int is called by the PIT
#define PIT_FREQ		0x1234dd // this is the speed of the PIT clock
#define STD_COUNTER	0x10000	// usually the counter is at 0x10000
#define GAME_FREQ		500		// set the update frequency of the game

// BoardManager BM is declared in zzt.cpp main file as a global variable.
// Since the timerisr routine sends messages to BM, we need to know about it here.
extern BoardManager BM;

// this is a global variable which can be accessed directly by all of zzt++.
// i'm doing it this way since this is very time critical. i'm making it
// volatile to tell the compiler that the value can be changed by a background
// process and the real value will always be taken instead of taking advantage
// of the fact that CycleCount is stored in some register.
extern volatile unsigned int CycleCount;

#endif




