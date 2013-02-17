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

RootMenu::RootMenu() :
	BaseMenu()
{
}

// This parses the menu file and creates the root menu.
void RootMenu::parseMenuConfigurationFile()
{
	//char *menupath = "/.sapphire/menu/default";
	//char path[strlen(HOME)+strlen(menupath)];
	//sprintf(path, "%s%s", HOME, menupath);

	char *menupath = "/menu/default";
	char path[strlen(configuration_home)+strlen(menupath)+1];
	sprintf(path, "%s%s", configuration_home, menupath);
	
	MenuLex *ml = new MenuLex(path, this);
	ml->parse();
	delete ml;
}

void RootMenu::defaultMenu()
{
	printf("somethings wrong with your menu configuration, using default menu\n");
		
	setMenuTitle("Default Menu");
	insert("xterm", "xterm", 0);
	insert("reconfigure", "", 5);
	insert("exit", "", 1);	
	update_all();
	
	wm->addToMenuList(this);
}

