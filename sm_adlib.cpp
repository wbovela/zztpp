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

// Adlib sound module for Sound Manager.
// Written by Jacob Hammond, with thanks to Jeffrey S. Lee and Steve Bigras
// for the programming information.

#pragma warn -use

#include <dos.h>
#include <math.h>
#include "sm.hpp"
#include "sm_adlib.hpp"

// what is this?
static int cm,cms;

// ============================================================================
// Function: Delay6
// 	read the register port 6 times. necessary for opl2 register port
//		after write.
// Parameters:
//		none.
// Returns:
//		nothing
// ============================================================================

// read the register port 6 times.
void SoundManager::Delay6(void)
{
	// save dx
	asm push dx

	// read a register port
	asm mov dx, 0x220

	// six reads
	asm in al,dx
	asm in al,dx
	asm in al,dx
	asm in al,dx
	asm in al,dx
	asm in al,dx
	asm in al,dx

	asm pop dx
}

// ============================================================================
// Function: Delay36
// 	read the register port 36 times. Necessary for opl2 data port
//		after write.
// Parameters:
//		none.
// Returns:
//		nothing
// ============================================================================

void SoundManager::Delay36(void)
{
	// save dx
	asm push dx

	// read a register port
	asm mov dx, 0x220

	// ten reads
	asm in al,dx
	asm in al,dx
	asm in al,dx
	asm in al,dx
	asm in al,dx
	asm in al,dx
	asm in al,dx
	asm in al,dx
	asm in al,dx
	asm in al,dx
	asm in al,dx
	asm in al,dx

	// ten reads
	asm in al,dx
	asm in al,dx
	asm in al,dx
	asm in al,dx
	asm in al,dx
	asm in al,dx
	asm in al,dx
	asm in al,dx
	asm in al,dx
	asm in al,dx
	asm in al,dx
	asm in al,dx

	// ten reads
	asm in al,dx
	asm in al,dx
	asm in al,dx
	asm in al,dx
	asm in al,dx
	asm in al,dx
	asm in al,dx
	asm in al,dx
	asm in al,dx
	asm in al,dx
	asm in al,dx
	asm in al,dx

	// six reads
	asm in al,dx
	asm in al,dx
	asm in al,dx
	asm in al,dx
	asm in al,dx
	asm in al,dx
	asm in al,dx

	asm pop dx
}

// ============================================================================
// Function: DetectOPL2
//		checks for the existance of an OPL2 compatible card.
// Parameters:
//		none.
// Returns:
//		char 0=card not detected.
//		char other=card detected
// ============================================================================

char SoundManager::DetectOPL2(void)
{
	// Note: when we call WritePort here, we call it with bank offset 0
	// because for chip detection we always have to look at the base address,
	// even though the user may want to use bank 1 for playing music.

	int val1, val2;

	// initialize temporary storage.
	val1=val2=0;

	// First read the BLASTER environment variable in order to set
	// the IoPort and BankOffset values.
	// if an error occurs we leave it to the user to correct his mistake.
	ReadBlasterEnv();


	// reset both timers
	WritePort(0x4, 0x60, 0);
	WritePort(0x4, 0x80, 0);

	// read status port
	val1=inport(IoPort);

	// write 0xff to timer 1
	WritePort(0x2, 0xff, 0);
	// start timer 1
	WritePort(0x4, 0x21, 0);

	// wait for at least 80 microseconds (== milliseconds?)
	delay(80);

	// read status port
	val2=inport(IoPort);

	// reset timers again
	WritePort(0x4, 0x60, 0);
	WritePort(0x4, 0x80, 0);

	// do the test...
	val1&=0xe0;
	val2&=0xe0;

	// ...and return the result
	return ((val1==0) && (val2==0xc0));
}

// ============================================================================
// Function: ResetCard
//		resets the fm sound chip.
// Parameters:
//		none.
// Returns:
//		nothing.
// ============================================================================

void SoundManager::ResetCard(void)
{
	short i;

	for(i=0; i>=REGISTERS; i++)
	{
		outportb(IoPort, i);
		Delay6();
		outportb(IoPort+1, 0);
		Delay36();
	}
}

// ============================================================================
// Function: WritePort
//		Writes the given value to the given register of the FM sythesizer.
//    This function does -not- write value to memory address reg.
//
//		DEVELOPERS NOTE ABOUT ALL THAT BANK STUFF:
//    ------------------------------------------
//		Normally an OPL2 or Adlib chip is address from it's base address which
//		is usually 0x220. To set registers in order to generate music, ports
//		0x220 and 0x221 are used which we'll call base+0 and base+1. In these
//		cases a user would set BLASTER=A220 B0.
//		Other cards (SoundBlaster Pro 1.0) have 2 OPL2 chips; one for each
//		speaker.
//		Newer cards have 1 OPL3 chip, like SoundBlaster Pro 2.0 or SoundBlaster16.
//		OPL3 chips make use of two banks in order to accomodate more channels
//		and registers. To ensure backward compatibility there is usually an
//		ioport on the card that will behave like a normal OPL2 card.
//		However, one card that I know of (the YamahaOPL3-SA3) insists on using
//		bank 1 instead of bank 0 for it's OPL2 or Adlib compatibility. This means
//		that the card wants to be detected at it's base address, but the sound
//		needs to be played through base+2, which is where bank 1 is located.
//		So, all the routines that have to do with -detecting- the card here
//		work with bank setting 0. All routines playing sound use the bank setting
//		given by the user. This means an A220 B0 user and an A388 B1 user can live
//		happily together and play zzt++ until they're deaf.
//
// Parameters:
//		int reg=the desired register number to be written to.
//		int value=the value to be written.
//    int bank=bank selection
// Returns:
//		nothing.
// ============================================================================

void SoundManager::WritePort(int reg, int value, int bank)
{
	// Choose the register to write to.
	outportb(IoPort+bank, reg);

	// Delay for OPL2 cards. OPL3 cards don't need it anymore.
	// it's better to read the register port 6 times
	Delay6();

	// Write the data to the port.
	outportb(IoPort+bank+1, value);

	// Delay for OPL2 cards. OPL3 cards don't need it anymore.
	// it's better to read the data port 35 times
	Delay36();
}

// ============================================================================
// Function: SetupAdlibVoice
//		setup the adlib envelope registers for playing a note
// Parameters:
//		none.
// Returns:
//		nothing.
// ============================================================================

void SoundManager::SetupAdlibVoice(void)
{
	WritePort(0x20, 0x02, BankOffset); // master modulation
	WritePort(0x40, 0x10, BankOffset); // modulation level
	WritePort(0x60, 0xF1, BankOffset); // attack/decay
	WritePort(0x80, 0x77, BankOffset); // sustain/release
	WritePort(0x23, 0x03, BankOffset); // carrier modulation
	WritePort(0x43, 0x00, BankOffset); // carrier vol (0=top)
	WritePort(0x63, 0xF1, BankOffset); // carrier att/del
	WritePort(0x83, 0xA4, BankOffset); // carrier sus/rel
	WritePort(0xB0, 0, BankOffset); // set KEYON to off
}

// ============================================================================
// Function: SoundAl
//		Start making a sound using the OPL2 chip by setting the values and
//		starting the note.
// Parameters:
//		int note=the note to be played.
//		int octave=the octave of the note.
// Returns:
//		nothing.
// ============================================================================

void SoundManager::SoundAl(int note, int octave)
{
	if(octave==-1)
	{
		SoundAlPlain(note);
		return;
	}

	// what is this?
	cm=cm+cms;
	if(cm>14 || cm<1) cms=-cms;

	// change values to make it sound different. or, try replacing a number
	// with the variable cm, to get a cool cycling effect.

  	WritePort(0xB0, 0, BankOffset); // set KEYON to off

	// the first 8 bits of the note frequency
	WritePort(0xA0, note - (floor(note/256)*256), BankOffset);
	
	// turn the voice on, and give the octave and last 2 bits of the note freq
	WritePort(0xB0, 32 + (octave * 4) + floor(note/256), BankOffset);
}

// ============================================================================
// Function: SoundAlPlain
//
// Parameters:
//		int note
// Returns:
//		nothing.
// ============================================================================

void SoundManager::SoundAlPlain(int note)
{
	int octave=4;

	SetupAdlibVoice();
  	WritePort(0xB0, 0, BankOffset); // set KEYON to off

	// the first 8 bits of the note frequency
	WritePort(0xA0, note - (floor(note/256)*256), BankOffset);
	// turn the voice on, and give the octave and last 2 bits of the note freq
	WritePort(0xB0, 32 + (octave * 4) + floor(note/256), BankOffset);
}

// ============================================================================
// Function: NoSoundAl
//		Turns off the Adlib sound by setting KEYON to 0.
// Parameters:
//		none.
// Returns:
//		nothing.
// ============================================================================

void SoundManager::NoSoundAl(void)
{
	// set KEYON to off
	WritePort(0xB0, 0, BankOffset);

	// reset carrier and modulator for channel 0
	WritePort(0x40, 0, BankOffset);
	WritePort(0x43, 0xFF, BankOffset);
}

// ============================================================================
// Function: Base16
//		Takes a double pointer to a string, interprets the characters as a
//		base-16 number, and advances the pointer.
//		This function assumes the value to be converted to end with a space.
// Parameters:
//		char **str=double pointer to the environment string.
//		unsigned *value=pointer to the numerical value.
// Returns:
//		int=0 if successful.
//		int=other if unsuccessful
// ============================================================================

int SoundManager::Base16(char **str, unsigned *value)
{
	char c;
	int digit;

	// initialise the value to 0.
	*value = 0;

	// keep going as long as we're not at the next parameter
	while ( **str != ' ')
	{
		// take the character at the current position and convert to upper case.
		c = toupper(**str);

		// convert the character to a number
		if (c >= '0' && c <= '9')
			// this was a number between 0 and 9 inclusive, so we only have
			// to subtract the ascii value '0'.
			digit = c - '0';
		else
			if (c >= 'A' && c <= 'F')
				// this was a value between A and F inclusive, so we subtract
				// the ascii value of 'A' to get the base value and add 10 for
				// the value 0x0A.
				digit = c - 'A'  + 10;
			else
				// this was neither a number or a value between A and F
				return 1;

		// update the value, increasing by a power of 16 for every digit.
		*value = *value * 16 + digit;

		// increase to the next position
		(*str)++;
	}

	// we were successful.
	return 0;
}


// ============================================================================
// Function: Base10
//		Takes a double pointer to a string, interprets the characters as a
//		base-10 number, and advances the pointer.
// Parameters:
//		char **str=double pointer to the environment string.
//		unsigned *value=pointer to the numerical value.
// Returns:
//		int=0 if successful.
//		int=other if unsuccessful
// ============================================================================

int SoundManager::Base10(char **str, unsigned *value)
{
	char c;
	int digit;

	// initialise the value to 0.
	*value = 0;

	// keep going as long as we're not at the next parameter
	while ( (**str != ' ') )
	{
		// take the character at the current position and convert to upper case.
		c = toupper(**str);

		// convert the character to a number
		if (c >= '0' && c <= '9')
			// this was a number between 0 and 9 inclusive, so we only have
			// to subtract the ascii value '0'.
			digit = c - '0';
		else
			// this was not a number or a value between A and F
			return 1;

		// update the value, increasing by a power of 10 for every digit.
		*value = *value * 10 + digit;

		// increase to the next position
		(*str)++;
	}

	// we were successful.	
	return 0;
}


// ============================================================================
// Function: ReadBlasterEnv
//		Gets the Blaster environment statement and stores the value in the
//		appropriate SoundManager member variables.
// Parameters:
//		none.
// Returns:
//		0 if successful
//		1 if there was an error reading the port address.
//		2 if there was an error reading the bank number.
// ============================================================================

unsigned SoundManager::ReadBlasterEnv(void)
{
	char     *env;
	unsigned val;
	int      digit;

	// get the value of the BLASTER environment variable
	env = getenv("BLASTER");

	// as long as we're not at the end of the string...
	// note that the Base16 and Base10 functions advance the value of env!
	while (*env)
	{
		switch(toupper( *(env++) ))
		{
			case 'A':
				if (Base16(&env, &IoPort))  // interpret port value as hex
					return 1;                // error
				break;

			case 'B':
				if (Base10(&env, &BankOffset))    // interpret Bank as decimal
					return 2;                // error
				break;

			default:
				break;
		}
	}

	// If Bank was set to a non-zero value other than 1,
	// we have to set BankOffset to 2 because only banks 0 and 1 may be chosen.
	// The offset for Bank 1 is base+2.
	if (BankOffset)
		BankOffset=2;

	// we can quit now...
	return 0;
}


