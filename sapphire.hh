/*
 * Copyright (C) 1999,2000,2001 Frank Hale
 * frankhale@yahoo.com
 * http://sapphire.sourceforge.net/
 *
 * Updated: 3 Nov 2001
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


#ifndef _SAPPHIRE_HH_
#define _SAPPHIRE_HH_

#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <stdarg.h>
#include <sys/stat.h>
#include <sys/types.h>

#include <X11/Xlib.h>
#include <X11/Xos.h>
#include <X11/Xutil.h>
#include <X11/Xproto.h>
#include <X11/keysym.h>
#include <X11/Xatom.h>
#include <X11/cursorfont.h>
#include <X11/Xresource.h>
#include <X11/extensions/shape.h>

extern char* windowmanager_author;
extern char* windowmanager_author_email;
extern char* windowmanager_name;
extern char* windowmanager_version;
extern char* windowmanager_usage;
extern char* windowmanager_exit_message;

// If you change the following you must change ./data.inst so that 
// the configuration files get put into the same directory. At the 
// current time Sapphire will crash if it cannot find its config
// files. This will be fixed in the future.
extern char* configuration_home;

extern char* HOME;

/* default settings */
#define DEFAULT_FOREGROUND_COLOR 	"black"
#define DEFAULT_BACKGROUND_COLOR	"lightgray"
#define DEFAULT_BORDER_COLOR		"black"
#define DEFAULT_BORDER_SIZE		1
#define DEFAULT_SPACE			3
#define DEFAULT_MARGIN			6
#define DEFAULT_MINIMUM_SIZE		10

// MOTIF hints
#define MwmHintsFunctions     		(1l << 0)
#define MwmHintsDecorations   		(1l << 1)

#define MwmFuncAll            		(1l << 0)
#define MwmFuncResize         		(1l << 1)
#define MwmFuncMove           		(1l << 2)
#define MwmFuncIconify        		(1l << 3)
#define MwmFuncMaximize       		(1l << 4)
#define MwmFuncClose          		(1l << 5)

#define MwmDecorAll           		(1l << 0)
#define MwmDecorBorder        		(1l << 1)
#define MwmDecorHandle        		(1l << 2)
#define MwmDecorTitle         		(1l << 3)
#define MwmDecorMenu          		(1l << 4)
#define MwmDecorIconify       		(1l << 5)
#define MwmDecorMaximize      		(1l << 6)

#define PropMwmHintsElements 	 	3

typedef struct MwmHints {
  unsigned long flags, functions, decorations;
} MwmHints;


struct Widget 
{
	Window window;
	int x,y;
	int width,height;
	bool pressed;
};

extern int handle_xerror(Display *dpy, XErrorEvent *e);
extern void err(char *fmt, ...);
extern void show_event(XEvent e);
extern void signalhandler(int i);

// Forward declarations for all the classes in Sapphire
class BImageControl;
class BImage;
class WindowManager;
class Client;
class BaseMenuItem;
class BaseMenu;
class RootMenu;
class IconMenu;
class Toolbar;
class Theme;
class Scanner;
class MenuLex;

// Include all of the header files
#include "linkedlist.hh"
#include "image.hh"
#include "theme.hh"
#include "basemenuitem.hh"
#include "basemenu.hh"
#include "client.hh"
#include "toolbar.hh"
#include "rootmenu.hh"
#include "iconmenu.hh"
#include "scanner.hh"
#include "menulex.hh"
#include "windowmanager.hh"

#endif
