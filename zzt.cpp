// ZZT++
//
// Copyright (C) 1998-2014 Wouter E.P. Bovelander
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

// Main ZZT++ routine

#pragma warn -use

#include <conio.h> // gotoxy(), clrscr()

// MS Visual C++
#ifdef _MSC_VER
#include <graph.h> // _settestposition() and _clearscreen()
#endif

#include "bm.hpp"
#include "ge.hpp"
#include "mm.hpp"
#include "sm.hpp"
#include "timerisr.hpp"
#include "zconst.hpp"
#include "ope.hpp"

// This needs to be global since our interrupt service routine sends
// messages to BM.
BoardManager BM;

// This starts our Menu Manager.
MenuManager MM;

// This is the only game engine.
GameEngine GE;

// Define the Sound Manager.
SoundManager SM;

//=============================================================================
// Function: Exit
//		Prints exit message and error code.
// Parameters:
//		int err=error code
// Returns:
//		Nothing
//=============================================================================

void Exit(int err)
{
	// clear the screen
	BM.Cls();

	// Reset the screen to color mode.
	// Enable cursor which was disabled while reading the game configuration.
	// Enabling the cursor while it was already enabled does no harm.
	BM.SetColorMode();
	BM.EnableCursorSmall();

	if (err) printf("\nAn error has occurred: %d\n",err);

	printf("\n  Published under GNU license -- "
			 "Thank you for playing ZZT++.\n\n");
}

//=============================================================================
//		Reads game configuration.
// Parameters:
//		char *Control=pointer to 1 character, k=keyboard, m=mouse
//		char *Monitor=pointer to 1 character, c=color, m=monochrome
// Returns:
//		0=success
//		1=escape was pressed
//=============================================================================

int GameConfig(char *Control, char *Monitor, char *Sound)
{
	// clear screen
	BM.Cls();

	BM.TypeString(33, 2, "<=-  ZZT++  -=>", 15);
	BM.TypeString(35, 3, ZZTVERSION, 14);
	BM.TypeString(26, 4, "Created by Wouter Bovelander", 14);
	BM.TypeString(16, 5, "Based on ZZT by Epic Megagames. Copyright (c) 1991", 14);
	BM.TypeString(1, 7, "=============================",9);
	BM.TypeString(31,7,"Game Configuration", 15);
	BM.TypeString(51, 7, "==============================",9);
	BM.TypeString(1, 24, "================================================================================", 9);
	BM.TypeString(2, 25, "Copyright (c) 1998 - 2003 Wouter Bovelander", 15);
	BM.TypeString(59,25, "Press ", 15);
	BM.TypeString(65,25,"ESC", 112);
	BM.TypeString(68,25," to abort", 15);

	//==========================================================================
	// Read controller type. K=Keyboard, M=Mouse
	//==========================================================================
	BM.TypeString(2,9,"Game controller:  K)eyboard, M)ouse? ",14);

	// Position cursor at end of question.
	// MS Visual C++
	#ifdef _MSC_VER
		_settextposition(9, 40);
	#endif

	// Borland C++
	#ifdef __BORLANDC__
		gotoxy(40, 9);
	#endif

	// Read the answer.
	while ( *Control=MM.GetChar(), (strcspn ("KkMm", Control) >= 4) && *Control!=27 );
	// Quit game if escape was pressed
	if (*Control==27)
		return 1;
	// Make *Control upper case if larger than 'a'
	if (*Control>97) *Control-=32;


	//==========================================================================
	// Read monitor type. C=Color, M=Monochrome.
	//==========================================================================
	BM.TypeString(2,12,"Video mode:  C)olor, M)onochrome? ",10);

	// Position cursor at end of question.
	// MS Visual C++
	#ifdef _MSC_VER
		_settextposition(12, 37);
	#endif

	// Borland C++
	#ifdef __BORLANDC__
		gotoxy(37, 12);
	#endif

	while ( *Monitor=MM.GetChar(), (strcspn ("CcMm", Monitor) >= 4) && *Monitor!=27 );
	// Quit game if escape was pressed
	if (*Monitor==27)
		return 1;
	// Make *Monitor upper case if larger than 'a'
	if (*Monitor>97) *Monitor-=32;

	//==========================================================================
	// Read sound type. S=Speaker, A=Adlib card.
	//==========================================================================

	// set mode to PCSPEAKER by default
	SM.SetSoundMode(PCSPEAKER);

	// only if there's an OPL2 compatible card present, go in here.
	if (SM.DetectOPL2())
	{
		BM.TypeString(2,15,"Sound mode:  S)peaker, A)dLib card? ",14);

		// Position cursor at end of question.
		// MS Visual C++
		#ifdef _MSC_VER
			_settextposition(15, 40);
		#endif

		// Borland C++
		#ifdef __BORLANDC__
			gotoxy(40, 15);
		#endif

		while ( *Sound=MM.GetChar(), (strcspn ("SsAa", Sound) >= 4) && *Sound!=27 );
		// Quit game if escape was pressed
		if (*Sound==27)
			return 1;

		// Make *Sound upper case if larger than 'a'
		if (*Sound>=97) *Sound-=32;

		if (*Sound=='A')
			SM.SetSoundMode(ADLIB);
	}

	// Clear screen.
	BM.Cls();

	// Set screen mode to 350 scan lines..
	BM.SetScan350();

	// ..and 80x25 16 color or monochrome mode
	if (*Monitor=='C') BM.SetColorMode();
	else BM.SetMonoMode();

	// disable cursor
	BM.DisableCursor();

	return 0;
}

//=============================================================================
// Function: ParseCmdLine
//		Parse command line for options and filenames.
// Parameters:
//		None.
// Returns:
//		0=success
//		10=Too many parameters
//		20=Unknown option
//=============================================================================

// turn off warnings about possibly incorrent assignments
#pragma warn -pia

int ParseCmdLine(int argc, char **argv, char *world, int *r, int *w, int *s)
{
	char c, *walk;

	// the loops below were taken directly from Kernighan and Richie's
	// "The C Programming Language". it's too complex but elegant. also,
	// it alters argc and argv which is bad in my humble opinion.
	while (--argc && (*++argv)[0]=='-')
		while (c = *++argv[0])
			switch (c)
			{
				case 'r':
					*r=1;
					break;
				case 'w':
					*w=1;
					break;
				case 's':
					*s=1;
					break;
				default:
					argc=0;
					return 20;
			}
	// *argv now points to first unprocessed parameter

	// Too many parameters
	if (argc>1)
		return 10;

	// Use town.zzt by default.
	world=strcpy(world,"TOWN.ZZT\0");

	// No file name given so use default.
	if (argc==0)
		return 0;

	// A file name was given. Use it.
	world=strcpy(world,*argv);

	// Look for file extention
	walk=*argv;
	while (*walk!='.' && *walk) walk++;

	// If found then don't add .ZZT
	if (*walk=='.') return 0;

	// Otherwise, do add .ZZT
	world=strcat(world,".ZZT\0");

	return 0;
}

#pragma warn .pia

//=============================================================================
// Function: Usage
//		Print ZZT++ usage message.
// Parameters:
//		None.
// Returns:
//		Nothing.
//=============================================================================

void Usage(int err)
{
	// Clear screen.
	BM.Cls();

	printf("An error has occurred: %d\n\n",err);
	printf("ZZT++ %s\nCopyright (c) 1998-2003 by W.Bovelander " \
	"http://www.planetzztpp.com\n\n", ZZTVERSION);
	printf("Usage: ZZT [-option] [filename]\n\nOptions:\n" \
	"\tr = create report on world content into report.txt\n" \
	"\ts = report on screen information in report.txt\n" \
	"\tw = write this world in .ZZT format to world.zzt\n\n");
}

//=============================================================================
// Function: CtrlBreak
//		Catch CTRl-C. Return without action so zzt cannot be quit.
// Parameters:
//		None.
// Returns:
//		int 1, so program does not abort.
//=============================================================================

int c_break(void)
{
	return 1;
}

// turn off warnings about possibly incorrent assignments
// turn off warnings about parameter never used
#pragma warn -pia
#pragma warn -par

//=============================================================================
//==                  Z Z T + +  M A I N  F U N C T I O N                    ==
//=============================================================================
// Function: Main
//		ZZT++ main program.
// Parameters:
//		Standard command line parameters:
//		argc=number of arguments given, including ZZT++ itself
//		argv=array of program arguments, including ZZT++ itself
//		env=array of OS environment variables
// Returns:
//		10=Too many parameters
//		20=Unknown option
//=============================================================================
//=============================================================================
int main(int argc, char **argv, char **env)
{
	// This will point to the active world's file name.
	char *world;

	int	err=0,		// Holds an error number
			Report=0,	// Create report while reading world?
			Write=0,		// Write the current world to a file?
			Screen=0,	// Report with screen information?
			board=0,		// board number to show.
			menu=1;		// menu number to show.

	char 	Control='K',	// K=Keyboard, M=Mouse
			Monitor='C',	// C=Color, M=Monochrome
			Sound='S';		// S=Speaker, A=Adlib

	// Catch CTRL-Break
	ctrlbrk(c_break);

	// Clear screen.
	BM.Cls();

	// Use these to create asc.exe or col.exe
	//	char c; BM.AsciiChart(); while ((c=getch())!=27); return 99;
	//	char c; BM.ColorList(); while ((c=getch())!=27); return 99;

	// Allocate space for file name. Parse command line
	world=(char *)malloc(13*sizeof(char));
	if (err=ParseCmdLine(argc, argv, world, &Report, &Write, &Screen))
	{
		Usage(err);
		return err;
	}

	// Read world into memory. Use town.zzt by default.
	if (err=BM.ReadWorld(world, Report, Screen))
	{
		Usage(err);
		return err;
	}

	// FOR BETA PURPOSE: Write current world to outworld.zzt
	if (Write)
		err=BM.WriteWorld("OUTWORLD.ZZT");

	// Read game configuration
	if(err=GameConfig(&Control, &Monitor, &Sound))
	{
		// Ony exits with an error if escape was pressed.
		Exit(0);
		return err;
	}

	//////////////////////////////////////////////////////
	// Let the games begin
	//////////////////////////////////////////////////////

	// Install ISR timing routine now.
	InstallISR();

	// Feed music manager with a tune. It will start playing automatically
	// because InstallISR() has run.
	// #include "DEMOMUS.TXT"

	// Call main menu and start things from there.
	MM.MainLoop();

	// If we reach this code, the player has chosen to exit zzt++.
	// Uninstall ISR timing routine now.
	UninstallISR();

	// free up last of memory
	free(world);

	// Print message and exit program. Thank you for playing zzt++!
	Exit(err);
	return err;
}

#pragma warn .pia
#pragma warn .par
