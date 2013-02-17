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

Theme::Theme()
{			
}

Theme::~Theme()
{
	delete [] currentTheme;
	delete [] font;
	delete [] menufont;
}

void Theme::parseTheme(char *themeFile)
{
	XrmInitialize();
	XrmValue value;
	char *value_type;

	currentTheme = new char[strlen(themeFile)+1];
	strcpy(currentTheme, themeFile);

	resource_db = XrmGetFileDatabase(themeFile);
	if (resource_db==NULL) return;
	
	if (! (titlebar_style=readDatabaseTexture("titlebar.style", "Titlebar.Style")))
	{
	 	titlebar_style=BImage_Gradient|BImage_Horizontal|BImage_Raised|BImage_Bevel2;
	}

	if (! (button_style=readDatabaseTexture("button.style", "Button.Style")))
	{
	 	button_style=BImage_Gradient|BImage_Diagonal|BImage_Raised|BImage_Bevel2;
	}

	if (! (toolbar_style=readDatabaseTexture("toolbar.style", "Toolbar.Style")))
	{
	 	toolbar_style=BImage_Gradient|BImage_Vertical|BImage_Raised|BImage_Bevel2;
	}

	if (! (clock_style=readDatabaseTexture("clock.style", "clock.Style")))
	{
	 	clock_style=BImage_Gradient|BImage_Vertical|BImage_Raised|BImage_Bevel2;
	}

	if (! (menu_style=readDatabaseTexture("menu.style", "Menu.Style")))
	{
	 	menu_style=BImage_Gradient|BImage_Diagonal|BImage_Raised|BImage_Bevel2;
	}

	if (! (menu_title_style=readDatabaseTexture("menutitle.style", "MenuTitle.Style")))
	{
	 	menu_title_style=BImage_Gradient|BImage_Horizontal|BImage_Raised|BImage_Bevel2;
	}

	if (! (menu_select_style=readDatabaseTexture("menuselect.style", "MenuSelect.Style")))
	{
	 	menu_select_style=BImage_Gradient|BImage_Vertical|BImage_Raised|BImage_Bevel2;
	}

	if (! (resize_style=readDatabaseTexture("resize.style", "Resize.Style")))
	{
	 	resize_style=BImage_Gradient|BImage_Horizontal|BImage_Raised|BImage_Bevel2;
	}

	if (! (right_sidebar_style=readDatabaseTexture("handle.style", "Handle.Style")))
	{
	 	right_sidebar_style=BImage_Gradient|BImage_Horizontal|BImage_Raised|BImage_Bevel2;
	}

	if (XrmGetResource(resource_db, "root.background",
		     "Root.Background", &value_type, &value))
		{
			if(strcmp(value.addr, "true")==0) 
			{
				root_extra_style=ROOT_ON;
			} else {
				root_extra_style=ROOT_OFF;
			}
		}

	if (! (root_style=readDatabaseTexture("root.style", "Root.Style")))
	{
		root_extra_style=ROOT_OFF;
	 	root_style=BImage_Solid;
	}

	if (! (font_style=readDatabaseTexture("font.style", "Font.Style")))
	{
	 	font_style=BImage_Solid;
	}

	// Client color from
	if (XrmGetResource(resource_db, "titlebar.color.from.red",
		     "Titlebar.Color.From.Red", &value_type, &value))
	{
		sscanf(value.addr, "%d", &client[0].red); 	

	} else {
			client[0].red=89;
	}

	if (XrmGetResource(resource_db, "titlebar.color.from.green",
		     "Titlebar.Color.From.Green", &value_type, &value))
	{
		sscanf(value.addr, "%d", &client[0].green); 	
	
	} else {
			client[0].green=107;
	}

	if (XrmGetResource(resource_db, "titlebar.color.from.blue",
		     "Titlebar.Color.From.Blue", &value_type, &value))
	{
		sscanf(value.addr, "%d", &client[0].blue); 	

	} else {
			client[0].blue=137;
	}

	// Client color to
	if (XrmGetResource(resource_db, "titlebar.color.to.red",
		     "Titlebar.Color.To.Red", &value_type, &value))
	{
		sscanf(value.addr, "%d", &client[1].red); 	

	} else {
			client[1].red=200;
	}

	if (XrmGetResource(resource_db, "titlebar.color.to.green",
		     "Titlebar.Color.To.Green", &value_type, &value))
	{
		sscanf(value.addr, "%d", &client[1].green); 	
	
	} else {
			client[1].green=200;
	}

	if (XrmGetResource(resource_db, "titlebar.color.to.blue",
		     "Titlebar.Color.To.Blue", &value_type, &value))
	{
		sscanf(value.addr, "%d", &client[1].blue); 	

	} else {
			client[1].blue=200;
	}

	//  Client Focus from
	if (XrmGetResource(resource_db, "titlebarFocus.color.from.red",
		     "TitlebarFocus.Color.From.Red", &value_type, &value))
	{
		sscanf(value.addr, "%d", &client_focus[0].red); 	

	} else {
			client_focus[0].red=100;
	}

	if (XrmGetResource(resource_db, "titlebarFocus.color.from.green",
		     "TitlebarFocus.Color.From.Green", &value_type, &value))
	{
		sscanf(value.addr, "%d", &client_focus[0].green); 	
	
	} else {
			client_focus[0].green=100;
	}

	if (XrmGetResource(resource_db, "titlebarFocus.color.from.blue",
		     "TitlebarFocus.Color.From.Blue", &value_type, &value))
	{
		sscanf(value.addr, "%d", &client_focus[0].blue); 	

	} else {
			client_focus[0].blue=100;
	}

	// Client Focus color to
	if (XrmGetResource(resource_db, "titlebarFocus.color.to.red",
		     "TitlebarFocus.Color.To.Red", &value_type, &value))
	{
		sscanf(value.addr, "%d", &client_focus[1].red); 	

	} else {
			client_focus[1].red=255;
	}

	if (XrmGetResource(resource_db, "titlebarFocus.color.to.green",
		     "TitlebarFocus.Color.To.Green", &value_type, &value))
	{
		sscanf(value.addr, "%d", &client_focus[1].green); 	
	
	} else {
			client_focus[1].green=255;
	}

	if (XrmGetResource(resource_db, "titlebarFocus.color.to.blue",
		     "TitlebarFocus.Color.To.Blue", &value_type, &value))
	{
		sscanf(value.addr, "%d", &client_focus[1].blue); 	

	} else {
			client_focus[1].blue=255;
	}	

	// Button color from
	if (XrmGetResource(resource_db, "button.color.from.red",
		     "Button.Color.From.Red", &value_type, &value))
	{
		sscanf(value.addr, "%d", &buttons[0].red); 	

	} else {
			buttons[0].red=181;
	}

	if (XrmGetResource(resource_db, "button.color.from.green",
		     "Button.Color.From.Green", &value_type, &value))
	{
		sscanf(value.addr, "%d", &buttons[0].green); 	
	
	} else {
			buttons[0].green=181;
	}

	if (XrmGetResource(resource_db, "button.color.from.blue",
		     "Button.Color.From.Blue", &value_type, &value))
	{
		sscanf(value.addr, "%d", &buttons[0].blue); 	

	} else {
			buttons[0].blue=181;
	}

	// Button color to
	if (XrmGetResource(resource_db, "button.color.to.red",
		     "Button.Color.To.Red", &value_type, &value))
	{
		sscanf(value.addr, "%d", &buttons[1].red); 	

	} else {
			buttons[1].red=225;
	}

	if (XrmGetResource(resource_db, "button.color.to.green",
		     "Button.Color.To.Green", &value_type, &value))
	{
		sscanf(value.addr, "%d", &buttons[1].green); 	
	
	} else {
			buttons[1].green=225;
	}

	if (XrmGetResource(resource_db, "button.color.to.blue",
		     "Button.Color.To.Blue", &value_type, &value))
	{
		sscanf(value.addr, "%d", &buttons[1].blue); 	

	} else {
			buttons[1].blue=225;
	}

	// Menu color from

	if (XrmGetResource(resource_db, "menu.color.from.red",
		     "Menu.Color.From.Red", &value_type, &value))
	{
		sscanf(value.addr, "%d", &menu[0].red); 	

	} else {
			menu[0].red=100;
	}

	if (XrmGetResource(resource_db, "menu.color.from.green",
		     "Menu.Color.From.Green", &value_type, &value))
	{
		sscanf(value.addr, "%d", &menu[0].green); 	
	
	} else {
			menu[0].green=100;
	}

	if (XrmGetResource(resource_db, "menu.color.from.blue",
		     "Menu.Color.From.Blue", &value_type, &value))
	{
		sscanf(value.addr, "%d", &menu[0].blue); 	

	} else {
			menu[0].blue=100;
	}

	// Menu color to

	if (XrmGetResource(resource_db, "menu.color.to.red",
		     "Menu.Color.To.Red", &value_type, &value))
	{
		sscanf(value.addr, "%d", &menu[1].red); 	

	} else {
			menu[1].red=255;
	}

	if (XrmGetResource(resource_db, "menu.color.to.green",
		     "Menu.Color.To.Green", &value_type, &value))
	{
		sscanf(value.addr, "%d", &menu[1].green); 	
	
	} else {
			menu[1].green=255;
	}

	if (XrmGetResource(resource_db, "menu.color.to.blue",
		     "Menu.Color.To.Blue", &value_type, &value))
	{
		sscanf(value.addr, "%d", &menu[1].blue); 	

	} else {
			menu[1].blue=255;
	}

	// Menu Title from

	if (XrmGetResource(resource_db, "menuTitle.color.from.red",
		     "MenuTitle.Color.From.Red", &value_type, &value))
	{
		sscanf(value.addr, "%d", &menu_title[0].red); 	

	} else {
			menu_title[0].red=89;
	}

	if (XrmGetResource(resource_db, "menuTitle.color.from.green",
		     "MenuTitle.Color.From.Green", &value_type, &value))
	{
		sscanf(value.addr, "%d", &menu_title[0].green); 	
	
	} else {
			menu_title[0].green=107;
	}

	if (XrmGetResource(resource_db, "menuTitle.color.from.blue",
		     "MenuTitle.Color.From.Blue", &value_type, &value))
	{
		sscanf(value.addr, "%d", &menu_title[0].blue); 	

	} else {
			menu_title[0].blue=137;
	}

	// Menu Title color to

	if (XrmGetResource(resource_db, "menuTitle.color.to.red",
		     "MenuTitle.Color.To.Red", &value_type, &value))
	{
		sscanf(value.addr, "%d", &menu_title[1].red); 	

	} else {
			menu_title[1].red=200;
	}

	if (XrmGetResource(resource_db, "menuTitle.color.to.green",
		     "MenuTitle.Color.To.Green", &value_type, &value))
	{
		sscanf(value.addr, "%d", &menu_title[1].green); 	
	
	} else {
			menu_title[1].green=200;
	}

	if (XrmGetResource(resource_db, "menuTitle.color.to.blue",
		     "MenuTitle.Color.To.Blue", &value_type, &value))
	{
		sscanf(value.addr, "%d", &menu_title[1].blue); 	

	} else {
			menu_title[1].blue=200;
	}

	// Menu Select from

	if (XrmGetResource(resource_db, "menuSelect.color.from.red",
		     "MenuSelect.Color.From.Red", &value_type, &value))
	{
		sscanf(value.addr, "%d", &menu_select[0].red); 	

	} else {
			menu_select[0].red=200;
	}

	if (XrmGetResource(resource_db, "menuSelect.color.from.green",
		     "MenuSelect.Color.From.Green", &value_type, &value))
	{
		sscanf(value.addr, "%d", &menu_select[0].green); 	
	
	} else {
			menu_select[0].green=200;
	}

	if (XrmGetResource(resource_db, "menuSelect.color.from.blue",
		     "MenuSelect.Color.From.Blue", &value_type, &value))
	{
		sscanf(value.addr, "%d", &menu_select[0].blue); 	

	} else {
			menu_select[0].blue=200;
	}

	// Menu Select color to

	if (XrmGetResource(resource_db, "menuSelect.color.to.red",
		     "MenuSelect.Color.To.Red", &value_type, &value))
	{
		sscanf(value.addr, "%d", &menu_select[1].red); 	

	} else {
			menu_select[1].red=255;
	}

	if (XrmGetResource(resource_db, "menuSelect.color.to.green",
		     "MenuSelect.Color.To.Green", &value_type, &value))
	{
		sscanf(value.addr, "%d", &menu_select[1].green); 	
	
	} else {
			menu_select[1].green=255;
	}

	if (XrmGetResource(resource_db, "menuSelect.color.to.blue",
		     "MenuSelect.Color.To.Blue", &value_type, &value))
	{
		sscanf(value.addr, "%d", &menu_select[1].blue); 	

	} else {
			menu_select[1].blue=255;
	}


	// Toolbar from

	if (XrmGetResource(resource_db, "toolbar.color.from.red",
		     "Toolbar.Color.From.Red", &value_type, &value))
	{
		sscanf(value.addr, "%d", &toolbar[0].red); 	

	} else {
			toolbar[0].red=117;
	}

	if (XrmGetResource(resource_db, "toolbar.color.from.green",
		     "Toolbar.Color.From.Green", &value_type, &value))
	{
		sscanf(value.addr, "%d", &toolbar[0].green); 	
	
	} else {
			toolbar[0].green=142;
	}

	if (XrmGetResource(resource_db, "toolbar.color.from.blue",
		     "Toolbar.Color.From.Blue", &value_type, &value))
	{
		sscanf(value.addr, "%d", &toolbar[0].blue); 	

	} else {
			toolbar[0].blue=183;
	}

	// Toolbar color to

	if (XrmGetResource(resource_db, "toolbar.color.to.red",
		     "Toolbar.Color.To.Red", &value_type, &value))
	{
		sscanf(value.addr, "%d", &toolbar[1].red); 	

	} else {
			toolbar[1].red=70;
	}

	if (XrmGetResource(resource_db, "toolbar.color.to.green",
		     "Toolbar.Color.To.Green", &value_type, &value))
	{
		sscanf(value.addr, "%d", &toolbar[1].green); 	
	
	} else {
			toolbar[1].green=70;
	}

	if (XrmGetResource(resource_db, "toolbar.color.to.blue",
		     "Toolbar.Color.To.Blue", &value_type, &value))
	{
		sscanf(value.addr, "%d", &toolbar[1].blue); 	

	} else {
			toolbar[1].blue=70;
	}

	//  Resize from

	if (XrmGetResource(resource_db, "resize.color.from.red",
		     "Resize.Color.From.Red", &value_type, &value))
	{
		sscanf(value.addr, "%d", &resize[0].red); 	

	} else {
			resize[0].red=100;
	}

	if (XrmGetResource(resource_db, "resize.color.from.green",
		     "Resize.Color.From.Green", &value_type, &value))
	{
		sscanf(value.addr, "%d", &resize[0].green); 	
	
	} else {
			resize[0].green=100;
	}

	if (XrmGetResource(resource_db, "resize.color.from.blue",
		     "Resize.Color.From.Blue", &value_type, &value))
	{
		sscanf(value.addr, "%d", &resize[0].blue); 	

	} else {
			resize[0].blue=100;
	}

	// Resize color to

	if (XrmGetResource(resource_db, "resize.color.to.red",
		     "Resize.Color.To.Red", &value_type, &value))
	{
		sscanf(value.addr, "%d", &resize[1].red); 	

	} else {
			resize[1].red=255;
	}

	if (XrmGetResource(resource_db, "resize.color.to.green",
		     "Resize.Color.To.Green", &value_type, &value))
	{
		sscanf(value.addr, "%d", &resize[1].green); 	
	
	} else {
			resize[1].green=255;
	}

	if (XrmGetResource(resource_db, "resize.color.to.blue",
		     "Resize.Color.To.Blue", &value_type, &value))
	{
		sscanf(value.addr, "%d", &resize[1].blue); 	

	} else {
			resize[1].blue=255;
	}	

	//  Clock color from
	if (XrmGetResource(resource_db, "clock.color.from.red",
		     "Clock.Color.From.Red", &value_type, &value))
	{
		sscanf(value.addr, "%d", &clock[0].red); 	

	} else {
			clock[0].red=100;
	}

	if (XrmGetResource(resource_db, "clock.color.from.green",
		     "Clock.Color.From.Green", &value_type, &value))
	{
		sscanf(value.addr, "%d", &clock[0].green); 	
	
	} else {
			clock[0].green=100;
	}

	if (XrmGetResource(resource_db, "clock.color.from.blue",
		     "Clock.Color.From.Blue", &value_type, &value))
	{
		sscanf(value.addr, "%d", &clock[0].blue); 	

	} else {
			clock[0].blue=100;
	}

	// Clock color to
	if (XrmGetResource(resource_db, "clock.color.to.red",
		     "Clock.Color.To.Red", &value_type, &value))
	{
		sscanf(value.addr, "%d", &clock[1].red); 	

	} else {
			clock[1].red=255;
	}

	if (XrmGetResource(resource_db, "clock.color.to.green",
		     "Clock.Color.To.Green", &value_type, &value))
	{
		sscanf(value.addr, "%d", &clock[1].green); 	
	
	} else {
			clock[1].green=255;
	}

	if (XrmGetResource(resource_db, "clock.color.to.blue",
		     "Clock.Color.To.Blue", &value_type, &value))
	{
		sscanf(value.addr, "%d", &clock[1].blue); 	

	} else {
			clock[1].blue=255;
	}

	//  Handle from
	if (XrmGetResource(resource_db, "handle.color.from.red",
		     "Handle.Color.From.Red", &value_type, &value))
	{
		sscanf(value.addr, "%d", &right_sidebar[0].red); 	

	} else {
			right_sidebar[0].red=100;
	}

	if (XrmGetResource(resource_db, "handle.color.from.green",
		     "Handle.Color.From.Green", &value_type, &value))
	{
		sscanf(value.addr, "%d", &right_sidebar[0].green); 	
	
	} else {
			right_sidebar[0].green=100;
	}

	if (XrmGetResource(resource_db, "handle.color.from.blue",
		     "Handle.Color.From.Blue", &value_type, &value))
	{
		sscanf(value.addr, "%d", &right_sidebar[0].blue); 	

	} else {
			right_sidebar[0].blue=100;
	}

	// Handle color to

	if (XrmGetResource(resource_db, "handle.color.to.red",
		     "Handle.Color.To.Red", &value_type, &value))
	{
		sscanf(value.addr, "%d", &right_sidebar[1].red); 	

	} else {
			right_sidebar[1].red=255;
	}

	if (XrmGetResource(resource_db, "handle.color.to.green",
		     "Handle.Color.To.Green", &value_type, &value))
	{
		sscanf(value.addr, "%d", &right_sidebar[1].green); 	
	
	} else {
			right_sidebar[1].green=255;
	}

	if (XrmGetResource(resource_db, "handle.color.to.blue",
		     "Handle.Color.To.Blue", &value_type, &value))
	{
		sscanf(value.addr, "%d", &right_sidebar[1].blue); 	

	} else {
			right_sidebar[1].blue=255;
	}
	
	if (XrmGetResource(resource_db, "command",
		     "Command", &value_type, &value))
	{
		char command[value.size+8];
		sprintf(command, "exec %s &", value.addr);
		system(command);
	}

	if (XrmGetResource(resource_db, "font",
		     "Font", &value_type, &value))
	{
		font = new char[value.size];
		strcpy(font, value.addr);
		
		xfont = XLoadQueryFont(wm->getDisplay(), font);
		
		if(! xfont)
		{
			char f[43];
			sprintf(f, "-*-lucida-medium-r-normal-*-12-*-*-*-*-*-*-*");
			font = new char[43];
		
			strcpy(font, f);
		}
		
	} else {
	
		char f[43];
		sprintf(f, "-*-lucida-medium-r-normal-*-12-*-*-*-*-*-*-*");
		font = new char[43];
		
		strcpy(font, f);
	}

	if (XrmGetResource(resource_db, "menufont",
		     "MenuFont", &value_type, &value))
	{
		menufont = new char[value.size];
		strcpy(menufont, value.addr);
		
		xfont = XLoadQueryFont(wm->getDisplay(), menufont);
		
		if(! xfont)
		{
			char f[43];
			sprintf(f, "-*-lucida-bold-r-normal-*-15-*-*-*-*-*-*-*");
			menufont = new char[43];
		
			strcpy(menufont, f);	
		}
		
	} else {

		char f[43];
		sprintf(f, "-*-lucida-bold-r-normal-*-15-*-*-*-*-*-*-*");
		menufont = new char[43];
		
		strcpy(menufont, f);		
	}

	//  Font color from
	if (XrmGetResource(resource_db, "font.color.from.red",
		     "Font.Color.From.Red", &value_type, &value))
	{
		sscanf(value.addr, "%d", &font_color[0].red); 	

	} else {
			font_color[0].red=0;
	}

	if (XrmGetResource(resource_db, "font.color.from.green",
		     "Font.Color.From.Green", &value_type, &value))
	{
		sscanf(value.addr, "%d", &font_color[0].green); 	
	
	} else {
			font_color[0].green=0;
	}

	if (XrmGetResource(resource_db, "font.color.from.blue",
		     "Font.Color.From.Blue", &value_type, &value))
	{
		sscanf(value.addr, "%d", &font_color[0].blue); 	

	} else {
			font_color[0].blue=0;
	}

	// Font color to

	if (XrmGetResource(resource_db, "font.color.to.red",
		     "Font.Color.To.Red", &value_type, &value))
	{
		sscanf(value.addr, "%d", &font_color[1].red); 	

	} else {
			font_color[1].red=255;
	}

	if (XrmGetResource(resource_db, "font.color.to.green",
		     "Font.Color.To.Green", &value_type, &value))
	{
		sscanf(value.addr, "%d", &font_color[1].green); 	
	
	} else {
			font_color[1].green=255;
	}

	if (XrmGetResource(resource_db, "font.color.to.blue",
		     "Font.Color.To.Blue", &value_type, &value))
	{
		sscanf(value.addr, "%d", &font_color[1].blue); 	

	} else {
			font_color[1].blue=255;
	}
		
	//  MenuFont color from
	if (XrmGetResource(resource_db, "menufont.color.from.red",
		     "MenuFont.Color.From.Red", &value_type, &value))
	{
		sscanf(value.addr, "%d", &menufont_color[0].red); 	

	} else {
			menufont_color[0].red=0;
	}

	if (XrmGetResource(resource_db, "menufont.color.from.green",
		     "MenuFont.Color.From.Green", &value_type, &value))
	{
		sscanf(value.addr, "%d", &menufont_color[0].green); 	
	
	} else {
			menufont_color[0].green=0;
	}

	if (XrmGetResource(resource_db, "menufont.color.from.blue",
		     "MenuFont.Color.From.Blue", &value_type, &value))
	{
		sscanf(value.addr, "%d", &menufont_color[0].blue); 	

	} else {
			menufont_color[0].blue=0;
	}

	// MenuFont color to
	if (XrmGetResource(resource_db, "menufont.color.to.red",
		     "MenuFont.Color.To.Red", &value_type, &value))
	{
		sscanf(value.addr, "%d", &menufont_color[1].red); 	

	} else {
			menufont_color[1].red=255;
	}

	if (XrmGetResource(resource_db, "menufont.color.to.green",
		     "MenuFont.Color.To.Green", &value_type, &value))
	{
		sscanf(value.addr, "%d", &menufont_color[1].green); 	
	
	} else {
			menufont_color[1].green=255;
	}

	if (XrmGetResource(resource_db, "menufont.color.to.blue",
		     "MenuFont.Color.To.Blue", &value_type, &value))
	{
		sscanf(value.addr, "%d", &menufont_color[1].blue); 	

	} else {
			menufont_color[1].blue=255;
	}

	//  Root color from
	if (XrmGetResource(resource_db, "root.color.from.red",
		     "Root.Color.From.Red", &value_type, &value))
	{
		sscanf(value.addr, "%d", &root[0].red); 	

	} else {
			root[0].red=0;
	}

	if (XrmGetResource(resource_db, "root.color.from.green",
		     "Root.Color.From.Green", &value_type, &value))
	{
		sscanf(value.addr, "%d", &root[0].green); 	
	
	} else {
			root[0].green=0;
	}

	if (XrmGetResource(resource_db, "root.color.from.blue",
		     "Root.Color.From.Blue", &value_type, &value))
	{
		sscanf(value.addr, "%d", &root[0].blue); 	

	} else {
			root[0].blue=0;
	}

	// Root color to

	if (XrmGetResource(resource_db, "root.color.to.red",
		     "Root.Color.To.Red", &value_type, &value))
	{
		sscanf(value.addr, "%d", &root[1].red); 	

	} else {
			root[1].red=255;
	}

	if (XrmGetResource(resource_db, "root.color.to.green",
		     "Root.Color.To.Green", &value_type, &value))
	{
		sscanf(value.addr, "%d", &root[1].green); 	
	
	} else {
			root[1].green=255;
	}

	if (XrmGetResource(resource_db, "root.color.to.blue",
		     "Root.Color.To.Blue", &value_type, &value))
	{
		sscanf(value.addr, "%d", &root[1].blue); 	

	} else {
			root[1].blue=255;
	}

	XrmDestroyDatabase(resource_db);
}

unsigned long Theme::readDatabaseTexture(char *rname, char *rclass) 
{
  XrmValue value;
  char *value_type;
  unsigned long texture = 0;

  if (XrmGetResource(resource_db, rname, rclass, &value_type,
		     &value)) {
    if (strstr(value.addr, "Inverted")) 
		{
      texture |= BImage_Invert;
    } 
		else
		{
      if (strstr(value.addr, "Solid")) 
			{
				texture |= BImage_Solid;
      } 
			else if (strstr(value.addr, "Gradient")) 
			{
				texture |= BImage_Gradient;
	
				if (strstr(value.addr, "Diagonal")) 
				{
			  	texture |= BImage_Diagonal;
				} 
				else if (strstr(value.addr, "Horizontal")) 
				{
				  texture |= BImage_Horizontal;
				} 
				else if (strstr(value.addr, "Vertical")) 
				{
				  texture |= BImage_Vertical;
				} 
				else  texture |= BImage_Diagonal;
      } 
			else texture |= BImage_Solid;
      
    	if (strstr(value.addr, "Raised"))
				texture |= BImage_Raised;
      else if (strstr(value.addr, "Sunken"))
				texture |= BImage_Sunken;
      else if (strstr(value.addr, "Flat"))
				texture |= BImage_Flat;
      else
				texture |= BImage_Raised;
      
      if (! (texture & BImage_Flat))
				if (strstr(value.addr, "Bevel"))
				  if (strstr(value.addr, "Bevel1"))
	    			texture |= BImage_Bevel1;
				  else if (strstr(value.addr, "Bevel2"))
	    			texture |= BImage_Bevel2;
				  else
				    texture |= BImage_Bevel1;
    }
  }
  
  return texture;
  
  
  return 0;
}


