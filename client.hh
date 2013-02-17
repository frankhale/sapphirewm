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


#ifndef _CLIENT_HH_

class Client 
{
private:
	Display 	*dpy;
	
	Window  	title;
	Window		window;
	Window		parent;
	Window		transient;
	Window		right_sidebar;
	Window		resize_win;

	Pixmap 		frame_pix;	// Pixmap used for the titlebar
	Pixmap 		button_pix;	// Pixmap used for titlebar buttons
	Pixmap		right_pix;
	Pixmap		resize_pix;
	
	Widget 		shadeButton;
	Widget 		maximize;
	Widget 		minimize;
	Widget 		close;
	
	bool		moving;
	
	int		right_sidebar_width;
	
	int 		default_button_width;
	int 		default_button_height;

	int 		default_close_button_x;
	int		default_close_button_y;
	
	int 		default_shade_button_x;
	int		default_shade_button_y;
	
	int 		default_minimize_button_x;
	int		default_minimize_button_y;	

	int 		default_maximize_button_x;
	int		default_maximize_button_y;

	int 		default_titlebar_height;
	
	int 		default_client_name_x;
	int		default_client_name_y;
	
	int		close_button_pressed;
	int 		shade_button_pressed;
	int		minimize_button_pressed;
	int 		maximize_button_pressed;
	
	char		*name;

	char		client_position[40];
	int		text_width;
	
	bool		shaped;
	
	long 		state;

	// bitmap of what desktops this client is in
	int             desktop_mask;
	int             away;

	Colormap	cmap;
	XSizeHints	*size;
	int		ignore_unmap;

	int		x;
	int		y;
	int		width;
	int		height;

	bool 		override_redirect;

	int		oumx;
	int		oumy;
	int		oumwidth;
	int		oumheight;

	int		old_width;
	int		old_height;

	bool 		button1;
	bool 		button2;
	bool 		button3;
	
	bool 		shaded;
	bool 		maxsized;

	// Titlebar turned on/off
	bool 		has_titlebar;
	
	// Client buttons turned on/off
	bool 		has_close_button;
	bool		has_minimize_button;
	bool		has_maximize_button;
	bool 		has_shade_button;
	
	bool 		has_focus;
	
	MwmHints *mwm_hint;
	
public: /* member functions */

	Client(); 
	~Client();

	Client *find_client(Window w);

private:
	
	Window createChildWindow(Window parent, int x, int y, unsigned int width,
		unsigned int height,unsigned int borderwidth);
	
	Window createTopLevelWindow(int x, int y, unsigned int width,
		unsigned int height,unsigned int borderwidth);

	void init_position(Client *c);
	
	void reparent(Client *c);
	void send_config(Client *c);
	
	void resize(Client *c);
	void move(Client *c);
	void change_gravity(Client *c, int multiplier);
	void shade(Client *c);
	void maximumresize(Client *c);
	
	void closeButtonPressed(Client *c);
	void shadeButtonPressed(Client *c);
	void minimizeButtonPressed(Client *c);
	void maximizeButtonPressed(Client *c);
	
	void set_shape(Client *c);

	void drag(Client *c);
	void sweep(Client *c);
	void recalculate_sweep(Client *c, int x1, int y1, int x2, int y2);
	void draw_outline(Client *c);
	void draw_shade_button(Client *c);
	int  get_wm_state(Client *c);
	void set_wm_state(Client *c, int state);
	void redraw(Client *c);
	void installColorMap(Colormap cmap);
	void isIconic(Client *c);			// If this client is iconic the unmap it otherwise map it
	int isViewable(Client *c);			// Is this client visible?
	void mapClient(Client *c);			// Map Client to screen
	void getTransients(Client *c);			// Set this clients transient
	
	// Client window decoration member functions
	void determineWindowDecorations(Client *c);	// Determines if this client needs to have a titlebar and what buttons to display.
	void createTitleImage(Client* c);		// Creates the Title Image 
	void createButtonImage(Client* c);		// Creates the Button Image
	void createRightSidebarImage(Client *c);
	void createResizeImage(Client *c);

	void setButtonPositionDefaults(Client *c);
	
	void createParentWindow(Client *c);		// Creates the Parent Window
	void createTitlebarWindow(Client *c);		// Creates the Titlebar Window
	void createCloseButtonWindow(Client *c);	// Creates the Close Button Window
	void createMinimizeButtonWindow(Client *c);	// Creates the Minimize Button Window
	void createMaximizeButtonWindow(Client *c);	// Creates the Maximimize Button Window
	void createShadeButtonWindow(Client *c);	// Creates the Shade Button Window
	void createRightSidebarWindow(Client *c);
	void createResizeWindow(Client *c);
	
	int  theight(Client *c) const; 			// returns the titlebar height for a client
	
	int send_xmessage(Window w, Atom a, long x, long y);
	void send_wm_delete(Client *c);

	void reconfigure(Client *c);

public:
	void hide(Client *c);
	void unhide(Client *c);
	void show(Client *c) { mapClient(c); }
	
	void raise();

	void get_mouse_position(int *x, int *y);	
	void make_new_client(Window w);
	
	// Client Events
	void handle_unmap_event(XUnmapEvent *e);
	void handle_button_press_event(XButtonEvent *e);
	void handle_client_message(XClientMessageEvent *e);
	void handle_colormap_change(XColormapEvent *e);
	void handle_configure_request(XConfigureRequestEvent *e);
	void handle_enter_event(XCrossingEvent *e);
	void handle_expose_event(XExposeEvent *e);
	void handle_map_request(XMapRequestEvent *e);
	void handle_property_change(XPropertyEvent *e);
	void handle_destroy_notify(XDestroyWindowEvent *e);
	
	Window getParentWindow() const { return parent; }
	
	void themeChange(Client *c);
	void handle_shape_change(XShapeEvent *e);
	void remove_client(Client *c);
};

#endif 
