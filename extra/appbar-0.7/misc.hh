/*
 * misc.hh - Various global declarations and includes.
 * Copyright (C) 2001 Frank Hale
 * frankhale@yahoo.com
 * http://sapphire.sourceforge.net/
 *
 * Updated: 19 July 2001
 *
 * NOTE:
 *
 * 	This code borrows a little from fspanel.c and desklaunch.c so
 * credits go to the authors who wrote those two programs. 
 * 
 * I borrowed the GC and grill drawing code from fspanel.
 * I borrowed GNOME and MWM hints stuff from desklaunch. 
 *
 * fspanel: http://www.chatjunkies.org/fspanel/
 * desklaunch : http://www.kensden.pwp.blueyonder.co.uk/Oroborus/
 *
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 */

// CONFIG FILES NEED TO GO IN /etc/X11/appbar
// Icons to go in /usr/share/appbar

#include "linkedlist.hh"
#include "image.hh"
#include "scanner.hh"
#include "icon.hh"

#include <sys/wait.h> 
#include <signal.h> 
#include <iostream>
#include <X11/Xlib.h>
#include <X11/Xproto.h>
#include <X11/Xatom.h>
#include <X11/extensions/shape.h>

// Default directory where appbar looks for its rc file.
#define APPBAR_RC_FILE "/etc/X11/appbar/appbar.rc"

// If you just wanna test from the source directory uncomment
// the following and run it locally from the source directory.
//#define APPBAR_RC_FILE "appbar_test.rc"

#define WIN_STATE		0
#define WIN_HINTS		1
#define WIN_LAYER		2
#define GNOME_HINT_COUNT	3

#define WIN_STATE_STICKY 	(1<<0)
#define WIN_HINTS_SKIP_FOCUS 	(1<<0)
#define WIN_HINTS_SKIP_WINLIST 	(1<<1)
#define WIN_HINTS_SKIP_TASKBAR 	(1<<2)
#define WIN_HINTS_DO_NOT_COVER  (1<<5)

#define MWM_DECORATIONS		(1<<1)

typedef struct
{
  unsigned long flags;
  unsigned long functions;
  unsigned long decorations;
  unsigned long inputMode;
  unsigned long status;
} PropMwmHints;

// Parts below borrowed from fspanel.c
static unsigned short cols[] = {
	0xd75c, 0xd75c, 0xd75c,		  /* 0. light gray */
	0xbefb, 0xbaea, 0xbefb,		  /* 1. mid gray */
	0xaefb, 0xaaea, 0xaefb,		  /* 2. dark gray */
	0xefbe, 0xefbe, 0xefbe,		  /* 3. white */
	0x8617, 0x8207, 0x8617,		  /* 4. darkest gray */
	0x0000, 0x0000, 0x0000		  /* 5. black */
};

#define PALETTE_COUNT (sizeof (cols) / sizeof (cols[0]) / 3)

//static unsigned long palette[PALETTE_COUNT];
extern unsigned long palette[PALETTE_COUNT];

void signal_handler(int signal);
void init_signals();
