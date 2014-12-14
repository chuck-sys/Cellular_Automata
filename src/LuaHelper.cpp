/*
 * LuaHelper.cpp
 *
 *  Created on: Dec 13, 2014
 *      Author: cheukyin699
 */

#include "LuaHelper.h"

#include <iostream>

using namespace std;


Lua_Helper::Lua_Helper()
{
	state = luaL_newstate();

	// Load all LUA libraries
	luaL_openlibs(state);
}

Lua_Helper::~Lua_Helper()
{
}

/* To report errors */
void Lua_Helper::report_errors(int status)
{
	if(status != 0)
	{
		// Tell user if there is an error message
		cerr << "-- " << lua_tostring(this->state, -1) << endl;
		// Pop value from stack
		lua_pop(this->state, 1);
	}
}

template<>
bool Lua_Helper::lua_get<bool>()
{
	return (bool)lua_toboolean(this->state, -1);
}

template<>
int Lua_Helper::lua_get<int>()
{
	return (int)lua_tonumber(this->state, -1);
}

template<>
double Lua_Helper::lua_get<double>()
{
	return (double)lua_tonumber(this->state, -1);
}

template<>
char* Lua_Helper::lua_get<char*>()
{
	return (char*)lua_tostring(this->state, -1);
}

void Lua_Helper::close()
{
	lua_close(this->state);
}

