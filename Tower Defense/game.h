#pragma once
#include "raylib.h"
#include "constants.h"
#include "enemy.h"
#include "tower.h"
#include "wave.h"

struct best_score {
    int wave;
    int gold;
};

extern best_score best_scores[3];

void save_scores();
void load_scores();

enum class Screen {MENU, MAP_SELECT, GAME, GAME_OVER, WIN};

struct Explosion {
    float x, y, r, max_r, timer;
    Color col;
};

struct Game {
    Screen screen = Screen::MENU;
    int map_idx = 0, gold = 200, lives = 10, wave = 0;
    bool wave_running = false;

    Enemy* enemies[MAX_ENEMIES];
    int enemy_count = 0;
    Tower* towers[MAX_TOWERS];
    int tower_count = 0;
    Projectile* projectiles[MAX_PROJECTILES];
    int proj_count = 0;
    Explosion explosions[MAX_EXPLOSIONS];
    int expl_count = 0;

    int spawn_phase = 0, spawn_count = 0;
    float spawn_timer = 0;
    int selected_tower = -1;
    int hovered_tower  = -1;
    Tower* inspect_tower = nullptr;

    Texture2D bg_tex;
    bool bg_loaded   = false;
    Music bg_music;
    bool music_loaded = false;

    Game();
    void reset(int map);
};

Enemy* spawn_enemy(int type, const path_t* path);

void draw_text_centered(const std::string& s, int cx, int y, int fs, Color c);
float dist(float ax, float ay, float bx, float by);

void draw_left_panel(Game& g);
void draw_right_panel(Game& g);
void remove_enemy(Game& g, int idx);
void remove_projectile(Game& g, int idx);
void remove_explosion(Game& g, int idx);
void draw_bg(const Game& g);