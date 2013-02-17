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


#ifndef _MENULEX_HH_
#define _MENULEX_HH_

class MenuLex : public Scanner
{
private: /* variables */

	char* name;	// Name displayed in menu title
	char* exe;	
	
	RootMenu *root;

	LinkedList<BaseMenu> *menulist;

	int submenus;

	bool error;

public: /* Constructor, Destructor and Member functions */
	MenuLex(char *filename, RootMenu *menu);
	~MenuLex();
	
	void parse();

private: /* Member functions */
	bool menu(bool validate);
	bool exec(BaseMenu *sub, bool validate);
	bool theme(BaseMenu *sub, bool validate);
	bool submenu(BaseMenu *sub, bool validate);
	bool separator(BaseMenu *sub, bool validate);
	bool exit(BaseMenu* sub, bool validate);
	bool restart(BaseMenu* sub, bool validate);
	bool reconfigure(BaseMenu* sub, bool validate);
	
	BaseMenu* getMenu(int index);	
	BaseMenu* getLastMenu();
	
	bool statement(BaseMenu *sub, bool validate);
};

#endif
