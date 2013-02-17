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

Client::Client() 
{
	// One thing I cannot stand is fucking variables that are cryptic.
	// I gotta lay off the dope when I code, shit!
	
	shaped	 = false;
	moving	 = false;
	shaded	 = false;
	maxsized = false;

	name     = "unknown";
	
	// Defaults for this Clients x, y position and width and height. 
	x	= 1;
	y	= 1;
	width	= 1;
	height	= 1;

	oumx	  = 1;
	oumy	  = 1;
	oumwidth  = 1;
	oumheight = 1;

	old_width  = width;
	old_height = height;

	default_titlebar_height = wm->getIHeight() + DEFAULT_SPACE;
	default_client_name_x = DEFAULT_SPACE + 20;
	default_client_name_y = wm->getFontPosition(); //DEFAULT_SPACE + wm->getXFont()->ascent + 2;

	default_button_width= 13;
	default_button_height= 13;

	client_position[40]=0;
	text_width=0;

	right_sidebar_width=10;

	// These are set to there real defaults in the reparent function.
	default_close_button_x = 0;
	default_close_button_y = 0;
	default_shade_button_x = 0;
	default_shade_button_y = 0;
	default_minimize_button_x = 0;
	default_minimize_button_y = 0;	
	default_maximize_button_x = 0;
	default_maximize_button_y = 0;

	close_button_pressed=false;
	shade_button_pressed=false;
	minimize_button_pressed=false;
	maximize_button_pressed=false;

	// Mouse Button defaults
	button1=button2=button3=false;
	
	// Window Decorations
	//
	// The parent_window is the main parent. The titlebar is a child of the parent_window
	// all other button windows are childs of the titlebar.
	has_titlebar=true;
	has_close_button=true;
	has_minimize_button=true;
	has_maximize_button=true;
	has_shade_button=true;

	frame_pix=None;	// Pixmap used for the titlebar
	button_pix=None;// Pixmap used for titlebar buttons

	ignore_unmap=0;

	close.window = maximize.window 
		     = minimize.window 
		     = shadeButton.window
		     = window 
		     = parent 
		     = transient 
		     = title
		     = right_sidebar
		     = resize_win
		     = None;
}

Client::~Client() 
{
}

Window Client::createTopLevelWindow(int x, int y, 
	unsigned int width,unsigned int height,unsigned int borderwidth)
{
	XSetWindowAttributes attrib;

	unsigned long create_mask = CWOverrideRedirect | 
		CWBackPixmap | CWBackPixel | CWBorderPixel | CWCursor | 
		CWEventMask;

	attrib.background_pixmap = None;
	attrib.background_pixel = wm->getBackgroundColor().pixel;
	attrib.border_pixel = wm->getBorderColor().pixel;
	attrib.override_redirect = true;
	attrib.cursor = XCreateFontCursor(wm->getDisplay(), XC_left_ptr);
	attrib.event_mask =  StructureNotifyMask | SubstructureNotifyMask |
		SubstructureRedirectMask | ExposureMask | ButtonMotionMask | 
		ButtonPressMask | ButtonReleaseMask | FocusChangeMask;
	
	return (XCreateWindow(wm->getDisplay(), wm->getRootWindow(), x,y,width,height,borderwidth,
		DefaultDepth(wm->getDisplay(), wm->getScreen()), InputOutput, DefaultVisual(wm->getDisplay(), wm->getScreen()),
		create_mask, &attrib));
}

Window Client::createChildWindow(Window parent, int x, int y, 
	unsigned int width,unsigned int height,unsigned int borderwidth)
{
	XSetWindowAttributes attrib;

	unsigned long create_mask = 
		CWBackPixmap | CWBackPixel | CWBorderPixel | CWCursor | 
		CWEventMask;

	attrib.background_pixmap = None;
	attrib.background_pixel = wm->getBackgroundColor().pixel;
	attrib.border_pixel = wm->getBorderColor().pixel;
	attrib.cursor = XCreateFontCursor(wm->getDisplay(), XC_left_ptr);
	attrib.event_mask = StructureNotifyMask | SubstructureNotifyMask |
		SubstructureRedirectMask | ExposureMask | EnterWindowMask |
		LeaveWindowMask | ButtonMotionMask | ButtonPressMask | 
		ButtonReleaseMask;
	
	return (XCreateWindow(wm->getDisplay(), parent, x,y,width,height,borderwidth,
		DefaultDepth(wm->getDisplay(), wm->getScreen()), InputOutput, DefaultVisual(wm->getDisplay(), wm->getScreen()),
		create_mask, &attrib));
}

void Client::make_new_client(Window w)
{
	long dummy;
	XWMHints *hints;
	XWindowAttributes attr;

	// Populate the hints structure with the window hints (w) has.
	hints = XGetWMHints(wm->getDisplay(), w);

	// Create a new Client object.
	Client *c = new Client();
	// Insert the client into the window manager client list.
	wm->insertClient(c);

	// Assign the window (w) to c->window, c->window represents the application.
	c->window = w;
	c->ignore_unmap = 0;

	XGetWindowAttributes(wm->getDisplay(), c->window, &attr);

	c->x = attr.x;
	c->y = attr.y;
	c->width = attr.width;
	c->height = attr.height;
	c->cmap = attr.colormap;

	c->size = XAllocSizeHints();

	init_position(c);
	
	XGetWMNormalHints(wm->getDisplay(), c->window, c->size, &dummy);

	XSelectInput(wm->getDisplay(), c->window, ColormapChangeMask | PropertyChangeMask | EnterWindowMask );
	
	getTransients(c);

	if (XFetchName(wm->getDisplay(), c->window, &c->name)==0)
	  c->name=NULL;

	if (isViewable(c)) c->ignore_unmap++;

	change_gravity(c, 1);

	// Determine if this client needs window decorations.
	setButtonPositionDefaults(c);
	determineWindowDecorations(c);
	
	if (hints) XFree(hints);
	
	// Do all the necessary reparenting for this client
	reparent(c);
	
	// If this client should be iconic on start then we won't map it.
	isIconic(c);
		
	XAddToSaveSet(wm->getDisplay(), c->window);		
	XSetWindowBorderWidth(wm->getDisplay(), c->window, 0);
}

void Client::init_position(Client *c)
{
	/* Adapted from aewm 1.1.2 */
	
	int xmax = wm->getXRes();
    	int ymax = wm->getYRes();
    	int mouse_x=0, mouse_y=0;

    	if (c->size->flags & (USSize)) 
	{
        	c->width = c->size->width;
	        c->height = c->size->height;
    	} else {
        	/* we would check PSize here, if GTK didn't blow goats */
	        /* make sure it's big enough to click at */
	       if (c->width < 2 * theight(c)) c->width = 2 * theight(c);
       	       if (c->height < theight(c)) c->height = theight(c);
    	}

    	if (c->size->flags & USPosition) 
	{
        	c->x = c->size->x;
        	c->y = c->size->y;
    	} else {
        	if (c->size->flags & PPosition) 
		{
            		c->x = c->size->x;
            		c->y = c->size->y;
        	}

	        if (c->x < 0) c->x = 0;
	        if (c->y < 0) c->y = 0;
        	if (c->x > xmax) c->x = xmax - 5;
	        if (c->y > ymax) c->y = ymax - 5;
    	}

    	if (c->x == 0 && c->y == 0) 
	{
        	get_mouse_position(&mouse_x, &mouse_y);

        	if (c->width < xmax)
        		c->x = (mouse_x / 2);
   
        	if (c->height + theight(c) < ymax)
  			c->y = (mouse_y / 2);

        	c->y += theight(c);
		change_gravity(c, -1);
    	}
}

void Client::remove_client(Client *c)
{
	XEvent foo;

	XGrabServer(wm->getDisplay());
	
	if(! XCheckTypedWindowEvent(wm->getDisplay(), c->window, DestroyNotify, &foo))
	{
		// Sets input focus back to the root window
		XSetInputFocus(wm->getDisplay(), wm->getRootWindow(), RevertToPointerRoot, CurrentTime);
		
		change_gravity(c, -1);
		
		XReparentWindow(wm->getDisplay(), c->window, wm->getRootWindow(), c->x, c->y);
		XRemoveFromSaveSet(wm->getDisplay(), c->window);
		
		XDestroyWindow(wm->getDisplay(), c->close.window);
		XDestroyWindow(wm->getDisplay(), c->maximize.window);
		XDestroyWindow(wm->getDisplay(), c->minimize.window);
		XDestroyWindow(wm->getDisplay(), c->shadeButton.window);
		XDestroyWindow(wm->getDisplay(), c->right_sidebar);
		XDestroyWindow(wm->getDisplay(), c->resize_win);
		XDestroyWindow(wm->getDisplay(), c->title);
		XDestroyWindow(wm->getDisplay(), c->parent);
					
		if (c->name) XFree(c->name);
		if (c->size) XFree(c->size);

		wm->getImageControl()->removeImage(c->frame_pix);
		wm->getImageControl()->removeImage(c->right_pix);
		wm->getImageControl()->removeImage(c->button_pix);
		wm->getImageControl()->removeImage(c->resize_pix);

		wm->removeClient(c);
	
		delete c;
	} 
	
	XSync(wm->getDisplay(), false);	
	XUngrabServer(wm->getDisplay());
}

void Client::setButtonPositionDefaults(Client *c)
{
	c->default_close_button_x = 4;
	c->default_close_button_y = theight(c) / 2 - 7;
	
	c->default_shade_button_x = c->width - 44;
	c->default_shade_button_y = theight(c) / 2 - 7;
	
	c->default_minimize_button_x = c->width - 27;
	c->default_minimize_button_y = theight(c) / 2 - 7;	
	
	c->default_maximize_button_x = c->width - 10;
	c->default_maximize_button_y = theight(c) / 2 - 7;
}

void Client::reparent(Client *c)
{		
	createParentWindow(c);
	createTitlebarWindow(c);
	createRightSidebarWindow(c);
	createResizeWindow(c);
	createCloseButtonWindow(c);
	createMinimizeButtonWindow(c);
	createMaximizeButtonWindow(c);
	createShadeButtonWindow(c);

	if (wm->getShape()) {
		XShapeSelectInput(wm->getDisplay(), c->window, ShapeNotifyMask);
		set_shape(c);
	}
	
	// If this window doesn't request that it shouldn't be reparented
	// then we will go ahead an reparent it to c->parent which is the 
	// parent window which holds the client application window ( c->window ) aswell
	// as the titlebar window and the button windows.
	if (c->has_titlebar) 
	{
		int x, y;
		unsigned int width, height, border, depth;
		Window win;
		XGetGeometry(wm->getDisplay(), c->parent, &win, &x, &y, &width, &height, &border, &depth);

		XReparentWindow(wm->getDisplay(), c->window, c->parent, 0, theight(c));
	
		if(y <= theight(c)) 
		{
			XMoveWindow(wm->getDisplay(), c->parent, x, y);
		
			c->y = theight(c);
		}
		else {
		
			XMoveWindow(wm->getDisplay(), c->parent, x, y - theight(c));
			c->y = y;
		}
	}
	else 
	{
		// If we don't need to reparent it with window decorations then we
		// reparent it back to the root window and move it to where it wishes
		// to be when mapped.
		XReparentWindow(wm->getDisplay(), c->window, wm->getRootWindow(), 0, 0);
		XMoveWindow(wm->getDisplay(), c->window, c->x, c->y);
		
	}

	createButtonImage(c);	
	createResizeImage(c);

	send_config(c);
}

void Client::createParentWindow(Client *c)
{
	c->parent = createTopLevelWindow
			(
				c->x,
				c->y - theight(c),
				c->width + right_sidebar_width, 
				c->height + theight(c), 
				DEFAULT_BORDER_SIZE
			);
	
}

void Client::createTitlebarWindow(Client *c)
{
	c->title = createChildWindow
			(
				c->parent, 
				0, 
				0, 
				c->width + right_sidebar_width, 
				theight(c),
				0
			);
}

void Client::createRightSidebarWindow(Client *c)
{
	c->right_sidebar = createChildWindow
			(
				c->parent, 
				c->width, 
				theight(c), 
				right_sidebar_width, 
				c->height - 10,
				1
			);
}

void Client::createResizeWindow(Client *c)
{
	c->resize_win = createChildWindow
			(
				c->parent, 
				c->width, 
				c->height + 10, 
				right_sidebar_width, 
				10,
				1
			);
}

void Client::createCloseButtonWindow(Client *c)
{
	c->close.x      = c->default_close_button_x;
	c->close.y      = c->default_close_button_y;
	c->close.width  = c->default_button_width;
	c->close.height = c->default_button_height;
	
	c->close.window = createChildWindow
			(
				c->title, 
				c->close.x, 
				c->close.y, 
				c->close.width, 
				c->close.height,
				1
			);
}

void Client::createMinimizeButtonWindow(Client *c)
{
	c->minimize.x = c->default_minimize_button_x;
	c->minimize.y = c->default_minimize_button_y;
	c->minimize.width =  c->default_button_width;
	c->minimize.height =  c->default_button_height;

	c->minimize.window = createChildWindow
			(
				c->title, 
				c->minimize.x, 
				c->minimize.y, 
				c->minimize.width, 
				c->minimize.height,
				1
			);
}

void Client::createMaximizeButtonWindow(Client *c)
{
	c->maximize.x      = c->default_maximize_button_x;
	c->maximize.y      = c->default_maximize_button_y;
	c->maximize.width  = c->default_button_width;
	c->maximize.height = c->default_button_height;

	c->maximize.window = createChildWindow
			(
				c->title, 
				c->maximize.x, 
				c->maximize.y,
				c->maximize.width, 
				c->maximize.height,
				1
			);
}

void Client::createShadeButtonWindow(Client *c)
{
	c->shadeButton.x      = c->default_shade_button_x;
	c->shadeButton.y      = c->default_shade_button_y;
	c->shadeButton.width  = c->default_button_height;
	c->shadeButton.height = c->default_button_height;

	c->shadeButton.window = createChildWindow
			(
				c->title, 
				c->shadeButton.x, 
				c->shadeButton.y,
				c->shadeButton.width, 
				c->shadeButton.height,
				1
			);	
}

int Client::theight(Client *c) const 
{ 
	//return ((c->transient ? 2*DEFAULT_SPACE : wm->iheight) + DEFAULT_SPACE); 

	// For now this function will pass a Client object. Even though we don't 
	// use it for much, I still might want to have transients have a different style
	// than there top level parents later on.
	//
	// For instance I could turn off some of the titlebar buttons, or I could change the
	// height of the client's titlebar and button width and height. 
	//
	// This is the reason why I am passing the Client object to this function. I simply
	// haven't made up my mind at this point.
	return c->default_titlebar_height; 
} 

void Client::getTransients(Client *c)
{
	Window trans;
	
	XGetTransientForHint(wm->getDisplay(), c->window, &trans);
	
	c->transient = trans;
}

void Client::determineWindowDecorations(Client *c)
{
	// This code was ripped from somewhere but since Client.cc
	// is coded so fucking badly it doesn't work for anything.

	int format;
	Atom atom_return;
	unsigned long num, len;
      
	if (XGetWindowProperty(wm->getDisplay(), c->window,
                         wm->getMotifWMHintsAtom(), 0,
                         PropMwmHintsElements, false,
                         wm->getMotifWMHintsAtom(), &atom_return,
                         &format, &num, &len,
                         (unsigned char **) &c->mwm_hint) == Success && c->mwm_hint)
	
	if (num == PropMwmHintsElements) {
		if (c->mwm_hint->flags & MwmHintsDecorations)
		{
			if (c->mwm_hint->decorations & MwmDecorAll) 
			{
				c->has_titlebar=c->has_close_button=c->has_minimize_button=c->has_maximize_button=c->has_shade_button=true;
	    		}
	    		else {
				c->has_titlebar=c->has_close_button=c->has_minimize_button=c->has_maximize_button=c->has_shade_button=false;
	      
	      			if (c->mwm_hint->decorations & MwmDecorTitle) c->has_titlebar = true;
				
				c->has_minimize_button = true;
				c->has_maximize_button = true;
			
				c->has_shade_button=true;
				c->has_close_button=true;
	    		}
		}
	}}

Client *Client::find_client(Window w)
{
	if (w && w != wm->getRootWindow()) 
	{
		if(wm->getClientCount()) 
		{
			LinkedListIterator<Client> it(wm->getClientListObject());
			for(; it.current(); it++)
			{
				if (w == it.current()->parent ||
				    w == it.current()->title ||
				    w == it.current()->window ||
				    w == it.current()->close.window ||
				    w == it.current()->maximize.window ||
				    w == it.current()->minimize.window ||
				    w == it.current()->resize_win ||
				    w == it.current()->right_sidebar ||
				    w == it.current()->shadeButton.window )
				{
					return it.current();
				} 
			}
		}
	}

	return NULL;
}

int Client::isViewable(Client *c)
{
	XWindowAttributes attr;

	XGetWindowAttributes(wm->getDisplay(), c->window, &attr);
	
	return attr.map_state;
}

void Client::createTitleImage(Client* c)
{
	unsigned long style=0;

	style = wm->getTheme()->getTitlebarStyle();
						
	c->frame_pix = wm->getImageControl()->renderImage(
			c->width+c->right_sidebar_width, 
			theight(c), 
			style, 
		       	wm->getTheme()->getClientColorFrom(), 
		      	wm->getTheme()->getClientColorTo());
			
	XSetWindowBackgroundPixmap(wm->getDisplay(), c->title, c->frame_pix);
}

void Client::createRightSidebarImage(Client *c)
{
	unsigned long style=0;

	style = wm->getTheme()->getRightSidebarStyle();
		
	c->right_pix = wm->getImageControl()->renderImage(
			10, 
			c->height + theight(c) - 10, 
			style, 
		       	wm->getTheme()->getRightSidebarColorFrom(), 
		      	wm->getTheme()->getRightSidebarColorTo());

	XSetWindowBackgroundPixmap(wm->getDisplay(), c->right_sidebar, c->right_pix);
}

void Client::createResizeImage(Client *c)
{
	unsigned long style=0;

	style = wm->getTheme()->getResizeStyle();
		
	c->resize_pix = wm->getImageControl()->renderImage(
			10, 
			10, 
			style, 
		       	wm->getTheme()->getResizeColorFrom(), 
		      	wm->getTheme()->getResizeColorTo());

	XSetWindowBackgroundPixmap(wm->getDisplay(), c->resize_win, c->resize_pix);

}

void Client::createButtonImage(Client* c)
{
	unsigned long style=0;

	style = wm->getTheme()->getButtonStyle();		
	
	c->button_pix = wm->getImageControl()->renderImage(
			c->default_button_width, 
			c->default_button_height, 
			style, 
			wm->getTheme()->getButtonColorFrom(), 
			wm->getTheme()->getButtonColorTo());
	
	
	XSetWindowBackgroundPixmap(wm->getDisplay(), c->shadeButton.window, c->button_pix);
	XSetWindowBackgroundPixmap(wm->getDisplay(), c->close.window, c->button_pix);
	XSetWindowBackgroundPixmap(wm->getDisplay(), c->minimize.window, c->button_pix);
	XSetWindowBackgroundPixmap(wm->getDisplay(), c->maximize.window, c->button_pix);
}

void Client::reconfigure(Client *c)
{	
	createTitleImage(c);
	createRightSidebarImage(c);
	
	redraw(c);
}

void Client::send_config(Client *c)
{
	XConfigureEvent ce;
	XEvent foo;
	
	ce.type = ConfigureNotify;
	ce.event = c->window;
	ce.window = c->window;
	ce.x = c->x;
	ce.y = c->y;
	ce.width = c->width;
	ce.height = c->height;
	ce.border_width = 0;
	ce.above = None;
	ce.override_redirect = 0;
	
	if(! XCheckTypedWindowEvent(wm->getDisplay(), c->window, DestroyNotify, &foo))
	{
		if(! moving) reconfigure(c);
	}
	
	XSendEvent(wm->getDisplay(), c->window, false, StructureNotifyMask, (XEvent *)&ce);
}

void Client::move(Client *c)
{
	if (c) {	
		moving=true;
		
		drag(c);
 		XMoveWindow(wm->getDisplay(), c->parent, c->x, c->y -theight(c));
		send_config(c);
		
		moving=false;
	}
}

void Client::drag(Client *c)
{
	XEvent ev;
	int x1=c->x;
	int y1=c->y;
	int old_cx = c->x;
	int old_cy = c->y;

	wm->grabPointer(wm->getRootWindow(), ButtonPressMask | ButtonReleaseMask | PointerMotionMask, wm->getMoveCursor());
	
	XGrabServer(wm->getDisplay());
	
	get_mouse_position(&x1, &y1);

	draw_outline(c);
	
	for (;;) {
		XMaskEvent(wm->getDisplay(), ButtonPressMask | ButtonReleaseMask | PointerMotionMask, &ev);
		switch (ev.type) {
			case MotionNotify:
				draw_outline(c); /* clear */
				c->x = old_cx + (ev.xmotion.x_root - x1);
				c->y = old_cy + (ev.xmotion.y_root - y1);
				draw_outline(c);
			break;
				
			case ButtonRelease:
				draw_outline(c); /* clear */
				XUngrabServer(wm->getDisplay());
				wm->ungrabPointer();
				return;
		}
	}
}

void Client::resize(Client *c)
{
	if (c) {
		sweep(c);
		
		if (c->maxsized) {
			c->maxsized = false;
		}

		XMoveResizeWindow(wm->getDisplay(), c->parent, c->x, c->y - theight(c), c->width+c->right_sidebar_width, c->height + theight(c));			
		XMoveResizeWindow(wm->getDisplay(), c->title, 0, 0, c->width+c->right_sidebar_width, theight(c));
		XMoveResizeWindow(wm->getDisplay(), c->right_sidebar, c->width, theight(c), c->right_sidebar_width, c->height - 10); 		
		XMoveResizeWindow(wm->getDisplay(), c->resize_win, c->width, c->height + 10, 10, 10); 		
		XMoveWindow(wm->getDisplay(), c->maximize.window, c->width - 10, c->maximize.y);
		XMoveWindow(wm->getDisplay(), c->minimize.window, c->width - 27, c->minimize.y);
		XMoveWindow(wm->getDisplay(), c->shadeButton.window, c->width - 44, c->shadeButton.y);
		XMoveResizeWindow(wm->getDisplay(), c->window, 0, theight(c), c->width, c->height);	
		
		send_config(c);
	}
}

void Client::recalculate_sweep(Client *c, int x1, int y1, int x2, int y2)
{
	int basex=0;
	int basey=0;

	c->width  = abs(x1 - x2) - DEFAULT_BORDER_SIZE;
	c->height = abs(y1 - y2) - DEFAULT_BORDER_SIZE;

	if (c->size->flags & PResizeInc) {
	
		basex = (c->size->flags & PBaseSize) ? c->size->base_width :
			(c->size->flags & PMinSize) ? c->size->min_width : c->x;
		basey = (c->size->flags & PBaseSize) ? c->size->base_height :
			(c->size->flags & PMinSize) ? c->size->min_height : c->y;
		c->width -= (c->width - basex) % c->size->width_inc;
		c->height -= (c->height - basey) % c->size->height_inc;
	}
	
	if (c->size->flags & PMinSize) {
		if (c->width < c->size->min_width) c->width = c->size->min_width;
		if (c->height < c->size->min_height) c->height = c->size->min_height;
	}
	
	if (c->size->flags & PMaxSize) {
		if (c->width > c->size->max_width) c->width = c->size->max_width;
		if (c->height > c->size->max_height) c->height = c->size->max_height;
	}
	
	c->x = (x1 <= x2) ? x1 : x1 - c->width;
	c->y = (y1 <= y2) ? y1 : y1 - c->height;
}

void Client::sweep(Client *c)
{
	XEvent ev;
	int old_cx = c->x;
	int old_cy = c->y;

	wm->grabPointer(wm->getRootWindow(), ButtonReleaseMask | PointerMotionMask, wm->getResizeCursor());
	
	draw_outline(c);
	
	for (;;) {
		XMaskEvent(wm->getDisplay(), ButtonPressMask | ButtonReleaseMask | PointerMotionMask, &ev);
		switch (ev.type) {
			case MotionNotify:
				draw_outline(c); 				
				recalculate_sweep(c, old_cx, old_cy, ev.xmotion.x_root , ev.xmotion.y_root);				
				draw_outline(c);
			break;
				
			case ButtonRelease:
				draw_outline(c);
				wm->ungrabPointer();
			return;
		}
	}
}

void Client::draw_outline(Client *c)
{
	if(c->shaded)
	{
		XDrawRectangle(wm->getDisplay(), wm->getRootWindow(), wm->getInvertGC(),
			c->x, 							/* x */
			c->y - theight(c) + DEFAULT_BORDER_SIZE/2,		/* y */
			c->width+10,						/* width */
			theight(c) + DEFAULT_BORDER_SIZE);			/* height */			
					
	} else {
		
		XDrawRectangle(wm->getDisplay(), wm->getRootWindow(), wm->getInvertGC(),
		c->x + DEFAULT_BORDER_SIZE/2,                   	/* x */
		c->y - theight(c) + DEFAULT_BORDER_SIZE/2,		/* y */
		c->width + 10 + DEFAULT_BORDER_SIZE, 			/* width */
		c->height + theight(c) + DEFAULT_BORDER_SIZE);	/* height */

		sprintf(c->client_position, "%dx%d+%d+%d", 
			c->width+c->right_sidebar_width, 
			c->height+theight(c), 
			c->x, 
			c->y);
		
		c->text_width=XTextWidth(wm->getXFont(), c->client_position, strlen(c->client_position));
	
		XDrawString(wm->getDisplay(), 
			wm->getRootWindow(), 
			wm->getInvertGC(),
			c->x + c->width - XTextWidth(wm->getXFont(), 
			c->client_position, 
			strlen(c->client_position)),
			c->y + c->height - DEFAULT_SPACE,
			c->client_position, strlen(c->client_position));
	
	
	}
}

void Client::maximumresize(Client *c)
{
	// Check to see if this window can be resized more
	// if its at its max size then lets leave it alone.
	if (c && !(c->size->flags & PMaxSize) ) {

		if (c->maxsized)
		{
			c->x = c->oumx;
			c->y = c->oumy;
			c->width = c->oumwidth;
			c->height = c->oumheight;
			c->maxsized = false;
		} else {
			c->oumx = c->x;
			c->oumy = c->y;
			c->oumwidth = c->width;
			c->oumheight = c->height;

			c->x = 1;

			if (wm->getTheme()->getToolbarExtraStyle() & TOOLBAR_TOP)
			{
				c->y = 28 + theight(c);
				c->width = wm->getXRes() - 14;
				c->height = wm->getYRes() - theight(c) - 34;
				c->maxsized = true;
 	  		
			} else {
				c->y = 0 + theight(c);

				c->width = wm->getXRes() - 14;
				c->height = wm->getYRes() - 28 - theight(c);
				c->maxsized = true;
 			}
		}

		XMoveResizeWindow(wm->getDisplay(), c->parent, c->x, c->y - theight(c), c->width + c->right_sidebar_width, c->height + theight(c));
		XMoveResizeWindow(wm->getDisplay(), c->title, 0, 0, c->width+c->right_sidebar_width, theight(c)); 
		XMoveResizeWindow(wm->getDisplay(), c->right_sidebar, c->width, theight(c), c->right_sidebar_width, c->height - 10);
		XMoveResizeWindow(wm->getDisplay(), c->resize_win, c->width, c->height + 10, right_sidebar_width, 10); 	
		XMoveWindow(wm->getDisplay(), c->maximize.window, c->width - 10, c->maximize.y);
		XMoveWindow(wm->getDisplay(), c->minimize.window, c->width - 27, c->minimize.y);
		XMoveWindow(wm->getDisplay(), c->shadeButton.window, c->width - 44, c->shadeButton.y);
		XMoveResizeWindow(wm->getDisplay(), c->window, 0, theight(c), c->width, c->height);
			
		send_config(c);
	}
}

void Client::hide(Client *c)
{
	c->ignore_unmap++;
	
	if(c->has_titlebar)
	{
		XUnmapSubwindows(wm->getDisplay(), c->parent);
		XUnmapWindow(wm->getDisplay(), c->parent);
	} else {
		XUnmapWindow(wm->getDisplay(), c->window);
	}
	
	set_wm_state(c, IconicState);
	
	wm->getIconMenu()->addIcon(c->name, c);
}

void Client::unhide(Client *c)
{
	mapClient(c);
}

// This repaints the titlebar and buttons on it, It also paints the window title.
void Client::redraw(Client *c)
{
	if (c->has_titlebar)
	{		
		XClearWindow(wm->getDisplay(), c->title);

		XClearWindow(wm->getDisplay(), c->close.window);
		XClearWindow(wm->getDisplay(), c->minimize.window);		
		XClearWindow(wm->getDisplay(), c->maximize.window);
		XClearWindow(wm->getDisplay(), c->shadeButton.window);
		XClearWindow(wm->getDisplay(), c->right_sidebar);
		XClearWindow(wm->getDisplay(), c->resize_win);
		
		// Close Button		
		XDrawLine(wm->getDisplay(), c->close.window, wm->getBorderGC(), 2,2,c->close.width - 3,c->close.height-3);
		XDrawLine(wm->getDisplay(), c->close.window, wm->getBorderGC(), 2,c->close.height - 3,c->close.width-3,2);

		// Maximize Button
		XDrawRectangle(wm->getDisplay(), c->maximize.window,
			wm->getBorderGC(), 2, 2, c->maximize.width - 5, c->maximize.height - 5);
 		XDrawLine(wm->getDisplay(),  c->maximize.window,
	  		wm->getBorderGC(), 2, 3, c->maximize.width - 3, 3);

		// Minimize Button
		XDrawRectangle(wm->getDisplay(), c->minimize.window,
			wm->getBorderGC(), 2, c->minimize.height - 5, c->minimize.width - 5, 2);
				
		draw_shade_button(c);
				
		if (c->name) 
		{
			XDrawString(wm->getDisplay(), 
				c->title, 
				wm->getStringGC(), 
				c->default_client_name_x, 
				c->default_client_name_y, 
				c->name, 
				strlen(c->name));		
		}
	}
}

void Client::draw_shade_button(Client *c)
{
	if (c->shaded)
	{		
		// Draws a triangle pointing down which indicates that this
		// window is shaded.
		//
		XDrawLine(wm->getDisplay(),  c->shadeButton.window,wm->getBorderGC(), 
				2, 5, 11, 5);
		XDrawLine(wm->getDisplay(),  c->shadeButton.window,wm->getBorderGC(), 
				3, 6, 10, 6);
		XDrawLine(wm->getDisplay(),  c->shadeButton.window,wm->getBorderGC(), 
				4, 7, 9, 7);
		XDrawLine(wm->getDisplay(),  c->shadeButton.window,wm->getBorderGC(), 
				5, 8, 8, 8);
		XDrawLine(wm->getDisplay(),  c->shadeButton.window,wm->getBorderGC(), 
				6, 9, 7, 9);	
	} else {

		// Draws a triangle pointing up which indicates that this
		// window is not shaded.
		//
		XDrawLine(wm->getDisplay(),  c->shadeButton.window,wm->getBorderGC(), 
				6, 5, 7, 5);
		XDrawLine(wm->getDisplay(),  c->shadeButton.window,wm->getBorderGC(), 
				5, 6, 8, 6);
		XDrawLine(wm->getDisplay(),  c->shadeButton.window,wm->getBorderGC(), 
				4, 7, 9, 7);
		XDrawLine(wm->getDisplay(),  c->shadeButton.window,wm->getBorderGC(), 
				3, 8, 10, 8);
		XDrawLine(wm->getDisplay(),  c->shadeButton.window,wm->getBorderGC(), 
				2, 9, 11, 9);
	}
}

void Client::change_gravity(Client *c, int multiplier)
{
	int dy = 0;
	int gravity = (c->size->flags & PWinGravity) ?
		c->size->win_gravity : NorthWestGravity;

	switch (gravity) {
		case NorthWestGravity:
		case NorthEastGravity:
		case NorthGravity: dy = theight(c); break;
		case CenterGravity: dy = theight(c)/2; break;		
	}

	c->y += multiplier * dy;
}

void Client::set_shape(Client *c)
{
	int n, order;
	XRectangle *rect;

	rect = XShapeGetRectangles(wm->getDisplay(), c->window, ShapeBounding, &n, &order);
	if (n > 1) XShapeCombineShape(wm->getDisplay(),
		c->parent, ShapeBounding, 0, theight(c),
		c->window, ShapeBounding, ShapeSet);
	XFree(rect);
}

void Client::get_mouse_position(int *x, int *y)
{
	Window dw1=None;
	Window dw2=None;
	int t1=0;
	int t2=0;
	unsigned int t3=0;

	XQueryPointer(wm->getDisplay(), wm->getRootWindow(), &dw1, &dw2, x, y, &t1, &t2, &t3);
}

void Client::shade(Client *c)
{
	if (c->shaded)
	{
	   	XResizeWindow(wm->getDisplay(), c->parent, c->width+10, c->height + theight(c));
	   	c->shaded=false;
	} else {
	   	XResizeWindow(wm->getDisplay(), c->parent, c->width+10, theight(c));
	  	c->shaded=true;
	}
}

void Client::closeButtonPressed(Client *c)
{
	XSetWindowBorderWidth(wm->getDisplay(), c->close.window, 2);
	
	XEvent ev;
	
	for (;;) {
		XMaskEvent(wm->getDisplay(), ButtonReleaseMask | LeaveWindowMask, &ev);
		switch (ev.type) 
		{
			case LeaveNotify:
			{
				XSetWindowBorderWidth(wm->getDisplay(), c->close.window, 1);
				return;
			}
			
			case ButtonRelease:
			{
				XSetWindowBorderWidth(wm->getDisplay(), c->close.window, 1);
				send_wm_delete(c);
				return;
			}
		}
	}
}

void Client::shadeButtonPressed(Client *c)
{
	XSetWindowBorderWidth(wm->getDisplay(), c->shadeButton.window, 2);

	XEvent ev;
	
	for (;;) {
		XMaskEvent(wm->getDisplay(), ButtonReleaseMask | LeaveWindowMask, &ev);
		switch (ev.type) 
		{
			case LeaveNotify:
			{
				XSetWindowBorderWidth(wm->getDisplay(), c->shadeButton.window, 1);
				return;
			}
			
			case ButtonRelease:
			{
				XSetWindowBorderWidth(wm->getDisplay(), c->shadeButton.window, 1);
				XRaiseWindow(wm->getDisplay(), c->parent);
				shade(c);
				return;
			}
		}
	}
}

void Client::minimizeButtonPressed(Client *c)
{
	XSetWindowBorderWidth(wm->getDisplay(), c->minimize.window, 2);
	
	XEvent ev;
	
	for (;;) {
		XMaskEvent(wm->getDisplay(), ButtonReleaseMask | LeaveWindowMask, &ev);
		switch (ev.type) 
		{
			case LeaveNotify:
			{
				XSetWindowBorderWidth(wm->getDisplay(), c->minimize.window, 1);
				return;
			}
			
			case ButtonRelease:
			{
				XSetWindowBorderWidth(wm->getDisplay(), c->minimize.window, 1);
				hide(c);
				return;
			}
		}
	}
}

void Client::maximizeButtonPressed(Client *c)
{
	XSetWindowBorderWidth(wm->getDisplay(), c->maximize.window, 2);
	
	XEvent ev;
	
	for (;;) {
		XMaskEvent(wm->getDisplay(), ButtonReleaseMask | LeaveWindowMask, &ev);
		switch (ev.type) 
		{
			case LeaveNotify:
			{
				XSetWindowBorderWidth(wm->getDisplay(), c->maximize.window, 1);
				return;
			}
			
			case ButtonRelease:
			{
				XSetWindowBorderWidth(wm->getDisplay(), c->maximize.window, 1);
				
				switch(ev.xbutton.button)
				{
					case 1: 
					{
						if(c->shaded)
						{
							shade(c);
							XRaiseWindow(wm->getDisplay(), c->parent);
						
							// Make sure it doesn't keep resizing if its already max resized
							if (! c->maxsized) maximumresize(c);
						
						} else {
							XRaiseWindow(wm->getDisplay(), c->parent);
							maximumresize(c);
						}
					return;
					}
				}
			}
		}
	}
}

void Client::send_wm_delete(Client *c)
{
	if (c) send_xmessage(c->window, wm->getWMProtocolsAtom(), wm->getWMDeleteAtom(), CurrentTime);
}

int Client::send_xmessage(Window w, Atom a, long x, long y)
{
	XEvent ev;
	long mask;

	memset(&ev, '\0', sizeof ev);
	ev.type = ClientMessage;
	ev.xclient.window = w;
	ev.xclient.message_type = a;
	ev.xclient.format = 32;
	ev.xclient.data.l[0] = x;
	ev.xclient.data.l[1] = y;
	mask = (w == wm->getRootWindow()) ? SubstructureRedirectMask : 0L;
	
	return XSendEvent(wm->getDisplay(), w, false, mask, &ev);
}

void Client::set_wm_state(Client *c, int state)
{
	long data[2];

	data[0] = (long) state;
	data[1] = (long) None; /* icon window */

	c->state = state;
	
	XChangeProperty(wm->getDisplay(), c->window, wm->getWMStateAtom(), wm->getWMStateAtom(),
		32, PropModeReplace, (unsigned char *)data, 2);
}

int Client::get_wm_state(Client *c)
{
	Atom real_type;
	int real_format, state;
	unsigned long n, extra;
	unsigned char *data;

	if ((XGetWindowProperty(wm->getDisplay(), c->window, wm->getWMStateAtom(), 0L, 2L, false,
				AnyPropertyType, &real_type, &real_format, &n,
				&extra, &data) == Success) && n) {
		state = *(int *)data;
		XFree(data);
		return state;
	} else return WithdrawnState;
}

void Client::handle_enter_event(XCrossingEvent *e)
{
	Client *c = find_client(e->window);	

	if(c) XSetInputFocus(wm->getDisplay(), c->window, RevertToPointerRoot, CurrentTime);
}

void Client::handle_button_press_event(XButtonEvent *e)
{
	Client *c = find_client(e->window);

	if (c) 
	{
		switch (e->button) 
		{
			case Button1:
			{
				if(c->close.window == e->window) 	     closeButtonPressed(c);
				else if (c->maximize.window == e->window)    maximizeButtonPressed(c);
				else if (c->minimize.window == e->window)    minimizeButtonPressed(c);
				else if (c->shadeButton.window == e->window) shadeButtonPressed(c);
				else if (c->right_sidebar == e->window) 
				{
					XRaiseWindow(wm->getDisplay(), c->parent);
					redraw(c);
					move(c);
				} 
				else if (c->resize_win == e->window)
				{
					XRaiseWindow(wm->getDisplay(), c->parent);
					resize(c);
				} 
				else if (c->title == e->window)
				{
					XRaiseWindow(wm->getDisplay(), c->parent);
					redraw(c);
					move(c);
				} 
			break;
			}

			case Button2:
			{	
			 	if (c->title == e->window) 
				{
					// Middle click also allows you to shade the window.
					if(c->shaded)
					{
						XRaiseWindow(wm->getDisplay(), c->parent);
						shade(c);
						redraw(c);
					} else {
						XRaiseWindow(wm->getDisplay(), c->parent);
						shade(c);
						redraw(c);
					}
				}	
			break;
			}
		}
	}
}

void Client::handle_configure_request(XConfigureRequestEvent *e)
{
	Client *c = find_client(e->window);
	XWindowChanges wc;

	if (c) 
	{
		change_gravity(c, -1);
		
		if (e->value_mask & CWX) c->x = e->x;
		if (e->value_mask & CWY) c->y = e->y;
		if (e->value_mask & CWWidth) c->width = e->width;
		if (e->value_mask & CWHeight) c->height = e->height;
		
		change_gravity(c, 1);

		if (c->width != wc.width && c->height != wc.height)
		{
			if (c->has_titlebar) XMoveResizeWindow(wm->getDisplay(), c->window, 0, theight(c), c->width, c->height);
			if (c->has_titlebar) XMoveResizeWindow(wm->getDisplay(), c->parent, c->x, c->y - theight(c), c->width+c->right_sidebar_width, c->height + theight(c));	
			XMoveResizeWindow(wm->getDisplay(), c->title, 0, 0, c->width+c->right_sidebar_width, theight(c));
			XMoveResizeWindow(wm->getDisplay(), c->right_sidebar, c->width, theight(c), c->right_sidebar_width, c->height - 10);
			XMoveResizeWindow(wm->getDisplay(), c->resize_win, c->width, c->height + 10, right_sidebar_width, 10); 	 
			XMoveWindow(wm->getDisplay(), c->maximize.window, c->width - 10, c->maximize.y);
			XMoveWindow(wm->getDisplay(), c->minimize.window, c->width - 27, c->minimize.y);
			XMoveWindow(wm->getDisplay(), c->shadeButton.window, c->width - 44, c->shadeButton.y);
		}
		
		wc.x = c->x;
		wc.y = c->y - theight(c);
		wc.width = c->width + c->right_sidebar_width;
		wc.height = c->height + theight(c);		
		wc.border_width = DEFAULT_BORDER_SIZE;
		wc.sibling = e->above;
		wc.stack_mode = e->detail;
		
		XConfigureWindow(wm->getDisplay(), c->parent, e->value_mask, &wc);

		// If a window has been shaded and we get a configure request
		// we need to fix the shade button so it shows the correct state.
		// Otherwise the client will be mapped but show that its been 
		// shaded. If you try and move it, it will draw a shaded outline.
		if(c->shaded) shade(c);
		
		send_config(c);
	} 
		
	if(c && c->has_titlebar)
	{
		wc.x = 0;
		wc.y = theight(c);
	} else {
		wc.x = e->x;
		wc.y = e->y;
	}
	
	wc.width = e->width;
	wc.height = e->height;
	wc.sibling = e->above;
	wc.stack_mode = e->detail;

	XConfigureWindow(wm->getDisplay(), e->window, e->value_mask, &wc);
}

void Client::handle_map_request(XMapRequestEvent *e)
{
	Client *c = find_client(e->window);

	if (c) mapClient(c);
	else make_new_client(e->window);
}

void Client::handle_unmap_event(XUnmapEvent *e)
{
	Client *c = find_client(e->window);

	if (c) {
		if (c->ignore_unmap) c->ignore_unmap--;
		else 
		{			
			if (get_wm_state(c) == IconicState)
				XMapWindow(wm->getDisplay(), c->window);
			else 
			{			  
				set_wm_state(c, WithdrawnState);
				remove_client(c);
			}
		}
	} 
}

void Client::handle_client_message(XClientMessageEvent *e)
{
	Client *c = find_client(e->window);
	
	if (c)
	{
		if(e->message_type == wm->getWMChangeStateAtom())
			if(e->format == 32 && e->data.l[0] == IconicState) hide(c);
	}
}

void Client::handle_colormap_change(XColormapEvent *e)
{
	Client *c =  find_client(e->window);

	if (c && e->c_new) 
	{
		c->cmap = e->colormap;
		installColorMap(c->cmap);
	}	
}

void Client::handle_property_change(XPropertyEvent *e)
{
	Client *c = find_client(e->window);
	long dummy;

	if (c) 
	{
		switch (e->atom) 
		{
			case XA_WM_NAME:
			{
				if (c->name) XFree(c->name);
				XFetchName(wm->getDisplay(), c->window, &c->name);
				XClearWindow(wm->getDisplay(), c->title);
				redraw(c);
			}
			break;
			
			case XA_WM_NORMAL_HINTS:
			{
				XGetWMNormalHints(wm->getDisplay(), c->window, c->size, &dummy);				
			}
			break;
						
			//case XA_WM_ICON_NAME:
			//break;
			
			case XA_WM_TRANSIENT_FOR:
				getTransients(c);
			break;
		}
	}
}


void Client::handle_expose_event(XExposeEvent *e)
{
	Client *c = find_client(e->window);

	if (c && e->count == 0) redraw(c); 	
}

void Client::handle_destroy_notify(XDestroyWindowEvent *e)
{
	Client *c = find_client(e->window);
	
	if(c) remove_client(c);			
}

void Client::handle_shape_change(XShapeEvent *e)
{
	Client *c = find_client(e->window);

	if (c) 
	{
		c->shaped=true;
		set_shape(c);
	}	
}

void Client::installColorMap(Colormap cmap)
{
	XInstallColormap(wm->getDisplay(), cmap);
}

void Client::isIconic(Client *c)
{
	// Determine if this client is already in its iconic state upon creation
	// if so then we won't map it, but just iconify it.
	if (get_wm_state(c) == IconicState) hide(c);
	else mapClient(c);
}

void Client::mapClient(Client *c)
{
	if (c->has_titlebar)
	{				
		if(c->has_shade_button)    XMapWindow(wm->getDisplay(), c->shadeButton.window);
		if(c->has_close_button)    XMapWindow(wm->getDisplay(), c->close.window);
		if(c->has_minimize_button) XMapWindow(wm->getDisplay(), c->minimize.window);
		if(c->has_maximize_button) XMapWindow(wm->getDisplay(), c->maximize.window);
		
		XMapWindow(wm->getDisplay(), c->right_sidebar);
		XMapWindow(wm->getDisplay(), c->resize_win);
		
		XMapWindow(wm->getDisplay(), c->title);
		
		XMapWindow(wm->getDisplay(), c->window);
		XMapRaised(wm->getDisplay(), c->parent);

	} 
	else XMapWindow(wm->getDisplay(), c->window);
	
	set_wm_state(c, NormalState);		
}

void Client::themeChange(Client *c)
{	
	if (c->has_titlebar)
	{
		XFreePixmap(wm->getDisplay(), c->frame_pix);
		XFreePixmap(wm->getDisplay(), c->right_pix);
		XFreePixmap(wm->getDisplay(), c->button_pix);
		XFreePixmap(wm->getDisplay(), c->resize_pix);
	
		createTitleImage(c);
		createRightSidebarImage(c);
		createButtonImage(c);	
		createResizeImage(c);

		redraw(c);
	}
}

void Client::raise() 
{
	XRaiseWindow(wm->getDisplay(), parent);
	redraw(this);
}
