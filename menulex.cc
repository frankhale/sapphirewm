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

MenuLex::MenuLex(char *filename, RootMenu *menu) : Scanner(filename) 
{ 
	root = menu;
	
	menulist = wm->getMenuList();
		
	error=false;
	
	submenus=1;
	
	name=new char[255];
	exe=new char[255];
}

MenuLex::~MenuLex()
{	
	delete [] name;
	delete [] exe;
}

BaseMenu* MenuLex::getLastMenu()
{
	if(menulist->count())
		return menulist->last();

	return NULL;
}

BaseMenu* MenuLex::getMenu(int index)
{
	int counter=0;
	
	if(index==0) return root;
	
	LinkedListIterator<BaseMenu> it(menulist);
	for(; it.current(); it++)
	{	
		if(counter==index) return it.current();
		
		counter++;
	}
	
	return NULL;
}


void MenuLex::parse()
{
	// Check to see if this menu file is gramatically correct.
	do {
	
		getNextToken();
	
	} while(menu(true));

	// If we didn't get any errors when we validated the menu
	// then lets build the menu.
	if(!error) {
	
		rewind();
		
		do {
	
			getNextToken();
	
		} while(menu(false));
	
		LinkedListIterator<BaseMenu> it(menulist);
		for(; it.current(); it++)
		{	
			it.current()->update_all();
		}
	}
	else {
		
		// If there was an error use the default menu.
		
		root->defaultMenu();
	}
	
}

bool MenuLex::menu(bool validate)
{
	if(match("menu"))
	{
		getNextToken();		
		
		if(match("("))
		{
			clearToken();
			getNextToken(true);
			strcpy(name, currentToken());
			//printf("name = %s\n", currentToken());

			if(! validate)
				root->setMenuTitle(name);
	
			getNextToken();
			if(match(")"))
			{
				getNextToken();
				
				if(match("{"))
				{
					getNextToken();
						
					if(match("}"))
					{
						printf("There was an error parsing your menu configuration file, I have to use the default menu because of this problem.\n");
						error = true;
						return true;
					} else
					{
												
						while(statement(root, validate)) {
							getNextToken();
						} 
							
						if(match("}"))
						{
							return true;
						} else {
							printf("missing } near (%s)\n", name);
							error = true;
							return false;
						}
	
					}
				} else {
					printf("missing { near menu\n");
					error = true;
					return false;
				}
			
			} else {
				printf("missing ) near menu\n");
				error = true;
				return false;
			}
		
		} else {
			printf("missing ( near menu\n");
			error = true;
			return false;
		}
	}
	
	return false;
}

bool MenuLex::submenu(BaseMenu *sub, bool validate)
{
	BaseMenu *child=NULL;
	if(match("submenu"))
	{
		getNextToken();		
		
		if(match("("))
		{
			clearToken();
			getNextToken(true);
			strcpy(name, currentToken());
			
			getNextToken();
			if(match(")"))
			{
				getNextToken();
				
				if(match("{"))
				{
					getNextToken();
												
					if(! validate)
					{
						child = new BaseMenu();
						child->setMenuTitle(name);
						menulist->insert(child);
						sub->insert(name, child);
					}
						
					while(statement(child, validate)) {
						getNextToken();
					} 
							
					if(match("}"))
					{
						if(! validate)
						{
							submenus=0;
						}

						return true;
					} 
	
				} else {
					printf("missing { near submenu\n");
					error = true;
					return false;
				}
			
			} else {
				printf("missing ) near submenu\n");
				error = true;
				return false;
			}
		
		} else {
			printf("missing ( near submenu\n");
			error = true;
			return false;
		}
	}
	
	return false;
}

bool MenuLex::separator(BaseMenu *sub, bool validate)
{
	if(match("separator"))
	{
		if(! validate)
			sub->insert("separator", "", 2);
				
		return true;
	}
	
	return false;
}

bool MenuLex::exit(BaseMenu* sub, bool validate)
{
	if(match("exit"))
	{
		if(! validate)
			sub->insert("exit", "", 1);
		
		return true;
	}
	
	return false;
}

bool MenuLex::restart(BaseMenu* sub, bool validate)
{
	if(match("restart"))
	{
		if(! validate)
			sub->insert("restart", "", 4);
		
		return true;
	}
	
	return false;
}

bool MenuLex::reconfigure(BaseMenu* sub, bool validate)
{
	if(match("reconfigure"))
	{
		if(! validate)
			sub->insert("reconfigure", "", 5);
		
		return true;
	}
	
	return false;
}

bool MenuLex::exec(BaseMenu *sub, bool validate)
{
	if(match("exec"))
	{
		getNextToken();		
		
		if(match("("))
		{
			clearToken();
			getNextToken(true);
			strcpy(name, currentToken());
			//printf("name = %s\n", name);

			getNextToken();
			if(match(")"))
			{
				getNextToken();
				
				if(match("{"))
				{
					clearToken();
					getNextToken(true);
					
					strcpy(exe, currentToken());
					//printf("exec: %s\n", currentToken());
						
					if(! validate)
					{
						//printf("sub1's name = %s | inserting %s\n", sub1->getMenuTitle(), name);
						sub->insert(name, exe, 0);
					}
						
					getNextToken();
							
					if(match("}"))
						return true;
					
				} else {
					printf("missing { near exec\n");
					error = true;
					return false;
				}
			
			} else {
				printf("missing ) near exec\n");
				error = true;
				return false;
			}
		
		} else {
			printf("missing ( near exec\n");
			error = true;
			return false;
		}
	}
	
	return false;
}

bool MenuLex::theme(BaseMenu *sub, bool validate)
{
	if(match("theme"))
	{
		getNextToken();		
		
		if(match("("))
		{
			clearToken();
			getNextToken(true);
			strcpy(name, currentToken());
			//printf("theme name = %s\n", name);

			getNextToken();
			if(match(")"))
			{
				getNextToken();
				
				if(match("{"))
				{
					clearToken();
					getNextToken(true);
						
					
					strcpy(exe, currentToken());
					//printf("theme patch: %s\n", currentToken());
						
					if(! validate)
					{
						//printf("sub1's name = %s | inserting %s\n", sub1->getMenuTitle(), name);
						sub->insert(name, exe, 6);
					}
						
					getNextToken();
							
					if(match("}"))
						return true;
					
				} else {
					printf("missing { near theme\n");
					error = true;
					return false;
				}
			
			} else {
				printf("missing ) near theme\n");
				error = true;
				return false;
			}
		
		} else {
			printf("missing ( near theme\n");
			error = true;
			return false;
		}
	}
	
	return false;
}

bool MenuLex::statement(BaseMenu *sub, bool validate)
{
	// This dispatches each statement function.
	
	if(submenu(sub, validate)) 	return true; 
	if(exec(sub, validate)) 	return true;
	if(theme(sub, validate)) 	return true;
	if(separator(sub, validate)) 	return true;
	if(exit(sub, validate)) 	return true;
	if(restart(sub, validate)) 	return true;
	if(reconfigure(sub, validate)) 	return true;
	
	return false;
}
