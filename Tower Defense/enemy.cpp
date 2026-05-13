#include "enemy.h"
#include "constants.h"
#include <cmath>

void Enemy::set_path(const path_t* p) {
    path = p;
    x = p->pts[0].x + MAP_X;
    y = p->pts[0].y;
    wp_idx = 1;
}

void Enemy::apply_slow(float mult, float dur) {
    slow_mult = mult;
    slow_timer = dur;
    speed = base_speed * mult;
}

void Enemy::take_damage(int d) {
    if (shielded) 
    { 
        shielded = false;
        return;
    }
    hp -= d;
    if (hp <= 0) 
    { 
        hp = 0; 
        alive = false; 
    }
}

void Enemy::update(float dt) {
    if (!alive || reached_end) 
    {
        return;
    }
    slow_timer -= dt;
    if (slow_timer <= 0) 
    { 
        slow_timer = 0; 
        speed = base_speed; 
        slow_mult = 1.0f; 
    }
    else 
    {
        speed = base_speed * slow_mult;
    }

    if (!path || wp_idx >= path->count) 
    {
        return;
    }
    Vector2 tgt = {path->pts[wp_idx].x + (float)MAP_X, path->pts[wp_idx].y};
    float dx = tgt.x - x, dy = tgt.y - y, d = sqrtf(dx*dx + dy*dy);
    if (d < 2.0f) 
    {
        wp_idx++;
        if (wp_idx >= path->count) 
        { 
            reached_end = true; 
            alive = false; 
            return; 
        }
    } 
    else 
    {
        float s = speed * dt;
        x += dx / d * s;
        y += dy / d * s;
    }
}

void Enemy::draw_hp_bar() const 
{
    float bw = 40, bh = 6;
    DrawRectangle((int)(x - bw/2), (int)(y - 26), (int)bw, (int)bh, RED);
    DrawRectangle((int)(x - bw/2), (int)(y - 26), (int)(bw * ((float)hp / max_hp)), (int)bh, GREEN);
    DrawRectangleLines((int)(x - bw/2), (int)(y - 26), (int)bw, (int)bh, DARKGRAY);
    if (shielded) 
    {
        DrawRectangle((int)(x - bw/2), (int)(y - 33), (int)bw, 5, SKYBLUE);
    }
}

FastEnemy::FastEnemy(const path_t* p): Enemy(0, 0, 160, 155.0f, 15, 2, "Fast Enemy"), weave_timer(0) { set_path(p); }

void FastEnemy::update(float dt) {
    Enemy::update(dt);
    if (!alive || reached_end || !path || wp_idx >= path->count) 
    {
        return;
    }
    weave_timer += dt * 6.0f;
    Vector2 tgt = {path->pts[wp_idx].x + (float)MAP_X, path->pts[wp_idx].y};
    float dx = tgt.x - x, dy = tgt.y - y, len = sqrtf(dx*dx + dy*dy);
    if (len > 0.001f) 
    {
        float px2 = -dy / len, py2 = dx / len;
        float wave = sinf(weave_timer) * 10.0f;
        x += px2 * wave * dt;
        y += py2 * wave * dt;
    }
}

void FastEnemy::render() const {
    int cx = (int)x, cy = (int)y;
    DrawRectangle(cx-10, cy+10, 20, 5, {0,0,0,60});
    DrawTriangle({(float)cx,(float)(cy-14)}, {(float)(cx+11),(float)cy}, {(float)(cx-11),(float)cy}, {200,200,200,255});
    DrawTriangle({(float)(cx-11),(float)cy}, {(float)(cx+11),(float)cy}, {(float)cx,(float)(cy+14)}, {160,160,160,255});
    DrawLine(cx,cy-14, cx+11,cy, {80,80,80,255});
    DrawLine(cx+11,cy, cx,cy+14, {80,80,80,255});
    DrawLine(cx,cy+14, cx-11,cy, {80,80,80,255});
    DrawLine(cx-11,cy, cx,cy-14, {80,80,80,255});
    DrawRectangle(cx-11, cy-2, 22, 4, {220,50,50,200});
    DrawCircle(cx, cy-20, 7, {220,220,220,255});
    DrawCircleLines(cx, cy-20, 7, {80,80,80,255});
    DrawCircle(cx-3, cy-21, 2, {220,50,50,255});
    DrawCircle(cx+3, cy-21, 2, {220,50,50,255});
    draw_hp_bar();
}

TankEnemy::TankEnemy(const path_t* p): Enemy(0, 0, 1200, 35.0f, 40, 4, "Tank Enemy"), regen_timer(0) { set_path(p); }

void TankEnemy::update(float dt) {
    Enemy::update(dt);
    if (!alive || reached_end) 
    {
        return;
    }
    regen_timer += dt;
    if (regen_timer >= 1.0f) 
    {
        regen_timer = 0;
        if (hp < max_hp) 
        {
            hp = hp + 6 < max_hp ? hp + 6 : max_hp;
        }
    }
}

void TankEnemy::render() const {
    int cx = (int)x, cy = (int)y;
    DrawRectangle(cx-22, cy+16, 44, 6, {0,0,0,60});
    DrawRectangle(cx-24, cy+2,  10, 14, {50,50,50,255});
    DrawRectangle(cx+14, cy+2,  10, 14, {50,50,50,255});
    DrawRectangleLines(cx-24,cy+2, 10,14, {20,20,20,255});
    DrawRectangleLines(cx+14,cy+2, 10,14, {20,20,20,255});
    DrawRectangle(cx-20, cy-8,  40, 22, {90,90,90,255});
    DrawRectangleLines(cx-20,cy-8, 40,22, {40,40,40,255});
    DrawRectangle(cx-16, cy-6,  32, 10, {150,150,150,255});
    DrawRectangleLines(cx-16,cy-6, 32,10, {80,80,80,255});
    DrawCircle(cx, cy-12, 11, {120,120,120,255});
    DrawCircleLines(cx, cy-12, 11, {50,50,50,255});
    DrawRectangle(cx+8, cy-15, 18, 6, {80,80,80,255});
    DrawRectangleLines(cx+8,cy-15, 18,6, {30,30,30,255});
    DrawCircleLines(cx, cy-12, 15, {0,220,220,80});
    DrawCircle(cx, cy-12, 4, {0,220,220,255});
    draw_hp_bar();
}

ShieldedEnemy::ShieldedEnemy(const path_t* p): Enemy(0, 0, 500, 60.0f, 25, 3, "Shielded Enemy", true), stun_timer(0), rot_angle(0) { set_path(p); }

void ShieldedEnemy::take_damage(int d) 
{
    bool had_shield = shielded;
    Enemy::take_damage(d);
    if (had_shield && !shielded) 
    {
        stun_timer = 1.2f;
    }
}

void ShieldedEnemy::update(float dt) {
    rot_angle += dt * 60.0f;
    if (stun_timer > 0) 
    { 
        stun_timer -= dt; 
        return; 
    }
    Enemy::update(dt);
}

void ShieldedEnemy::render() const {
    int cx = (int)x, cy = (int)y;
    DrawRectangle(cx-14, cy+14, 28, 5, {0,0,0,60});
    if (shielded) 
    {
        DrawRectangle(cx-22, cy-18, 12, 28, {200,180,20,255});
        DrawRectangleLines(cx-22,cy-18,12,28, {120,100,0,255});
        DrawCircle(cx-16, cy-4, 4, {240,210,0,255});
    } 
    else 
    {
        DrawRectangleLines(cx-22,cy-18,12,28, {100,80,0,140});
    }
    DrawCircle(cx+4, cy, 14, {180,180,180,255});
    DrawCircleLines(cx+4, cy, 14, {60,60,60,255});
    DrawCircle(cx+4, cy-20, 9, {210,210,210,255});
    DrawCircleLines(cx+4, cy-20, 9, {60,60,60,255});
    DrawCircle(cx+1, cy-21, 2, {240,210,0,255});
    DrawCircle(cx+7, cy-21, 2, {240,210,0,255});
    DrawRectangle(cx-2, cy+12, 5, 10, {120,120,120,255});
    DrawRectangle(cx+8, cy+12, 5, 10, {120,120,120,255});
    if (stun_timer > 0) 
    {
        DrawText("STUN", cx-14, cy-42, 12, YELLOW);
    }
    draw_hp_bar();
}

BasicEnemy::BasicEnemy(const path_t* p): Enemy(0, 0, 300, 110.0f, 20, 5, "Basic Enemy"), spin_angle(0), pulse_timer(0) { set_path(p); }

void BasicEnemy::update(float dt) {
    Enemy::update(dt);
    spin_angle  += dt * 180.0f;
    pulse_timer += dt * 3.0f;
}

void BasicEnemy::render() const {
    int cx = (int)x, cy = (int)y;
    DrawRectangle(cx-12, cy+14, 24, 5, {0,0,0,60});
    DrawRectangle(cx-20, cy-8,  8, 5, {110,110,110,255});
    DrawRectangle(cx+12, cy-8,  8, 5, {110,110,110,255});
    DrawRectangleLines(cx-20,cy-8,8,5, {50,50,50,255});
    DrawRectangleLines(cx+12,cy-8,8,5, {50,50,50,255});
    DrawRectangle(cx-10, cy+8, 7, 10, {100,100,100,255});
    DrawRectangle(cx+3,  cy+8, 7, 10, {100,100,100,255});
    DrawRectangleLines(cx-10,cy+8,7,10, {50,50,50,255});
    DrawRectangleLines(cx+3, cy+8,7,10, {50,50,50,255});
    DrawRectangle(cx-12, cy-10, 24, 20, {160,160,160,255});
    DrawRectangleLines(cx-12,cy-10,24,20, {60,60,60,255});
    DrawRectangle(cx-12, cy+2, 24, 4, {80,80,80,255});
    DrawCircle(cx, cy-20, 10, {200,200,200,255});
    DrawCircleLines(cx, cy-20, 10, {60,60,60,255});
    DrawCircle(cx-4, cy-21, 2, {240,110,0,255});
    DrawCircle(cx+4, cy-21, 2, {240,110,0,255});
    draw_hp_bar();
}