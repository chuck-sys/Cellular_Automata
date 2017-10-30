set(ca-sources
    LuaHelper.cpp
    Tile.cpp
    Utils.cpp
    Main.cpp)

add_executable(Cellular_Automata ${ca-sources})

target_link_libraries(Cellular_Automata ${FLTK_LIBRARIES})
target_link_libraries(Cellular_Automata ${LUA_LIBRARIES})

