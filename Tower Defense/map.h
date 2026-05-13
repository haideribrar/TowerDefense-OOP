#pragma once
#include "raylib.h"
#include "constants.h"

struct path_t {
    Vector2 pts[MAX_PATH_PTS];
    int count;
};

extern path_t map_paths[3];
extern bool grid_is_path[3][grid_rows][grid_cols];

float snap_cell(float v);
void  init_paths();
void  build_grid(int m);
void  draw_grid(int m);
void  snap_to_grid(float mx, float my, int& oc, int& or2, float& ox, float& oy);
bool  on_path(float px, float py, const path_t& path, float thick = 35.0f);
void  draw_path_markers(const path_t& path);
void  draw_mini_path(const path_t& path, int ox, int oy, float scale, Color c);
