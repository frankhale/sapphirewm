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


#ifndef _WM_HH_
#define _WM_HH_

#include "sapphire.hh"

class WindowManager 
{

private: /* variables */

	LinkedList<BaseMenu> *menulist;
	LinkedList<BaseMenu> *extramenulist;
	LinkedList<Client>   *ClientList;

	Client		*head_client;	

	Toolbar  	*t;
	IconMenu 	*im;
	RootMenu 	*rm;	
	Theme 	 	*theme;
	BImageControl 	*image_control;
	BaseMenu 	*menu;

	Atom		xa_wm_state;
	Atom		xa_wm_change_state;
	Atom		xa_wm_protos;
        Atom		xa_wm_delete;
	Atom		xa_wm_cmapwins;
	Atom		xa_wm_take_focus;
	Atom		xa_wm_motif_hints;

	Window		root;
	
	Display		*dpy;
	
	Visual 		*visual;
	
	Pixmap		font_pix;
		
	XFontStruct	*xfont;	

	XGCValues 	gv;
	
	XColor		xforeground_color;
	XColor		xbackground_color; 
	XColor		xborder_color;

	GC		invert_gc;
	GC		string_gc;
	GC		border_gc;

	Cursor		move_curs;
	Cursor		resize_curs;
	Cursor		left_arrow;
	
	char 		*display;
	char 		*font;
	char 		*menufont;
	char 		*foreground_color;
	char 		*background_color;
	char 		*border_color;

	int		depth;
	int		xres;
	int 		yres;
	int		shape_event;
	int 		colors_per_channel;
	int 		toolbar_position;
	int 		toolbar_visible;
	int		screen;
	int		iheight;
	int 		font_position;

	bool 		image_dither;
	bool		shape;
	bool 		menu_event;

	// Configuration file stuff
        char		*clock_format;
	char		*configurationPath;
	char		*windowmanager_conf;

	static KeySym 	AltKeys[];
public: 

	// member functions below
	
	WindowManager(int argc, char **argv);
	~WindowManager();

	XColor 	getBorderColor()     	const { return xborder_color; }
	XColor 	getForegroundColor() 	const { return xforeground_color; }
	XColor 	getBackgroundColor() 	const { return xbackground_color; }
	
	GC 	getInvertGC() 		const { return invert_gc; }
	GC 	getStringGC() 		const { return string_gc; }
	GC 	getBorderGC() 		const { return border_gc; }
	
	int 	getIHeight() 		const { return iheight; }
	char*  	getMenuFont() 	    	const { return menufont; }
	
	Cursor 	getMoveCursor() 	const { return move_curs; }
	Cursor 	getResizeCursor() 	const { return resize_curs; }
	Cursor 	getLeftArrowCursor() 	const { return left_arrow; }
	
	XFontStruct* getXFont() 	const { return xfont; }

	bool getShape() 		const { return shape; }
	
	void setShape(bool s) { shape=s; }
	
	int getFontPosition() const { return font_position; }

	Theme* getTheme() const { return theme; }

	void changeTheme(char* themeFile);

	// Client List member functions
	void insertClient(Client *c) { ClientList->insert(c); }
	void removeClient(Client *c) { ClientList->remove(c); }

	unsigned int getClientCount() const { return ClientList->count(); }
	LinkedList<Client>* getClientListObject() const { return ClientList; }

	// Server Grab member functions	
	void Grab() { XGrabServer(dpy); }
	void Ungrab() { XUngrabServer(dpy); }
	void SyncUngrab() { XSync(dpy, False); XUngrabServer(dpy); }

	void addToMenuList(BaseMenu *m) { menulist->insert(m); }
	void addToExtraMenuList(BaseMenu *m) { extramenulist->insert(m); }
	void reconfigureMenu();
	void removeFromMenuList(BaseMenu *m) { menulist->remove(m); }
	LinkedList<BaseMenu>* getMenuList() { return menulist; }		
	BaseMenu* findInMenuList(Window *w);
		
  	Atom getWMChangeStateAtom() const { return xa_wm_change_state; }
  	Atom getWMStateAtom() const       { return xa_wm_state; }
  	Atom getWMDeleteAtom() const      { return xa_wm_delete; }
 	Atom getWMProtocolsAtom() const   { return xa_wm_protos; }
  	Atom getWMFocusAtom() const       { return xa_wm_take_focus; }
  	Atom getWMColormapAtom() const    { return xa_wm_cmapwins; }
	Atom getMotifWMHintsAtom() const      { return xa_wm_motif_hints; }

	int 	getDepth() const { return depth; }
	int	getScreen() const { return screen; }
	Visual* getVisual() const { return visual; }
	int	getColorsPerChannel() const { return colors_per_channel; }
	bool    getImageDither() const { return image_dither; }
	BImageControl *getImageControl() const { return image_control; }

	Display* getDisplay() const { return dpy; }
	Window  getRootWindow() const { return root; }

	RootMenu* getRootMenu() const { return rm; }
	IconMenu* getIconMenu() const { return im; }
	Toolbar*  getToolbar() const { return t; }

	void grabPointer(Window w, unsigned int mask, Cursor curs);
	void ungrabPointer();

	void quit_nicely();
	void restart();
	
	int getXRes() const { return xres; }
	int getYRes() const { return yres; }

	char* getClockFormat() const { return clock_format; }
	//void handleKeyPress(XEvent *ev);

	// desktop is from 0 to 15, inclusive (not a bit mask like in changeDesktop)
	void goToDesktop(int desktop);
	void goToNextDesktop();
	void goToPreviousDesktop();

	void setMenu(BaseMenu* bm) { menu=bm; }
	void setMenuEvent(bool me) { menu_event=me; }

private: /* member functions */
	
	int QueryShapeExtentions();
	
	void scan_windows();
	void do_event_loop();	
	void createFontPixmap();
	void createMenuFontPixmap();
	void saveCurrentTheme();
	void getCurrentTheme();
};

extern WindowManager *wm;

#endif
