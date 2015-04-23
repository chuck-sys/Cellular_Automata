Cellular_Automata
=================


**Cellular_Automata** is a program intended to experiment with different<br>
types of cellular automata, such as *Game of Life*, *Maze*, *HighLife*, etc.<br>
It gives you the opportunity to program it to do what you want it to do, and<br>
have more control over what you are doing.

Version:	v1.1.0

Author:		Cheuk Yin Ng <cheukyin699@yahoo.com>



Build
=====

To build **Cellular Automata**, you will need the following installed on your<br>
computer:

- GNU gcc/g++ v4.8 or higher
- GNU make v3.8 or higher
- FLTK v1.3 or higher

The correct LUA libraries are included.

Do:

	make clean
	make

Features
========

**Cellular Automata** can:
- Run a configuration of cells with custom rules
- Create your own rules!
- Step through a configuration of cells with rules
- Project a configuration of cells for *n* number of generations passed
- Rules included:
	* Game of Life
	* HighLife
	* Maze
	* Mazectric
	* Replicator
- Save/Load boards in plain text format
- Save/Load stamps in plain text format
- Stamps included:
	* Block
	* Blinker
	* Glider
- Select areas to apply:
	* Inversion
	* Random tiles
	* Reset
- Drag mode!
- Lua configuration file
- Lua autorun file
- Embeded Lua runtime commands and support
	* Edit Lua scripts
	* Save Lua scripts
	* Run Lua scripts

