/*
 * LuaHelper.h
 *
 *  Created on: Dec 13, 2014
 *      Author: cheukyin699
 */

#ifndef LUAHELPER_H_
#define LUAHELPER_H_

extern "C"
{
	#include <lua.h>
	#include <lualib.h>
	#include <lauxlib.h>
}

class Lua_Helper
{
private:
	lua_State* state;
public:
	Lua_Helper();
	~Lua_Helper();

	// Implicitly act as lua_State pointer
	inline operator lua_State*() {
		return state;
	}
	// Other implicit functions
	template<typename T>
	T lua_get() {return 0;}

	// Getters (for constant config values)
	template<typename T>
	T get(char const* varname)
	{
		// The result
		T result;
		// Put variable on stack
		lua_getglobal(this->state, varname);
		// Get variable from stack
		result = this->lua_get<T>();
		// Pop from stack
		lua_pop(this->state, 1);

		return result;
	}

	// Report errors
	void report_errors(int);

	// Close it
	void close();
};


#endif /* LUAHELPER_H_ */
