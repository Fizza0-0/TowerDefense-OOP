#include "../include/Enemy.h"
#include <cmath>
#include <algorithm>

// ============================================================
//  Enemy Base
// ============================================================

Enemy::Enemy(float startX, float startY, int hp, float speed, int gold, const Path* p)
    : x(startX), y(startY), hp(hp), maxHp(hp),
      speed(speed), goldReward(gold), pathIndex(0), path(p)
{
    hpBarBg.setSize({ 36.f, 5.f });
    hpBarBg.setFillColor(sf::Color(60, 0, 0));

    hpBarFg.setSize({ 36.f, 5.f });
    hpBarFg.setFillColor(sf::Color(50, 220, 50));
}

bool Enemy::advanceAlongPath(float dt) {
    if (pathIndex >= (int)path->size()) return true;

    sf::Vector2f target = (*path)[pathIndex];
    float dx = target.x - x;
    float dy = target.y - y;
    float dist = std::sqrt(dx * dx + dy * dy);

    if (dist < 2.f) {
        x = target.x;
        y = target.y;
        pathIndex++;
        if (pathIndex >= (int)path->size()) return true;
    } else {
        float nx = dx / dist;
        float ny = dy / dist;
        x += nx * speed * dt;
        y += ny * speed * dt;
    }
    return false;
}

float Enemy::distanceTo(float ex, float ey) const {
    float dx = x - ex, dy = y - ey;
    return std::sqrt(dx * dx + dy * dy);
}

void Enemy::drawHpBar() {
    if (!window) return;
    float barW = 36.f;
    float barX = x - barW / 2.f;
    float barY = y - (walkAnim.frameH * scale / 2.f) - 8.f;

    hpBarBg.setPosition(barX, barY);
    window->draw(hpBarBg);

    float ratio = std::max(0.f, (float)hp / maxHp);
    hpBarFg.setSize({ barW * ratio, 5.f });
    sf::Color hpCol = (ratio > 0.5f) ? sf::Color(50, 220, 50)
                    : (ratio > 0.25f)? sf::Color(230, 180, 30)
                                     : sf::Color(220, 50, 50);
    hpBarFg.setFillColor(hpCol);
    hpBarFg.setPosition(barX, barY);
    window->draw(hpBarFg);
}

void Enemy::drawSprite(SpriteAnim& anim) {
    if (!window) return;
    anim.sprite.setScale(scale, scale);
    anim.sprite.setOrigin(anim.frameW / 2.f, anim.frameH / 2.f);
    anim.sprite.setPosition(x, y);
    window->draw(anim.sprite);
}

// ============================================================
//  BasicEnemy — Slime (folder 1)
//  Spritesheet: 288x48 = 6 frames of 48x48
// ============================================================

BasicEnemy::BasicEnemy(float sx, float sy, const Path* p)
    : Enemy(sx, sy, 100, 75.f, 12, p)
{
    scale = 1.1f;
    walkAnim.load("assets/enemies/slime_walk.png",  6, 48, 48, 0.13f);
    deathAnim.load("assets/enemies/slime_death.png", 6, 48, 48, 0.12f);
    walkAnim.reset(true);
    deathAnim.reset(false);
}

void BasicEnemy::update(float dt) {
    if (dying) {
        deathAnim.update(dt);
        deathTimer += dt;
    } else {
        if (hp <= 0) {
            dying = true;
            deathAnim.reset(false);
        } else {
            move(dt);
            walkAnim.update(dt);
        }
    }
}

void BasicEnemy::render() {
    if (!window) return;
    if (dying) {
        drawSprite(deathAnim);
    } else {
        drawSprite(walkAnim);
        drawHpBar();
    }
}

void BasicEnemy::move(float dt)      { advanceAlongPath(dt); }
void BasicEnemy::takeDamage(int dmg) { if (!dying) hp -= dmg; }

// ============================================================
//  FastEnemy — Bee (folder 4)
//  Spritesheet: 288x48 = 6 frames of 48x48
// ============================================================

FastEnemy::FastEnemy(float sx, float sy, const Path* p)
    : Enemy(sx, sy, 45, 190.f, 18, p)
{
    scale = 0.85f;
    walkAnim.load("assets/enemies/bee_walk.png",  6, 48, 48, 0.09f);
    deathAnim.load("assets/enemies/bee_death.png", 6, 48, 48, 0.09f);
    walkAnim.reset(true);
    deathAnim.reset(false);
}

void FastEnemy::update(float dt) {
    if (dying) {
        deathAnim.update(dt);
        deathTimer += dt;
    } else {
        if (hp <= 0) {
            dying = true;
            deathAnim.reset(false);
        } else {
            move(dt);
            walkAnim.update(dt);
        }
    }
}

void FastEnemy::render() {
    if (!window) return;
    if (dying) {
        drawSprite(deathAnim);
    } else {
        drawSprite(walkAnim);
        drawHpBar();
    }
}

void FastEnemy::move(float dt)      { advanceAlongPath(dt); }
void FastEnemy::takeDamage(int dmg) { if (!dying) hp -= dmg; }

// ============================================================
//  TankEnemy — Monster (folder 2)
//  Spritesheet: 288x48 = 6 frames of 48x48
// ============================================================

TankEnemy::TankEnemy(float sx, float sy, const Path* p)
    : Enemy(sx, sy, 350, 38.f, 55, p)
{
    scale = 1.4f;
    walkAnim.load("assets/enemies/monster_walk.png",  6, 48, 48, 0.15f);
    deathAnim.load("assets/enemies/monster_death.png", 6, 48, 48, 0.14f);
    walkAnim.reset(true);
    deathAnim.reset(false);
}

void TankEnemy::update(float dt) {
    if (dying) {
        deathAnim.update(dt);
        deathTimer += dt;
    } else {
        if (hp <= 0) {
            dying = true;
            deathAnim.reset(false);
        } else {
            move(dt);
            walkAnim.update(dt);
        }
    }
}

void TankEnemy::render() {
    if (!window) return;
    if (dying) {
        drawSprite(deathAnim);
    } else {
        drawSprite(walkAnim);
        drawHpBar();
    }
}

void TankEnemy::move(float dt)      { advanceAlongPath(dt); }
void TankEnemy::takeDamage(int dmg) { if (!dying) hp -= dmg; }
