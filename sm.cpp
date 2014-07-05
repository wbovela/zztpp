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

// Sound Manager code.

#pragma warn -use

#include <mem.h>
#include <stdlib.h>
#include <dos.h>

#include "sm.hpp"
#include "sm_adlib.hpp"

// ============================================================================
// Function: SetSoundMode
//		Sets media mode to pc speaker or adlib. If an unknown mode is passed,
//		the setting defaults to PCSPEAKER.
// Parameters:
//		char mode=PCSPEAKER or ADLIB (#defines from sm.hpp).
// Returns:
//		char current soundmode.
// ============================================================================

char SoundManager::SetSoundMode(char mode)
{
	if ((mode!=PCSPEAKER) && (mode!=ADLIB))
		SoundMode=PCSPEAKER;
	else
		{
			if (mode==ADLIB)
			{
				// make sure all the parameters are set up correctly.
				SetupAdlibVoice();
			}
			SoundMode=mode;
		}

	return SoundMode;
}

// ============================================================================
// Function: ResetSound
//		Reset FM sound card.
// Parameters:
//		None.
// Returns:
//		Nothing.
// ============================================================================

void SoundManager::ResetSound(void)
{
	// call adlib function from adlib module
	ResetCard();
}

// ============================================================================
// Function: SmSound
//		Start playing the given sound over current media (pc speaker or adlib).
// Parameters:
//		int frequency=frequency of desired note, value depending on media.
//		int octave=desired octave
// Returns:
//		Nothing.
// ============================================================================

void SoundManager::SmSound(int frequency, int octave)
{
	if(SoundMode==PCSPEAKER)
		sound(frequency);
	else
		SoundAl(frequency, octave);
}

// ============================================================================
// Function: SmNoSound
//		Kill all sound. Routine knows current media, pc speaker or adlib.
// Parameters:
//		None.
// Returns:
//		Nothing.
// ============================================================================

void SoundManager::SmNoSound(void)
{
	if(SoundMode==PCSPEAKER)
		// we're using the speaker, so use borland's nosound() function.
		nosound();
	else
		// otherwise, use our adlib NoSsoundAl() function.
		NoSoundAl();
}

// ============================================================================
// Function: NoteRemove
//		Remove top element from the Queue.
// Parameters:
//		None.
// Returns:
//		The number of element taken from the queue (0 or 1).
// ============================================================================

int SoundManager::NoteRemove(Note *note)
{
	// a pointer to a queue element.
	Note *ti;

	// can't remove from empty queue.
	if (!NoElements) return 0;

	// set ti to the front of the queue. advance the
	// queue header pointer and subtract 1 off the number
	// of elements. ti now points to the element to be removed
	// from the queue.
	ti=NoteQHead;
	NoteQHead=NoteQHead->next;
	NoElements--;

	// if no elements left, set the tail to NULL.
	if (!NoElements)
		NoteQTail=NULL;

	// now copy the contents of ti to the memory pointed to
	// by position. THERE MUST BE MEMORY ALLOCATED!
	// finally, free the memory for ti and return.
	memcpy(note, ti, sizeof(Note));
	free(ti);
	return 1;
}

// ============================================================================
// Function: NoteAdd
//		Add one element to the end of the music Queue.
// Parameters:
//		The Note structure to be pushed.
// Returns:
//		nothing.
// ============================================================================

void SoundManager::NoteAdd(Note note)
{
	// a pointer to a queue element
	Note *t;

	// allocate memory and return if unsuccessful.
	t=(Note *)malloc(sizeof(Note));
	if (!t) return;

	// copy the contents of the element to be added to our
	// newly acquired memory. it's next pointer is NULL by default.
	memcpy(t, &note, sizeof(Note));
	t->next=NULL;

	// if the queue was empty, the head is the tail is the
	// new element. the number of elements is 1. so is the counter
	// of the max number of elements in the queue.
	if (!NoElements)
	{
		NoteQHead=NoteQTail=t;
		NoElements=MaxElms=1;
	}
	// if the queue wasn't empty, add it to the back of the queue
	// by making the next pointer of the current last element point
	// to the new element. then, have the tail point to our new element.
	// update maxelements accordingly.
	else
	{
		NoteQTail->next=t;
		NoteQTail=t;
		NoElements++;
		if (NoElements>MaxElms) MaxElms=NoElements;
	}
}

// ============================================================================
// Function: NoteQIsEmpty
//		Check whether music queue is empty
// Parameters:
//		None.
// Returns:
//		0 if there is 1 or more elements on the stack.
//		non nul if the stack contains 0 elements
// ============================================================================

unsigned char SoundManager::NoteQIsEmpty(void)
{
	// if the element counter is 0, the queue must be empty.
	return NoElements==0;
}

// ============================================================================
// Function: AddTune
//		Add a tune string to the Notequeue.
// Parameters:
//		char *tone = a zzt++ music string, terminated by '\0'.
// Returns:
//		0 = tune added.
//		1 = music playing, tune not added.
// Syntax:
//		[parameters][note[#|!]|effect|X]
//
//		Parameters:
//			T	32nd	(default duration)
//			S	16th
//   		I	8th
//			Q	4th
//			H	half
//			W	whole
//			3  cut previous duration into thirds, so Q3ABC takes 1 quarter note.
//			. one and a half, so H. becomes a half and a quarter note.
//			+ up one octave
//			- down one octave
//
//		X=rest
//		#=sharp
//		!=flat
//		[] comment, has no effect.
//
//		Note:
//			ABCDEFG
//
//		Effect:
//			0 tick
//			1 tweet
//			2 cowbell
//			4 high snare
//			5 high woodblock
//			6 low snare
//			7 low tom
//			8 low woodblock
//			9 bass drum
// ============================================================================

int SoundManager::AddTune(char *tune)
{
	short octave=3;		// octave 3 is the default.
	short	temp_octave;	// temp storage for octave for use in # and !
	short duration=15;	// this is the default duration.
	short	note=0;			// this is the whole note's denomination.
	int	frequency=0;	// this is the note's frequency.
	char *walk;				//	use this to walk through tune

	// this is the new note that will be added.
	Note NewNote;

	// initialise the new note.
	NewNote.next=NULL;
	NewNote.Frequency=0;
	NewNote.Duration=0;
	NewNote.Oct=0;

	// if the queue is not empty, don't add the tune.
	// because of this, only one tune can be on the queue. since the
	// note queue is dynamic, we could skip this check. maybe later...
//	if (!NoteQIsEmpty())
//		return 1;

	//
	// here we start interpreting the sound string.
	//

	// first set a helper pointer to the start of the string.
	walk=tune;

	// keep going until we've encountered the termination '\0'
	while (*walk)
	{
		// translate to upper case if this is a note, rest or a sound effect.
		if ( (*walk >= 'a') && (*walk <= 'z'))
			*walk-=32;

		// alter the current duration if we encounter one of these.
		switch (*walk)
		{
			case 'T': duration=THIRTYSECOND; break; // (default duration)
			case 'S': duration=SIXTEENTH; break;
			case 'I': duration=EIGHTH; break;
			case 'Q': duration=QUARTER; break;
			case 'H': duration=HALF; break;
			case 'W': duration=WHOLE; break;
			case '3': duration/=3; break; // triplets, divide current by 3.
			case '.': duration+=(duration>>1); break; // add half current duration
		};

		// octave modifiers
		if (*walk=='+')
			if (octave < MAXOCTAVE)
				octave++;

		if (*walk=='-')
			if (octave > MINOCTAVE)
				octave--;

		// this is a note or a rest.
		if ( (*walk >= 'A') && (*walk <= 'G') || (*walk == 'X'))
		{
			// get the denomination for this note. not if it's a rest.
			if (*walk!='X')
				note=Xlat[(*walk)-'A'];
			else
				// use a dummy note denomination here.
				note=99;

			// the next character may alter the frequency by
			// changing setting the note to sharp or flat.
			// first, copy the current octave to a temporary storage.
			temp_octave=octave;
			if (*(walk+1)=='#')
			{
				// this is a sharp, so increase the frequency.
				// if we want to sharp a B (note denomination=13), we need to go to
				// note 0 of the next octave.
				// keep in mind that note is always increased here!
				if (++note==13)
				{
					// this is a C;
					note=0;

					// we shouldn't go higher than MAXOCTAVE.
					if ((++temp_octave) > MAXOCTAVE)
						temp_octave=MAXOCTAVE;
				}

				// we should now advance one position.
				walk++;
			}
			else	if (*(walk+1)=='!')
					{
						// this is a sharp, so increase the frequency.
						// if we want to flat a C (note denomination=0),
						// we need to go to note 12 of the previous octave.
						// keep in mind that note is always decreased here!
						if (--note==0)
						{
							// this is a B;
							note=12;

							// we shouldn't go higher than MAXOCTAVE.
							if ((--temp_octave) < MINOCTAVE)
								temp_octave=MINOCTAVE;
						}

						// we should now advance one position.
						walk++;
					}

			// now we can get the correct frequency.
			if (note!=99)
				if (SoundMode==ADLIB)
					// we're using adlib, so get the frequency from the
					// correct table.
					frequency=ADScale[note];
				else
					// otherwise, get it from the pc speaker table.
					frequency=Scale[note]>>(MAXOCTAVE-temp_octave);
			else
				// this is a rest, set an inaudible frequency.
				frequency=1;

			// add the note to the queue.
			NewNote.Frequency=frequency;
			NewNote.Duration=duration;

			// If we're playing adlib, don't forget to set the octave.
			if (SoundMode==ADLIB)
				NewNote.Oct=temp_octave;

			NoteAdd(NewNote);
		}// end of if this is a note

		// this is percussion
		if ( (*walk >= '0') && (*walk <= '9'))
		{
			// add a recognizable, but inaudible frequency.
			NewNote.Frequency=30000+(*walk-'0');
			// take the current duration, but we don't really use it.
			NewNote.Duration=duration;
			// add the note.
			NoteAdd(NewNote);
		}

		// go to the next byte.
		walk++;
	}// end of while
	return 0;
}

// ============================================================================
// Function: PlayTune
//		Plays the tune on the queue, if any. This function is called by
//		the timer interrupt. If there are still notes on the queue they are
//		played until the set duration has passed. The duration is measured in
//		ticks, where one whole note is 500 ticks or 1 second.
// Parameters:
//		none.
// Returns:
//		nothing
// ===========================================================================

void SoundManager::PlayTune(void)
{
	// make this static to make things faster.
	static Note CurrentNote;

	// NoteInPlay is a private member variable of SoundManager.
	// Check if we're playing a note.
	if (NoteInPlay)
	{
		// if this note was a percussion instrument
		if (DrumPlaying)
		{
			// then increase the timer for this note
			DrumTimer++;

			// if it's time to go to the next frequency for this note
			if (DrumTimer>DRUMSTEP)
			{
				// reset the timer
				DrumTimer=0;

				// play the next frequency in line
				SmSound(Drums[DrumNo][DrumStep], PLAINSOUND);

				// and record which step we're in now
				DrumStep++;

				// there is a maximum of 10 steps (0-9).
				if (DrumStep>9)
				{
					// we're no longer playing a percussion note
					DrumPlaying=0;
					DrumStep=0;
					DrumNo=0;
					// kill sound now.
					SmNoSound();
				}
			}
		}

		// note is still playing. take one tick off.
		NoteInPlay--;

		// make sure we get a small 4-tick space between notes
		// if we're playing the pc speaker.
		if (SoundMode==PCSPEAKER)
			if (NoteInPlay<=4) SmNoSound();
	}

	else
		// no more notes playing, check if we need to take a new one.
		// is the queue empty?
		if (!NoteQIsEmpty())
		{
			// there's a note. store its duration.
			NoteInPlay=NoteQHead->Duration;

			// now play it!
			// remember to kill sound when we're playing a rest!
			if (NoteQHead->Frequency==1)
				SmNoSound();
			else
					// check if this note is a percussion note
					if (NoteQHead->Frequency>=30000)
					{
						// if so, set the frequency by subtracting the
						// indicator and set the percussion indicator.
						DrumNo=NoteQHead->Frequency-30000;
						DrumPlaying=1;
						DrumStep=0;
						DrumTimer=0;
					}
					else
						// we start playing regular notes here.
						// percussion is played in the first part
						// of PlayTune().
						SmSound(NoteQHead->Frequency, NoteQHead->Oct);

			// and remove it from the queue. it can be discarded now.
			NoteRemove(&CurrentNote);
		}
		else
			// no more notes on the queue. kill sound.
			SmNoSound();
}

// ============================================================================
// Function: SetSwitch
//		Set sound on or off.
// Parameters:
//		short Sound=0 is off
//		short	Sound!=0 is on
// Returns:
//		nothing
// ============================================================================

void SoundManager::SetSound(short Sound)
{
	SoundSwitch=Sound;
}

// ============================================================================
// Function: SoundIsOn
//		Check status of soundswitch.
// Parameters:
//		none
// Returns:
//		short 0=sound is off
//				!0=sound is on
// ============================================================================

short SoundManager::SoundIsOn(void)
{
	return SoundSwitch;
}
