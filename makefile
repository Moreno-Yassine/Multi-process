#fichiers compiles
EXE=Parking

#programmes
COMP=g++
EDL=g++
ECHO=@echo
RM=rm
CLEAN=efface

#flags
CPPFLAGS=-c -Wall
EDLFLAGS=-g
RMFLAGS=-f

#files
LIBPATH=-L /public/tp/tp-multitache
INCPATH=-I /public/tp/tp-multitache
INTERFACE = Parking.h Simulation.h Entree.h ManagerSortie.h Config.h
LIBSYSTEME = -ltp -lncurses -ltcl
CFILES = $(INTERFACE:.h=.cpp)
OBJ=$(INTERFACE:.h=.o)

$(EXE): $(OBJ)
	$(ECHO) EdL de $(EXE)
	$(EDL) -o $(EXE) $(OBJ) $(LIBPATH) $(LIBSYSTEME)

%.o: %.cpp
	$(ECHO) Compilation de $<
	$(COMP) $(CPPFLAGS) $(INCPATH) $<

Parking.o: Parking.cpp Parking.h Config.h Entree.h Simulation.h Heure.h ManagerSortie.h
ManagerSortie.o: ManagerSortie.cpp ManagerSortie.h Config.h
Simulation.o: Simulation.cpp Simulation.h Menu.h Config.h
Entree.o: Entree.cpp Entree.h Config.h
Config.o: Config.cpp Config.h


.PHONY:$(CLEAN)
$(CLEAN):
	$(ECHO) Tout va disparaitre
	$(RM) $(RMFLAGS) $(OBJ) $(EXE) Core
