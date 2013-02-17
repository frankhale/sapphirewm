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

#ifndef _BASEMENU_HH_
#define _BASEMENU_HH_

class BaseMenu
{
protected:	
	// Contains the menu items of this menu.
	LinkedList<BaseMenuItem> *mi;
	
	Pixmap menu_pix;
	Pixmap select_pix;
	Pixmap title_pix;
	Pixmap menufont_pix;
	
	XSetWindowAttributes attrib;
	unsigned long create_mask;	

	GC menu_gc;
	GC select_gc;
	GC back_gc;
	XFontStruct *font;
	XCharStruct overall;
	
	int direction;
	int ascent;
	int descent;
	int counter;
	
	// for window menu icons / desktop number / etc.
	int extra_width;
	
	bool bottom_edge;
	bool right_edge;
	
	unsigned int item_width,item_height;
	
	Cursor curs;
	
	// Used to know which item to paint.
	BaseMenuItem *curr;
	
	// This is for our synthetic enter event and we only want this to happen once.
	bool enterOnce;	

public:
	struct Menu {
		char* menuTitle;
		Window title;
		Window window;
		Window item_window;
		int x,y;
		int x_move, y_move;
		int width,height;
		
		bool hasTitle;
		
		bool isVisible;
		
		unsigned int total_item_height;
	} menu;
	
public:		
	
	BaseMenu();
	virtual ~BaseMenu();
		
	void setMenuTitle(char* t) { menu.hasTitle=True; 
				     menu.menuTitle=new char[strlen(t)+1];
				     strcpy(menu.menuTitle,t); 
				}

	char* getMenuTitle() const { return menu.menuTitle; }				

	LinkedList<BaseMenuItem> *getMenuItemList() const { return mi; }
					
	void setTitleVisible(bool s);
	void setMenuPos(int x, int y);
	void show();
	void show(int x, int y);
	void showSub(BaseMenu *sub, int x, int y);
	void hide(BaseMenu *sub);
	void hideAllVisibleSubmenus();
	void setAllMenuTitlesVisible(BaseMenu *sub, bool s);
	
	int getMenuHeight() const { return menu.height; }
	
	void update_all();
	void update_menu();
	void reconfigure();

	virtual void insert(char *n, BaseMenu *sub);
	virtual void insert(char *n, char *exec, int func);
	virtual void insert(char *n, char *exec, BaseMenu *sub, int func);
	int remove(BaseMenuItem *element);
	void removeAll();

	BaseMenuItem *findMenuItem(int x, int y);

	void handle_button_press_event(XButtonEvent *e);
	void handle_button_release_event(XButtonEvent *e);
	void handle_enter_notify(XCrossingEvent *e);
	void handle_leave_notify(XCrossingEvent *e);
	void handle_expose_event(XExposeEvent *e);
	void handle_motion_notify_event(XMotionEvent *e);
	void handle_config_notify_event(XConfigureEvent *e);

	// The menu item behavoir is defined with these
	// virtual functions in a derived class.
	virtual void handle_button1_press(BaseMenuItem *curr)   { /* Has no default behavior */ }
	virtual void handle_button2_press(BaseMenuItem *curr)   { /* Has no default behavior */ }
	virtual void handle_button3_press(BaseMenuItem *curr)   { /* Has no default behavior */ }
	virtual void handle_button1_release(BaseMenuItem *curr) { /* Has no default behavior */ }
	virtual void handle_button2_release(BaseMenuItem *curr) { /* Has no default behavior */ }
	virtual void handle_button3_release(BaseMenuItem *curr) { /* Has no default behavior */ }

	void execute(char *s);

	// Count relative to zero
	int getItemCount() const { return mi->count() - 1; }
	
	Window getMenuTitleWindow() const { return menu.title; }
	Window getMenuWindow() const { return menu.window; }
	Window getMenuItemWindow() const { return menu.item_window; }

	virtual void redraw();
	
	void hide();
	
protected:
	virtual void redraw(BaseMenuItem *m);

private:

	void move();       
	
	void createMenuImage();
	void createMenuSelectImage();
	void createMenuTitleImage();
	void createMenuFontImage();
	
	void getMousePosition(int *x, int *y);
	void testMenuEdgeDetect(BaseMenu *sub);
};

#endif
