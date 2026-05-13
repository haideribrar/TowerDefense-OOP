#include "tower.h"
#include <cmath>
Projectile::Projectile(float x, float y, Enemy* t, float spd, int dmg, Color c, bool bomb, float r): x(x), y(y), tx(t->get_x()), ty(t->get_y()), spd(spd), dmg(dmg), col(c),active(true), radius(r), is_bomb(bomb), target(t) {}

void Projectile::update(float dt) {
    if (target && target->is_alive()) 
    {
        tx = target->get_x(); 
        ty = target->get_y(); 
    }
    float dx = tx - x, dy = ty - y, d = sqrtf(dx*dx + dy*dy);
    if (d < spd * dt) 
    { 
        x = tx; y = ty; 
        active = false; 
        return; 
    }
    x += dx / d * spd * dt;
    y += dy / d * spd * dt;
}

void Projectile::render() const {
    if (is_bomb) 
    {
        DrawCircle((int)x, (int)y, 6, col);
    }
    else        
    {
        DrawCircle((int)x, (int)y, 4, col);
    }
}

Tower::Tower(float x, float y, int hp, float rng, float fr, int dmg, int cost, const std::string& n, const std::string& tag, Color bc, Color ac)
    : Entity(x, y, hp, n), range(rng), fire_rate(fr), fire_timer(0), damage(dmg),
      cost(cost), level(0), upgrade_cost(cost / 2), type_tag(tag), body_col(bc), accent_col(ac) {}

void Tower::update(float dt) {
    fire_timer -= dt;
}

void Tower::render() const {
    DrawRectangle((int)x - CELL/2, (int)y - CELL/2, CELL, CELL, {50,52,62,210});
    DrawRectangle((int)x - 13, (int)y - 13, 26, 22, body_col);
    DrawCircle((int)x, (int)y - 6, 8, accent_col);
    for (int i = 0; i < level; i++) 
    {
        DrawCircle((int)x - 8 + i*8, (int)y + 8, 3, GOLD);
    }
    DrawCircleLines((int)x, (int)y, (int)range, {255,255,255,22});
}

void Tower::upgrade() {
    level++;
    range   *= 1.15f;
    damage   = (int)(damage * 1.3f);
    fire_rate *= 1.1f;
}

Enemy* Tower::find_closest(Enemy** enemies, int count) {
    Enemy* best = nullptr;
    int best_wp = -1;
    for (int i = 0; i < count; i++) 
    {
        Enemy* e = enemies[i];
        if (!e || !e->is_alive()) 
        {
            continue;
        }
        float dx = x - e->get_x(), dy = y - e->get_y();
        float d = sqrtf(dx*dx + dy*dy);
        if (d <= range) 
        {
            if (!best || e->get_wp_idx() > best_wp) 
            { 
                best = e; 
                best_wp = e->get_wp_idx(); 
            }
        }
    }
    return best;
}

const char* tower_names[5] = {"Cannon", "Sniper", "Machine Gun", "Slow", "Bomb"};
const int tower_costs[5] = {100, 120, 90, 80, 130};
const Color tower_cols[5]  = {
    {255,160, 30,255}, {100,200,255,255}, {255,80,80,255}, {80,240,160,255}, {255,120,0,255}
};
const char* tower_descs[5] = {
    "High dmg, slow\nfire. Best vs\nTank enemies.",
    "PIERCE shot:\nhits ALL enemies\nalong the line.",
    "DUAL BARREL:\nfires 2 bullets\nat same target.",
    "Slows all enemies\nin range at once.",
    "SPLASH bomb:\nhits all nearby\nenemies on impact."
};
const int tower_dmg[5] = {55, 90, 18, 10, 70};
const int tower_range[5] = {130, 260, 100, 115, 140};

cannon_tower::cannon_tower(float x, float y): Tower(x, y, 100, 130, 0.8f, 55, 100, "Cannon Tower", "CANNON", {60,60,70,255}, {255,160,30,255}) {}

int cannon_tower::try_shoot(Enemy** e, int n, Projectile** out, int out_max) {
    if (!ready_to_fire() || out_max < 1) 
    {
        return 0;
    }
    Enemy* t = find_closest(e, n);
    if (!t) 
    {
        return 0;
    }
    reset_timer();
    out[0] = new Projectile(x, y, t, 280, damage, {255,160,30,255});
    return 1;
}

void cannon_tower::render() const {
    Tower::render();
    DrawRectangle((int)x - 4, (int)y - 24, 8, 14, {40,40,40,255});
}

std::string cannon_tower::get_description() const {
    return "High dmg, slow\nfire. Best vs\nTank enemies.";
}

sniper_tower::sniper_tower(float x, float y): Tower(x, y, 80, 260, 0.4f, 90, 120, "Sniper Tower", "SNIPER", {30,80,140,255}, {100,200,255,255}) {}

int sniper_tower::try_shoot(Enemy** e, int n, Projectile** out, int out_max) {
    if (!ready_to_fire() || out_max < 1) 
    {
        return 0;
    }
    Enemy* primary = find_closest(e, n);
    if (!primary) 
    {
        return 0;
    }
    reset_timer();
    float dx = primary->get_x() - x, dy = primary->get_y() - y;
    float len = sqrtf(dx*dx + dy*dy);
    if (len < 0.001f) 
    {
        return 0;
    }
    float nx = dx / len, ny = dy / len;
    int count = 0;
    for (int i = 0; i < n && count < out_max; i++) 
    {
        if (!e[i] || !e[i]->is_alive()) 
        {
            continue;
        }
        float ex2 = e[i]->get_x() - x, ey2 = e[i]->get_y() - y;
        float along = ex2*nx + ey2*ny;
        float perp_x = ex2 - along*nx, perp_y = ey2 - along*ny;
        float perp_dist = sqrtf(perp_x*perp_x + perp_y*perp_y);
        if (along > 0 && along <= range && perp_dist < 18.0f) 
        {
            e[i]->take_damage(damage);
            out[count++] = new Projectile(x, y, e[i], 800, 0, {100,200,255,255});
        }
    }
    return count;
}

void sniper_tower::render() const {
    Tower::render();
    DrawRectangle((int)x - 3, (int)y - 30, 6, 20, {20,60,100,255});
}

std::string sniper_tower::get_description() const {
    return "PIERCE shot:\nhits ALL enemies\nalong the line.";
}

machinegun_tower::machinegun_tower(float x, float y): Tower(x, y, 90, 100, 4.0f, 18, 90, "Machine Gun", "MGUN", {100,60,30,255}, {255,80,80,255}) {}

int machinegun_tower::try_shoot(Enemy** e, int n, Projectile** out, int out_max) {
    if (!ready_to_fire() || out_max < 2) 
    {
        return 0;
    }
    Enemy* t = find_closest(e, n);
    if (!t) 
    {
        return 0;
    }
    reset_timer();
    out[0] = new Projectile(x - 4, y, t, 400, damage, {255, 80, 80,255});
    out[1] = new Projectile(x + 4, y, t, 400, damage, {255,140, 80,255});
    return 2;
}

void machinegun_tower::render() const {
    Tower::render();
    DrawRectangle((int)x - 5, (int)y - 26, 4, 16, {60,40,20,255});
    DrawRectangle((int)x + 1, (int)y - 26, 4, 16, {60,40,20,255});
}

std::string machinegun_tower::get_description() const {
    return "DUAL BARREL:\nfires 2 bullets\nat same target.";
}

slow_tower::slow_tower(float x, float y): Tower(x, y, 80, 115, 0.6f, 10, 80, "Slow Tower", "SLOW", {60,120,80,255}, {80,240,160,255}),
      slow_mult(0.4f) {}

int slow_tower::try_shoot(Enemy** e, int n, Projectile** out, int out_max) {
    if (!ready_to_fire() || out_max < 1) 
    {
        return 0;
    }
    Enemy* primary = find_closest(e, n);
    if (!primary) 
    {
        return 0;
    }
    reset_timer();
    for (int i = 0; i < n; i++) 
    {
        if (!e[i] || !e[i]->is_alive()) 
        {
            continue;
        }
        float dx = x - e[i]->get_x(), dy = y - e[i]->get_y();
        if (sqrtf(dx*dx + dy*dy) <= range)
        {
            e[i]->apply_slow(slow_mult, 2.5f);
        }
    }
    out[0] = new Projectile(x, y, primary, 250, damage, {80,240,160,255});
    return 1;
}

void slow_tower::render() const {
    Tower::render();
    DrawCircle((int)x, (int)y, 11, {80,240,160,80});
}

std::string slow_tower::get_description() const {
    return "Slows all enemies\nin range at once.";
}

bomb_tower::bomb_tower(float x, float y): Tower(x, y, 100, 140, 0.5f, 70, 130, "Bomb Tower", "BOMB", {120,60,20,255}, {255,120,0,255}) {}

int bomb_tower::try_shoot(Enemy** e, int n, Projectile** out, int out_max) {
    if (!ready_to_fire() || out_max < 1) 
    {
        return 0;
    }
    Enemy* t = find_closest(e, n);
    if (!t) 
    {
        return 0;
    }
    reset_timer();
    out[0] = new Projectile(x, y, t, 220, damage, {255,120,0,255}, true, 70.0f);
    return 1;
}

void bomb_tower::render() const {
    Tower::render();
    DrawCircle((int)x, (int)y - 20, 9, {80,40,10,255});
    DrawRectangle((int)x - 1, (int)y - 30, 2, 8, {200,160,80,255});
}

std::string bomb_tower::get_description() const {
    return "SPLASH bomb:\nhits all nearby\nenemies on impact.";
}

Tower* build_tower(int type, float x, float y) {
    switch (type) {
        case 0: return new cannon_tower(x, y);
        case 1: return new sniper_tower(x, y);
        case 2: return new machinegun_tower(x, y);
        case 3: return new slow_tower(x, y);
        case 4: return new bomb_tower(x, y);
        default: return nullptr;
    }
}