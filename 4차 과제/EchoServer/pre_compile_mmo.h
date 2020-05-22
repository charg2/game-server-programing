#pragma once

#include "../C2Server/C2Server/pre_compile.h"
//#include "MMOSimulator.h"
//#include "MMOZone.h"
//#include "MMOSector.h"

constexpr auto SCREEN_WIDTH = 8;
constexpr auto SCREEN_HEIGHT = 8;

constexpr auto TILE_WIDTH = 65;
constexpr auto WINDOW_WIDTH = TILE_WIDTH * SCREEN_WIDTH + 10;   // size of window
constexpr auto WINDOW_HEIGHT = TILE_WIDTH * SCREEN_WIDTH + 10;
constexpr auto BUF_SIZE = 200;
constexpr auto MAX_USER = 10;


// 추후 확장용.
int NPC_ID_START = 10000;
