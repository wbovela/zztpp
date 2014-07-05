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

#include <dos.h>
#include <stdlib.h>

#include "sm.hpp"
#include "timerisr.hpp"

extern SoundManager SM;

// These variables are global variables.
// GlobalLocked is used by NewISR() to prevent re-entrancy.
// GlobalTickCount is used by NewISR() to count interrupts.

// Borland C++
#ifdef __BORLANDC__
void interrupt NewISR(...);
void interrupt (*OldISR)(...);
#endif

// MS Visual C++
#ifdef _MSC_VER
void interrupt far NewISR(void);
void (interrupt far *OldISR)(void);
#endif

char GlobalLocked; 					// re-entrancy lock variable
long GlobalTickCount=0;				// Keeps track of clock ticks for INT 8
int GlobalTicker=0;					// For testing purposes only!

// this is a global variable which can be accessed directly by all of zzt++.
// i'm doing it this way since this is very time critical. i'm making it
// volatile to tell the compiler that the value can be changed by a background
// process and the real value will always be taken instead of taking advantage
// of the fact that CycleCount is stored in some register.
volatile unsigned int CycleCount;

// calculate the value of the counter needed for the required game
// update frequency. e.g.: we want the game to be updated 500 times
// per second. the PIT timer runs at a fixed 0x1234dd ticks per second.
// we need to set the counter to 0x1234dd/500=0x952 so int8 will be
// generated 500 times per second.
int counter=0x952;

//==========================================================================
// Function: InstallISR
//		Install a new Interrupt Service Routine for int 8.
//		The 8254 PIT chip's frequency is NOT altered!
// Parameters:
//		None.
// Returns:
//		Nothing.
//==========================================================================

void InstallISR(void)
{
	// Borland C++
	#ifdef __BORLANDC__
	// Get the original interrupt vector for INT1c, which is called.
	// by the INT8 timer interrupt.
	OldISR=getvect(TIMER_INT);

	// Disable interrupts.
	disable();

	// Program the PIT.
	// Use counter 0, write frequency's LSB then MSB, counter mode 2.
	// NOTE: counter is a global variable in this file.
	outportb(0x043,0x034);
	outportb(0x040,counter%256);	// counter's LSB
	outportb(0x040,counter/256); 	// counter's MSB

	// Set the vector to our routine.
	setvect(TIMER_INT,NewISR);

	// Enable interrupts.
	enable();
	#endif

	// MS Visual C++
	#ifdef _MSC_VER
	OldISR=_dos_getvect(TIMER_INT);
	_disable();

	// 20-feb-2000: not tested under msvc++ yet!
	// Program the PIT.
	// Use counter 0, write frequency's LSB then MSB, counter mode 2.
	outportb(0x043,0x034);
	outportb(0x040,counter%256);	// counter's LSB
	outportb(0x040,counter/256); 	// counter's MSB

	_dos_setvect(TIMER_INT,NewISR);
	_enable();
	#endif
}

/*
** Function: UninstallISR
**		Set the Interrupt Service Routine for int 8 to the one we saved.
** Parameters:
**		None.
** Returns:
**		Nothing.
*/

void UninstallISR(void)
{
	// Borland C++
	#ifdef __BORLANDC__

	// Disable interrupts.
	disable();

	// Reset 8254 PIT counter 0 bij writing LSB, then MSB.
	outportb(0x043,0x034);
	outportb(0x040,0x000);
	outportb(0x040,0x000);

	// Set vector to original vector.
	setvect(TIMER_INT,OldISR);

	// Enable interrupts.
	enable();
	#endif

	// MS Visual C++
	#ifdef _MSC_VER
	_disable();

	// 20-feb-2000: not tested under msvc++ yet!
	// Reset 8254 PIT counter 0 bij writing LSB, then MSB.
	outportb(0x043,0x034);
	outportb(0x040,0x000);
	outportb(0x040,0x000);

	_dos_setvect(TIMER_INT,OldISR);
	_enable();
	#endif
}

/*
** Function: NewISR
**		The PIT chip runs at a fixed frequency of 0x1234dd Hz. To this chip
**		some counters are attached which count down. When they reach zero, an
**		interrupt 8 is generated. By adjusting the counter startoff value
**		we can influence the frequency with which interrupt 8 is called.
**		However, in that case we must not forget to call the original interrupt
**		8 interrupt service route which handles the system clock among other
**		things.
**		In this routine we do not alter the counter value but use the fact that
**		by default the counter is called about 18.2 times a second, which is the
**		original setting. The PIT frequency is 0x1234dd and the counter is
**		originally set at 0x10000. Therefore interrupt 8 is called every
**		0x1234dd / 0x10000 times giving about 18.2 Hz.
**		By taking over the interrupt service routine and making sure we call the
**		original ISR every time, we can time our own calls by counting the
**		number of times our ISR is called. If we want our routine to be called
**		4 or 5 times a second, we just wait for 4 calls to pass before we
**		call our own routine.
**
**		The GlobalLocked variable keeps us from re-entering the code before
**		the last thread was finished.
**		Note that in a multi-tasking environment, re-entrancy cannot be prevented
**		this way. Semaphores must be used instead.
**		oldint8() is always called to handle the PIC chip and the IRET so we
**		don't have to do that.
**		Implementing the ISR this way means the clock will run at it's normal
**		speed. If our code takes longer than the period between two clock ticks,
**		the oldint8() code won't be called, slowing the clock down a bit.
** Parameters:
**		None.
** Returns:
**		Nothing.
*/

// turn off warnings about Constant is long
#pragma warn -cln

// Borland C++
#ifdef __BORLANDC__
void interrupt NewISR(...)
#endif

// MS Visual C++
#ifdef _MSC_VER
void interrupt far NewISR(void)
#endif
{
	// Increase our global counter.
	CycleCount++;

	// Make sure SM gets time to play music
	SM.PlayTune();

	// Increase our counter to see if the original INT8 needs
	// to be called already.
	GlobalTickCount+=counter;

	// Check to see if we're there already.
	if (GlobalTickCount>=STD_COUNTER)
	{
		// Decrease the global counter
		GlobalTickCount-=STD_COUNTER;

		// and call the original int8 ISR
		OldISR();
	}
	else
		// otherwise, acknowledge this interrupt
		outportb(0x20,0x20);
}

#pragma warn .cln

