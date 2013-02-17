// Image.hh for Blackbox - an X11 Window manager
// Copyright (c) 1997 - 2000 Brad Hughes (bhughes@tcac.net)
//
// Permission is hereby granted, free of charge, to any person obtaining a
// copy of this software and associated documentation files (the "Software"),
// to deal in the Software without restriction, including without limitation
// the rights to use, copy, modify, merge, publish, distribute, sublicense,
// and/or sell copies of the Software, and to permit persons to whom the
// Software is furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL
// THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
// DEALINGS IN THE SOFTWARE.

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

