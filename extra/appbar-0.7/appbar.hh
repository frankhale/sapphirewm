/*
 * appbar.hh - Header for appbar (application launch toolbar)
 * Copyright (C) 2001 Frank Hale
 * frankhale@yahoo.com
 * http://sapphire.sourceforge.net/
 *
 * Updated: 24 July 2001
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

#ifndef _APPBAR_HH_
#define _APPBAR_HH_

#include "misc.hh"

enum { HORIZONTAL, VERTICAL }; // Appbar style
enum { NORTHEAST, NORTHWEST, SOUTHEAST, SOUTHWEST }; // Appbar placement

class Appbar
{
private: /* member variables */
	Display *dpy;
	Window win, root;
	Icon *current_icon;

	Atom gnome[GNOME_HINT_COUNT], motif;
	PropMwmHints mwm_hints;

	Atom atom_wm_protos;
	Atom atom_wm_delete;
	Atom net_wm_strut;
	
	//XFontStruct *font;
	bool hidden;
	Visual *visual;
	int screen, depth;
	Pixmap win_pix;
	GC gc;
	BImageControl *image_control;
	BColor to, from;

	LinkedList<Icon> *iconList;

	Scanner *appbarConfig;
	
	int window_width, window_height;
	int screen_width, screen_height;
	
	int placement;
	int screen_placement;
	
	unsigned char temp_color;
	
private: /* member functions */
	void set_gnome_hint(Window w, int a, long value);
	void set_mwm_hints(Window w, PropMwmHints *hints);
	void set_foreground (int index);
	void draw_dot (Window win, int x, int y);
	void draw3DLine(Window win, int style, int x1, int y1, int x2, int y2);
	void draw3DRectRaised(Window win, int x, int y, int h, int w);
	void draw3DRectSunken(Window win, int x, int y, int h, int w);
	
	void drawVerticalGrill(Window win, int x, int height);
	void drawHoriontalGrill(Window win, int y, int height);
	
	bool imageKeyword(Scanner *s);
	bool executeKeyword(Scanner *s);
	bool iconKeyword(Scanner *s);
	bool horizontalKeyword(Scanner *s);
	bool verticalKeyword(Scanner *s);
	bool northeastKeyword(Scanner *s);
	bool northwestKeyword(Scanner *s);
	bool southeastKeyword(Scanner *s);
	bool southwestKeyword(Scanner *s);
	bool rgbKeywords(Scanner *s);
	bool fromKeyword(Scanner *s);
	bool toKeyword(Scanner *s);
	void parseAppbarConfig(Scanner* s);
	
	void appbarScreenPlacement();
	
	void horizontalBarIconPlacement();
	void verticalBarIconPlacement();

	void drawHorizontalBarGrill();
	void drawVerticalBarGrill();
	
	Icon* findIconWindow(Window win);
	
	void doEventLoop();
	
	void setLeftStrut();
	void setRightStrut();
	void setTopStrut();
	void setBottomStrut();
	
public: /* member functions */
	Appbar();
	~Appbar();
	
	void quit();
};

extern Appbar *ab;

#endif
