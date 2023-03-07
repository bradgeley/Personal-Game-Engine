// Bradley Christensen - 2022-2023
#pragma once
#include <cstdint>



//-------------------------------------------------------------------------------------
// SUPPORTED KEY CODES -- based on windows VKs
//
enum class KeyCode : int
{
    F1 = 112, F2 = 113, F3 = 114, F4 = 115, F5 = 116, F6 = 117, 
    F7 = 118, F8 = 119, F9 = 120, F10 = 121, F11 = 122,
    ESCAPE = 27, TILDE = 192, SPACE = 32, ENTER = 13, SHIFT = 16, 
    UP = 38, DOWN = 40, LEFT = 37, RIGHT = 39,
    HOME = 36, END = 35, DEL = 46, BACKSPACE = 8,
    PERIOD = 190, COMMA = 188,
};
//