/*
 * appbar.cc - Defines how appbar looks and feels
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
 
#include "appbar.hh"

Appbar *ab;

unsigned long palette[PALETTE_COUNT];

void signal_handler(int signal) 
{ 
  switch(signal) 
  { 
	case SIGQUIT:
	case SIGKILL:
	case SIGINT: 
  	case SIGTERM: 
		ab->quit();
    	break;
  	
	//case SIGHUP:
   	//break;
  
  	case SIGCHLD: 
    		wait(NULL); 
    	break; 
  } 
} 

void init_signals() 
{ 
	struct sigaction act; 
 
	act.sa_handler=signal_handler; 
  	act.sa_flags=0; 
  
  	sigaction(SIGQUIT, &act, NULL);
  	sigaction(SIGKILL, &act, NULL);
	sigaction(SIGTERM, &act, NULL); 
	sigaction(SIGINT, &act, NULL); 
  	//sigaction(SIGHUP, &act, NULL); 
  	sigaction(SIGCHLD, &act, NULL); 
} 

Appbar::Appbar()
{
	::ab=this;

	init_signals();

	placement = VERTICAL;
	screen_placement = NORTHWEST;	
	temp_color=0;
	
	hidden=true;
	window_width=0;
	
	if (!(dpy = XOpenDisplay (NULL)))
	{
		cerr << "Can't open display!" << endl;
		exit(0);
	}

	// The to and from structures are used in creating
	// the color theme for the main window. This is just
	// a default, this is configurable in the rc file.

	// Nice shade of gray
	from.red   = 0xcc;
	from.green = 0xcc;
	from.blue  = 0xcc;
	
	// Another nice shade of gray
	to.red	 = 0x99;
	to.green = 0x99;
	to.blue	 = 0x99;
	
	gnome[WIN_STATE]=XInternAtom(dpy, "_WIN_STATE", False);
  	gnome[WIN_HINTS]=XInternAtom(dpy, "_WIN_HINTS", False);
  	gnome[WIN_LAYER]=XInternAtom(dpy, "_WIN_LAYER", False);
	motif=XInternAtom(dpy, "_MOTIF_WM_HINTS", False);

	atom_wm_protos = XInternAtom(dpy, "WM_PROTOCOLS", False);
	atom_wm_delete = XInternAtom(dpy, "WM_DELETE_WINDOW", False);
	
	net_wm_strut = XInternAtom(dpy, "_NET_WM_STRUT", False);
	
	win = XCreateSimpleWindow
		(dpy, DefaultRootWindow (dpy), 0, 0, 100, 25, 0,
		 BlackPixel (dpy, DefaultScreen (dpy)),
		 WhitePixel (dpy, DefaultScreen (dpy)));
		 
	XSelectInput (dpy, win, ExposureMask    	|
	                        ButtonPressMask 	|
				ButtonReleaseMask 	|
				EnterWindowMask 	|
				LeaveWindowMask);

    	set_gnome_hint(win, WIN_STATE, WIN_STATE_STICKY);
	set_gnome_hint(win, WIN_HINTS, WIN_HINTS_SKIP_FOCUS   |
				       WIN_HINTS_SKIP_TASKBAR |
				       WIN_HINTS_SKIP_WINLIST |
				       WIN_HINTS_DO_NOT_COVER
				       );
    	set_gnome_hint(win, WIN_LAYER, 0);

    	mwm_hints.flags=MWM_DECORATIONS;
	mwm_hints.decorations=0;
    	set_mwm_hints(win, &mwm_hints);

	screen = DefaultScreen(dpy);
	depth = DefaultDepth(dpy, screen);
	visual = DefaultVisual(dpy, screen);
	root = RootWindow(dpy, screen);

	screen_width  = WidthOfScreen(ScreenOfDisplay(dpy, screen));
	screen_height = HeightOfScreen(ScreenOfDisplay(dpy, screen));

	image_control = new BImageControl(dpy, depth, root, screen, visual); 

	// Default color theme style
	unsigned long style=0;
	style = BImage_Flat | BImage_Gradient | BImage_Diagonal | BImage_Bevel1;
	//style = BImage_Flat | BImage_Solid | BImage_Bevel1;

	win_pix = None;

	gc = XCreateGC (dpy, win, 0, NULL);

	XColor xcl;
	unsigned int i, j;

	i = j = 0;
	do
	{
		xcl.red = cols[i];
		i++;
		xcl.green = cols[i];
		i++;
		xcl.blue = cols[i];
		i++;
		XAllocColor (dpy, DefaultColormap (dpy, screen), &xcl);
		palette[j] = xcl.pixel;
		j++;
	}
	while (j < PALETTE_COUNT);

	iconList = new LinkedList<Icon>;

	std::string appbar_rc_file = APPBAR_RC_FILE;
		
	cout << "Appbar is using rc file: " << appbar_rc_file << endl;

	appbarConfig = new Scanner((char*) appbar_rc_file.c_str());
	parseAppbarConfig(appbarConfig);
	
	if(iconList->count() <= 0)
	{
		cerr << "appbar.rc contains no icons, using defaults." << endl;
		
		Icon *t = new Icon(dpy, "images/terminal.xpm", win);
		t->setExecuteCommand("xterm");
		iconList->insert(t);
	} 
		
	if(placement==HORIZONTAL)
		horizontalBarIconPlacement();
		
	if(placement==VERTICAL)
		verticalBarIconPlacement();
	
	win_pix = image_control->renderImage(
		window_width, 
		window_height, 
		style, 
	       	to, 
	      	from);
	
	XSetWindowBackgroundPixmap(dpy, win, win_pix);
	
	XResizeWindow(dpy,win, window_width , window_height);
	
	XMapWindow(dpy,win);
	
	appbarScreenPlacement();
	
	doEventLoop();
}

Appbar::~Appbar()
{
	quit();
}

// This function takes care of determing the position of the appbar
// and wether its shaded or not.
void Appbar::appbarScreenPlacement()
{
	int new_x=0, new_y=0;
	
	switch (screen_placement)
	{
		case NORTHEAST:
		{
			if(placement==HORIZONTAL)
			{
				setTopStrut();
				
				if(hidden) 
				{
					new_x = screen_width - window_width;
					new_y = 0;
					hidden=false;
				}
				else 
				{
					new_x = screen_width - 10;
					new_y = 0;
					hidden=true;
				}
			}

			if(placement==VERTICAL)
			{
				setRightStrut();
				
				if(hidden) 
				{
					new_x = screen_width - window_width;
					new_y = 0;
					
					hidden=false;
				}
				else 
				{
					new_x = screen_width - window_width;
					new_y = -(window_height-9);
					
					hidden=true;
				}
			}		
		}
		break;
		
		case NORTHWEST:
			if(placement==HORIZONTAL)
			{
				setTopStrut();
				
				if(hidden) 
				{
					new_x=0;
					new_y=0;
					hidden=false;
				}
				else 
				{
					new_x = -(window_width-9);
					new_y = 0;
					hidden=true;
				}
			}
			else 
			if(placement==VERTICAL)
			{
				setLeftStrut();
				
				if(hidden) 
				{
					new_x=0;
					new_y=0;
					
					hidden=false;
				}
				else 
				{
					new_x = 0;
					new_y = -(window_height-9);
					
					hidden=true;
				}
			}		
		break;
		
		case SOUTHEAST:
			if(placement==HORIZONTAL)
			{
				setBottomStrut();
				
				if(hidden) 
				{
					new_x = screen_width - window_width;
					new_y = screen_height - window_height;
					hidden=false;
				}
				else 
				{
					new_x = screen_width - 10;
					new_y = screen_height - window_height;
					hidden=true;
				}
			}
			else 
			if(placement==VERTICAL)
			{
				setRightStrut();
				
				if(hidden) 
				{
					new_x = screen_width - window_width;
					new_y = screen_height - window_height;
					hidden=false;
				}
				else 
				{
					new_x = screen_width - window_width;
					new_y = (screen_height - 10);
					hidden=true;
				}
			}
		break;
		
		case SOUTHWEST:
			if(placement==HORIZONTAL)
			{
				setBottomStrut();
			
				if(hidden) 
				{
					new_x = 0;
					new_y = screen_height - window_height;
					hidden=false;
				}
				else 
				{
					new_x = -(window_width-10);
					new_y = screen_height - window_height;
					hidden=true;
				}
			}
			else 
			if(placement==VERTICAL)
			{
				setLeftStrut();
			
				if(hidden) 
				{
					new_x = 0;
					new_y = screen_height - window_height;
					hidden=false;
				}
				else 
				{
					new_x = 0;
					new_y = (screen_height - 10);
					hidden=true;
				}
			}	
		break;
	}
	
	XMoveWindow(dpy,win, new_x, new_y);
}

// Positions icons horizontally on the appbar and resizes 
// it accordingly.
void Appbar::horizontalBarIconPlacement()
{
	window_width= iconList->count() * 20 + 15;
	window_height=20;
	
	LinkedListIterator<Icon> it(iconList);
	int temp_x=5;
	int temp_y=5;
	int icon_wrap=0;
	int icon_rows=0;
	
	if ( (screen_placement == NORTHWEST) || (screen_placement == SOUTHWEST) ) 
		temp_x = 5;
	
	if ( (screen_placement == NORTHEAST) || (screen_placement == SOUTHEAST) ) 
		temp_x = 13;
	
	for(; it.current(); it++)
	{
		if(icon_wrap==25)
		{ 
			temp_x = 5;
			temp_y +=25;
			
			window_width = icon_wrap * 20 + 25;
			
			icon_wrap=0;
			icon_rows++;
		}
				
		XMapWindow(dpy, it.current()->getIconWindow());
		XMoveWindow(dpy, it.current()->getIconWindow(), temp_x, temp_y);
		
		temp_x+=20;
		icon_wrap++;
	}
	
	window_height+=temp_y;
}

// Positions icons vertically on the appbar and resizes 
// it accordingly.
//
// This function can be written better, its still not very
// well thought out. Icon wrapping is not really how I'd 
// like it to be.
void Appbar::verticalBarIconPlacement()
{
	window_width = 1;
	window_height= iconList->count() * 20 + 15;
	
	LinkedListIterator<Icon> it(iconList);
	int temp_x=5;
	int temp_y=5;
	int icon_wrap=0;
	int icon_rows=0;
	
	if ( (screen_placement == NORTHWEST) || (screen_placement == NORTHEAST) ) 
		temp_y=5;
		
	if ( (screen_placement == SOUTHWEST) || (screen_placement == SOUTHEAST) ) 
		temp_y=13;
		
	for(; it.current(); it++)
	{
		if(icon_wrap==1)
		{ 
			temp_x=5;
			temp_y+=20;
			
			window_width = icon_wrap * 20 + 6;
			
			icon_wrap=0;
			icon_rows++;
		}
				
		XMapWindow(dpy, it.current()->getIconWindow());
		XMoveWindow(dpy, it.current()->getIconWindow(), temp_x, temp_y);
		
		temp_x+=20;
		icon_wrap++;
	}
}

void Appbar::doEventLoop()
{
	XEvent ev;
	
	for (;;)
	{
		XNextEvent(dpy, &ev);
	
		Icon* ico=NULL;
				
		switch(ev.type)
		{
			case ButtonPress:
				ico = findIconWindow(ev.xbutton.window);
				
				if (ico)
				{	
					ico->updateXYPos();
					draw3DRectSunken(win, ico->getX()-1,
					   	   ico->getY()-1, 
						   ico->getWidth()+1, 
						   ico->getHeight()+1
						);
						
				}
				
				switch(ev.xbutton.button)
				{
					case Button1:
						
						if (ev.xbutton.window == win)
						{
							if(placement==HORIZONTAL)
							{
								if ( (screen_placement == NORTHWEST) || (screen_placement == SOUTHWEST) ) 
									if(ev.xbutton.x >= window_width-10) appbarScreenPlacement();

								if ( (screen_placement == NORTHEAST) || (screen_placement == SOUTHEAST) )
									if(ev.xbutton.x <= 10) appbarScreenPlacement(); 
							}
					
							if(placement==VERTICAL)
							{
								if ( (screen_placement == NORTHWEST) || (screen_placement == NORTHEAST) )  
									if(ev.xbutton.y >= window_height-10) appbarScreenPlacement();
	
								if ( (screen_placement == SOUTHWEST) || (screen_placement == SOUTHEAST) ) 
									if(ev.xbutton.y <= 10) appbarScreenPlacement();
							}
						}
	
					break;
					
					case Button2:
						if (ev.xbutton.window == win)
						{
							if(placement==HORIZONTAL)
							{
								if ( (screen_placement == NORTHWEST) || (screen_placement == SOUTHWEST) ) 
									if(ev.xbutton.x >= window_width-10) quit();	
	
								if ( (screen_placement == NORTHEAST) || (screen_placement == SOUTHEAST) ) 
									if(ev.xbutton.x <= 10) quit();
							}
						
							if(placement==VERTICAL)
							{
								if ( (screen_placement == NORTHWEST) || (screen_placement == NORTHEAST) ) 
									if(ev.xbutton.y >= window_height-10) quit();
	
								if ( (screen_placement == SOUTHWEST) || (screen_placement == SOUTHEAST) ) 
									if(ev.xbutton.y <= 10) quit();
							}
						}
					break;
					
					//case Button3:
					//
					// Possibly allow for arbitrary execution of something later 
					// maybe for like a config tool to pop up. Anyway Mark wants it.
					//
					//break;
				}
			break;
				
			case ButtonRelease:
				ico = findIconWindow(ev.xbutton.window);
				
				if (ico)
				{	
					ico->updateXYPos();
					draw3DRectRaised(win, ico->getX()-1,
					   	   ico->getY()-1, 
						   ico->getWidth()+1, 
						   ico->getHeight()+1
						);
					
					ico->executeCommand();
				}
			break;
			
			//case DestroyNotify:
			//	quit();
			//break;
			
			case ClientMessage:
				if (ev.xclient.message_type == atom_wm_protos)
				{
					if(ev.xclient.data.l[0]== (long) atom_wm_delete)
						delete this;
				}
			break;
			
			case EnterNotify:
				ico = findIconWindow(ev.xcrossing.window);

				if(ico)	
				{
				   ico->updateXYPos();
				   draw3DRectRaised(win, ico->getX()-1,
					   	   ico->getY()-1, 
						   ico->getWidth()+1, 
						   ico->getHeight()+1
						);
				}
					
			break;
				
			case LeaveNotify:
				ico = findIconWindow(ev.xcrossing.window);
					
				if (ico) 
				{ 
					XClearWindow(dpy, win);
					
					if(placement==HORIZONTAL)
						drawHorizontalBarGrill();
					
					if(placement==VERTICAL)
						drawVerticalBarGrill();
				}
			break;
				
			case Expose:
				if(placement==HORIZONTAL)
					drawHorizontalBarGrill();
					
				if(placement==VERTICAL)
					drawVerticalBarGrill();
			break;
		}
	}
}

void Appbar::set_gnome_hint(Window w, int a, long value)
{
  XChangeProperty(dpy, w, gnome[a], XA_CARDINAL, 32, PropModeReplace, (unsigned char *)&value, 1);
}

void Appbar::set_mwm_hints(Window w, PropMwmHints *hints)
{
  XChangeProperty(dpy, w, motif, motif, 32, PropModeReplace, (unsigned char *)hints, sizeof(*hints));
}

void Appbar::set_foreground (int index)
{
	XSetForeground (dpy, gc, palette[index]);
}

void Appbar::draw_dot (Window win, int x, int y)
{
	set_foreground (4);
	XDrawPoint (dpy, win, gc, x, y);
	set_foreground (3);
	XDrawPoint (dpy, win, gc, x + 1, y + 1);
}

void Appbar::draw3DLine(Window win, int style, int x1, int y1, int x2, int y2)
{
	set_foreground(3);
	XDrawLine(dpy, win, gc, x1, y1, x2, y2);
	set_foreground(4);

  	if (style==HORIZONTAL)
  	 	XDrawLine(dpy, win, gc, x1, y1-1, x2, y2-1);
  
  	if (style==VERTICAL)
    		XDrawLine(dpy, win, gc, x1-1, y1, x2-1, y2);
}

void Appbar::draw3DRectSunken(Window win, int x, int y, int h, int w)
{
 	set_foreground(4);
  	XDrawLine(dpy, win, gc, x, y, x, y+h);
  	XDrawLine(dpy, win, gc, x, y, x+w, y);

  	set_foreground(3);
  	XDrawLine(dpy, win, gc,   x+w, y, x+w, y+h);
  	XDrawLine(dpy, win, gc,   x, y+h, x+w, y+h);
}

void Appbar::draw3DRectRaised(Window win, int x, int y, int h, int w)
{
  	set_foreground(3);
  	XDrawLine(dpy, win, gc, x, y, x, y+h);
  	XDrawLine(dpy, win, gc, x, y, x+w, y);

  	set_foreground(4);
  	XDrawLine(dpy, win, gc,   x+w, y, x+w, y+h);
  	XDrawLine(dpy, win, gc,   x, y+h, x+w, y+h);
}

void Appbar::drawHorizontalBarGrill()
{
	if ( (screen_placement == NORTHWEST) || (screen_placement == SOUTHWEST) ) 
	{ 
		draw3DLine(win, VERTICAL,
			window_width-10,0,
			window_width-10, window_height);
	
		drawVerticalGrill(win, window_width-10, window_height);
	}
	
	if ( (screen_placement == NORTHEAST) || (screen_placement == SOUTHEAST) ) 
	{ 
		draw3DLine(win, VERTICAL,
			10, 0,
			10, window_height);
	
		drawVerticalGrill(win, 0, window_height);
	}
}

void Appbar::drawVerticalBarGrill()
{
	if ( (screen_placement == SOUTHWEST) || (screen_placement == SOUTHEAST) ) 
	{ 
		draw3DLine(win, HORIZONTAL,
		0, 10,
		window_width, 10);	
	
		drawHoriontalGrill(win, 0, window_width);
	}
	
	if ( (screen_placement == NORTHEAST) || (screen_placement == NORTHWEST) ) 
	{ 
		draw3DLine(win, HORIZONTAL,
		0, window_height-10,
		window_width, window_height-10);	
	
		drawHoriontalGrill(win, window_height-10, window_width);
	}
}

void Appbar::drawVerticalGrill (Window win, int x, int height)
{
	int y = 0;
	while (y < height - 5)
	{
		y += 3;
		draw_dot (win, x + 3, y);
		draw_dot (win, x + 6, y);
	}
}

void Appbar::drawHoriontalGrill(Window win, int y, int width)
{
	int x = 0;
	while (x < width - 5)
	{
		x += 3;
		draw_dot (win, x, y + 3);
		draw_dot (win, x, y + 6);
	}
}

Icon* Appbar::findIconWindow(Window win)
{
	LinkedListIterator<Icon> it(iconList);
	for(; it.current() ; it++)
	{
		if(it.current()->getIconWindow() == win)
			return it.current();
	}
	
	return NULL;
}

bool Appbar::imageKeyword(Scanner *s)
{
	std::string t="image";	
	std::string a="";

	if ( s->match ( t ) )
	{
		t = "=";

		if ( s->match ( t ) )
		{
			while (! s->match( ";" ) )
			{
				a += s->currentToken();
				
				s->getNextToken();
			} 
			
			char *t = (char*) a.c_str();
			current_icon->loadPixmap(t);
				
			return true;
		}
	}
	
	return false;
}

bool Appbar::executeKeyword(Scanner *s)
{
	std::string t="execute";	
	std::string a="";

	if ( s->match ( t ) )
	{
		t = "=";

		if ( s->match ( t ) )
		{
			if(s->currentToken() == "}") 
			{
				cerr << "parse error: cannot determine execute command, using default (xterm)." << endl;
				current_icon->setExecuteCommand("xterm");
				return true;
			}
			
			while (! s->match( ";" ) )
			{
				a += s->currentToken();
				a += " "; // put whitespace back that may
					  // have gotten eaten by the scanner.
					  // This is probably not the best
					  // place to do it. The scanner should
					  // be smarter.
					  				
				s->getNextToken();
			} 
			
			char *t = (char*)a.c_str();
			current_icon->setExecuteCommand(t);

			return true;
		} 
	}
	
	return false;
}

bool Appbar::iconKeyword(Scanner *s)
{
	std::string t="icon";	

	if ( s->match ( t ) )
	{
		t = "{";

		if ( s->match ( t ) )
		{
			current_icon = new Icon(dpy, win);			
						
			imageKeyword(s);
			executeKeyword(s);
			
			iconList->insert(current_icon);

			t = "}";
			
			if ( s->match ( t ) )
			{
				return true;
			}
		}

	}
	
	return false;
}

bool Appbar::northeastKeyword(Scanner *s)
{
	std::string t="northeast_placement";	

	if ( s->match ( t ) )
	{
		screen_placement=NORTHEAST;
		return true;
	}
	
	return false;
}

bool Appbar::northwestKeyword(Scanner *s)
{
	std::string t="northwest_placement";	

	if ( s->match ( t ) )
	{
		screen_placement=NORTHWEST;
		return true;
	}
	
	return false;
}

bool Appbar::southeastKeyword(Scanner *s)
{
	std::string t="southeast_placement";	

	if ( s->match ( t ) )
	{
		screen_placement=SOUTHEAST;
		return true;
	}
	
	return false;
}

bool Appbar::southwestKeyword(Scanner *s)
{
	std::string t="southwest_placement";	

	if ( s->match ( t ) )
	{
		screen_placement=SOUTHWEST;
		return true;
	}
	
	return false;
}

bool Appbar::horizontalKeyword(Scanner *s)
{
	std::string t="horizontal_placement";	

	if ( s->match ( t ) )
	{
		placement=HORIZONTAL;
		return true;
	}
	
	return false;
}

bool Appbar::verticalKeyword(Scanner *s)
{
	std::string t="vertical_placement";	

	if ( s->match ( t ) )
	{
		placement=VERTICAL;
		return true;
	}
	
	return false;
}

bool Appbar::rgbKeywords(Scanner *s)
{
	std::string t="";	
	std::string a="";

	if ( s->match ( "red" ) || s->match ( "green" ) || s->match ( "blue" ) )
	{
		t = "=";

		if ( s->match ( t ) )
		{
			while (! s->match( ";" ) )
			{
				a += s->currentToken();
				
				s->getNextToken();
			} 
			
			temp_color = strtol(a.c_str(),0,16);
			
			return true;
		}
	}
	
	return false;
}

bool Appbar::fromKeyword(Scanner *s)
{
	std::string t="";	

	if ( s->match ( "appbar_color_from" ) )
	{
		t = "{";

		if ( s->match ( t ) )
		{
			//cout << s->currentToken() << endl;
			
			rgbKeywords(s);
			from.red = temp_color;
			
			rgbKeywords(s);
			from.green = temp_color;
			
			rgbKeywords(s);
			from.blue = temp_color;
			
			t = "}";
			
			if ( s->match ( t ) )
			{
				return true;
			}
		}

	}
	
	return false;
}

bool Appbar::toKeyword(Scanner *s)
{
	std::string t="";	

	if ( s->match ( "appbar_color_to" ) )
	{
		t = "{";

		if ( s->match ( t ) )
		{
			//cout << s->currentToken() << endl;
			
			rgbKeywords(s);
			to.red = temp_color;

			rgbKeywords(s);
			to.green = temp_color;

			rgbKeywords(s);
			to.blue = temp_color;
			
			t = "}";
			
			if ( s->match ( t ) )
			{
				return true;
			}
		}

	}
	
	return false;
}

void Appbar::parseAppbarConfig(Scanner* s)
{
	while(! s->eof())
	{
		s->getNextToken();

		while ( iconKeyword(s) 	     || 
			verticalKeyword(s)   || 
			horizontalKeyword(s) ||
			northeastKeyword(s)  ||
			northwestKeyword(s)  ||
			southeastKeyword(s)  ||
			southwestKeyword(s)  ||
			toKeyword(s)	     ||
			fromKeyword(s)
		) {
			if(s->eof()) return;
		}

	}
}

void Appbar::quit()
{
	cerr << "Performing Appbar cleanup!" << endl;

	iconList->removeAll();
	delete iconList;
	
	image_control->removeImage( win_pix );
	delete image_control;
	
	delete current_icon;
	delete appbarConfig;

	XFreeGC(dpy, gc);
	XDestroyWindow (dpy, win);
	XCloseDisplay(dpy);
	
	exit(0);
}

void Appbar::setLeftStrut()
{
	CARD32 strut[] = { window_width+2,0,0,0 };
	XChangeProperty (dpy, win, net_wm_strut, XA_CARDINAL, 32, PropModeReplace, (unsigned char *) &strut, 4);
}

void Appbar::setRightStrut()
{
	CARD32 strut[] = { 0,window_width+3,0,0 };
	XChangeProperty (dpy, win, net_wm_strut, XA_CARDINAL, 32, PropModeReplace, (unsigned char *) &strut, 4);
}

void Appbar::setTopStrut()
{
	CARD32 strut[] = { 0,0,window_height+2,0 };
	XChangeProperty (dpy, win, net_wm_strut, XA_CARDINAL, 32, PropModeReplace, (unsigned char *) &strut, 4);
}

void Appbar::setBottomStrut()
{
	CARD32 strut[] = { 0,0,0,window_height+2 };
	XChangeProperty (dpy, win, net_wm_strut, XA_CARDINAL, 32, PropModeReplace, (unsigned char *) &strut, 4);
}
