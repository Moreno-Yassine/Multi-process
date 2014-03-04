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
LIBPATH=-L./
INCPATH=-I./
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


.PHONY:$(CLEAN)
$(CLEAN):
	$(ECHO) Tout va disparaitre
	$(RM) $(RMFLAGS) $(OBJ) $(EXE) Core
