#pragma once

static const int SW = 1280, SH = 720;
static const int SIDE_W = 220;
static const int INFO_W = 200;
static const int MAP_X  = INFO_W;
static const int MAP_W  = SW - INFO_W - SIDE_W;
static const int MAP_H  = SH;

static const int CELL      = 40;
static const int grid_cols = MAP_W / CELL;
static const int grid_rows = MAP_H / CELL;

static const int MAX_ENEMIES     = 128;
static const int MAX_TOWERS      = 64;
static const int MAX_PROJECTILES = 256;
static const int MAX_EXPLOSIONS  = 32;
static const int MAX_PATH_PTS    = 24;