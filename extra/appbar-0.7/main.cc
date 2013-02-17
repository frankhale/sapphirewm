/*
 * main.cc - starts up the appbar program.
 * Copyright (C) 2001 Frank Hale
 * frankhale@yahoo.com
 * http://sapphire.sourceforge.net/
 *
 * Updated: 19 July 2001
 *
 * NOTE:
 *
 * 	This code borrows a little from fspanel.c and desklaunch.c so
 * credits go to the authors who wrote those two programs. 
 * 
 * I borrowed the GC and grill drawing code from fspanel.
 * I borrowed GNOME and MWM hints stuff from desklaunch. 
 *
 * fspanel: http://www.chatjunkies.org/fspanel/
 * desklaunch : http://www.kensden.pwp.blueyonder.co.uk/Oroborus/
 *
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

#include "appbar.hh"

int main()
{
	Appbar *a = new Appbar();
	delete a;

	return 0;
}
