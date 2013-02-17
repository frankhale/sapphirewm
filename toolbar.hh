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


#ifndef _TOOLBAR_HH_
#define _TOOLBAR_HH_

// Toolbar menu is a combination of your icon and root menus. 
class ToolbarMenu : public BaseMenu
{
public:
	ToolbarMenu();
	
	// addMenu inserts an already created menu into the Toolbar menu
	// the idea is that the toolbar menu will be a combination of your
	// icon menu, root menu, and in the future the workspace menu.
	void addMenu(char *title, BaseMenu *sub);
	
};

// The declaration of the Toolbar class. This class describes all the code
// used to maintain a toolbar in Sapphire.
class Toolbar 
{
private: /* variables */
	Pixmap frame_pix;
	Pixmap clock_pix;
	Pixmap button_pix;
	Pixmap root_pix;

	XSetWindowAttributes attrib;
	unsigned long create_mask;

	struct Frame {
		Window window;
		int x,y;
		unsigned int width,height;
	};
	
	Frame frame;
	
	// This describes the default button width/height used by the toolbar. I have
	// adopted the size as 15 pixels for both width and height for buttons. That goes
	// for toolbar buttons aswell.
	int default_button_width;
	int default_button_height;

	// This describes the width of the font used to calculate the size of the clock 
	// window. Its also used in order to place the clock on the toolbar so that its
	// fully visible.
	int font_width;

	// This is the current set of widgets on the toolbar.
	Widget startmenu;
	Widget clock;
	Widget cycleUp;
	Widget cycleDown;
	
	// Used to hold 
	char *Month[12];

	// Represents the toolbar menu
	ToolbarMenu *tm;
	
	// Is the toolbarMenuVisible? 
	bool toolbarMenuVisible;

public: /* member functions */

	// Toolbar constructor this creates the Sapphire toolbar.
	Toolbar();
	
	// Destructor takes care of clean up (ie. deleting the pixmaps, detroying windows
	// no longer needed, and removing images from the image cache.
	~Toolbar();

	// lets another class gain access to the toolbar menu.
	ToolbarMenu *getToolbarMenu() const { return tm; } 

	// If things change then we need to reconfigure
	void reconfigure();
	
	void do_button_press_event(XButtonEvent *e);
	void handle_expose_event(XExposeEvent *e);	
	
	void redraw();	
	void checkClock(bool redraw=False);
	
	void hideToolbar();
	void showToolbar();
	
	// I use this to reset the start button if you select the menu
	// from the root background. If the menu is popped up on the start
	// menu then we have to reset and hide that menu since it contains the same
	// menu's as the ones which pops up when you right click or middle click
	// on the root window.
	void setStartMenuButtonStateNotPressed();
	void changeTheme();	

	//void desktopChanged();
private:
	
	// The next set of 5 functions create the windows which are associated with
	// the Sapphire toolbar.
	void createToolbarWindow();
	void createClockWindow();
	void createCycleUpButtonWindow();
	void createCycleDownButtonWindow();
	void createStartMenuButtonWindow();;
	
	// The next 4 functions are used to create the images and pixmaps which are 
	// used to make the toolbar look cool.
	void createToolBarImage();
	void createButtonImage();
	void createClockImage();
	void createRootImage();
	
	// The next 3 functions are button handler functions. These dispatch the code
	// which executes the various functions the toolbar offers.
	void popup_toolbar_menu();
	void cycle_windows_up();
	void cycle_windows_down();

	// This function draws the start menu button in two different states. 
	// 
	// 	- pointing up
	//	- pointing down
	void drawStartMenuButton();
};

#endif
