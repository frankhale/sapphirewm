/* setrootimage.cc - sets the root image with a nice diagonal gradient.
 *
 * Author - Frank Hale
 * Email - frankhale@yahoo.com
 * Date - 9 October 2001
 *
 * Released under the GNU GPL license
 * www.gnu.org
 */
#include <X11/Xlib.h>
#include <X11/xpm.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>
#include "image.hh"

BColor parseCommandlineColor(char* the_color)
{
	BColor color;
	
	char temp_red[3] = "/0"; 
	char temp_green[3]= "/0"; 
	char temp_blue[3]= "/0"; 
	
	for (int i=0; i<6; i++)
	{
		temp_red[0] = the_color[i++];
		temp_red[1] = the_color[i++];
		temp_green[0] = the_color[i++];
		temp_green[1] = the_color[i++];
		temp_blue[0] = the_color[i++];
		temp_blue[1] = the_color[i];
	}

	color.red = strtol(temp_red,0,16);
	color.green = strtol(temp_green,0,16);
	color.blue = strtol(temp_blue,0,16);
	
	return color;
}

// The next to macros are from aewm
// Thanks to Decklin Foster for this code!!!
#define OPT_STR(name, variable)                                      \
    if (strcmp(argv[i], name) == 0 && i+1<argc) {                    \
        variable = argv[++i];                                        \
        continue;                                                    \
    }
#define OPT_INT(name, variable)                                      \
    if (strcmp(argv[i], name) == 0 && i+1<argc) {                    \
        variable = atoi(argv[++i]);                                  \
        continue;                                                    \
    }

int main (int argc, char * argv[])
{
	Display *dpy;
	Window root;
	int screen, depth;
	Visual *visual;
	BImageControl *image_control;
	Pixmap root_pix = None;
	BColor to, from;
	
	char* opt_from="999999";
	char* opt_to="cccccc"; 
	char* opt_display="";
	
   	int i;	
   	
	for (i = 1; i < argc; i++) {
        	OPT_STR("-from", opt_from)
		OPT_STR("-to", opt_to)
		OPT_STR("-display", opt_display)
        	
		if(strcmp(argv[i], "-help") == 0)
		{
	        	cerr << "usage: setrootimage [options]" << endl;
        		cerr << "   options are: -display <display>, -from <color>, -to <color>" << endl << endl;

			cerr << "example:" << endl;
			cerr << "   setrootimage -from 708090 -to cccccc" << endl;						
		}
        	exit(-1);
    	}

	if(opt_display)
		dpy = XOpenDisplay (opt_display);
	else
		dpy = XOpenDisplay (NULL);

	if(!dpy)
	{
		cerr << "couldn't open display" << endl;
		exit(-1);
	}
	
	screen = DefaultScreen(dpy);
	depth = DefaultDepth(dpy, screen);
	visual = DefaultVisual(dpy, screen);
	root = RootWindow(dpy, screen);

	// Nice shade of gray
	from.red=0x70;
	from.green=0x80;
	from.blue=0x90;

	// Another nice shade of gray
	to.red=0xff;
	to.green=0xff;
	to.blue=0xff;

	from = parseCommandlineColor(opt_from);
	to = parseCommandlineColor(opt_to);

	image_control = new BImageControl(dpy, depth, root, screen, visual); 

	// Default color theme style
	unsigned long style=0;
	style = BImage_Gradient | BImage_Diagonal;
	//style = BImage_Flat | BImage_Solid;

	root_pix = image_control->renderImage(
			WidthOfScreen(ScreenOfDisplay(dpy, screen)), 
			HeightOfScreen(ScreenOfDisplay(dpy, screen)), 
			style, 
		       	from, 
		      	to);

	XSetWindowBackgroundPixmap(dpy, root, root_pix);
	XClearWindow(dpy, root);
	
	image_control->removeImage( root_pix );
	delete image_control;
		
	XCloseDisplay(dpy);
	
	return 0;
}
