CPP := g++
CPPFLAGS := `fltk-config --cxxflags` -std=c++11 -c -Ilua/include/ -O2
LD := g++
LDFLAGS := `fltk-config --ldflags` -Llua/lib -llua
LDSTATIC := `fltk-config --ldstaticflags` -Llua/lib -llua

OBJ := obj/Tile.o obj/LuaHelper.o obj/Main.o obj/Utils.o
EXE := Cellular_Automata

.PHONY: all clean

all: $(EXE)

clean:
	rm -rf $(EXE) $(OBJ)

$(EXE): obj/ $(OBJ)
	$(LD) $(OBJ) $(LDFLAGS) -o $@

obj/Main.o: src/Main.cpp src/Rulestring.h src/Tempvars.h src/Utils.h

obj/:
	mkdir obj

obj/%.o: src/%.cpp
	$(CPP) $(CPPFLAGS) $< -o $@

obj/%.o: src/%.cpp src/%.h
	$(CPP) $(CPPFLAGS) $< -o $@
