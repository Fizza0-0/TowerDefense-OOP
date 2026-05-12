#pragma once
#include "Entity.h"
#include <SFML/Graphics.hpp>
#include <vector>
#include <string>

// ============================================================
//  Enemy.h  —  Sprite-animated Enemy Base + 3 Types
//  Slime  = BasicEnemy  (folder 1, S_Walk/S_Death)
//  Monster= TankEnemy   (folder 2, S_Walk/S_Death)
//  Bee    = FastEnemy   (folder 4, S_Walk/S_Death)
// ============================================================

using Path = std::vector<sf::Vector2f>;

// ---------------------------------------------------------------
//  SpriteAnim — simple sprite-sheet animator
// ---------------------------------------------------------------
struct SpriteAnim {
    sf::Texture texture;
    sf::Sprite  sprite;
    int   frameCount  = 6;
    int   frameW      = 48;
    int   frameH      = 48;
    int   curFrame    = 0;
    float frameTimer  = 0.f;
    float frameSpeed  = 0.12f; // seconds per frame
    bool  looping     = true;
    bool  done        = false;

    bool load(const std::string& path, int frames, int fw, int fh, float spd) {
        if (!texture.loadFromFile(path)) return false;
        texture.setSmooth(false);
        sprite.setTexture(texture);
        frameCount = frames;
        frameW     = fw;
        frameH     = fh;
        frameSpeed = spd;
        curFrame   = 0;
        frameTimer = 0.f;
        done       = false;
        sprite.setTextureRect(sf::IntRect(0, 0, fw, fh));
        return true;
    }

    void update(float dt) {
        if (done && !looping) return;
        frameTimer += dt;
        if (frameTimer >= frameSpeed) {
            frameTimer = 0.f;
            curFrame++;
            if (curFrame >= frameCount) {
                if (looping) curFrame = 0;
                else { curFrame = frameCount - 1; done = true; }
            }
            sprite.setTextureRect(sf::IntRect(curFrame * frameW, 0, frameW, frameH));
        }
    }

    void reset(bool loop) {
        looping    = loop;
        curFrame   = 0;
        frameTimer = 0.f;
        done       = false;
        sprite.setTextureRect(sf::IntRect(0, 0, frameW, frameH));
    }
};

// ---------------------------------------------------------------
//  ABSTRACT BASE CLASS: Enemy
// ---------------------------------------------------------------
class Enemy : public Entity {
protected:
    float x, y;
    int   hp;
    int   maxHp;
    float speed;
    int   goldReward;
    int   pathIndex;
    const Path* path;

    // Sprite animations
    SpriteAnim walkAnim;
    SpriteAnim deathAnim;
    bool       dying     = false;
    float      deathTimer = 0.f;
    float      scale     = 1.f;

    // HP bar shapes (kept as primitives)
    sf::RectangleShape hpBarBg;
    sf::RectangleShape hpBarFg;

    bool advanceAlongPath(float dt);

public:
    Enemy(float startX, float startY, int hp, float speed, int gold, const Path* p);
    virtual ~Enemy() {}

    virtual void update(float dt) override = 0;
    virtual void render()         override = 0;
    virtual void move(float dt)            = 0;
    virtual void takeDamage(int dmg)       = 0;

    bool  isDead()        const { return hp <= 0 && (!dying || deathAnim.done); }
    bool  isHpZero()      const { return hp <= 0; }
    bool  reachedEnd()    const { return pathIndex >= (int)path->size(); }
    int   getGoldReward() const { return goldReward; }
    float getX()          const { return x; }
    float getY()          const { return y; }
    float distanceTo(float ex, float ey) const;
    sf::Vector2f getPosition() const { return { x, y }; }

    void setRenderTarget(sf::RenderWindow* win) { window = win; }

protected:
    sf::RenderWindow* window = nullptr;
    void drawHpBar();
    void drawSprite(SpriteAnim& anim);
};

// ---------------------------------------------------------------
//  BasicEnemy  —  Slime  (folder 1, medium hp/speed)
// ---------------------------------------------------------------
class BasicEnemy : public Enemy {
public:
    BasicEnemy(float sx, float sy, const Path* p);
    void update(float dt)    override;
    void render()            override;
    void move(float dt)      override;
    void takeDamage(int dmg) override;
};

// ---------------------------------------------------------------
//  FastEnemy  —  Bee  (folder 4, low hp, very fast)
// ---------------------------------------------------------------
class FastEnemy : public Enemy {
public:
    FastEnemy(float sx, float sy, const Path* p);
    void update(float dt)    override;
    void render()            override;
    void move(float dt)      override;
    void takeDamage(int dmg) override;
};

// ---------------------------------------------------------------
//  TankEnemy  —  Monster (folder 2, very high hp, slow)
// ---------------------------------------------------------------
class TankEnemy : public Enemy {
public:
    TankEnemy(float sx, float sy, const Path* p);
    void update(float dt)    override;
    void render()            override;
    void move(float dt)      override;
    void takeDamage(int dmg) override;
};
