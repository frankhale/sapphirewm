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

#include "windowmanager.hh"

WindowManager *wm;

char* windowmanager_author 	= "Frank Hale";
char* windowmanager_author_email = "frankhale@yahoo.com";
char* windowmanager_name 	= "sapphire";
char* windowmanager_version 	= "Sapphire 0.15.7pre7 (Blackberry)";
char* windowmanager_usage 	= "[-display <display>] [-version]\n";
char* windowmanager_exit_message = "\nThank you for using Sapphire.\n\nPlease Send all bug reports, suggestions, ideas, patches to frankhale@yahoo.com\n";

char* configuration_home = "/usr/share/sapphire";

char* HOME;

// This function sets up default values for variables, gets command line args,
// sets up our signal handlers, and performs initialization of certain things
// to get ready for the window manager to start managing windows.
WindowManager::WindowManager(int argc, char **argv) 
{
	::wm = this;
	
	HOME = new char[strlen(getenv("HOME"))+1];
	strcpy(HOME, getenv("HOME"));
	
	font_pix 		= None;
	
	menu_event		= false;
	
	clock_format		= "%m %d %H:%M";
	toolbar_visible		= true;
	toolbar_position	= false;
	
	configurationPath  	= NULL;
	windowmanager_conf 	= NULL;
	display		   	= NULL;

	XSetWindowAttributes 	attr;
	XColor 			dummy;

	head_client = new Client();
	
	// Allocate memory for our linked lists.
	ClientList    = new LinkedList<Client>;
	menulist      = new LinkedList<BaseMenu>;
	extramenulist = new LinkedList<BaseMenu>;

	for (int i = 1; i < argc; i++) 
	{
		if ((strcmp(argv[i], "-display")==0) && i+1<argc)
		{
			display = argv[++i];
			continue;
		}
		if ((strcmp(argv[i], "-version")==0) && i<argc) {
			printf("version %s\nby: %s\nemail: %s\n", windowmanager_version, windowmanager_author, windowmanager_author_email);
			exit(0);
		}
		else {
			printf("usage: %s %s", windowmanager_name, windowmanager_usage);
			exit(1);
		}
	}
        
	// PP: Sun Feb 20 05:36:53 CET 2000
        // If we got the DISPLAY via a cmd line option
        // we should now set it as an evironment variable
        // so that programs we call have it.
        //
        // The other way round also applies, i.e.
        // if there was no cmd line option we should now
        // set the display variable to the value
        // of the environment variable.
	if (display)
        	setenv("DISPLAY", display, 1);
	else
        	display = getenv("DISPLAY");
	     
	XSetErrorHandler(handle_xerror);
	signal(SIGTERM, (void (*)(int)) signalhandler);
	signal(SIGINT,  (void (*)(int)) signalhandler);
	signal(SIGHUP,  (void (*)(int)) signalhandler);

	dpy = XOpenDisplay(display);

	if (!dpy) { 
		err("sorry can't open display %s", display);
		exit(1);
	}

	screen = DefaultScreen(dpy);
	root = RootWindow(dpy, screen);

	depth = DefaultDepth(dpy, screen);
	visual = DefaultVisual(dpy, screen);
		
	XDefineCursor(dpy, root, XCreateFontCursor(wm->dpy, XC_left_ptr));

	// Atoms			
	xa_wm_state        = XInternAtom(dpy, "WM_STATE", False);
	xa_wm_change_state = XInternAtom(dpy, "WM_CHANGE_STATE", False);
	xa_wm_protos       = XInternAtom(dpy, "WM_PROTOCOLS", False);
	xa_wm_delete       = XInternAtom(dpy, "WM_DELETE_WINDOW", False);
	xa_wm_cmapwins     = XInternAtom(dpy, "WM_COLORMAP_WINDOWS", False);
	xa_wm_take_focus   = XInternAtom(dpy, "WM_TAKE_FOCUS", False);
	xa_wm_motif_hints  = XInternAtom(dpy, "_MOTIF_WM_HINTS", False);
	
	foreground_color = DEFAULT_FOREGROUND_COLOR;
	background_color = DEFAULT_BACKGROUND_COLOR;
	border_color 	 = DEFAULT_BORDER_COLOR;

	// Colors
	XAllocNamedColor(dpy, DefaultColormap(dpy, screen), foreground_color, &xforeground_color, &dummy);
	XAllocNamedColor(dpy, DefaultColormap(dpy, screen), background_color, &xbackground_color, &dummy);
	XAllocNamedColor(dpy, DefaultColormap(dpy, screen), border_color, &xborder_color, &dummy);

	colors_per_channel = 4;
	image_dither = True;

	image_control = new BImageControl();
  	image_control->installRootColormap(); 

	theme = new Theme();

	getCurrentTheme();

	font 	 = theme->getFont();
	menufont = theme->getMenuFont();
	
	xfont = XLoadQueryFont(dpy, font);
	
	if (!xfont) 
	{ 
		// We should really check here to see if we can 
		// find another font just incase. Dying here without
		// checking again is not cool!
		
		err("the default font was not found, aborting."); 
		exit(-1); 
	}
	
	iheight = xfont->ascent + xfont->descent + 2 * DEFAULT_SPACE;

	font_position = iheight - xfont->descent;

	if(! QueryShapeExtentions())
	{
		printf("What no shape extentions, God you must be using an old X11 release.\n\nHave a nice life\n\nBa Bye...\n\n");	
	}
	
	move_curs   = XCreateFontCursor(dpy, XC_fleur);
	resize_curs = XCreateFontCursor(dpy, XC_plus);
	left_arrow  = XCreateFontCursor(dpy, XC_left_ptr);
	
	createFontPixmap();
	
	gv.font = xfont->fid;
	gv.tile = font_pix;	
	string_gc = XCreateGC(dpy, root,  GCTile | GCFont , &gv);
	XSetFillStyle(dpy, string_gc, FillTiled);

	gv.foreground = xborder_color.pixel;
	gv.line_width = DEFAULT_BORDER_SIZE;
	border_gc = XCreateGC(dpy, root, GCForeground | GCLineWidth, &gv);

	gv.function = GXinvert;
	gv.subwindow_mode = IncludeInferiors;
	invert_gc = XCreateGC(dpy, root, GCFunction | GCSubwindowMode | GCLineWidth, &gv);

	attr.event_mask = SubstructureRedirectMask | 
			  SubstructureNotifyMask   | 
			  ColormapChangeMask       | 
	                  ButtonPressMask          | 
			  ButtonReleaseMask        | 
			  PropertyChangeMask       | 
			  PointerMotionMask        | 
			  FocusChangeMask          |
		          KeyPressMask             |
			  ExposureMask		   ;
			  
	XChangeWindowAttributes(dpy, root, CWEventMask, &attr);
	
	xres = WidthOfScreen(ScreenOfDisplay(wm->dpy, wm->screen));
	yres = HeightOfScreen(ScreenOfDisplay(wm->dpy, wm->screen));
		
	rm = new RootMenu();
	t = new Toolbar();
	im = new IconMenu();

	addToMenuList(rm);
	rm->parseMenuConfigurationFile();

	t->getToolbarMenu()->addMenu("icon menu", im);
	t->getToolbarMenu()->addMenu("root menu", rm);
	t->getToolbarMenu()->update_all();

	scan_windows();

	do_event_loop();
}

WindowManager::~WindowManager() 
{
	unsigned int i, nwins;
	Window dw1, dw2, *wins;
	Client *temp_client=NULL;

	XQueryTree(dpy, root, &dw1, &dw2, &wins, &nwins);
	for (i = 0; i < nwins; i++) 
	{
		temp_client = head_client->find_client(wins[i]);
		
		if(temp_client)
		{
			temp_client->remove_client(temp_client);
		}
		
	}
	XFree(wins);

	ClientList->removeAll();		
	
	delete head_client;
	delete ClientList;

	image_control->removeImage(font_pix);

	XFreeFont(dpy, xfont);
	
	XFreeGC(dpy, invert_gc);
	XFreeGC(dpy, string_gc);
	XFreeGC(dpy, border_gc);
		
	delete t;
	delete rm;
	delete im;

	menulist->removeAll();

	extramenulist->removeAll();

	delete menulist;
	delete extramenulist;

	delete theme;

	
	
	delete image_control;
	
	delete [] HOME;
	delete [] windowmanager_conf;
	delete [] configurationPath;
}

void WindowManager::getCurrentTheme()
{
	XrmDatabase resource_db;	
	XrmValue value;
	char *value_type;
	FILE* currentTheme;
	bool themeLoaded = false;

	windowmanager_conf = new char[strlen(HOME) + 32];
	sprintf(windowmanager_conf, "%s/.sapphire/wmconf", HOME);
	
	if((currentTheme = fopen(windowmanager_conf, "r"))!=NULL)
	{
		fclose(currentTheme);
		
		XrmInitialize();

		resource_db = XrmGetFileDatabase(windowmanager_conf);

		if (XrmGetResource(resource_db, "toolbar.visible",
		     "Toolbar.Visible", &value_type, &value))
		{
			if(strcmp(value.addr, "true")==0) 
			{
				theme->setToolbarVisible();
				toolbar_visible=true;
			} else {
				toolbar_visible=false;
			}
		}

		if (XrmGetResource(resource_db, "toolbar.position",
		     "Toolbar.Position", &value_type, &value))
		{
			if(strcmp(value.addr, "top")==0) 
			{
				theme->setToolbarTop();
				toolbar_position = true;
			} else {
				toolbar_position = false;
			}
		}
		
		// Clock format
		if (XrmGetResource(resource_db,"clock.format","Clock.Format",&value_type,&value))
		  {
		    clock_format= new char[value.size];
		    strcpy(clock_format,value.addr);
		  } else {
		    clock_format="%m %d %H:%M";
		  }
	
		if (XrmGetResource(resource_db, "start.theme",
		     "Start.Theme", &value_type, &value))
		{
			char *buffer = new char[value.size+strlen(HOME)];
			
			strcpy(buffer, value.addr);
			
			if (value.addr[0] == '~') 
			{
				sprintf(buffer, "%s%s", HOME, value.addr+1);
			}

			if((currentTheme = fopen(buffer, "r"))!=NULL)
			{
				fclose(currentTheme);	
				theme->parseTheme(buffer);
				themeLoaded=true;
			}
			
			delete [] buffer;
		}

		XrmDestroyDatabase(resource_db);
		
	}
	if (! themeLoaded) {
		char* defaultTheme = new char[strlen(configuration_home) + 32];
		sprintf(defaultTheme, "%s/themes/default.theme", configuration_home);

		theme->parseTheme(defaultTheme);
		theme->setToolbarVisible();
		
		delete [] defaultTheme;
	}
}

void WindowManager::createFontPixmap()
{
	unsigned long style=0;
	
	style = wm->getTheme()->getFontStyle();		

	font_pix = wm->getImageControl()->renderImage(
			//"xfont",
			xfont->ascent, 
			xfont->descent, 
			style, 
			wm->getTheme()->getFontColorFrom(), 
			wm->getTheme()->getFontColorTo()
			);
}

// We really should have some way to check if the menu needs
// reconfiguring. It shouldn't do anymore processing than
// it needs to.
void WindowManager::reconfigureMenu()
{
	XGrabServer(dpy);
	
	if(menulist->count())
	{
		menulist->removeAll();

		rm->removeAll();		
		addToMenuList(rm);
		rm->parseMenuConfigurationFile();
	}

	XUngrabServer(dpy);
}

int WindowManager::QueryShapeExtentions()
{
	int dummy2;

	return shape = XShapeQueryExtension(dpy, &shape_event, &dummy2);
}

void WindowManager::scan_windows()
{
	unsigned int i, nwins;
	Window dw1, dw2, *wins;
	XWindowAttributes attr;

	XQueryTree(dpy, root, &dw1, &dw2, &wins, &nwins);
	for (i = 0; i < nwins; i++) {
		XGetWindowAttributes(dpy, wins[i], &attr);
		if (!attr.override_redirect && attr.map_state == IsViewable)
			head_client->make_new_client(wins[i]);
	}
	XFree(wins);
}

void WindowManager::do_event_loop()
{
	XEvent ev;

	BaseMenu *m = None;

	int xfd = ConnectionNumber(wm->dpy);

	int dx=0,dy=0;

	for (;;) 
	{
		if(XPending(wm->dpy))
		{
			XNextEvent(wm->dpy, &ev);

			switch (ev.type) 
			{
				case ButtonPress:
				{
					head_client->handle_button_press_event(&ev.xbutton); 
					t->do_button_press_event(&ev.xbutton);
					
					m = findInMenuList(&ev.xbutton.window);
					
					if(m)
					{
						m->handle_button_press_event(&ev.xbutton);	
					} 
									
					switch(ev.xbutton.button)
					{
						case 2: {
							if(ev.xbutton.window == root)
					  		{
								t->setStartMenuButtonStateNotPressed();

								im->update_all();
								im->setAllMenuTitlesVisible(im,True);
								im->show();
					  		}
							
						break;
						}

						case 3: {
							
							if(ev.xbutton.window == root)
					  		{
								t->setStartMenuButtonStateNotPressed();
								
								rm->hideAllVisibleSubmenus();
								
								reconfigureMenu();
								
								rm->setAllMenuTitlesVisible(rm, True);
								rm->show();
					  		}
						break;	
						}
						
						default:
						{
							if(ev.xbutton.window == root)
					  		{
								t->setStartMenuButtonStateNotPressed();

								im->setAllMenuTitlesVisible(im, False);
								im->hide(im);

								rm->setAllMenuTitlesVisible(rm, False);
								rm->hideAllVisibleSubmenus();
								rm->hide(rm);
							}

						}
						break;

					}
				break;
				}
			
				case ButtonRelease:
				{
					m = findInMenuList(&ev.xbutton.window);
					
					if(m)
						m->handle_button_release_event(&ev.xbutton);	
										
					switch(ev.xbutton.button)
					{
					        case 1:
						case 2: 
						case 3: 
						default:
						{
							if(menu_event)
							{
								wm->ungrabPointer();
								menu_event=false;
							}
						
						}
						break;
					}
					
					
				break;
				}
				
				case ConfigureRequest:
				{
					head_client->handle_configure_request(&ev.xconfigurerequest); 
				
				break;
				}
				
				case ConfigureNotify:
				{
				break;
				}
				
				case DestroyNotify:
				{
					head_client->handle_destroy_notify(&ev.xdestroywindow);
				break;
				}
				
				case MapRequest:
				{
					head_client->handle_map_request(&ev.xmaprequest);
				
				break;
				}
			
				case UnmapNotify:
				{
					head_client->handle_unmap_event(&ev.xunmap); 
				break;
				}
			
				case ClientMessage:
				{
					head_client->handle_client_message(&ev.xclient); 
				break;
				}
			
				case ColormapNotify:
				{
					head_client->handle_colormap_change(&ev.xcolormap); 
				break;
				}
			
				case PropertyNotify:
				{
					head_client->handle_property_change(&ev.xproperty); 
				break;
				}

				case LeaveNotify:
				{
					m = findInMenuList(&ev.xcrossing.window);
													
					if(m)
						m->handle_leave_notify(&ev.xcrossing);	

				break;
				}
			
				case EnterNotify:
				{
					head_client->handle_enter_event(&ev.xcrossing); 
						
					m = findInMenuList(&ev.xcrossing.window);
										
					if(m)
						m->handle_enter_notify(&ev.xcrossing);	
				break;
				}

				case MotionNotify:
				{
					if(menu_event)
					{
						grabPointer(root, ButtonReleaseMask | PointerMotionMask, move_curs);
						dx = ev.xmotion.x_root - menu->menu.x_move;
						dy = ev.xmotion.y_root - menu->menu.y_move;
						menu->menu.x = dx;
						menu->menu.y = dy;
						XMoveWindow(dpy, menu->menu.window, dx, dy);				
					}
					
					m = findInMenuList(&ev.xmotion.window);
					
					if(m)
						m->handle_motion_notify_event(&ev.xmotion);
					
				break;
				}
						
				case Expose:
				{
					head_client->handle_expose_event(&ev.xexpose);
					t->handle_expose_event(&ev.xexpose); 
				
					rm->handle_expose_event(&ev.xexpose); 
					
					m = findInMenuList(&ev.xexpose.window);
					
					if(m)
						m->handle_expose_event(&ev.xexpose); 
				
					break;
				}
										
				default: 
				{				
					if (wm->shape && ev.type == wm->shape_event) 
					head_client->handle_shape_change((XShapeEvent *)&ev);
				break;		 
				}	
			
			
			}
		
		} else {

			fd_set rfds;
			FD_ZERO(&rfds);
			FD_SET(xfd, &rfds);

			struct timeval tv;
			tv.tv_sec = 0;
			tv.tv_usec = 1000;
			
			select(xfd + 1, &rfds, 0, 0, &tv);

			t->checkClock();
		}

	}
	

}

// Function:
//
// void WindowManager::quit_nicely()
//
// Purpose:
//
// This functions sole purpose in life is to preform clean up so that we can
// shutdown Sapphire safely.
//
// Returns:
// 
// This function returns nothing.
//
void WindowManager::quit_nicely()
{
	saveCurrentTheme();
	
	this->~WindowManager();

	XSetInputFocus(dpy, PointerRoot, RevertToPointerRoot, CurrentTime);
	XInstallColormap(dpy, DefaultColormap(dpy, screen));
	XCloseDisplay(dpy);
	
	printf("%s", windowmanager_exit_message);
	
	exit(0);
}

void WindowManager::restart()
{
	saveCurrentTheme();
	
	this->~WindowManager();
	
	XSetInputFocus(dpy, PointerRoot, RevertToPointerRoot, CurrentTime);

	XInstallColormap(dpy, DefaultColormap(dpy, screen));

	XCloseDisplay(dpy);
		
	execl("/bin/sh", "sh", "-c", windowmanager_name, 0);
}

BaseMenu* WindowManager::findInMenuList(Window *w)
{
	if(extramenulist->count())
	{
		LinkedListIterator<BaseMenu> mx(extramenulist);
				
		for(; mx.current(); mx++) 
		{
			if(*w == mx.current()->getMenuWindow()      || 
			   *w == mx.current()->getMenuTitleWindow() ||
			   *w == mx.current()->getMenuItemWindow())
			{
				return mx.current();
			} 
		}	
	
	} 

	if(menulist->count())
	{
		LinkedListIterator<BaseMenu> my(menulist);
				
		for(; my.current(); my++) 
		{
			if(*w == my.current()->getMenuWindow()      || 
			  *w == my.current()->getMenuTitleWindow() ||
			  *w == my.current()->getMenuItemWindow())
			{
				return my.current();
			} 
		}
	
		
	}
	
	return NULL;
}

void WindowManager::grabPointer(Window w, unsigned int mask, Cursor curs)
{
	if (! XGrabPointer(dpy, w, False, mask, GrabModeAsync, GrabModeAsync, None, curs, CurrentTime) == GrabSuccess ) 
		return;	
}

void WindowManager::ungrabPointer()
{
	XUngrabPointer(dpy, CurrentTime);	
}

void WindowManager::changeTheme(char* themeFile)
{	
	image_control->removeAllImages();	
	
	theme->parseTheme(themeFile);

	XFreeFont(dpy, xfont);
	XFreeGC(dpy, string_gc);
	if(font_pix) XFreePixmap(dpy, font_pix);
	
	xfont = XLoadQueryFont(dpy, theme->getFont());
	
	if(!xfont) 
	{ 
		err("the font specified in the current theme was not found, using default font.");
		
		xfont = XLoadQueryFont(dpy, font);
	}
	
	font_position = iheight - xfont->descent;
	
	createFontPixmap();
	
	gv.font = xfont->fid;
	gv.tile = font_pix;	
	string_gc = XCreateGC(dpy, root,  GCTile | GCFont , &gv);
	XSetFillStyle(dpy, string_gc, FillTiled);
	
	t->changeTheme();	
	
	if(ClientList->count())
	{
		LinkedListIterator<Client> c(ClientList);
		for(; c.current(); c++) 
		{
			c.current()->themeChange(c.current());
		}
	}
	
	if(menulist->count())
	{
		LinkedListIterator<BaseMenu> mx(menulist);
		for(; mx.current(); mx++) 
		{
			mx.current()->reconfigure();
		}
	}

	if(extramenulist->count())
	{
		LinkedListIterator<BaseMenu> my(extramenulist);
		for(; my.current(); my++) 
		{
			my.current()->reconfigure();
		}
	}
}

void WindowManager::saveCurrentTheme()
{
	// Hopefully I've gotten the bugs out of this function
	// Only time will tell....

	char* save_theme = theme->getCurrentTheme();
	
	FILE *wmconf;
	
	if((wmconf = fopen(windowmanager_conf, "w"))==NULL)
	{
		char *sapphire_config_dir = new char[strlen(HOME)+30];
		sprintf(sapphire_config_dir, "%s/.sapphire", HOME);
		
		printf("sapphire_config_dir = %s\n", sapphire_config_dir);
		
		mkdir(sapphire_config_dir, S_IRWXU);

		delete [] sapphire_config_dir;

		if((wmconf = fopen(windowmanager_conf, "w"))==NULL)
		{
			perror("error:\n");
			exit(-1);
		} 
	}
	
	if(toolbar_visible)
		fprintf(wmconf, "toolbar.visible: true\n");
	else 
		fprintf(wmconf, "toolbar.visible: false\n");
	
	if(toolbar_position)
		fprintf(wmconf, "toolbar.position: top\n");
	else 
		fprintf(wmconf, "toolbar.position: bottom\n");

	fprintf(wmconf, "start.theme: %s\n", save_theme);		
	fprintf(wmconf, "clock.format: %s\n", clock_format);
	//fprintf(wmconf, "desktops.count: %d\n", max_desktops);

	fclose(wmconf);
}
