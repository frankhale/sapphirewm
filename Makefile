# Sapphire Makefile
#
# frankhale@yahoo.com
# 27 Oct 2001

CC       = g++
CFLAGS   = -g -O2 -Wall

prefix   = /usr
XROOT    = $(prefix)/X11R6
DESTDIR  =
INCLUDES = -I$(XROOT)/include
LDPATH   = -L$(XROOT)/lib
LIBS     = -lXext -lX11

# If you change this, remember to change it in windowmanager.cc (line 34),
# and in data/menu/default (lines 43-47, the themes section).
CONFIGURATION_PATH=/usr/share/sapphire

# DEBUG = Outputs debug information
DEFINES  = #-DDEBUG 
HEADERS  = linkedlist.hh 	\
	   basemenu.hh   	\
	   rootmenu.hh		\
	   iconmenu.hh		\
	   menulex.hh		\
	   scanner.hh		\
	   theme.hh		\
	   toolbar.hh		\
	   image.hh		\
	   windowmanager.hh 	\
	   client.hh		\
	   sapphire.hh   

OBJS     = linkedlist.o 	\
	   basemenu.o   	\
	   rootmenu.o		\
	   iconmenu.o		\
	   menulex.o		\
	   scanner.o		\
	   theme.o		\
	   toolbar.o		\
	   image.o		\
	   windowmanager.o 	\
	   client.o 		\
	   misc.o 		\
	   main.o 

all: sapphire

sapphire: $(OBJS)
	$(CC) $(OBJS) $(LDPATH) $(LIBS) -o $@

$(OBJS): %.o: %.cc $(HEADERS)
	$(CC) $(CFLAGS) $(DEFINES) $(INCLUDES) -c $< -o $@

install: all
	install -s sapphire $(DESTDIR)$(prefix)/bin/
	
	install -d $(DESTDIR)$(CONFIGURATION_PATH)/
	
	install -d $(DESTDIR)$(CONFIGURATION_PATH)/themes/
	install -m 644 data/themes/* $(DESTDIR)$(CONFIGURATION_PATH)/themes/
	
	install -d $(DESTDIR)$(CONFIGURATION_PATH)/menu/
	install -m 644 data/menu/* $(DESTDIR)$(CONFIGURATION_PATH)/menu/

uninstall:
	rm $(DESTDIR)$(prefix)/bin/sapphire
	rm -rf $(DESTDIR)$(CONFIGURATION_PATH)
	
clean:
	rm -f sapphire $(OBJS) core

.PHONY: all install clean
