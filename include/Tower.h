#pragma once
#include "Entity.h"
#include "Enemy.h"
#include <SFML/Graphics.hpp>
#include <vector>
#include <string>

// ============================================================
//  Tower.h  —  Sprite-animated Tower Base + 3 Types
//  Uses archer towers sprite pack (3 upgrade tiers)
// ============================================================

// Simple projectile for visual feedback
struct Projectile {
    sf::Vector2f pos;
    sf::Vector2f dir;
    float speed   = 400.f;
    float lifetime = 0.4f;
    sf::Color color;
    float size   = 4.f;
};

// ---------------------------------------------------------------
//  ABSTRACT BASE CLASS: Tower
// ---------------------------------------------------------------
class Tower : public Entity {
protected:
    float x, y;
    int   damage;
    float range;
    float fireRate;
    float fireCooldown;

    // Sprite (tower base)
    sf::Texture baseTex;
    sf::Sprite  baseSprite;

    // Archer unit sprite animation
    sf::Texture idleTex;
    sf::Sprite  idleSprite;
    sf::Texture attackTex;
    sf::Sprite  attackSprite;

    int   idleFrames   = 4;
    int   attackFrames = 6;
    int   archerFrameW = 48;
    int   archerFrameH = 48;
    int   idleCurFrame = 0;
    int   atkCurFrame  = -1; // -1 = not attacking
    float idleTimer    = 0.f;
    float atkTimer     = 0.f;
    float atkAnimDur   = 0.5f;

    // Range circle
    sf::CircleShape rangeCircle;

    // Projectiles
    std::vector<Projectile> projectiles;
    sf::CircleShape projShape;

    Enemy* findTarget(std::vector<Enemy*>& enemies);

    bool loadBase(const std::string& path, float scaleX, float scaleY);
    bool loadArcher(const std::string& idlePath, const std::string& atkPath,
                    int iFrames, int aFrames);

public:
    Tower(float x, float y, int damage, float range, float fireRate);
    virtual ~Tower() {}

    virtual void update(float dt) override = 0;
    virtual void render()         override = 0;
    virtual void attack(std::vector<Enemy*>& enemies) = 0;

    float getX() const { return x; }
    float getY() const { return y; }
    int   getDamage() const { return damage; }
    float getRange()  const { return range; }
    float getFireRate() const { return fireRate; }

    void setRenderTarget(sf::RenderWindow* win) { window = win; }

protected:
    sf::RenderWindow* window = nullptr;
    void updateAnimations(float dt);
    void updateProjectiles(float dt);
    void renderBase();
    void renderArcher();
    void renderProjectiles();
    void renderRange();
    void fireProjectile(Enemy* target, sf::Color col);
    void triggerAttackAnim();
};

// ---------------------------------------------------------------
//  CannonTower  — archer tier 1 (medium dmg, medium range)
// ---------------------------------------------------------------
class CannonTower : public Tower {
public:
    CannonTower(float x, float y);
    void update(float dt)                     override;
    void render()                             override;
    void attack(std::vector<Enemy*>& enemies) override;
};

// ---------------------------------------------------------------
//  MachineGunTower — archer tier 2 (low dmg, fast fire)
// ---------------------------------------------------------------
class MachineGunTower : public Tower {
public:
    MachineGunTower(float x, float y);
    void update(float dt)                     override;
    void render()                             override;
    void attack(std::vector<Enemy*>& enemies) override;
};

// ---------------------------------------------------------------
//  SniperTower — archer tier 3 (high dmg, long range)
// ---------------------------------------------------------------
class SniperTower : public Tower {
public:
    SniperTower(float x, float y);
    void update(float dt)                     override;
    void render()                             override;
    void attack(std::vector<Enemy*>& enemies) override;
};
