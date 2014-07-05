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

#ifndef _ZCONST_H
#define _ZCONST_H

// define the current ZZT++ version. This is shown in the
// config screen. Please refer to zzt.cpp.
#define ZZTVERSION	"Version 0.8.47"

// ZZT++ uses a translation table in order to find the actual
// ASCII codes for codes read from the ZZT world file. Terrain,
// objects, creatures, etc. are not indicated by their ASCII values
// in the ZZT world file, but by their own ZZT code.
// e.g. a character code 0x04 is read. ZZT++ knows that this is the
// player object. The player object is represented on the screen by
// the (well known) smiley, which is ASCII character 0x02.
// There is an entry for every object on the screen in the translation
// table.
// Any index greater than or equal to 0x2f is text entered with f4.
// The translation table contains 0x36 rows.
#define TEXT			0x2f
#define TABLE_SIZE	0x36

// Color codes for objects etc. in ZZT++ are not mapped to
// other values, except for text colors. When a character value
// is read from the world file that is larger than 0x2f, we know
// it is text. Furthermore, we know that for text the world file
// holds the text color first, and then the ASCII value of the letter.
// e.g. 0x30 is read from the world file. It's larger than 0x2f so we're
// dealing with text. The translation table refers to the table below
// and presto: the actual color code is found. The color codes in the
// table below can be used to put in the color attribute byte on the screen.
//	Colorname	Real Code	ZZT++ code
//	==========================================
#define T_BLUE		0x1f		// 0x2f
#define T_GREEN	0x2f		// 0x30
#define T_CYAN		0x3f		// 0x31
#define T_RED		0x4f		//	0x32
#define T_PURPLE	0x5f		// 0x33
#define T_BROWN	0x6f		// 0x34
#define T_BLACK	0x0f		// 0x35

// ZZT++ uses the color attribute byte in video memory to display color.
// DOS handles colors as follows: bit 0, 1, 2 are the blue, green and red
// components of the foreground color. Bit 3 is the intesity of the FG.
// Bits 4, 5 and 6 are B, G and R of the BG color. Bit 7 is the blink
// bit of the FG character.
// The table below holds the bit wise color codes with which we can
// assemble color attribute bytes. Also see the m_color macro below.
//	Colorname		Code
//	========================
#define R_BLACK	0x00
#define R_BLUE		0x01
#define R_GREEN	0x02
#define R_CYAN		0x03
#define R_RED		0x04
#define R_MAGENTA	0x05 // a.k.a. purple ;)
#define R_BROWN	0x06 // bright makes yellow
#define R_GREY		0x07 // bright makes white

// The following macro can be used to compose a color attribute byte that
// can be written directly to video memory. It can be used as a function
// that takes the background color as first parameter, then foreground
// color, then the blink attribute and last the brightness indicator.
// example:	BM.DrawChar(10,10,'X', m_color(R_CYAN, R_BLUE, 0, 1));
//				will draw a bright blue X on a cyan background.
// any value greater than zero will indicate blink or brightness.
#define m_color(bg,fg,blink,hi) ((((bg<<4)+(fg))|(blink?128:0))|(hi?8:0))

// Items, creatures and terrain are all coded in the ZZT file.
// Below are the ascii values for each item in ZZT.

// General items
#define EMPTY			0x00
#define DOWN_ARROW	0x19
#define RIGHT_ARROW	0x1a
#define LEFT_ARROW	0x1b
#define DOUBLE_CROSS 0xce
#define SPACE			0x20
#define DOWN_TRIAN	0x1F

// Items available through F1
#define PLAYER			0x02
#define AMMO			0x84
#define TORCH			0x9d
#define GEM				0x04
#define KEY				0x0c
#define DOOR			0x0a
#define SCROLL			0xe8
#define PASSAGE		0xf0
#define DUPLICATOR	0xfa
#define BOMB			0x0b
#define ENERGIZER		0x7f
#define CONVEYOR_C	0x2f
#define CONVEYOR_CC	0x5c

// Terrains available through F3
#define WATER			0xb0
#define FOREST			0xb0
#define SOLID			0xdb
#define NORMAL			0xb2
#define BREAKABLE		0xb1
#define BOULDER		0xfe
#define SLIDER_NS		0x12
#define SLIDER_EW		0x1d
#define FAKE			0xb2
#define INVISIBLE		0xb0
#define BLINK_WALL	0xce
#define TRANSPORTER	0xc5
#define RICOCHET		0x2a

// Creatures available through F2
#define BEAR			0x99
#define RUFFIAN		0x05
#define OBJECT			0x4f // this is a default dummy value.
#define SLIME			0x2a
#define SHARK			0x5e
#define SPINNING_GUN	0x18
#define PUSHER			0x10 // N=30 S=31 E=16 W=17
#define LION			0xea
#define TIGER			0xe3
#define HEAD			0xe9
#define SEGMENT		0x4f

// This struct is meant to hold an ascii value and a color value
// for all items, creatures and terrains that occur in ZZT++.
// Note that for text entered with f4, these values are switched.
// The code takes care of that.
struct Pair
{
	unsigned char AsciiValue;
	unsigned char ColorValue;
};

// This translation table holds all Pairs for every item, creature
// and terrain. It is indexed by the object code and returns at that
// position the Pair containing the correct ascii value and color for
// that item.
// When an ascii value is 0x00 it means that the appropriate value is
// held in an Object struct.
// When a color value is 0x00 it means the color is taken from the block
// of screen information where the item came from.
// When items are read from the board, and the index is larger than 0x2f
// it means a text was stored. The first byte is then the color value,
// the second is the ascii value.
// Example:
// At some point zzt++ reads a certain board and finds the code 0x04 there.
// We know that this is not an ascii character so we can't write it to the
// screen. By looking at entry 0x04 in Table, we find the ascii character for
// the player. Simple..ain't it?
static Pair Table[TABLE_SIZE]=
{
	// Items available through F1. 21
	{EMPTY, 0x00},			// 0x00 empty
	{0x00, 0x00}, 			// 0x01 edge of board
	{DOWN_ARROW, 0x00},	// 0x02 down arrow
	{RIGHT_ARROW, 0x00},	// 0x03 right arrow
	{PLAYER, 0x00}, 		// 0x04 Player
	{AMMO, 0x00}, 			// 0x05 Ammo 0x03
	{TORCH, 0x00}, 		// 0x06 Torch
	{GEM, 0x00}, 			// 0x07 Gem
	{KEY, 0x00}, 			// 0x08 Key
	{DOOR, 0x00}, 			// 0x09 Door
	{SCROLL, 0x00}, 		// 0x0a Scroll
	{PASSAGE, 0x00}, 		// 0x0b Passage
	{DUPLICATOR, 0x00}, 	// 0x0c Duplicator
	{BOMB, 0x00}, 			// 0x0d Bomb
	{ENERGIZER, 0x00}, 	// 0x0e Energizer
	{LEFT_ARROW, 0x00}, 	// 0x0f left arrow
	{CONVEYOR_C, 0x00}, 	// 0x10 Conveyor Clockwise
	{CONVEYOR_CC, 0x00}, // 0x11 Conveyor Counter Clockwise
	{DOUBLE_CROSS, 0x00},// 0x12 double cross

	// Terrains available through F3. 13
	{WATER, 0x00}, 		// 0x13 Water
	{FOREST, 0x00}, 		// 0x14 Forest
	{SOLID, 0x00}, 		// 0x15 Solid
	{NORMAL, 0x00}, 		// 0x16 Normal
	{BREAKABLE, 0x00}, 	// 0x17 Breakable
	{BOULDER, 0x00}, 		// 0x18 Boulder
	{SLIDER_NS, 0x00},	// 0x19 Slider NS
	{SLIDER_EW, 0x00}, 	// 0x1a Slider EW
	{FAKE, 0x00}, 			// 0x1b Fake
	{INVISIBLE, 0x00}, 	// 0x1c Invisible
	{BLINK_WALL, 0x00}, 	// 0x1d Blink Wall
	{TRANSPORTER, 0x00}, // 0x1e Transporter
	{SPACE, 0x00}, 		// 0x1f Space
	{RICOCHET, 0x00}, 	// 0x20 Ricochet
	{DOWN_TRIAN, 0x00},	// 0x21 Down Triangle

	// Creatures available through F2. 13
	{BEAR, 0x06}, 			// 0x22 Bear
	{RUFFIAN, 0x0d}, 		// 0x23 Ruffian
	{OBJECT, 0x00}, 		// 0x24 Object -> 0x4f is a temporary value!!!
	{SLIME, 0x00}, 		// 0x25 Slime
	{SHARK, 0x07}, 		// 0x26 Shark
	{SPINNING_GUN, 0x00},// 0x27 Spinning gun
	{PUSHER, 0x00}, 		// 0x28 Pusher
	{LION, 0x0c}, 			// 0x29 Lion
	{TIGER, 0x0b}, 		// 0x2a Tiger
	{0x00, 0x00}, 			// 0x2b
	{HEAD, 0x00}, 			// 0x2c Head
	{SEGMENT, 0x00}, 		// 0x2d Segment
	{0x00, 0x00}, 			// 0x2e

	// Text colors. 7
	{T_BLUE, 0x00},		// 0x2f White on blue
	{T_GREEN, 0x00},		// 0x30 White on green
	{T_CYAN, 0x00},		// 0x31 White on cyan
	{T_RED, 0x00},			// 0x32 White on red
	{T_PURPLE, 0x00},		// 0x33 White on purple
	{T_BROWN, 0x00},		// 0x34 White on Brown
	{T_BLACK, 0x00},		// 0x35 White on Black
};

// We also need to know the index numbers for all items on
// the board. How else would we be able to see the difference
// between a Fake and a Normal?
// In other words..these are the ZZT coded things

// Items available through F1. 21
#define	I_EMPTY			0x00
#define	I_EDGE			0x01
#define	I_DOWN_ARROW	0x02
#define	I_RIGHT_ARROW	0x03
#define	I_PLAYER			0x04
#define	I_AMMO			0x05
#define	I_TORCH			0x06
#define	I_GEM				0x07
#define	I_KEY				0x08
#define	I_DOOR			0x09
#define	I_SCROLL			0x0a
#define	I_PASSAGE		0x0b
#define	I_DUPLICATOR	0x0c
#define	I_BOMB			0x0d
#define	I_ENERGIZER		0x0e
#define	I_LEFT_ARROW	0x0f
#define	I_CONVEYOR_C	0x10
#define	I_CONVEYOR_CC	0x11
#define	I_DOUBLE_CROSS	0x12

// Terrains available through F3. 13
#define	I_WATER			0x13
#define	I_FOREST			0x14
#define	I_SOLID			0x15
#define	I_NORMAL			0x16
#define	I_BREAKABLE		0x17
#define	I_BOULDER		0x18
#define	I_SLIDER_NS		0x19
#define	I_SLIDER_EW		0x1a
#define	I_FAKE			0x1b
#define	I_INVISIBLE		0x1c
#define	I_BLINK_WALL	0x1d
#define	I_TRANSPORTER	0x1e
#define	I_SPACE			0x1f
#define	I_RICOCHET		0x20
#define	I_DOWN_TRIAN	0x21

// Creatures available through F2. 13
#define	I_BEAR			0x22
#define	I_RUFFIAN		0x23
#define	I_OBJECT			0x24
#define	I_SLIME			0x25
#define	I_SHARK			0x26
#define	I_SPINNING_GUN	0x27
#define	I_PUSHER			0x28
#define	I_LION			0x29
#define	I_TIGER			0x2a
#define	I_HEAD			0x2c
#define	I_SEGMENT		0x2d

// Menu numbers
#define	MAINMENU	0x01
#define	GAMEMENU	0x02
#define	EDITMENU	0x03
#define	ITEMMENU	0x04
#define	CREAMENU	0x05 // creatures
#define	TERRMENU	0x06 // terrain

// Keyboard character values
#define	ESC		0x1b	// 27
#define	UP			0x48	// 72
#define	DOWN		0x50	// 80
#define	RIGHT		0x4d	// 77
#define	LEFT		0x4b	// 75
#define  ENTER		0x0d	// 13
#define	BACKSP	0x08	// 8
#define	DELETE	0x53	// 83
#define	INSERT	0x52	// 82
#define	CTRL_Y	0x19	// 25

// Keyboard modifier status
#define	KMOD_INS	0x80	// 	Bit 7	0x80	Insert on
#define	KMOD_CAP	0x40	//		Bit 6	0x40	Caps on
#define	KMOD_NUM	0x20	//		Bit 5	0x20	Num Lock on
#define	KMOD_SCR	0x10	//		Bit 4	0x10	Scroll Lock on
#define	KMOD_ALT	0x08	//		Bit 3	0x08	Alt pressed
#define	KMOD_CTR	0x04	//		Bit 2	0x04	Ctrl pressed
#define	KMOD_LFT	0x02	//		Bit 1	0x02	Left Shift pressed
#define	KMOD_RGT	0x01	//		Bit 0	0x01	Right Shift pressed

#endif




