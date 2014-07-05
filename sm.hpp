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

// Sound Manager header file.

#ifndef _SM_H
#define _SM_H

#include "timerisr.hpp"
#include "sm_adlib.hpp"
#include <dos.h>

// the highest octave is 6.
#define MAXOCTAVE	6
#define MINOCTAVE 0

// sound modes
#define PCSPEAKER 1
#define ADLIB 2

// this is used in place of octave to indicate that it is a plain sound
#define PLAINSOUND -1

// These are the frequencies of notes C thru G# in octave 6 and A to B of 7.
// The highest octave we want to handle is octave 6. The highest notes of
// octave 7 are very high. One octave higher still renders inaudible notes.
// Frequencies for notes are calculated by dividing the frequency for a note
// found in this table by two for the correct number of times. This is based
// on the theory that if a note E is 440Hz, the frequency of E in the
// next octave can be found by multiplying 440Hz by 2.
// Since the default octave=3, we will almost always need to do some
// work to calculate the correct frequency. We accept that extra work because
// it is easier to program if we know that the frequencies in this table are
// of octave 6; we always have to shift right.
static short Scale[12]=
{
	2032, // 0=C
	2152, // 1=C#
	2280, // 2=D
	2416, // 3=D#
	2560, // 4=E
	2712, // 5=F
	2880, // 6=F#
	3048, // 7=G
	3232, // 8=G#
	3424, // 9=A
	3624, // 10=A#
	3840  // 11=B
};

// The table below holds the frequencies (also called F-numbers in adlib-land)
// for playing FM sounds over the adlib chip.
// These values are written to port 0xA0.
static short ADScale[12]=
{
	0x14F,	// low c
	0x16B,  // c#
	0x181,	// d
	0x198,	// d#
	0x1B0,	// e
	0x1CA,	// f
	0x1E5,	// f#
	0x202,	// g
	0x220,	// g#
	0x241,	// a
	0x263,	// a#
	0x287   // b
};

// This translation table helps to index the Scale[] table. Since notes
// are stored like 'A', 'B', etc and they correspond to a certain frequency
// in the Scale[] table, the note needs to be translated to the corresponding
// frequency.
// e.g.: a note 'A' is found. 'A' is translated to the index of Xlat[].
// Xlat['A'-'A']=Xlat[0]=index 9. Scale[9] is the correct frequency for note A.
// Sharp and Flat notes can be found by increasing or decreasing the index
// found.
static short Xlat[7]=
{
	9,    // indexes A in Scale[]
	11,   // B
	0,    // C
	2,    // D
	4,    // E
	5,    // F
	7     // G
};

// percussion effects are processed differently from regular notes.
// a percussion effect is a frequency modulation in a very short
// duration. this means that we want to be able to alter the
// frequency in set steps at set intervals. each drum effect has
// been translated into 10 steps, with differing frequencies. when
// zzt++ encounters a percussion note it plays the first freq of
// the given effect and after a fixed number of ticks, it plays the
// next freq, until all 10 have been played.
// the Drums table holds the frequencies for each percussion effect.
static short Drums[10][10]= {
		{   0,   0, 175, 175, 100,  90,  80,  70,  60,  50},  // 0
		{ 500, 300, 520, 320, 540, 340, 550, 350, 540, 340},  // 1
		{1000,1200,1250,1400,1100,1150,1300,1000,1200, 500},  // 2
		{   0,   0,   0,   0,   0,   0,   0,   0,   0,   0},  // 3 (not a sound)
		{ 950,1950, 750,1750, 550,1550, 350,1350, 150,1150},  // 4
		{ 200, 210, 220, 230, 240, 250, 260, 270, 280, 600},  // 5
		{ 900, 800, 700, 600, 500, 400, 300, 200, 100,   0},  // 6
		{ 300, 200, 290, 190, 280, 180, 270, 170, 260, 160},  // 7
		{ 400, 380, 360, 340, 320, 300, 280, 260, 250, 240},  // 8
		{ 150, 100, 140,  90, 130,  80, 120,  70, 110,  60}}; // 9

// this tells SM how many ticks a drumstep should take
#define DRUMSTEP 1

// The following table holds all zzt sounds stored as strings.
static char *Sounds[25]=
{
	"t+cegcegceg+sc\0",			// Key=0
	"tcgbcgb+ic\0",				// Door=1
	"t+c-gec\0",					// Gem=2
	"tcc#d\0",						// Ammo=3
	"tcase\0",						// Torch=4
	"t+c-c-c\0",					// Shoot=5
	"t--ct+cd#\0",					// Ouch=6
	"scdefg\0",						// Dupe=7
	"tc-c+d-d+e-e+f-f+g-g\0",	// Scroll=8
	"t-c\0",							// Break=9
	"t--f\0",						// Push=10
	"tcegc#fg#df#ad#ga#eg#+c\0",	// Passage=11
	"t--dc\0",						// Invis=12
	"t5\0",							// BombTick=13
	"t8\0",							// BombTock=14
	"tcf+cf+c\0",					// BombSet=15
	"t+c+c\0",						// Water=16
	"s.-cd#e\0"						// Energizer=17
	"s.-f+f-fd#c+c-d#ef+f-fd#c+c-d#e"
	"s.-f+f-fd#c+c-d#ef+f-fd#c+c-d#e"
	"s.-f+f-fd#c+c-d#e"
	"s.-c-a#gf#fd#c\0",
	"t--gc\0",					   // LockedDoor=18
	"ta\0",							// Forest=19
	"t9\0",							// Ricochet=20
	"?\0",							// Transporter=21
	"?\0",							// Bomb Explosion=22
	"?\0",							// Torch Extinguish=23
	"?\0",							// Enemy Death=24
};

// We also want to know the indexes into this table so we can
// do this in the code SM.AddTune(Sounds[GEMSOUND]);
//			SOUND				INDEX
//---------------------------
#define	KEYSOUND			0
#define	DOORSOUND		1
#define	GEMSOUND			2
#define	AMMOSOUND		3
#define	TORCHSOUND		4
#define	SHOOTSOUND		5
#define	OUCHSOUND		6
#define	DUPESOUND		7
#define	SCROLLSOUND		8
#define	BREAKSOUND		9
#define	PUSHSOUND		10
#define	PASSAGESOUND	11
#define	INVISSOUND		12
#define	BOMBTICKSOUND	13
#define	BOMBTOCKSOUND	14
#define	BOMBSETSOUND	15
#define	WATERSOUND		16
#define	ENERGIZERSOUND	17
#define	LCKDDOORSOUND	18
#define	FORESTSOUND		19
#define	RICOCHETSOUND	20
#define	TRANSPORTSOUND	21
#define	BOMBEXPSOUND	22
#define	TORCHEXTSOUND	23
#define	ENEMYDTHSOUND	24

// Now for the default durations.
//			Length			Duration at 500 ticks/second
//--------------------------------------------------
#define	WHOLE				800
#define	HALF				400
#define	QUARTER			200
#define	EIGHTH			100
#define	SIXTEENTH      50
#define	THIRTYSECOND   25

// This is an element of the Notes queue. A note consists of a frequency
// and a duration.
struct Note
{
	Note *next;
	int Frequency;
	int Duration;
	int Oct; // used for notes played in ADLIB mode.
};

// This class defines the sound system. It is used for all
// sound handling routines and is called from NewISR().

class SoundManager
{
	private:

	// Queue space for storing notes.
	// Memory is requested during run time.
	// This Queue is implemented as a dynamic linked list.
	Note *NoteQHead;
	Note *NoteQTail;
	int NoElements, MaxElms;

	// Internal stack manipulation functions.
	int NoteRemove(Note *note);
	void NoteAdd(Note note);
	unsigned char NoteQIsEmpty(void);

	// General sound funtions. These funtions are an interface to
	// the pc speaker and adlib functions.
	void SmSound(int frequency, int octave);
	void SmNoSound(void);
	void ResetSound(void);

	// the sound mode: PCSPEAKER or ADLIB
	int SoundMode;

	// IoPort of AdLib card and bank number to be used.
	// Only OPL3 cards make use of two banks. Specifically on the YamahaOPL3-SA3
	// it is necessary to make use of bank 1 instead of bank 0.
	unsigned IoPort;
	unsigned BankOffset;

	// Drum variables.
	// DrumPlaying indicates if a drum is playing at the moment.
	// DrumNo is the number of the drum (0-9).
	// DrumStep is the drum's current position in its sound data.
	// DrumTimer counts up, and is used for determining when the next
	// drum sound step will occur.
	short DrumPlaying, DrumNo, DrumStep, DrumTimer;

	// Indicates whether a note is now in play or not.
	// This is done by storing the number of ticks left of its duration.
	int NoteInPlay;

	// this determines whether sound is on or off.
	short SoundSwitch;

	// Adlib sound member functions
	void ResetCard(void);
	void WritePort(int reg, int value, int bank);
	void SoundAl(int note, int octave);
	void SoundAlPlain(int note);
	void NoSoundAl(void);
	void Delay6(void);
	void Delay36(void);
	int Base16(char **str, unsigned *value);
	int Base10(char **str, unsigned *value);
	unsigned ReadBlasterEnv(void);
	void SetupAdlibVoice(void);

	public:

	// NewISR() is our friend because only it may call PlayTune().
	#ifdef __BORLANDC__
	friend void interrupt NewISR(...);
	#endif

	#ifdef _MSC_VER
	friend void interrupt far NewISR(void);
	#endif

	// Add a zzt music string to the note queue
	int AddTune(char *tune);

	// set sound between on or off.
	void SetSound(short Sound);
	short SoundIsOn(void);

	// general routines
	char SetSoundMode(char mode);
	char DetectOPL2(void);

	// Constructor
	SoundManager(void)
	{
		// initialise the note queue.
		NoteQHead=NoteQTail=NULL;
		NoElements=MaxElms=0;

		// no notes are in play.
		NoteInPlay=0;

		// initialise the percussion settings.
		DrumPlaying=0;
		DrumNo=0;
		DrumStep=0;
		DrumTimer=0;

		// set default media to pc speaker sound
		SoundMode=PCSPEAKER;

		// set sound to on
		SoundSwitch=1;

		// set ioport to default of 0x220 and bank to 0, which is most
		// common.
		IoPort=0x220;
		BankOffset=0;
	}

#pragma warn -inl

	// Destructor
	~SoundManager(void)
	{
		// Temporary storage
		Note n;

		// Empty NoteQ
		while (!NoteQIsEmpty()) NoteRemove(&n);

		// kill all sound just to be safe.
		SmNoSound();
	}

#pragma warn .inl	

	protected:

	// This function is a special one. It may only be called by
	// NewISR() at 500 times per second.
	void PlayTune(void);
};

#endif