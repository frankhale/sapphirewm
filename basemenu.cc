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

BaseMenu::BaseMenu() {

	menu_pix=None;
	select_pix=None;
	title_pix=None;

	select_gc = None;
	menu_gc = None;
	back_gc = None;

	mi = new LinkedList<BaseMenuItem>;
	
	menu.menuTitle="Sapphire Menu";

	menu.hasTitle=False;

	bottom_edge=False;
	right_edge=False;

	counter=0;

	extra_width=0;

	font = XLoadQueryFont(wm->getDisplay(), wm->getTheme()->getMenuFont());
	
  	create_mask = CWBackPixmap       | 
		      CWBackPixel        | 
		      CWBorderPixel      |
	    	      CWOverrideRedirect |
		      CWEventMask; 
		      
  	attrib.background_pixmap = None;
  	attrib.background_pixel = wm->getBackgroundColor().pixel;
  	attrib.border_pixel = wm->getBorderColor().pixel;
  	attrib.override_redirect = True;
  	attrib.event_mask = ButtonPressMask   |
	                    ButtonReleaseMask | 
			    PointerMotionMask |
    			    ExposureMask      |
			    EnterWindowMask   |
			    LeaveWindowMask;


	//gv.background = wm->getBackgroundColor().pixel;	
	//select_gc = XCreateGC(wm->getDisplay(), wm->getRootWindow(),  GCTile , &gv);
	//back_gc = XCreateGC(wm->getDisplay(), wm->getRootWindow(),  GCTile, &gv);

	menu.x = 1;
  	menu.y = 1;
	menu.x_move = 1;
  	menu.y_move = 1;
	menu.height=1;
	menu.width=1;
    
	menu.total_item_height=1;
    
    	item_width = menu.width;
	item_height = font->ascent + font->descent + 8;
	
	menu.window =
    		XCreateWindow(wm->getDisplay(), wm->getRootWindow(), menu.x, menu.y,
		  menu.width, menu.height, 1, DefaultDepth(wm->getDisplay(), wm->getScreen()),
		  InputOutput, DefaultVisual(wm->getDisplay(), wm->getScreen()), create_mask, &attrib);

	menu.title =
    		XCreateWindow(wm->getDisplay(), menu.window, 0, 0,
		  menu.width, item_height, 0, DefaultDepth(wm->getDisplay(), wm->getScreen()),
		  InputOutput, DefaultVisual(wm->getDisplay(), wm->getScreen()), create_mask, &attrib);
	
	menu.item_window =	
    		XCreateWindow(wm->getDisplay(), menu.window, 0, item_height - 2,
		  menu.width, menu.height-item_height, 0, DefaultDepth(wm->getDisplay(), wm->getScreen()),
		  InputOutput, DefaultVisual(wm->getDisplay(), wm->getScreen()), create_mask, &attrib);

	curs = XDefineCursor(wm->getDisplay(), menu.window, XCreateFontCursor(wm->getDisplay(), XC_left_ptr));

	
	menu.isVisible = False;
	
	curr = new BaseMenuItem();
	
	curr = None;
	
	enterOnce = True;
}

BaseMenu::~BaseMenu() {			
	mi->removeAll();

	XDestroyWindow(wm->getDisplay(), menu.title);
	XDestroyWindow(wm->getDisplay(), menu.item_window);
	XDestroyWindow(wm->getDisplay(), menu.window);

	XFreeGC(wm->getDisplay(), menu_gc);
	XFreeGC(wm->getDisplay(), select_gc);
	XFreeGC(wm->getDisplay(), back_gc);
	
	wm->getImageControl()->removeImage(title_pix);
	wm->getImageControl()->removeImage(menu_pix);
	wm->getImageControl()->removeImage(select_pix);
	wm->getImageControl()->removeImage(menufont_pix);
	
	delete mi;
	delete curr;
}

// This updates all graphics contexts, gradient pixmaps and determines placement of all menu items.
void BaseMenu::update_all()
{
	LinkedListIterator<BaseMenuItem> it(mi);
	
	int temp_width=0;
	int temp_ascent=0;
	int temp_descent=0;
	
	unsigned int count=0;
	unsigned int char_count=0;

	menu.total_item_height=1;

	// If menu is created with no items lets make a few defaults
	// so things work out nice.
	
	if (mi->count() == 0)
	{
		char_count = strlen(menu.menuTitle);

		menu.width = XTextWidth(wm->getXFont(), menu.menuTitle, strlen(menu.menuTitle));

		XTextExtents(font, menu.menuTitle , char_count, &direction, &ascent, 
		           &descent, &overall);	

		temp_width  = overall.width;
		temp_ascent = overall.ascent;
		temp_descent= overall.descent;

		menu.width = temp_width + 40;
		item_width  = temp_ascent + temp_descent + 8;
		menu.height = item_height;

		menu.total_item_height = item_height+3;
	
	} else {

	//=============================================================

	for(; it.current(); it++)
	{
		char_count = strlen(it.current()->name);

    		XTextExtents(font, it.current()->name , char_count, &direction, &ascent, 
            		&descent, &overall);		

		if (char_count < strlen(menu.menuTitle)) {
		
			char_count = strlen(menu.menuTitle);
			
			XTextExtents(font, menu.menuTitle , char_count, &direction, &ascent, 
		           &descent, &overall);		
		}
	
		if (char_count > count) {
			count = char_count; 
			temp_width  = overall.width;
			temp_ascent = overall.ascent;
			temp_descent= overall.descent;
		}
		
		menu.width = temp_width + 40;
		item_width  = temp_ascent + temp_descent + 8;
		menu.height = item_height * mi->count();
		
		it.current()->item_x = item_width/2+4;
		it.current()->item_y = item_height/2+4+menu.total_item_height;
		
		menu.total_item_height += item_height;
	}

	menu.total_item_height += item_height;
	
	}
	
	menu.width+=extra_width;

	setTitleVisible(menu.hasTitle);	

	createMenuImage();
	createMenuSelectImage();
	createMenuTitleImage();
	createMenuFontImage();
	
	XClearWindow(wm->getDisplay(), menu.title);
	XClearWindow(wm->getDisplay(), menu.item_window);
			
	XGCValues gv;
	gv.tile = select_pix;
	select_gc = XCreateGC(wm->getDisplay(), wm->getRootWindow(),  GCTile , &gv);
	XSetFillStyle(wm->getDisplay(), select_gc, FillTiled);	
	
	gv.tile = menu_pix;
	back_gc = XCreateGC(wm->getDisplay(), wm->getRootWindow(),  GCTile, &gv);
	XSetFillStyle(wm->getDisplay(), back_gc, FillTiled);

	gv.font = font->fid;
	gv.tile = menufont_pix;
	menu_gc = XCreateGC(wm->getDisplay(), wm->getRootWindow(),  GCTile | GCFont, &gv);
	XSetFillStyle(wm->getDisplay(), menu_gc, FillTiled);
	
	testMenuEdgeDetect(this);
	
	if (menu.y + menu.height >= wm->getYRes()) 
	{
		bottom_edge=True;
 	}else {
				
		bottom_edge=False;
	}
	
	if(bottom_edge)
	{
		XMoveWindow(wm->getDisplay(), menu.window, menu.x, wm->getYRes() - menu.height - item_height - 4);
	}
}

// This updates all graphics contexts, gradient pixmaps and determines placement of all menu items.
void BaseMenu::update_menu()
{
	LinkedListIterator<BaseMenuItem> it(mi);
	
	int temp_width=0;
	int temp_ascent=0;
	int temp_descent=0;
	
	unsigned int count=0;
	unsigned int char_count=0;

	menu.total_item_height=1;

	// If menu is created with no items lets make a few defaults
	// so things work out nice.
	
	if (mi->count() == 0)
	{
		char_count = strlen(menu.menuTitle);

		menu.width = XTextWidth(wm->getXFont(), menu.menuTitle, strlen(menu.menuTitle));

		XTextExtents(font, menu.menuTitle , char_count, &direction, &ascent, 
		           &descent, &overall);	

		temp_width  = overall.width;
		temp_ascent = overall.ascent;
		temp_descent= overall.descent;

		menu.width = temp_width + 40;
		item_width  = temp_ascent + temp_descent + 8;
		menu.height = item_height;

		menu.total_item_height = item_height+3;
	
	} else {

	//=============================================================

	for(; it.current(); it++)
	{
		char_count = strlen(it.current()->name);

    		XTextExtents(font, it.current()->name , char_count, &direction, &ascent, 
            		&descent, &overall);		

		if (char_count < strlen(menu.menuTitle)) {
		
			char_count = strlen(menu.menuTitle);
			
			XTextExtents(font, menu.menuTitle , char_count, &direction, &ascent, 
		           &descent, &overall);		
		}
	
		if (char_count > count) {
			count = char_count; 
			temp_width  = overall.width;
			temp_ascent = overall.ascent;
			temp_descent= overall.descent;
		}
		
		menu.width = temp_width + 40;
		item_width  = temp_ascent + temp_descent + 8;
		menu.height = item_height * mi->count();
		
		it.current()->item_x = item_width/2+4;
		it.current()->item_y = item_height/2+4+menu.total_item_height;
		
		menu.total_item_height += item_height;
	}

	menu.total_item_height += item_height;
	
	}

	menu.width+=extra_width;
	
	setTitleVisible(menu.hasTitle);
}

void BaseMenu::hide()
{
	XUnmapWindow(wm->getDisplay(), menu.window);
}

void BaseMenu::setTitleVisible(bool s) 
{ 	
	menu.hasTitle=s; 
		
	if(menu.hasTitle)
	{
		XResizeWindow(wm->getDisplay(), menu.window, menu.width, menu.total_item_height);
		XResizeWindow(wm->getDisplay(), menu.title, menu.width, item_height);
		XMoveResizeWindow(wm->getDisplay(), menu.item_window, 
				0,
				item_height,
				menu.width, 
				menu.total_item_height);
	
	} else {
		XResizeWindow(wm->getDisplay(), menu.window, menu.width, menu.total_item_height - item_height);
		XResizeWindow(wm->getDisplay(), menu.title, menu.width, item_height);
		XMoveResizeWindow(wm->getDisplay(), menu.item_window, 
				0, 0,
				menu.width, 	
				menu.total_item_height);	
	}
}

void BaseMenu::reconfigure()
{	
	XGCValues gv;

	XFreeFont(wm->getDisplay(), font);
	font = XLoadQueryFont(wm->getDisplay(),  wm->getTheme()->getMenuFont());
	update_menu();
	
	XFreePixmap(wm->getDisplay(), title_pix);
	XFreePixmap(wm->getDisplay(), menu_pix);
	XFreePixmap(wm->getDisplay(), select_pix);
	XFreePixmap(wm->getDisplay(), menufont_pix);

	createMenuImage();
	createMenuSelectImage();
	createMenuTitleImage();
	createMenuFontImage();
	
	XClearWindow(wm->getDisplay(), menu.title);
	XClearWindow(wm->getDisplay(), menu.item_window);
		
	XFreeGC(wm->getDisplay(), menu_gc);
	XFreeGC(wm->getDisplay(), select_gc);
	XFreeGC(wm->getDisplay(), back_gc);
		
	gv.tile = select_pix;
	select_gc = XCreateGC(wm->getDisplay(), wm->getRootWindow(),  GCTile , &gv);
	XSetFillStyle(wm->getDisplay(), select_gc, FillTiled);	
	
	gv.tile = menu_pix;
	back_gc = XCreateGC(wm->getDisplay(), wm->getRootWindow(),  GCTile, &gv);
	XSetFillStyle(wm->getDisplay(), back_gc, FillTiled);

	gv.font = font->fid;
	gv.tile = menufont_pix;
	menu_gc = XCreateGC(wm->getDisplay(), wm->getRootWindow(),  GCTile | GCFont, &gv);
	XSetFillStyle(wm->getDisplay(), menu_gc, FillTiled);
		
	redraw();
}

// Redraws the entire contents of the menu.
void BaseMenu::redraw()
{
	//int text_width=0;
	
	// We need an Iterator for our menu items. This lets us traverse through
	// the list and paint each menu item.
	LinkedListIterator<BaseMenuItem> it(mi);
	
	for(; it.current(); it++)
	{	  				
		if(strcmp(it.current()->name, "separator")==0) 
		{
			XDrawLine(wm->getDisplay(), menu.item_window, menu_gc, it.current()->item_x - 14, it.current()->item_y-5, menu.width-1, it.current()->item_y-5);	
			XDrawLine(wm->getDisplay(), menu.item_window, menu_gc, it.current()->item_x - 14, it.current()->item_y-6, menu.width-1, it.current()->item_y-6);	
		} else {
		
			XDrawString(wm->getDisplay(), menu.item_window, menu_gc, it.current()->item_x, 
			it.current()->item_y + 2, it.current()->name, strlen(it.current()->name));
			
			if(it.current()->sub)
			{
				XDrawRectangle(wm->getDisplay(), menu.item_window, menu_gc, 
					it.current()->item_x + menu.width - 35, 
					it.current()->item_y - 7, 
					8,
					8);
			}
		}
		
		// Draws a bounding box around each menu item. This helped me figure out the algorithm to test which item the 
		// pointer was over.
		//
		//XDrawRectangle(wm->getDisplay(), menu.item_window, menu_gc, it.current()->item_x - 14, it.current()->item_y - 14, menu.width-1, it.current()->item_y + item_height - 15);
	
	}
	
	XDrawString(wm->getDisplay(), menu.title, menu_gc, 
   	   8, 16 , menu.menuTitle, strlen(menu.menuTitle));
	
	//text_width=XTextWidth(wm->font, menu.menuTitle, strlen(menu.menuTitle));

	//XDrawString(wm->getDisplay(), menu.title, menu_gc, 
   	//   menu.width/2 - text_width/2, 
	//   14 , 
	//   menu.menuTitle, strlen(menu.menuTitle));
}

// Redraws a single menu item on the menu.
void BaseMenu::redraw(BaseMenuItem *i)
{
	if(strcmp(i->name, "separator")==0) 
	{
		XDrawLine(wm->getDisplay(), menu.item_window, menu_gc, i->item_x - 14, i->item_y-5, menu.width-1, i->item_y-5);
		XDrawLine(wm->getDisplay(), menu.item_window, menu_gc, i->item_x - 14, i->item_y-6, menu.width-1, i->item_y-6);				
	} else {
	
		XDrawString(wm->getDisplay(), menu.item_window, menu_gc, 
		i->item_x, i->item_y + 2, i->name, strlen(i->name));
		
		if(i->sub)
		{
			XDrawRectangle(wm->getDisplay(), menu.item_window, menu_gc, 
					i->item_x + menu.width - 35, 
					i->item_y - 7, 
					8, 
					8);	
		}
		
	}
	
	// Draws a bounding box around each menu item. This helped me figure out the algorithm to test which item the 
	// pointer was over.
	//
	//XDrawRectangle(wm->getDisplay(), menu.item_window, menu_gc, it.current()->item_x - 14, it.current()->item_y - 14, menu.width-1, it.current()->item_y + item_height - 15);

}

void BaseMenu::insert(char *n, BaseMenu *sub)
{
	BaseMenuItem *item = new BaseMenuItem();
		
	item->name = new char[strlen(n)+1];	
	strcpy(item->name, n);
	
	item->sub  = sub;

	// defaults
	item->isSelected=False;
	item->exec = "";
	item->function = 0;
	item->item_x = item->item_y = 0;

	mi->insert(item);
}

void BaseMenu::insert(char *n, char *exec, int func)
{
	BaseMenuItem *item = new BaseMenuItem();
	
	char *buffer = new char[100];
        if (exec[0] == '~') 
	{
		sprintf(buffer, "%s%s", getenv("HOME"), exec+1);
		strcpy(exec, buffer);
	}
	delete [] buffer;
	
	item->exec = new char[strlen(exec)+1];
	strcpy(item->exec, exec);
	
	item->name = new char[strlen(n)+1];	
	strcpy(item->name, n);
	
	item->function = func;

	// defaults
	item->isSelected=False;
	item->sub  = None;
	item->item_x = item->item_y = 0;

	mi->insert(item);
}


void BaseMenu::insert(char *n, char *exec, BaseMenu *sub, int func)
{
	BaseMenuItem *item = new BaseMenuItem();
	
	char *buffer = new char[100];
        if (exec[0] == '~') 
	{
		sprintf(buffer, "%s%s", getenv("HOME"), exec+1);
		strcpy(exec, buffer);
	}
	delete [] buffer;
	
	item->exec = new char[strlen(exec)+1];
	strcpy(item->exec, exec);
	
	item->name = new char[strlen(n)+1];	
	strcpy(item->name, n);
	
	item->isSelected=False;
	item->sub  = sub;
	item->function = func;
	item->item_x = item->item_y = 0;
	
	mi->insert(item);
}

int BaseMenu::remove(BaseMenuItem* element)
{
	mi->remove(element);

	return mi->count();
}

void BaseMenu::removeAll()
{
	mi->removeAll();
	
	update_all();
}

void BaseMenu::execute(char *s)
{
	char command[strlen(s)+8];
	
	sprintf(command, "exec %s &", s);
	system(command);
}

void BaseMenu::show()
{	
	int mouse_x=0;
	int mouse_y=0;
	
	// Gets the coordinates of the mouse
	getMousePosition(&mouse_x, &mouse_y);
		
	// Check to make sure menu will be displayed on the screen.
 	if (mouse_x + menu.width > wm->getXRes()) {
 		menu.x = wm->getXRes() - menu.width - 1;
		
		right_edge=True;
 	} else {
		menu.x = mouse_x;
		
		right_edge=False;
	}
	
	if (mouse_y + menu.height > wm->getYRes()) {
		menu.y = wm->getYRes() - menu.height - item_height - 4;
		
		bottom_edge=True;
 	}else {
		
		menu.y = mouse_y;
		
		bottom_edge=False;
	}
	
	// Move the menu to the position of the mouse pointer
	XMoveWindow(wm->getDisplay(), menu.window, menu.x, menu.y);
	
	// Show the menu windows
	XMapRaised(wm->getDisplay(), menu.window);
	XMapSubwindows(wm->getDisplay(),menu.window);
	
	menu.isVisible=True;
}

void BaseMenu::getMousePosition(int *x, int *y)
{
	Window dw1=None;
	Window dw2=None;
	int t1=0;
	int t2=0;
	unsigned int t3=0;

	XQueryPointer(wm->getDisplay(), wm->getRootWindow(), &dw1, &dw2, x, y, &t1, &t2, &t3);
}

void BaseMenu::show(int x, int y)
{	
	menu.x = x;
	menu.y = y;

	XMoveWindow(wm->getDisplay(), menu.window, x, y);
	
	// Show the menu windows
	XMapSubwindows(wm->getDisplay(),menu.window);
	XMapRaised(wm->getDisplay(), menu.window);

	XClearWindow(wm->getDisplay(), menu.item_window);

	menu.isVisible=True;
}

void BaseMenu::showSub(BaseMenu *sub, int x, int y)
{
	sub->menu.x = x;
	sub->menu.y = y;
	
	XMoveWindow(wm->getDisplay(), sub->menu.window, x, y);
		
	// Show the menu windows
	XMapSubwindows(wm->getDisplay(), sub->menu.window);
	XMapRaised(wm->getDisplay(),  sub->menu.window);

	sub->menu.isVisible=True;
}

void BaseMenu::hide(BaseMenu *sub)
{
	
	if (sub->menu.isVisible)
	{
		// hide the menu windows
		XUnmapSubwindows(wm->getDisplay(), sub->menu.window);
		XUnmapWindow(wm->getDisplay(), sub->menu.window);

		sub->menu.isVisible=False;
	}
	
}

BaseMenuItem *BaseMenu::findMenuItem(int x, int y)
{
	int item_xpos2=0;
	int item_ypos2=0;
	
	if(mi->count()) 
	{
		LinkedListIterator<BaseMenuItem> it(mi);
		for(; it.current(); it++)
		{

			// Draws a bounding box around each menu item. This helped me figure out the algorithm to test which item the 
			// pointer was over.
			//
			//XDrawRectangle(wm->getDisplay(), menu.item_window, menu_gc, it.current()->item_x - 14, 
			//	it.current()->item_y - 14, menu.width-1, it.current()->item_y + item_height - 15);


			item_xpos2 = menu.width-1;
			item_ypos2 = it.current()->item_y + item_height - 15;
	  
			if ( (x >= it.current()->item_x - 14) && 
			     (x < item_xpos2) &&
			     (y >= it.current()->item_y - 14) &&  
			     (y < item_ypos2)
			   ) 
			    {
			    	
				    return it.current();
			    }
		}	
	}

	return NULL;
}

void BaseMenu::handle_button_press_event(XButtonEvent *e)
{
	switch (e->button) 
	{
			case Button1:
			{
				if(e->window == menu.title) move(); 
				else if (curr) handle_button1_press(curr);
			break;
			}

			case Button2:
			{
				if(curr) handle_button2_press(curr);
			break;
			}
			
			case Button3:
			{
				if (curr) handle_button3_press(curr);
			break;
			}
	} 
}

void BaseMenu::handle_button_release_event(XButtonEvent *e)
{
	switch (e->button) {
			case Button1:
			{
				if (curr) {
	
					handle_button1_release(curr);
				
					switch(	curr->function )
					{
						// function 1 is wm shutdown
						case 1:
						{
							wm->quit_nicely();
						}
						break;
							
						case 2: // do nothing this is a menu separator
						{
						}
						break;
							
						case 3: // This will start another window manager
						{
							//wm->start_another_windowmanager(curr->exec);
						}
						break;
						
						case 4: // restart Sapphire
						{
							wm->restart();
						}
						break;
										
						case 5: // reconfigure Sapphire (theme, menu)
						{
							wm->reconfigureMenu();
						}
						break;

						case 6: // change theme 
						{
							Toolbar *t = wm->getToolbar();
							t->setStartMenuButtonStateNotPressed();					
							t->getToolbarMenu()->hide(t->getToolbarMenu());
							
							wm->getRootMenu()->hideAllVisibleSubmenus();
							wm->changeTheme(curr->exec);
						}
						break;

						default: // Default is to execute a command or file
						{
							Toolbar *t = wm->getToolbar();
							t->setStartMenuButtonStateNotPressed();							
							t->getToolbarMenu()->hide(t->getToolbarMenu()); 
							wm->getRootMenu()->hideAllVisibleSubmenus();
							execute(curr->exec);
						}
						break;
					}										
				}
				
			break;
			}

			case Button2:
			{
				if (curr) handle_button2_release(curr);
			break;
			}
			
			case Button3:
			{
				if(e->window == menu.title) hideAllVisibleSubmenus();
				else if (curr) handle_button3_release(curr);
			break;
			}
	} 
}

void BaseMenu::move()
{
	wm->setMenu(this);
	wm->setMenuEvent(true);
}

void BaseMenu::setMenuPos(int x, int y)
{
	menu.x = x; 
	menu.y = y;
	
	XMoveWindow(wm->getDisplay(), menu.window, x, y);
}

void BaseMenu::handle_enter_notify(XCrossingEvent *e)
{
	if(curr)
	{					
		LinkedList<BaseMenuItem> *temp_mi;

		LinkedListIterator<BaseMenuItem> it(mi);
		for(; it.current(); it++)
		{	  	
			if (it.current()->sub && it.current()->sub->menu.isVisible)
			{
				it.current()->sub->hide(it.current()->sub);
				
				temp_mi = it.current()->sub->mi;
				
				LOOP:
				
				LinkedListIterator<BaseMenuItem> n(temp_mi);
				for(; n.current(); n++)
				{
					if(n.current()->sub && n.current()->sub->menu.isVisible)
					{
						n.current()->sub->hide(n.current()->sub);
					
						temp_mi = n.current()->sub->mi;
						
						goto LOOP;						
					} 					
				}
			} 
		}
   		
		XFillRectangle(wm->getDisplay(), menu.item_window, select_gc, 
			0, 
			curr->item_y - 15, 
			item_width - item_height - 2, 
			item_height - 1
			);	 

		if(curr->sub) 
		{
			testMenuEdgeDetect(curr->sub);
			
			if(curr->sub->right_edge) 
			{
				if(curr->sub->bottom_edge)
					showSub(curr->sub,menu.x - curr->sub->menu.width - 1, wm->getYRes() - curr->sub->menu.height - 27);
				else
					showSub(curr->sub,menu.x - curr->sub->menu.width - 1, menu.y + curr->item_y - 15);
			}
			else {
				if(curr->sub->bottom_edge)
				{
					showSub(curr->sub,menu.x + menu.width + 1, wm->getYRes() - curr->sub->menu.height - 27);
				}
				else {
					showSub(curr->sub,menu.x + menu.width + 1, menu.y + curr->item_y - 15);
				}
			}
		} 
		redraw(curr);
	}	
}

void BaseMenu::handle_leave_notify(XCrossingEvent *e)
{
	if(curr)
	{	
		XFillRectangle(wm->getDisplay(), menu.item_window, back_gc, 
			0, 
			curr->item_y -15, 
			item_width - item_height - 2, 
			item_height - 1);	 
		
		redraw(curr);
		curr = NULL;			
	}
}

void BaseMenu::hideAllVisibleSubmenus()
{
	LinkedList<BaseMenuItem> *temp_mi;
	
	hide(this);

	LinkedListIterator<BaseMenuItem> it(mi);
	for(; it.current(); it++)
	{	  	
		if (it.current()->sub && it.current()->sub->menu.isVisible)
		{
			it.current()->sub->hide(it.current()->sub);
			
			temp_mi = it.current()->sub->mi;
			
			LOOP: // Yuck I'm using a goto... ;-(
				
			LinkedListIterator<BaseMenuItem> n(temp_mi);
			for(; n.current(); n++)
			{
				if(n.current()->sub && n.current()->sub->menu.isVisible)
				{
					n.current()->sub->hide(n.current()->sub);
					
					temp_mi = n.current()->sub->mi;
						
					goto LOOP;						
				} 					
			}
		} 
	}
}

void BaseMenu::setAllMenuTitlesVisible(BaseMenu *sub, bool s)
{	
	sub->setTitleVisible(s);

	LinkedListIterator<BaseMenuItem> it(sub->mi);
	for(; it.current(); it++)
	{	  	
		if (it.current()->sub) setAllMenuTitlesVisible(it.current()->sub, s);
	}
}

void BaseMenu::handle_expose_event(XExposeEvent *e)
{
	if(e->count == 0) redraw();
}

void BaseMenu::handle_motion_notify_event(XMotionEvent *e)
{
	// for moving the menu.
	menu.x_move = e->x;
	menu.y_move = e->y;
	
	//----------------------
	XEvent temp;	
	
	if(e->window == menu.title) { /* DO NOTHING */ }
	else if(e->window == menu.item_window)
	{
		BaseMenuItem *i = findMenuItem(e->x, e->y);

		if (i && enterOnce)
		{
			curr = i;

			if(! strcmp(i->name, "separator")==0) handle_enter_notify(&temp.xcrossing);				

			enterOnce=False;
		} else {

			
			BaseMenuItem *i = findMenuItem(e->x, e->y);
			
			if (i != curr)
			{
				handle_leave_notify(&temp.xcrossing);
			
				curr =  i;
				
				enterOnce=True;
			} 
		}
	}	
}

void BaseMenu::handle_config_notify_event(XConfigureEvent *e)
{
}

void BaseMenu::createMenuImage()
{
	unsigned long style=0;

	style = wm->getTheme()->getMenuStyle();		
					
	menu_pix = wm->getImageControl()->renderImage(
			//"menu_frame",
			menu.width, 
			menu.height, 
			style, 
			wm->getTheme()->getMenuColorFrom(), 
			wm->getTheme()->getMenuColorTo()
			);
	
	XSetWindowBackgroundPixmap(wm->getDisplay(), menu.item_window, menu_pix);
}

void BaseMenu::createMenuSelectImage()
{
	unsigned long style=0;
	
	style = wm->getTheme()->getMenuSelectStyle();		

	select_pix = wm->getImageControl()->renderImage(
			//"menu_select",
			menu.width, 
			item_height, 
			style, 
			wm->getTheme()->getMenuSelectColorFrom(), 
			wm->getTheme()->getMenuSelectColorTo()
			);
}

void BaseMenu::createMenuFontImage()
{
	unsigned long style=0;
	
	style = wm->getTheme()->getFontStyle();		

	menufont_pix = wm->getImageControl()->renderImage(
			//"menu_font",
			font->ascent, 
			font->descent, 
			style, 
			wm->getTheme()->getMenuFontColorFrom(), 
			wm->getTheme()->getMenuFontColorTo()
			);
}

void BaseMenu::createMenuTitleImage()
{
	unsigned long style=0;

	style = wm->getTheme()->getMenuTitleStyle();		

	title_pix = wm->getImageControl()->renderImage(
			//"menu_title",
			menu.width, 
			item_height+1, 
			style, 
			wm->getTheme()->getMenuTitleColorFrom(), 
			wm->getTheme()->getMenuTitleColorTo()
			);
		
	XSetWindowBackgroundPixmap(wm->getDisplay(), menu.title, title_pix);
}

void BaseMenu::testMenuEdgeDetect(BaseMenu *sub)
{
	int mouse_x=0;
	int mouse_y=0;
	
	bool rightEdge=False;
	bool bottomEdge=False;
	
	// Gets the coordinates of the mouse
	getMousePosition(&mouse_x, &mouse_y);

	rightEdge = ( ( this->menu.x + sub->menu.width + 150 ) < (wm->getXRes()) ) ? True : False;
	
	bottomEdge = ( mouse_y + sub->menu.height >= wm->getYRes() ) ? True : False;

	// Check to make sure menu will be displayed on the screen.
	if (rightEdge) sub->right_edge=False;
 	else sub->right_edge=True;
	
	if (bottomEdge) sub->bottom_edge=True;
	else sub->bottom_edge=False;
}
