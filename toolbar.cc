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


#include "sapphire.hh"

Toolbar::Toolbar()
{
	font_width=0;

	frame_pix=None;
	clock_pix=None;
	button_pix=None;
	root_pix=None;

	toolbarMenuVisible=False;

	Month[0]  = "January";
	Month[1]  = "February";
	Month[2]  = "March";
	Month[3]  = "April";
	Month[4]  = "May";
	Month[5]  = "June";
	Month[6]  = "July";
	Month[7]  = "August";
	Month[8]  = "September";
	Month[9]  = "October";
	Month[10] = "November";
	Month[11] = "December";

  	create_mask = CWBackPixmap 	 | 
		      CWBackPixel  	 | 
		      CWBorderPixel      |
		      CWOverrideRedirect | 
		      CWEventMask;
				    
  	attrib.background_pixmap = None;
  	attrib.background_pixel = wm->getBackgroundColor().pixel;
  	attrib.border_pixel = wm->getBorderColor().pixel;
  	attrib.override_redirect = True;
  	attrib.event_mask = StructureNotifyMask |
			    LeaveWindowMask     |
			    ButtonPressMask     | 
			    ButtonReleaseMask   | 
			    ExposureMask;

	default_button_width=15;
	default_button_height=15;

	createToolbarWindow();
	createClockWindow();
	createCycleUpButtonWindow();
  	createCycleDownButtonWindow();
	createStartMenuButtonWindow();

	tm = new ToolbarMenu();
	
	reconfigure();
	
	if(wm->getTheme()->getRootExtraStyle() & ROOT_ON) createRootImage();
	
	if (wm->getTheme()->getToolbarExtraStyle())
	{
		XMapSubwindows(wm->getDisplay(), frame.window);
		XMapWindow(wm->getDisplay(), frame.window);
	}
}

Toolbar::~Toolbar()
{
	wm->getImageControl()->removeImage(clock_pix);
	wm->getImageControl()->removeImage(frame_pix);
	wm->getImageControl()->removeImage(button_pix);

	XDestroyWindow(wm->getDisplay(), clock.window);
	XDestroyWindow(wm->getDisplay(), cycleUp.window);
	XDestroyWindow(wm->getDisplay(), cycleDown.window);
	XDestroyWindow(wm->getDisplay(), startmenu.window);
	XDestroyWindow(wm->getDisplay(), frame.window);

	if(root_pix) wm->getImageControl()->removeImage(root_pix);
		
	delete tm;
}

void Toolbar::createToolbarWindow()
{
	// check to see if toolbar is to be at top or bottom of the screen
  	// then place toolbar in correct location
	if (wm->getTheme()->getToolbarExtraStyle() & TOOLBAR_TOP)
	{
  		frame.width=wm->getXRes() / 2;
  		frame.height = 25;
  		frame.x = wm->getXRes() / 2 - frame.width / 2;
  		frame.y = 0;
  	} else {
  		frame.width = wm->getXRes() / 2;
  		frame.height = 25;
		frame.x = wm->getXRes() / 2 - frame.width / 2;
  		frame.y = wm->getYRes() - frame.height;
	}
  
  	frame.window =
    		XCreateWindow(wm->getDisplay(), wm->getRootWindow(), frame.x, frame.y,
		  frame.width, frame.height, 1, DefaultDepth(wm->getDisplay(), wm->getScreen()),
		  InputOutput, DefaultVisual(wm->getDisplay(), wm->getScreen()), create_mask, &attrib);
}

void Toolbar::createClockWindow()
{
  	clock.x = frame.width - 160;
  	clock.y =  3;
  	clock.width = 130;
  	clock.height = wm->getXFont()->ascent + wm->getXFont()->descent+2;

  	clock.window =  XCreateWindow(wm->getDisplay(), frame.window, clock.x, clock.y,
		  clock.width, clock.height, 1, DefaultDepth(wm->getDisplay(), wm->getScreen()),
		  InputOutput, DefaultVisual(wm->getDisplay(), wm->getScreen()), create_mask, &attrib);
 
}

void Toolbar::createCycleUpButtonWindow()
{
  	cycleUp.x = 36;
	cycleUp.y = 4;
	cycleUp.width = default_button_width;
  	cycleUp.height = default_button_height;

  	cycleUp.window =
    		XCreateWindow(wm->getDisplay(), frame.window, cycleUp.x, cycleUp.y,
		  cycleUp.width, cycleUp.height, 1, DefaultDepth(wm->getDisplay(), wm->getScreen()),
		  InputOutput, DefaultVisual(wm->getDisplay(), wm->getScreen()), create_mask, &attrib);
}

void Toolbar::createCycleDownButtonWindow()
{
  	cycleDown.x = 54;
	cycleDown.y = 4;
	cycleDown.width = default_button_width;
  	cycleDown.height = default_button_height;

  	cycleDown.window =
    		XCreateWindow(wm->getDisplay(), frame.window, cycleDown.x, cycleDown.y,
		  cycleDown.width, cycleDown.height, 1, DefaultDepth(wm->getDisplay(), wm->getScreen()),
		  InputOutput, DefaultVisual(wm->getDisplay(), wm->getScreen()), create_mask, &attrib);
}

void Toolbar::createStartMenuButtonWindow()
{
 	startmenu.x = 10;
	startmenu.y = 4;
	startmenu.width = default_button_width;
  	startmenu.height = default_button_height;

  	startmenu.window =
    		XCreateWindow(wm->getDisplay(), frame.window, startmenu.x, startmenu.y,
		  startmenu.width, startmenu.height, 1, DefaultDepth(wm->getDisplay(), wm->getScreen()),
		  InputOutput, DefaultVisual(wm->getDisplay(), wm->getScreen()), create_mask, &attrib);
}


void Toolbar::createToolBarImage()
{
	unsigned long style=0;

	style = wm->getTheme()->getToolbarStyle();		
	
	frame_pix = wm->getImageControl()->renderImage(
			//"toolbar_frame",
			frame.width, 
			frame.height, 
			style, 
			wm->getTheme()->getToolbarColorFrom(), 
			wm->getTheme()->getToolbarColorTo()
			);

	XSetWindowBackgroundPixmap(wm->getDisplay(), frame.window, frame_pix);
}

void Toolbar::createButtonImage()
{
	unsigned long style=0;

	style = wm->getTheme()->getButtonStyle();		
	
	button_pix = wm->getImageControl()->renderImage(
			//"toolbar_button",
			default_button_width, 
			default_button_height, 
			style, 
			wm->getTheme()->getButtonColorFrom(), 
			wm->getTheme()->getButtonColorTo()
			);
	
	XSetWindowBackgroundPixmap(wm->getDisplay(), startmenu.window, button_pix);
	XSetWindowBackgroundPixmap(wm->getDisplay(), cycleUp.window, button_pix);
	XSetWindowBackgroundPixmap(wm->getDisplay(), cycleDown.window, button_pix);
}

void Toolbar::createClockImage()
{
	unsigned long style=0;

	style = wm->getTheme()->getClockStyle();		
				
	clock_pix = wm->getImageControl()->renderImage(
			//"toolbar_clock",
			clock.width, 
			clock.height, 
			style, 
			wm->getTheme()->getClockColorFrom(), 
			wm->getTheme()->getClockColorTo()
			);

	XSetWindowBackgroundPixmap(wm->getDisplay(), clock.window, clock_pix);
}

void Toolbar::createRootImage()
{
	unsigned long style=0;

	style = wm->getTheme()->getRootStyle();		
				
	root_pix = wm->getImageControl()->renderImage(
			//"root",
			wm->getXRes(), 
			wm->getYRes(), 
			style, 
			wm->getTheme()->getRootColorFrom(), 
			wm->getTheme()->getRootColorTo()
			);
	
	XSetWindowBackgroundPixmap(wm->getDisplay(), wm->getRootWindow(), root_pix);
	XClearWindow(wm->getDisplay(), wm->getRootWindow());
}

void Toolbar::changeTheme()
{	
	XFreePixmap(wm->getDisplay(), clock_pix);
	XFreePixmap(wm->getDisplay(), frame_pix);
	XFreePixmap(wm->getDisplay(), button_pix);

	if(wm->getTheme()->getRootExtraStyle() & ROOT_ON) 
	{
		XFreePixmap(wm->getDisplay(), root_pix);
		createRootImage();
	}

	createToolBarImage();
	createButtonImage();
	createClockImage();

	XClearWindow(wm->getDisplay(), frame.window);
	XClearWindow(wm->getDisplay(), clock.window);
	
	redraw();
}

void Toolbar::reconfigure()
{
	createToolBarImage();
	createButtonImage();
	createClockImage();
	
	XClearWindow(wm->getDisplay(), frame.window);
	XClearWindow(wm->getDisplay(), clock.window);
	XClearWindow(wm->getDisplay(), cycleUp.window);
	XClearWindow(wm->getDisplay(), cycleDown.window);
	XClearWindow(wm->getDisplay(), startmenu.window);
	
	// check to see if toolbar is to be at top or bottom of the screen
  	// then place toolbar in correct location
	if (wm->getTheme()->getToolbarExtraStyle() & TOOLBAR_TOP)
	{
		frame.width=wm->getXRes() / 2;
  		frame.height = 25;
  		frame.x = wm->getXRes() / 2 - frame.width / 2;
  		frame.y = 0;
		
		XMoveResizeWindow(wm->getDisplay(), frame.window, frame.x, frame.y , frame.width, frame.height);
  	} else {
		frame.width = wm->getXRes() / 2;
  		frame.height = 25;
		frame.x = wm->getXRes() / 2 - frame.width / 2;
  		frame.y = wm->getYRes() - frame.height;
		
		XMoveResizeWindow(wm->getDisplay(), frame.window, frame.x, frame.y , frame.width, frame.height);
	}

	checkClock(True);
}

void Toolbar::do_button_press_event(XButtonEvent *e)
{	
	wm->Grab();

	switch (e->button) {
			
		case Button1:
		{
			if (startmenu.window == e->window)
			{
				popup_toolbar_menu();
				break;
			}
			else if(cycleUp.window == e->window)
			{
				cycle_windows_up();
				break;
				
			} else if(cycleDown.window == e->window)
			{
				cycle_windows_down();
				break;
			}
		break;
		}
	
		//case Button2:
		//{ 
		//break;
		//}
				
		//case Button3:
		//{
		//break;
		//}
	}
	
	wm->Ungrab();
}

void Toolbar::redraw()
{
	XClearWindow(wm->getDisplay(), cycleUp.window);
	XClearWindow(wm->getDisplay(), cycleDown.window);
	XClearWindow(wm->getDisplay(), startmenu.window);

	int hh = cycleUp.height / 2, hw = cycleUp.width / 2;

	XPoint cycleDownpts[3];
	cycleDownpts[0].x = hw + 3; 
	cycleDownpts[0].y = hh -2;
	cycleDownpts[1].x = -3; 
	cycleDownpts[1].y = 6;
	cycleDownpts[2].x = -3; 
	cycleDownpts[2].y = -6;
		
	XFillPolygon(wm->getDisplay(), cycleDown.window, wm->getBorderGC(), cycleDownpts, 3, Convex,
	       CoordModePrevious);	
	
	XPoint cycleUppts[3];
	cycleUppts[0].x = hw - 4; 
	cycleUppts[0].y = hh + 4;
	cycleUppts[1].x = 7; 
	cycleUppts[1].y = 0;
	cycleUppts[2].x = -3; 
	cycleUppts[2].y = -7;
    
	XFillPolygon(wm->getDisplay(), cycleUp.window, wm->getBorderGC(), cycleUppts, 3, Convex,
	       CoordModePrevious);		
		
	drawStartMenuButton();

	checkClock(True);
}

void Toolbar::handle_expose_event(XExposeEvent *e)
{
	if(e->count == 0) redraw();
}

void Toolbar::checkClock(bool redraw)
{
  static int hour, minute, day, month, year;
  time_t tmp;
  struct tm *tt=NULL;
  
  if ((tmp = time(NULL)) != -1) {
    tt = localtime(&tmp);
    if (tt->tm_min != minute || tt->tm_hour != hour || day != tt->tm_mday ||
    	month != tt->tm_mon || year != tt->tm_year) {

      hour = tt->tm_hour;
      minute = tt->tm_min;
      day = tt->tm_mday;
      month = tt->tm_mon;
      year = tt->tm_year;
      XClearWindow(wm->getDisplay(), clock.window);
      redraw = True;
    }
  }
  
  if (redraw) {
	char t[25];
	strftime(t, 256,wm->getClockFormat(),tt);
	  
	int len = strlen(t);
	int font_width_test = XTextWidth(wm->getXFont(), t, strlen(t)) + 8;
	
	int x_pos=0;
	
	font_width = font_width_test;
	x_pos =  frame.width - font_width - 5;

	clock.width = font_width;
	//clock.height = wm->getXFont()->ascent + wm->getXFont()->descent * DEFAULT_SPACE - 2;
	clock.height = 20;
	int y_pos = 1;
	createClockImage();
		
	XMoveResizeWindow(wm->getDisplay(), clock.window, x_pos , y_pos , font_width,  clock.height);
	    
	XDrawString(wm->getDisplay(), clock.window, wm->getStringGC(),
		3, 25 / 2 + wm->getXFont()->descent, t, len); 
  }
}

void Toolbar::hideToolbar()
{
	XUnmapSubwindows(wm->getDisplay(), frame.window);
}

void Toolbar::showToolbar()
{
	XMapSubwindows(wm->getDisplay(), frame.window);
	XMapWindow(wm->getDisplay(), frame.window);
}

void Toolbar::popup_toolbar_menu()
{
	XSetWindowBorderWidth(wm->getDisplay(), startmenu.window, 2);
	
	XEvent ev;
	
	for (;;) {
		XMaskEvent(wm->getDisplay(), ButtonReleaseMask | LeaveWindowMask, &ev);
		switch (ev.type) 
		{
			case LeaveNotify:
			{
				XSetWindowBorderWidth(wm->getDisplay(), startmenu.window, 1);
				return;
			}

			
			case ButtonRelease:
			{
				XSetWindowBorderWidth(wm->getDisplay(), startmenu.window, 1);
				
				if(! toolbarMenuVisible)
				{
					wm->reconfigureMenu();
					
					toolbarMenuVisible=True;
					tm->hideAllVisibleSubmenus();
					tm->setAllMenuTitlesVisible(tm, False);
					
					if(wm->getTheme()->getToolbarExtraStyle() & TOOLBAR_TOP)
						tm->show(frame.x, frame.y + frame.height + 1);
					else
						tm->show(frame.x, frame.y - tm->getMenuHeight() - 2);
					
				} else {
					toolbarMenuVisible=False;
					tm->hideAllVisibleSubmenus();
					tm->setAllMenuTitlesVisible(tm, True);
				}
				return;
			}
		}
	}
}

void Toolbar::cycle_windows_up()
{
	XSetWindowBorderWidth(wm->getDisplay(), cycleUp.window, 2);
	
	XEvent ev;
	
	for (;;) {
		XMaskEvent(wm->getDisplay(), ButtonReleaseMask | LeaveWindowMask, &ev);
		switch (ev.type) 
		{
			case LeaveNotify:
			{
				XSetWindowBorderWidth(wm->getDisplay(), cycleUp.window, 1);
				return;
			}
		
			case ButtonRelease:
			{
				XSetWindowBorderWidth(wm->getDisplay(), cycleUp.window, 1);
				XCirculateSubwindowsUp(wm->getDisplay(), wm->getRootWindow());
				return;
			}
		}
	}
}

void Toolbar::cycle_windows_down()
{
	XSetWindowBorderWidth(wm->getDisplay(), cycleDown.window, 2);
	
	XEvent ev;
	
	for (;;) {
		XMaskEvent(wm->getDisplay(), ButtonReleaseMask | LeaveWindowMask, &ev);
		switch (ev.type) 
		{
			case LeaveNotify:
			{
				XSetWindowBorderWidth(wm->getDisplay(), cycleDown.window, 1);
				return;
			}

			case ButtonRelease:
			{
				XSetWindowBorderWidth(wm->getDisplay(), cycleDown.window, 1);
				XCirculateSubwindowsDown(wm->getDisplay(), wm->getRootWindow());
				return;
			}
		}
	}
}

void Toolbar::setStartMenuButtonStateNotPressed()
{
	if(toolbarMenuVisible)
	{
		tm->hideAllVisibleSubmenus();
		toolbarMenuVisible=False;
		drawStartMenuButton();
	} 
}

void Toolbar::drawStartMenuButton()
{
	bool top = wm->getTheme()->getToolbarExtraStyle() & TOOLBAR_TOP;
	
	XClearWindow(wm->getDisplay(), startmenu.window);

	if(top)	
	{
		if (toolbarMenuVisible)
		{		
		// Draws a triangle pointing down 
		//
		XDrawLine(wm->getDisplay(),  startmenu.window,wm->getBorderGC(), 
				3, 6, 12, 6);
		XDrawLine(wm->getDisplay(),  startmenu.window,wm->getBorderGC(), 
				4, 7, 11, 7);
		XDrawLine(wm->getDisplay(),  startmenu.window,wm->getBorderGC(), 
				5, 8, 10, 8);
		XDrawLine(wm->getDisplay(),  startmenu.window,wm->getBorderGC(), 
				6, 9, 9, 9);
		XDrawLine(wm->getDisplay(),  startmenu.window,wm->getBorderGC(), 
				7, 10, 8, 10);	
		} else {

		// Draws a triangle pointing up 
		//
		XDrawLine(wm->getDisplay(),  startmenu.window,wm->getBorderGC(), 
				7, 6, 8, 6);
		XDrawLine(wm->getDisplay(),  startmenu.window,wm->getBorderGC(), 
				6, 7, 9, 7);
		XDrawLine(wm->getDisplay(),  startmenu.window,wm->getBorderGC(), 
				5, 8, 10, 8);
		XDrawLine(wm->getDisplay(),  startmenu.window,wm->getBorderGC(), 
				4, 9, 11, 9);
		XDrawLine(wm->getDisplay(),  startmenu.window,wm->getBorderGC(), 
				3, 10, 12, 10);
		}
	} else {
	
		if (!toolbarMenuVisible)
		{		
		// Draws a triangle pointing down 
		//
		XDrawLine(wm->getDisplay(),  startmenu.window,wm->getBorderGC(), 
				3, 6, 12, 6);
		XDrawLine(wm->getDisplay(),  startmenu.window,wm->getBorderGC(), 
				4, 7, 11, 7);
		XDrawLine(wm->getDisplay(),  startmenu.window,wm->getBorderGC(), 
				5, 8, 10, 8);
		XDrawLine(wm->getDisplay(),  startmenu.window,wm->getBorderGC(), 
				6, 9, 9, 9);
		XDrawLine(wm->getDisplay(),  startmenu.window,wm->getBorderGC(), 
				7, 10, 8, 10);	
		} else {

		// Draws a triangle pointing up 
		//
		XDrawLine(wm->getDisplay(),  startmenu.window,wm->getBorderGC(), 
				7, 6, 8, 6);
		XDrawLine(wm->getDisplay(),  startmenu.window,wm->getBorderGC(), 
				6, 7, 9, 7);
		XDrawLine(wm->getDisplay(),  startmenu.window,wm->getBorderGC(), 
				5, 8, 10, 8);
		XDrawLine(wm->getDisplay(),  startmenu.window,wm->getBorderGC(), 
				4, 9, 11, 9);
		XDrawLine(wm->getDisplay(),  startmenu.window,wm->getBorderGC(), 
				3, 10, 12, 10);
		}
	
	}
}

// ToolbarMenu Class Definition below

ToolbarMenu::ToolbarMenu() : BaseMenu()
{
	setMenuTitle("Toolbar Menu");
	update_all();
	wm->addToExtraMenuList(this);
}

void ToolbarMenu::addMenu(char *title, BaseMenu *sub)
{
	insert(title, sub);
	update_all();
}
