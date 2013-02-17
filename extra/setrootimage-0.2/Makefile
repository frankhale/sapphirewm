CC        = g++
CFLAGS    = -g -O2 -Wall -arch=686
XROOT     = /usr/X11R6
INCLUDES  = -I$(XROOT)/include
LIBS      = -lX11 -lXext
LDPATH    = -L$(XROOT)/lib

PROG      = setrootimage
PREFIX    = /usr/local
#DEFINES   = -DDEBUG

OBJS      = setrootimage.o linkedlist.o image.o

all: $(PROG) 

$(PROG): $(OBJS)
	$(CC) $(OBJS) $(LDPATH) $(LIBS) -o $@

$(OBJS): %.o: %.cc $(HEADERS)
	$(CC) $(CFLAGS) $(DEFINES) $(INCLUDES) -c $< -o $@

install: all
	install -s $(PROG) $(PREFIX)/bin
	
uninstall:
	rm -f $(PREFIX)/bin/$(PROG)

clean:
	rm -f $(PROG) $(OBJS) core
