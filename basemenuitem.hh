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


#ifndef _BASEMENUITEM_HH_
#define _BASEMENUITEM_HH_

// Represents a menu item.
class BaseMenuItem {
public:	
	char *name;    // visible name appearing on menu

	char *exec;    // command this item executes.

	int function;  // this would replace exec and execute a 
		       // window manager function like shutting down.

	BaseMenu *sub; // submenu this item points to.
	
	bool isSelected;
	
	int item_x, item_y;
	
	Window icon;   // window associated with an icon.
	
	Client *client;
	
	int index;
};

#endif
