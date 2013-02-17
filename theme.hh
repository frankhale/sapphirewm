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


#ifndef _THEME_HH_
#define _THEME_HH_

#define TOOLBAR_TOP			(1<<1)
#define TOOLBAR_VISIBLE			(1<<2)

#define ROOT_ON				(1<<1)
#define ROOT_OFF			(1<<2)

class Theme
{
private:
	XrmDatabase resource_db;
	
	char* currentTheme;
	
	char* font;
	char* menufont;

	XFontStruct *xfont;
	
	// Each array has 2 elements 
	// 
	// 	0 = color_from used for gradients
	//	1 = color_to used for gradients
	//
	// If the image is supposed to be solid then the first color_from is used.
	//
	BColor client[2];
	BColor buttons[2];
	BColor clock[2];
	BColor menu[2];
	BColor menu_title[2];
	BColor menu_select[2];
	BColor toolbar[2];
	BColor root[2];
	BColor client_focus[2];
	BColor resize[2];
	BColor right_sidebar[2];
	BColor font_color[2];
	BColor menufont_color[2];
	
	// Bit masks for the different styles for window decorations
	unsigned long titlebar_style;
	unsigned long button_style;
	unsigned long toolbar_style;
	unsigned long toolbar_extra_style;
	unsigned long clock_style;
	unsigned long menu_style;
	unsigned long menu_title_style;
	unsigned long menu_select_style;
	unsigned long resize_style;
	unsigned long right_sidebar_style;
	unsigned long root_style;
	unsigned long root_extra_style;
	unsigned long font_style;

public:
	Theme(); 	// This creates the default theme
	~Theme();	// This has no purpose since we don't dynamically allocate any memory

	char* getFont() const { return font; }
	char* getMenuFont() const { return menufont; }

	void parseTheme(char *themeFile);
	char* getCurrentTheme() const { return currentTheme; }
	unsigned long readDatabaseTexture(char *rname, char *rclass);

	void setFontStyle(unsigned long style) 	{ font_style=style; }
	unsigned long getFontStyle() const 	{ return font_style; }

	void setRootExtraStyle(unsigned long style) 	{ root_extra_style=style; }
	unsigned long getRootExtraStyle() const 	{ return root_extra_style; }

	void setRootStyle(unsigned long style) 		{ root_style=style; }
	unsigned long getRootStyle() const 		{ return root_style; }

	void setRightSidebarStyle(unsigned long style) 	{ right_sidebar_style=style; }
	unsigned long getRightSidebarStyle() const 		{ return right_sidebar_style; }

	void setResizeStyle(unsigned long style) 	{ resize_style=style; }
	unsigned long getResizeStyle() const 		{ return resize_style; }

	void setTitlebarStyle(unsigned long style) 	{ titlebar_style=style; }
	unsigned long getTitlebarStyle() const 		{ return titlebar_style; }

	void setButtonStyle(unsigned long style) 	{ button_style=style; }
	unsigned long getButtonStyle() const 		{ return button_style; }

	void setToolbarStyle(unsigned long style) 	{ toolbar_style=style; }
	unsigned long getToolbarStyle() const 		{ return toolbar_style; }

	void setToolbarExtraStyle(unsigned long style) 	{ toolbar_extra_style=style; }
	unsigned long getToolbarExtraStyle() const 	{ return toolbar_extra_style; }
	void setToolbarVisible() 			{ toolbar_extra_style|=TOOLBAR_VISIBLE; }
	void setToolbarTop() 				{ toolbar_extra_style|=TOOLBAR_TOP; }

	void setClockStyle(unsigned long style) 	{ clock_style=style; }
	unsigned long getClockStyle() const 		{ return clock_style; }

	void setMenuStyle(unsigned long style) 		{ menu_style=style; }
	unsigned long getMenuStyle() const 		{ return menu_style; }

	void setMenuTitleStyle(unsigned long style) 	{ menu_title_style=style; }
	unsigned long getMenuTitleStyle() const 	{ return menu_title_style; }

	void setMenuSelectStyle(unsigned long style) 	{ menu_select_style=style; }
	unsigned long getMenuSelectStyle() const 	{ return menu_select_style; }

	void setMenuFontColor(BColor c) 		{ menufont_color[0]=c; 		}
	void setMenuFontColorFrom(BColor c) 		{ menufont_color[0]=c; 		}
	void setMenuFontColorTo(BColor c)		{ menufont_color[1]=c; 		}
	void setFontColor(BColor c) 			{ font_color[0]=c; 		}
	void setFontColorFrom(BColor c) 		{ font_color[0]=c; 		}
	void setFontColorTo(BColor c)			{ font_color[1]=c; 		}
	void setClockColor(BColor c) 			{ clock[0]=c; 		}
	void setClockColorFrom(BColor c) 		{ clock[0]=c; 		}
	void setClockColorTo(BColor c)			{ clock[1]=c; 		}
	void setRightSidebarColor(BColor c) 		{ right_sidebar[0]=c; 	}
	void setRightSidebarColorFrom(BColor c) 	{ right_sidebar[0]=c; 	}
	void setRightSidebarColorTo(BColor c)		{ right_sidebar[1]=c; 	}
	void setResizeColor(BColor c) 			{ resize[0]=c; 		}
	void setResizeColorFrom(BColor c) 		{ resize[0]=c; 		}
	void setResizeColorTo(BColor c)			{ resize[1]=c; 		}
	void setClientColor(BColor c) 			{ client[0]=c; 		}
	void setClientColorFrom(BColor c) 		{ client[0]=c; 		}
	void setClientColorTo(BColor c)			{ client[1]=c; 		}
	void setClientFocusColor(BColor c)		{ client_focus[0]=c; 	}
	void setClientFocusColorFrom(BColor c) 		{ client_focus[0]=c; 	}
	void setClientFocusColorTo(BColor c) 		{ client_focus[1]=c; 	}
	void setButtonColor(BColor c) 			{ buttons[0] = c; 	}
	void setButtonColorFrom(BColor c) 		{ buttons[0] = c; 	}
	void setButtonColorTo(BColor c) 		{ buttons[1] = c; 	}
	void setMenuTitleColor(BColor c) 		{ menu_title[0]=c; 	}
	void setMenuTitleColorFrom(BColor c) 		{ menu_title[0]=c; 	}
	void setMenuTitleColorTo(BColor c) 		{ menu_title[1]=c; 	}
	void setMenuColor(BColor c) 			{ menu[0]=c; 		}
	void setMenuColorFrom(BColor c) 		{ menu[0]=c; 		}
	void setMenuColorTo(BColor c) 			{ menu[1]=c; 		}
	void setMenuSelectColor(BColor c) 		{ menu_select[0]=c; 	}
	void setMenuSelectColorFrom(BColor c) 		{ menu_select[0]=c; 	}
	void setMenuSelectColorTo(BColor c) 		{ menu_select[1]=c; 	}
	void setToolbarColor(BColor c) 			{ toolbar[0]=c; 	}
	void setToolbarColorFrom(BColor c) 		{ toolbar[0]=c; 	}
	void setToolbarColorTo(BColor c) 		{ toolbar[1]=c; 	}
	void setRootColor(BColor c) 			{ root[0] = c; 		}		
	void setRootColorFrom(BColor c) 		{ root[0] = c; 		}	
	void setRootColorTo(BColor c) 			{ root[1] = c; 		}	

	BColor getMenuFontColor() 		const { return menufont_color[0];}
	BColor getMenuFontColorFrom() 		const { return menufont_color[0];}
	BColor getMenuFontColorTo() 		const { return menufont_color[1];}
	BColor getFontColor() 			const { return font_color[0]; 	 }
	BColor getFontColorFrom() 		const { return font_color[0]; 	 }
	BColor getFontColorTo() 		const { return font_color[1]; 	 }
	BColor getClockColor() 			const { return clock[0]; 	 }
	BColor getClockColorFrom() 		const { return clock[0]; 	 }
	BColor getClockColorTo() 		const { return clock[1]; 	 }
	BColor getRightSidebarColor() 		const { return right_sidebar[0]; }
	BColor getRightSidebarColorFrom() 	const { return right_sidebar[0]; }
	BColor getRightSidebarColorTo() 	const { return right_sidebar[1]; }
	BColor getResizeColor() 		const { return resize[0];	 }
	BColor getResizeColorFrom() 		const { return resize[0]; 	 }
	BColor getResizeColorTo() 		const { return resize[1]; 	 }
	BColor getClientColorFrom() 		const { return client[0]; 	 }
	BColor getClientColorTo() 		const { return client[1]; 	 }
	BColor getClientColor() 		const { return client[0]; 	 }
	BColor getClientFocusColorFrom()	const { return client_focus[0];  }
	BColor getClientFocusColorTo() 		const { return client_focus[1];  }	
	BColor getClientFocusColor() 		const { return client_focus[0];  }
	BColor getButtonColorFrom() 		const { return buttons[0]; 	 }
	BColor getButtonColorTo() 		const { return buttons[1]; 	 }
	BColor getButtonColor() 		const { return buttons[0]; 	 }
	BColor getMenuColorFrom() 		const { return menu[0]; 	 } 
	BColor getMenuColorTo() 		const { return menu[1]; 	 }
	BColor getMenuColor() 			const { return menu[0]; 	 } 
	BColor getMenuTitleColorFrom() 		const { return menu_title[0]; 	 } 
	BColor getMenuTitleColorTo() 		const { return menu_title[1]; 	 }
	BColor getMenuTitleColor() 		const { return menu_title[0]; 	 } 
	BColor getMenuSelectColorFrom() 	const { return menu_select[0]; 	 }
	BColor getMenuSelectColorTo() 		const { return menu_select[1]; 	 }
	BColor getMenuSelectColor() 		const { return menu_select[0]; 	 }
	BColor getToolbarColorFrom() 		const { return toolbar[0]; 	 }
	BColor getToolbarColorTo() 		const { return toolbar[1]; 	 }
	BColor getToolbarColor() 		const { return toolbar[0]; 	 }
	BColor getRootColorFrom() 		const { return root[0]; 	 }
	BColor getRootColorTo() 		const { return root[1]; 	 }
	BColor getRootColor() 			const { return root[0]; 	 }
};

#endif
