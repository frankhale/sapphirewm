CC        = g++
CFLAGS    = -g -O2 -Wall -arch=686
XROOT     = /usr/X11R6
INCLUDES  = -I$(XROOT)/include
LIBS      = -lX11 -lXext -lXpm 
LDPATH    = -L$(XROOT)/lib

PROG      = appbar
PREFIX    = /usr
DEFINES   = #-DDEBUG

OBJS      = main.o appbar.o scanner.o icon.o linkedlist.o image.o

all: $(PROG) 

$(PROG): $(OBJS)
	$(CC) $(OBJS) $(LDPATH) $(LIBS) -o $@

$(OBJS): %.o: %.cc $(HEADERS)
	$(CC) $(CFLAGS) $(DEFINES) $(INCLUDES) -c $< -o $@

install: all
	install -s $(PROG) $(PREFIX)/bin
	mkdir -p /etc/X11/appbar
	mkdir -p /usr/X11/share/appbar
	mkdir -p /usr/X11/share/appbar/images
	cp appbar.rc /etc/X11/appbar
	cp -r images/ /usr/X11/share/appbar/
	
uninstall:
	rm -f $(PREFIX)/bin/$(PROG)
	rm -rf $(PREFIX)/X11/share/appbar
	rm -rf /etc/X11/appbar
clean:
	rm -f $(PROG) $(OBJS) core
