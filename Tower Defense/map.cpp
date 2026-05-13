#include "map.h"
#include <cmath>

path_t map_paths[3];
bool   grid_is_path[3][grid_rows][grid_cols];

static const int path_half = 1;

float snap_cell(float v) {
    return (float)((int)(v / CELL) * CELL + CELL / 2);
}

void init_paths() {
    // Map 0 - S-curve
    map_paths[0].count = 8;
    map_paths[0].pts[0] = {snap_cell(0),   snap_cell(160)};
    map_paths[0].pts[1] = {snap_cell(240), snap_cell(160)};
    map_paths[0].pts[2] = {snap_cell(240), snap_cell(360)};
    map_paths[0].pts[3] = {snap_cell(440), snap_cell(360)};
    map_paths[0].pts[4] = {snap_cell(440), snap_cell(520)};
    map_paths[0].pts[5] = {snap_cell(620), snap_cell(520)};
    map_paths[0].pts[6] = {snap_cell(620), snap_cell(360)};
    map_paths[0].pts[7] = {(float)MAP_W - CELL / 2, snap_cell(360)};

    // Map 1 - Zigzag
    map_paths[1].count = 6;
    map_paths[1].pts[0] = {snap_cell(0),            snap_cell(120)};
    map_paths[1].pts[1] = {(float)MAP_W - CELL * 3, snap_cell(120)};
    map_paths[1].pts[2] = {(float)MAP_W - CELL * 3, snap_cell(360)};
    map_paths[1].pts[3] = {snap_cell(80),            snap_cell(360)};
    map_paths[1].pts[4] = {snap_cell(80),            snap_cell(580)};
    map_paths[1].pts[5] = {(float)MAP_W - CELL / 2, snap_cell(580)};

    // Map 2 - Spiral
    map_paths[2].count = 8;
    map_paths[2].pts[0] = {snap_cell(0),   snap_cell(360)};
    map_paths[2].pts[1] = {snap_cell(200), snap_cell(360)};
    map_paths[2].pts[2] = {snap_cell(200), snap_cell(120)};
    map_paths[2].pts[3] = {snap_cell(500), snap_cell(120)};
    map_paths[2].pts[4] = {snap_cell(500), snap_cell(360)};
    map_paths[2].pts[5] = {snap_cell(320), snap_cell(360)};
    map_paths[2].pts[6] = {snap_cell(320), snap_cell(560)};
    map_paths[2].pts[7] = {(float)MAP_W - CELL / 2, snap_cell(560)};
}

void build_grid(int m) {
    for (int r = 0; r < grid_rows; r++)
    {
        for (int c = 0; c < grid_cols; c++)
        {
            grid_is_path[m][r][c] = false;  
        }
    }

    const path_t& p = map_paths[m];

    for (int i = 0; i + 1 < p.count; i++) {
        float ax = p.pts[i].x, ay = p.pts[i].y;
        float bx = p.pts[i+1].x, by = p.pts[i+1].y;
        float dx = bx - ax, dy = by - ay, len = sqrtf(dx*dx + dy*dy);
        if (len < 0.001f) 
        {
            continue;
        }
        int steps = (int)(len / (CELL * 0.25f)) + 4;
        for (int s = 0; s <= steps; s++) 
        {
            float t  = (float)s / steps;
            float wx = ax + dx * t, wy = ay + dy * t;
            for (int dr = -path_half; dr <= path_half; dr++)
            {
                for (int dc = -path_half; dc <= path_half; dc++) 
                {
                    int c2 = (int)(wx / CELL) + dc;
                    int r2 = (int)(wy / CELL) + dr;
                    if (c2 >= 0 && c2 < grid_cols && r2 >= 0 && r2 < grid_rows)
                    {
                        grid_is_path[m][r2][c2] = true;
                    }
                }
            }
        }
    }

    for (int i = 1; i + 1 < p.count; i++) 
    {
        int cc = (int)(p.pts[i].x / CELL), rc = (int)(p.pts[i].y / CELL);
        for (int dr = -1; dr <= 1; dr++)
        {
            for (int dc = -1; dc <= 1; dc++) 
            {
                int c2 = cc + dc, r2 = rc + dr;
                if (c2 >= 0 && c2 < grid_cols && r2 >= 0 && r2 < grid_rows)
                {
                    grid_is_path[m][r2][c2] = true;
                }
            }
        }
    }
}

void draw_grid(int m) {
    static const Color g_a     = {44,  95, 32, 255};
    static const Color g_b     = {52, 110, 38, 255};
    static const Color p_a     = {185,162,108, 255};
    static const Color p_b     = {170,148, 95, 255};
    static const Color p_border= {110, 90, 52, 255};

    for (int r = 0; r < grid_rows; r++) 
    {
        for (int c = 0; c < grid_cols; c++) 
        {
            int px = MAP_X + c * CELL, py = r * CELL;
            if (grid_is_path[m][r][c]) 
            {
                DrawRectangle(px, py, CELL, CELL, ((r+c)%2==0) ? p_a : p_b);
                DrawRectangleLines(px, py, CELL, CELL, p_border);
            } 
            else 
            {
                DrawRectangle(px, py, CELL, CELL, ((r+c)%2==0) ? g_a : g_b);
                DrawRectangleLines(px, py, CELL, CELL, {34,74,24,80});
            }
        }
    }
}

void snap_to_grid(float mx, float my, int& oc, int& or2, float& ox, float& oy) 
{
    int c = (int)((mx - MAP_X) / CELL);
    int r = (int)(my / CELL);
    c = c < 0 ? 0 : (c >= grid_cols ? grid_cols - 1 : c);
    r = r < 0 ? 0 : (r >= grid_rows ? grid_rows - 1 : r);
    oc = c; or2 = r;
    ox = (float)(MAP_X + c * CELL + CELL / 2);
    oy = (float)(r * CELL + CELL / 2);
}

bool on_path(float px, float py, const path_t& path, float thick) 
{
    for (int i = 0; i + 1 < path.count; i++) 
    {
        float ax = path.pts[i].x   + (float)MAP_X, ay = path.pts[i].y;
        float bx = path.pts[i+1].x + (float)MAP_X, by = path.pts[i+1].y;
        float dx = bx - ax, dy = by - ay, len = sqrtf(dx*dx + dy*dy);
        if (len < 0.001f) 
        {
            continue;
        }
        float t = ((px-ax)*dx + (py-ay)*dy) / (len*len);
        t = fmaxf(0, fminf(1, t));
        float cx2 = ax + t*dx, cy2 = ay + t*dy;
        float d = sqrtf((px-cx2)*(px-cx2) + (py-cy2)*(py-cy2));
        if (d < thick) 
        {
            return true;
        }
    }
    return false;
}

void draw_path_markers(const path_t& path) {
    Vector2 entry = {path.pts[0].x + (float)MAP_X, path.pts[0].y};
    DrawCircle((int)entry.x, (int)entry.y, 10, {255,60,60,220});
    DrawText("IN", (int)entry.x - 9, (int)entry.y - 7, 12, WHITE);

    Vector2 ex = {path.pts[path.count-1].x + (float)MAP_X, path.pts[path.count-1].y};
    DrawCircle((int)ex.x, (int)ex.y, 10, {60,255,60,220});
    DrawText("OUT", (int)ex.x - 13, (int)ex.y - 7, 12, WHITE);
}

void draw_mini_path(const path_t& path, int ox, int oy, float scale, Color c) {
    for (int i = 0; i + 1 < path.count; i++) 
    {
        Vector2 a = {ox + path.pts[i].x   * scale, oy + path.pts[i].y   * scale};
        Vector2 b = {ox + path.pts[i+1].x * scale, oy + path.pts[i+1].y * scale};
        DrawLineEx(a, b, 5, c);
    }
}