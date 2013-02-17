/*
 * image.hh - Header for image class in Sapphire.
 * Copyright (C) 1997-1999 by Brad Hughes, bhughes@tcac.net
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

#ifndef _IMAGE_HH_
#define _IMAGE_HH_

#include "linkedlist.hh"

#include <X11/Xlib.h>
#include <X11/xpm.h>
#include <cstdio>

class BImageControl;
class BImage;

typedef struct BColor {
  unsigned char red, green, blue;
  unsigned long pixel;
};

// bevel options
#define BImage_Flat		(1<<1)
#define BImage_Sunken		(1<<2)
#define BImage_Raised		(1<<3)

// textures
#define BImage_Solid		(1<<4)
#define BImage_Gradient		(1<<5)

// gradients
#define BImage_Horizontal	(1<<6)
#define BImage_Vertical		(1<<7)
#define BImage_Diagonal		(1<<8)

// bevel types
#define BImage_Bevel1		(1<<9)
#define BImage_Bevel2		(1<<10)

// inverted image
#define BImage_Invert		(1<<11)

class BImage {
private:
  BImageControl *control;

  XColor *colors;

  BColor bg, from, to;
  int roff, goff, boff, ncolors, cpc, cpccpc;
  unsigned char *red, *green, *blue;
  unsigned int width, height;
  unsigned short *tr, *tg, *tb;


protected:
  void invert(void);

  void bevel1(Bool = True, Bool = False);
  void bevel2(Bool = True, Bool = False);

  void dgradient(void);
  void hgradient(void);
  void vgradient(void);

  void background(const BColor &);

  XImage *renderXImage(void);
  Pixmap renderPixmap(void);

public:
  BImage(BImageControl *, unsigned int, unsigned int);
  ~BImage(void);

  Pixmap render(unsigned long, const BColor &, const BColor &);
  Pixmap render_solid(unsigned long, const BColor &);
  Pixmap render_gradient(unsigned long, const BColor &, const BColor &);
};

class BImageControl {
private:
  Colormap root_colormap;
  Display *display;
  Window window; // == blackbox->Root();
  XColor *colors;
  Visual *visual;
  bool image_dither;
  int colors_per_channel, ncolors, screen_number, screen_depth,
    bits_per_pixel, red_offset, green_offset, blue_offset;
  unsigned short rmask_table[256], gmask_table[256], bmask_table[256],
    rerr_table[256], gerr_table[256], berr_table[256];

  unsigned int dither_buf_width;
  short *red_err, *green_err, *blue_err, *next_red_err, *next_green_err,
    *next_blue_err;

  typedef struct Cache {
  	char *name;
    	Pixmap pixmap;
    	unsigned int count, width, height;
    	unsigned long pixel1, pixel2, texture;
  } Cache;

  LinkedList<Cache> *cache;

protected:
  Pixmap searchCache(
		//char* name,
		unsigned int, 
  		unsigned int, 
		unsigned long, 
		const BColor &,
                const BColor &);

public:
  BImageControl(Display *dpy, int depth, Window root, int screen, Visual *vis);
  ~BImageControl(void);

  // user configurable information
  Bool dither(void);

  // details
  Display *d(void) { return display; }
  Visual *v(void);
  Window drawable(void) { return window; }
  int bpp(void) { return bits_per_pixel; }
  int depth(void) { return screen_depth; }
  int screen(void) { return screen_number; }
  int colorsPerChannel(void) { return colors_per_channel; }

  unsigned long getColor(const char *);
  unsigned long getColor(const char *, unsigned char *, unsigned char *,
                         unsigned char *);
  void installRootColormap(void);

  // image cache/render requests
  Pixmap renderImage(/*char* name,*/ unsigned int, unsigned int, unsigned long, const BColor &,
                     const BColor &);
  void removeImage(Pixmap);
  void removeAllImages();

  // dither buffers
  void getDitherBuffers(unsigned int,
			short **, short **, short **,
			short **, short **, short **,
			unsigned short **, unsigned short **,
			unsigned short **);

  // rgb mask color lookup tables
  void getMaskTables(unsigned short **, unsigned short **, unsigned short **,
		     int *, int *, int *);

  // allocated colors
  void getColorTable(XColor **, int *);
};


#endif 

