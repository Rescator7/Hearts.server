# C compiler to use
CC = gcc
CPP = g++

# Any special flags you want to pass to the compiler
#MYFLAGS = -D_REENTRANT -DQT_NO_DEBUG -DQT_THREAD_SUPPORT -DQT_SHARED

#Library
LIBRARY = -lmysqlclient -lcrypto
#CPP_LIBRARY = -L/usr/lib/qt3/lib -lqt-mt

#Includes
#CPP_INCLUDES = -I/usr/lib/qt3/include
#SDL_INCLUDES = -I/usr/include/SDL
#MOC = /usr/lib/qt3/bin/moc

##############################################################################
# Do Not Modify Anything Below This Line (unless you know what you're doing) #
##############################################################################

BINDIR = ./
BBB = .

VERSION = -DVERSION=\"Version-0.0.1\" # about.cpp, client.cpp

#CFLAGS = -O3 -pipe -fno-exceptions -Wall -march=i486 -fomit-frame-pointer -funroll-all-loops -mpreferred-stack-boundary=2 -mcpu=i686 $(MYFLAGS)
CFLAGS = -g -Wall

OBJFILES = commands/new.o commands/sitc.o commands/shutoff.o commands/shutdown.o commands/date.o commands/exit.o ansi.o comm.o commands.o log.o player.o sql.o string.o table.o main.o

default: all

all:
	$(MAKE) $(BINDIR)

$(BINDIR) : $(MOCFILES) $(OBJFILES)
	$(CPP) $(CFLAGS) -o server $(MOCFILES) $(OBJFILES) $(LIBRARY) $(CPP_LIBRARY)

clean:
	rm -f *.d commands/*.d
	rm -f *.o commands/*.o
	rm -f server
	rm -f server.log
	rm -f core

ref:
%.o: %.cpp
	$(CPP) -c $(CFLAGS) $(CPP_INCLUDES) $< -o $@

#########################################################################
# Dependencies
# #########################################################################

DEPENDS = $(OBJFILES:%.o=%.d)

ifneq ($(MAKECMDGOALS),clean)
include $(DEPENDS)
endif

%.d: %.cpp
	printf "$@ " > $@
	$(CC) -MM $(DEFFLAGS) $(INCLUDES) $< >> $@
