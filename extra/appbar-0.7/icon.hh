/*
 * icon.hh - Defines the icon widget.
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

#ifndef _ICON_HH_
#define _ICON_HH_

#include <iostream>
#include <string>
#include <X11/Xlib.h>
#include <X11/xpm.h>
#include <X11/extensions/shape.h>
#include <stdlib.h>

extern char * default_icon_xpm[];

class Icon 
{
private:
	Display *dpy;
	Window icon_win, root, parent_win;
	Visual *visual;
	int screen, depth;
	unsigned int x,y,w,h;
	Pixmap icon_pixmap, icon_pixmap_mask;
	XpmAttributes icon_pixmap_attr;
	XSetWindowAttributes attr;

	std::string exe;

private:
	void configureWindow();

public:
	Icon(Display *d, Window parent);
	Icon(Display *d, std::string iconFilename, Window parent);
	~Icon();

	bool loadPixmap(std::string iconFilename);
	
	void setExecuteCommand(std::string command) { exe=command; }
	
	unsigned int getX() const { return x; }
	unsigned int getY() const { return y; }
	unsigned int getWidth() const { return w; }
	unsigned int getHeight() const { return h; }
	
	void executeCommand();
	
	void updateXYPos(); // Makes sure the x,y variables are updated.

	Window getIconWindow() const { return icon_win; }
};

#endif
