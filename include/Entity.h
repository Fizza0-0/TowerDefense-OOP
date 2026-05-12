#pragma once

// ============================================================
//  Entity.h  —  Abstract Base Class
// ============================================================

class Entity {
public:
    virtual void update(float dt) = 0;
    virtual void render() = 0;
    virtual ~Entity() {}
};
