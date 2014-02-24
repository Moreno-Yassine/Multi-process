#fichiers compiles
EXE=Parking

#programmes
COMP=g++
EDL=g++
ECHO=@echo
RM=rm
CLEAN=efface

#flags
CPPFLAGS=-W -Wall
EDLFLAGS=-g
RMFLAGS=-f

#files
INCPATH=#-I
LIBPATH=#-L
OBJ=Parking.o

$(EXE): $(OBJ)
	$(ECHO) EdL de $(EXE)
	$(EDL) $(LIBPATH) -o $(EXE) $(OBJ)

%.o: %.cpp
	$(ECHO) Compilation de $@
	$(COMP) $(CPPFLAGS) $(INCPATH) -c $< -o $@

Parking.o: Parking.cpp Parking.h

.PHONY:$(CLEAN)
$(CLEAN):
	$(ECHO) Tout va disparaitre
	$(RM) $(RMFLAGS) $(OBJ) $(EXE) Core


