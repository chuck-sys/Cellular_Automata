CPP := g++
CPPFLAGS := `fltk-config --cxxflags` -std=c++0x -c
LD := g++
LDFLAGS := `fltk-config --ldflags`
LDSTATIC := `fltk-config --ldstaticflags`

SRC := src/Tile.cpp src/Main.cpp
OBJ := obj/Tile.o obj/Main.o
EXE := Cellular_Automata

.PHONY: all clean

all: $(EXE)

clean:
	rm -rf $(EXE) $(OBJ)

$(EXE): obj/ $(OBJ)
	$(LD) $(OBJ) $(LDFLAGS) -o $@

obj/Main.o: src/Main.cpp src/Rulestring.h
obj/Tile.o: src/Tile.cpp src/Tile.h

obj/:
	mkdir obj

obj/%.o: src/%.cpp
	$(CPP) $(CPPFLAGS) $< -o $@

obj/%.o: src/%.cpp src/%.h
	$(CPP) $(CPPFLAGS) $< -o $@
