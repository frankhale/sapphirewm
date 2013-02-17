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

void signalhandler(int i) {
  fprintf(stderr, "%s: Caught a signal, exiting...", windowmanager_name);
  wm->quit_nicely();
}

// This is a utility function which just prints some extra information
// when an error has occurred.
void err(char *fmt, ...)
{
	va_list argp;

	fprintf(stderr, "%s: ", windowmanager_name);
	
	va_start(argp, fmt);
	vfprintf(stderr, fmt, argp);
	va_end(argp);
	fprintf(stderr, "\n");
}

int handle_xerror(Display *dpy, XErrorEvent *e)
{
	if (e->error_code == BadAccess && e->resourceid == wm->getRootWindow()) {
		err("root window unavailible (maybe another wm is running?)");
		exit(1);
	}
	#ifdef DEBUG
	else {
		char msg[80];
		XGetErrorText(dpy, e->error_code, msg, sizeof(msg));
		err("X error (%#x): %s", e->resourceid, msg);
	}
	#endif
	
	return 0;
}

