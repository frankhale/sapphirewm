/*
 * icon.cc - Defines the icon look and feel.
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

#include "icon.hh"

char * default_icon_xpm[] = {
"16 16 16 1",
" 	c None",
".	c #323232",
"+	c #535353",
"@	c #4A8A8E",
"#	c #DEE2E2",
"$	c #7E827A",
"%	c #8A9292",
"&	c #D6D6D6",
"*	c #36767E",
"=	c #9E9E9E",
"-	c #FAFAFA",
";	c #B2B2B2",
">	c #DEEEEA",
",	c #464646",
"'	c #5EA2A2",
")	c #52969A",
"                ",
"                ",
" --#>>>>>>#-#-; ",
" -&%')))))=&=&+ ",
" >;$@*****=;%;+ ",
" &$$$$$$$$$$$$, ",
" &;;;;;;;;;;;;+ ",
" &;;;;;;;;;;;;+ ",
" #;;;;;;;;;;;;+ ",
" &;;;;;;;;;;;;+ ",
" #;;;;;;;;;;;;+ ",
" #;;;;;;;;;;;;+ ",
" &;;;;;;;;;;;;+ ",
" $............. ",
"                ",
"                "};

Icon::Icon(Display *d, Window parent)
{
	dpy = d;

	parent_win = parent;

	icon_win=root=None;
	icon_pixmap=icon_pixmap_mask=None;

	screen = DefaultScreen(dpy);
	depth  = DefaultDepth(dpy, screen);
	visual = DefaultVisual(dpy,screen);
	root   = RootWindow(dpy, screen);

	exe = "xterm";

	icon_pixmap_attr.valuemask=XpmSize;
	
	icon_win=XCreateSimpleWindow(
		dpy,
		parent_win,
		0,
		0,
		20,
		20,
		0,
		XWhitePixel(dpy,screen),
		XBlackPixel(dpy,screen));
	
	if(! XpmCreatePixmapFromData(dpy, 
		icon_win,
		default_icon_xpm,
		&icon_pixmap,
		&icon_pixmap_mask,
		&icon_pixmap_attr)==XpmSuccess)
	{
		cerr << "Error with default pixmap, exiting!" << endl;
		exit(-1);
	}

	configureWindow();
}

Icon::Icon(Display *d, std::string iconFilename, Window parent)
{
	dpy = d;

	parent_win = parent;

	icon_win=root=None;
	icon_pixmap=icon_pixmap_mask=None;

	screen = DefaultScreen(dpy);
	depth  = DefaultDepth(dpy, screen);
	visual = DefaultVisual(dpy,screen);
	root   = RootWindow(dpy, screen);

	exe = "xterm";

	icon_pixmap_attr.valuemask=XpmSize;
	
	icon_win=XCreateSimpleWindow(
		dpy,
		parent,
		0,
		0,
		20,
		20,
		0,
		XWhitePixel(dpy,screen),
		XBlackPixel(dpy,screen));
	
	if(! loadPixmap(iconFilename) )
  		cerr << "Error occurred trying to load pixmap - (" << iconFilename << ") - Using a default icon." << endl;
}

Icon::~Icon()
{
	XFreePixmap(dpy, icon_pixmap);
	XFreePixmap(dpy, icon_pixmap_mask);

	XDestroyWindow(dpy, icon_win);
}

bool Icon::loadPixmap(std::string iconFilename)
{
  	if(XpmReadFileToPixmap(dpy, parent_win, (char*) iconFilename.c_str(), &icon_pixmap, &icon_pixmap_mask, &icon_pixmap_attr)==0)
  	{
		configureWindow();
	
		return true;
 	} else {
		
		if(! XpmCreatePixmapFromData(dpy, 
			icon_win,
			default_icon_xpm,
			&icon_pixmap,
			&icon_pixmap_mask,
			&icon_pixmap_attr)==XpmSuccess)
		{
			cerr << "Error with default pixmap, exiting!" << endl;
			exit(-1);
		}
		
		// Using the default icon
		configureWindow();
		
		return false;
	}
}

void Icon::configureWindow()
{
	x=y=0;
		
	w=icon_pixmap_attr.width;
     	h=icon_pixmap_attr.height;

	XResizeWindow(dpy, icon_win, w,h);
	
	XSelectInput (dpy, icon_win, 
			ExposureMask 		| 
	                ButtonPressMask 	|
			ButtonReleaseMask 	|
			EnterWindowMask 	|
			LeaveWindowMask
				);

	XSetWindowBackgroundPixmap(dpy, icon_win, icon_pixmap);
    	XShapeCombineMask(dpy, icon_win, ShapeBounding, 0, 0, icon_pixmap_mask, ShapeSet);
}

void Icon::executeCommand()
{
	std::string temp;
	
	temp = "exec " + exe + " & ";
	
	// Execute the command stored in exe
	system((char*)temp.c_str());
}

void Icon::updateXYPos()
{
	XWindowAttributes attr;
	
	XGetWindowAttributes(dpy, icon_win, &attr);
	
	x=attr.x;
	y=attr.y;
}
