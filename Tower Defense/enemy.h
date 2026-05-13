#pragma once
#include "raylib.h"
#include "entity.h"
#include "map.h"

class Enemy : public Entity {
protected:
    float speed, base_speed;
    int reward, dmg;
    bool reached_end;
    float slow_timer, slow_mult;
    int wp_idx;
    const path_t* path;
    bool shielded;
public:
    Enemy(float x, float y, int hp, float sp, int rw, int dmg,
          const std::string& n, bool shield = false): Entity(x, y, hp, n), speed(sp), base_speed(sp), reward(rw), dmg(dmg),
          reached_end(false), slow_timer(0), slow_mult(1.0f),
          wp_idx(1), path(nullptr), shielded(shield) {}
    virtual ~Enemy() {}

    void set_path(const path_t* p);
    void apply_slow(float mult, float dur);
    virtual void take_damage(int d);
    virtual void update(float dt) override;
    virtual void render() const override = 0;

    bool has_reached_end() const { return reached_end; }
    int  get_reward() const { return reward; }
    int  get_damage() const { return dmg; }
    bool is_shielded() const { return shielded; }
    int  get_wp_idx() const { return wp_idx; }

    void draw_hp_bar() const;
};

class FastEnemy : public Enemy {
    float weave_timer;
public:
    FastEnemy(const path_t* p);
    void update(float dt) override;
    void render() const override;
};

class TankEnemy : public Enemy {
    float regen_timer;
public:
    TankEnemy(const path_t* p);
    void update(float dt) override;
    void render() const override;
};

class ShieldedEnemy : public Enemy {
    float stun_timer;
    float rot_angle;
public:
    ShieldedEnemy(const path_t* p);
    void take_damage(int d) override;
    void update(float dt) override;
    void render() const override;
};

class BasicEnemy : public Enemy {
    float spin_angle;
    float pulse_timer;
public:
    BasicEnemy(const path_t* p);
    void update(float dt) override;
    void render() const override;
};