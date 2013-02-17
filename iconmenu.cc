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

IconMenu::IconMenu() :
	BaseMenu()
{
	setMenuTitle("Icons");
	update_all();
	wm->addToExtraMenuList(this);
}


IconMenu::~IconMenu(void)
{
}

void IconMenu::insert(char *name, Client *c)
{
	BaseMenuItem *item = new BaseMenuItem();
			
	if(name==NULL) {
		char *noname = "no name";
		name = noname;
	}
	
	item->name = new char[strlen(name)+1];	
	strcpy(item->name, name);
	
	item->client = c;
	
	// defaults
	item->icon = None;
	item->sub  = None;
	item->isSelected=False;
	item->exec = "";
	item->function = 0;
	item->item_x = item->item_y = 0;

	getMenuItemList()->insert(item);
}

void IconMenu::addIcon(char *name, Client *c)
{
	insert(name, c);
	update_all();
}

// if I wanted all sub menus to have the same button behavior I would declare
// a class derived from BaseMenu which would only define the button functions
// and then derive the main menu and sub menus from that so all menus would
// have the same behavior when the items were clicked.
//
// With the code the way it is now only the first main menu will have the button
// behavior defined by these functions.
void IconMenu::handle_button1_press(BaseMenuItem *curr)
{
	if (curr)
	{
		curr->client->unhide(curr->client);
		hide(this);
		remove(curr);
		update_all();
	}
}

void IconMenu::handle_button1_release(BaseMenuItem *curr)
{
}

