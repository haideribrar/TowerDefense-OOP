#pragma once
#include "raylib.h"
#include <string>

class Entity {
protected:
    float x, y;
    int hp, max_hp;
    bool alive;
    std::string name;
public:
    Entity(float x, float y, int hp, const std::string& n)
        : x(x), y(y), hp(hp), max_hp(hp), alive(true), name(n) {}
    virtual ~Entity() {}

    virtual void update(float dt) = 0;
    virtual void render() const = 0;

    bool operator<(const Entity& o) const { return hp < o.hp; }
    bool operator==(const Entity& o) const { return hp == o.hp; }

    float get_x() const { return x; }
    float get_y() const { return y; }
    int   get_hp() const { return hp; }
    int   get_max_hp() const { return max_hp; }
    bool  is_alive() const { return alive; }
    void  set_alive(bool a) { alive = a; }
    std::string get_name() const { return name; }
};