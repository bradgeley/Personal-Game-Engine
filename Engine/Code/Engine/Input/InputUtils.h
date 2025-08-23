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
    Escape = 27, Tilde = 192, Space = 32, Enter = 13, Shift = 16, 
    Up = 38, Down = 40, Left = 37, Right = 39,
    Home = 36, End = 35, Delete = 46, Backspace = 8,
    Period = 190, Comma = 188, Ctrl = 17, Tab = 9
};