#pragma once
#include "raylib.h"
#include "entity.h"
#include "enemy.h"
#include "constants.h"
#include <string>

struct Projectile {
    float x, y, tx, ty, spd;
    int dmg;
    Color col;
    bool active;
    float radius;
    bool is_bomb;
    Enemy* target;

    Projectile() {}
    Projectile(float x, float y, Enemy* t, float spd, int dmg, Color c, bool bomb = false, float r = 0);
    void update(float dt);
    void render() const;
};


class Tower : public Entity {
protected:
    float range, fire_rate, fire_timer;
    int damage, cost, level, upgrade_cost;
    std::string type_tag;
    Color body_col, accent_col;

    Enemy* find_closest(Enemy** enemies, int count);
    bool ready_to_fire() 
    { 
        return fire_timer <= 0; 
    }
    void reset_timer()   
    { 
        fire_timer = 1.0f / fire_rate; }

public:
    Tower(float x, float y, int hp, float rng, float fr, int dmg, int cost, const std::string& n, const std::string& tag, Color bc, Color ac);
    virtual ~Tower() {}

    virtual void update(float dt) override;
    virtual void render() const override;
    virtual int try_shoot(Enemy** enemies, int n, Projectile** out, int out_max) = 0;
    virtual std::string get_description() const = 0;

    bool  can_upgrade() const { return level < 2; }
    int   get_upgrade_cost() const { return upgrade_cost * (level + 1); }
    void  upgrade();
    float get_range() const { return range; }
    int   get_cost() const { return cost; }
    int   get_damage() const { return damage; }
    float get_fire_rate() const { return fire_rate; }
    int   get_level() const { return level; }
    std::string get_tag() const { return type_tag; }
};


class cannon_tower : public Tower {
public:
    cannon_tower(float x, float y);
    int try_shoot(Enemy** e, int n, Projectile** out, int out_max) override;
    void render() const override;
    std::string get_description() const override;
};

class sniper_tower : public Tower {
public:
    sniper_tower(float x, float y);
    int try_shoot(Enemy** e, int n, Projectile** out, int out_max) override;
    void render() const override;
    std::string get_description() const override;
};

class machinegun_tower : public Tower {
public:
    machinegun_tower(float x, float y);
    int try_shoot(Enemy** e, int n, Projectile** out, int out_max) override;
    void render() const override;
    std::string get_description() const override;
};

class slow_tower : public Tower {
    float slow_mult;
public:
    slow_tower(float x, float y);
    int try_shoot(Enemy** e, int n, Projectile** out, int out_max) override;
    void render() const override;
    std::string get_description() const override;
};

class bomb_tower : public Tower {
public:
    bomb_tower(float x, float y);
    int try_shoot(Enemy** e, int n, Projectile** out, int out_max) override;
    void render() const override;
    std::string get_description() const override;
};

Tower* build_tower(int type, float x, float y);

extern const char* tower_names[5];
extern const int tower_costs[5];
extern const Color tower_cols[5];
extern const char* tower_descs[5];
extern const int tower_dmg[5];
extern const int tower_range[5];