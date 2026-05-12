#include "../include/Tower.h"
#include <cmath>
#include <limits>
#include <algorithm>

// ============================================================
//  Tower Base
// ============================================================

Tower::Tower(float x, float y, int damage, float range, float fireRate)
    : x(x), y(y), damage(damage), range(range),
      fireRate(fireRate), fireCooldown(0.f)
{
    rangeCircle.setRadius(range);
    rangeCircle.setOrigin(range, range);
    rangeCircle.setFillColor(sf::Color(255, 255, 200, 18));
    rangeCircle.setOutlineColor(sf::Color(255, 255, 150, 60));
    rangeCircle.setOutlineThickness(1.f);
    rangeCircle.setPosition(x, y);

    projShape.setRadius(4.f);
    projShape.setOrigin(4.f, 4.f);
}

bool Tower::loadBase(const std::string& path, float scaleX, float scaleY) {
    if (!baseTex.loadFromFile(path)) return false;
    baseTex.setSmooth(false);
    baseSprite.setTexture(baseTex);
    // Use first frame (70x130) for the base tower graphic
    baseSprite.setTextureRect(sf::IntRect(0, 0, 70, 130));
    baseSprite.setScale(scaleX, scaleY);
    baseSprite.setOrigin(35.f, 115.f); // anchor near base bottom
    baseSprite.setPosition(x, y);
    return true;
}

bool Tower::loadArcher(const std::string& idlePath, const std::string& atkPath,
                       int iFrames, int aFrames) {
    if (!idleTex.loadFromFile(idlePath)) return false;
    if (!attackTex.loadFromFile(atkPath)) return false;
    idleTex.setSmooth(false);
    attackTex.setSmooth(false);
    idleSprite.setTexture(idleTex);
    attackSprite.setTexture(attackTex);
    idleFrames   = iFrames;
    attackFrames = aFrames;
    idleSprite.setTextureRect(sf::IntRect(0, 0, archerFrameW, archerFrameH));
    attackSprite.setTextureRect(sf::IntRect(0, 0, archerFrameW, archerFrameH));
    idleSprite.setOrigin(archerFrameW / 2.f, archerFrameH / 2.f);
    attackSprite.setOrigin(archerFrameW / 2.f, archerFrameH / 2.f);
    return true;
}

Enemy* Tower::findTarget(std::vector<Enemy*>& enemies) {
    Enemy* best  = nullptr;
    float  minD  = std::numeric_limits<float>::max();
    int    maxPI = -1;

    for (Enemy* e : enemies) {
        if (!e || e->isHpZero()) continue;
        float dx   = e->getX() - x;
        float dy   = e->getY() - y;
        float dist = std::sqrt(dx * dx + dy * dy);
        // Target furthest along path (highest pathIndex) within range
        if (dist <= range) {
            // prioritise by path progress for more tactical feel
            if (best == nullptr || dist < minD) {
                minD  = dist;
                best  = e;
            }
        }
    }
    return best;
}

void Tower::triggerAttackAnim() {
    atkCurFrame = 0;
    atkTimer    = 0.f;
}

void Tower::fireProjectile(Enemy* target, sf::Color col) {
    if (!target) return;
    Projectile p;
    p.pos   = { x, y - 30.f };
    float dx = target->getX() - p.pos.x;
    float dy = target->getY() - p.pos.y;
    float d  = std::sqrt(dx * dx + dy * dy);
    if (d > 0.f) { p.dir = { dx / d, dy / d }; }
    p.color   = col;
    p.lifetime = std::min(0.5f, d / p.speed + 0.1f);
    projectiles.push_back(p);
}

void Tower::updateAnimations(float dt) {
    // Idle animation
    idleTimer += dt;
    float idleSpd = 0.15f;
    if (idleTimer >= idleSpd) {
        idleTimer = 0.f;
        idleCurFrame = (idleCurFrame + 1) % idleFrames;
        idleSprite.setTextureRect(sf::IntRect(idleCurFrame * archerFrameW, 0, archerFrameW, archerFrameH));
    }

    // Attack animation (plays once, then goes back to idle)
    if (atkCurFrame >= 0) {
        atkTimer += dt;
        float atkSpd = atkAnimDur / attackFrames;
        if (atkTimer >= atkSpd) {
            atkTimer = 0.f;
            atkCurFrame++;
            if (atkCurFrame >= attackFrames) {
                atkCurFrame = -1; // done
            } else {
                attackSprite.setTextureRect(sf::IntRect(atkCurFrame * archerFrameW, 0, archerFrameW, archerFrameH));
            }
        }
    }
}

void Tower::updateProjectiles(float dt) {
    for (auto& p : projectiles) {
        p.pos.x   += p.dir.x * p.speed * dt;
        p.pos.y   += p.dir.y * p.speed * dt;
        p.lifetime -= dt;
    }
    projectiles.erase(
        std::remove_if(projectiles.begin(), projectiles.end(),
            [](const Projectile& p) { return p.lifetime <= 0.f; }),
        projectiles.end());
}

void Tower::renderBase() {
    if (!window) return;
    window->draw(baseSprite);
}

void Tower::renderArcher() {
    if (!window) return;
    sf::Sprite& s = (atkCurFrame >= 0) ? attackSprite : idleSprite;
    s.setPosition(x, y - 55.f); // sit on top of tower base
    window->draw(s);
}

void Tower::renderProjectiles() {
    if (!window) return;
    for (auto& p : projectiles) {
        projShape.setFillColor(p.color);
        projShape.setPosition(p.pos);
        window->draw(projShape);
    }
}

void Tower::renderRange() {
    if (!window) return;
    window->draw(rangeCircle);
}

// ============================================================
//  CannonTower — Archer Tier 1, green arrows
// ============================================================

CannonTower::CannonTower(float x, float y)
    : Tower(x, y, 28, 125.f, 1.0f)
{
    loadBase("assets/towers/tower_idle1.png", 1.2f, 1.2f);
    loadArcher("assets/towers/archer1_idle.png",
               "assets/towers/archer1_attack.png", 4, 6);
    atkAnimDur = 0.45f;
}

void CannonTower::update(float dt) {
    if (fireCooldown > 0.f) fireCooldown -= dt;
    updateAnimations(dt);
    updateProjectiles(dt);
}

void CannonTower::render() {
    renderRange();
    renderBase();
    renderArcher();
    renderProjectiles();
}

void CannonTower::attack(std::vector<Enemy*>& enemies) {
    if (fireCooldown > 0.f) return;
    Enemy* target = findTarget(enemies);
    if (!target) return;
    target->takeDamage(damage);
    fireCooldown = 1.f / fireRate;
    triggerAttackAnim();
    fireProjectile(target, sf::Color(100, 220, 80));
}

// ============================================================
//  MachineGunTower — Archer Tier 2, rapid yellow arrows
// ============================================================

MachineGunTower::MachineGunTower(float x, float y)
    : Tower(x, y, 9, 85.f, 5.0f)
{
    loadBase("assets/towers/tower_idle2.png", 0.5f, 1.2f);
    loadArcher("assets/towers/archer2_idle.png",
               "assets/towers/archer2_attack.png", 4, 6);
    atkAnimDur = 0.18f;
}

void MachineGunTower::update(float dt) {
    if (fireCooldown > 0.f) fireCooldown -= dt;
    updateAnimations(dt);
    updateProjectiles(dt);
}

void MachineGunTower::render() {
    renderRange();
    renderBase();
    renderArcher();
    renderProjectiles();
}

void MachineGunTower::attack(std::vector<Enemy*>& enemies) {
    if (fireCooldown > 0.f) return;
    Enemy* target = findTarget(enemies);
    if (!target) return;
    target->takeDamage(damage);
    fireCooldown = 1.f / fireRate;
    triggerAttackAnim();
    fireProjectile(target, sf::Color(255, 220, 50));
}

// ============================================================
//  SniperTower — Archer Tier 3, long-range blue arrows
// ============================================================

SniperTower::SniperTower(float x, float y)
    : Tower(x, y, 90, 260.f, 0.4f)
{
    loadBase("assets/towers/tower_idle3.png", 0.5f, 1.2f);
    loadArcher("assets/towers/archer3_idle.png",
               "assets/towers/archer3_attack.png", 4, 6);
    atkAnimDur = 0.6f;

    // Override range circle style
    rangeCircle.setRadius(range);
    rangeCircle.setOrigin(range, range);
    rangeCircle.setFillColor(sf::Color(100, 150, 255, 12));
    rangeCircle.setOutlineColor(sf::Color(100, 180, 255, 70));
    rangeCircle.setOutlineThickness(1.f);
    rangeCircle.setPosition(x, y);
}

void SniperTower::update(float dt) {
    if (fireCooldown > 0.f) fireCooldown -= dt;
    updateAnimations(dt);
    updateProjectiles(dt);
}

void SniperTower::render() {
    renderRange();
    renderBase();
    renderArcher();
    renderProjectiles();
}

void SniperTower::attack(std::vector<Enemy*>& enemies) {
    if (fireCooldown > 0.f) return;
    Enemy* target = findTarget(enemies);
    if (!target) return;
    target->takeDamage(damage);
    fireCooldown = 1.f / fireRate;
    triggerAttackAnim();
    fireProjectile(target, sf::Color(120, 180, 255));
}
