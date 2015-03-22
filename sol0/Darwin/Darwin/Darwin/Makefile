# file: Makefile
# author: Ted Baker
# revised: 2005/01/08
# revised: 2015/01/05

#  Build example program sim.

#  Do not make the targets "programs" and "runtest" except from
#  within one of the two OS-specific build directories (Linux or SunOS).
#  Do not make the target "clean" except from the top-level (P1)
#  directory.

.POSIX:
.SILENT:default links
PROGRAMS= sim
HEADERS=events.h random.h simulator.h list.h
SOURCES=Makefile $(HEADERS) sim.c simulator.c events.c random.c 
OBJECTS=simulator.o events.o random.o
SYSTEMS=Linux SunOS Darwin
LINKS=$(SOURCES) $(SYSTEMS)
CFLAGS= -Wall -Wextra -pedantic -DDEBUG
CC=gcc

default: 
	if [ ! -d ../Linux -a ! -d Linux ]; then mkdir Linux; cd Linux; make -f ../Makefile links; fi
	if [ ! -d ../SunOS -a ! -d SunOS ]; then mkdir SunOS; cd SunOS; make -f ../Makefile links; fi
	if [ ! -d ../Darwin -a ! -d Darwin ]; then mkdir Darwin; cd Darwin; make -f ../Makefile links; fi
	if [ `uname -s` = "Linux" ]; then cd Linux; make programs;\
 elif [ `uname -s` = "SunOS" ]; then cd SunOS; make programs;\
 elif [ `uname -s` = "Darwin" ]; then cd Darwin; make programs;\
 else echo "failure: This makefile only works for Linux, SunOS, and Darwin (Mac OS)";\
 fi;
links:
	for NAME in $(LINKS); do ln -fs ../$${NAME}; done;
programs: $(PROGRAMS)
sim: sim.c $(HEADERS) sim.c simulator.o events.o random.o
	$(CC) $(CFLAGS) -o sim sim.c simulator.o events.o random.o -lm
$(OBJECTS): $(HEADERS)
runtest: $(PROGRAMS)
	./testrand
	./sim
clean:
	rm -rf *.o *# *~ $(PROGRAMS) core $(SYSTEMS)

