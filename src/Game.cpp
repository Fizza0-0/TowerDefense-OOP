#include "../include/Game.h"
#include <algorithm>
#include <string>
#include <sstream>
#include <cstdlib>
#include <ctime>
#include <cmath>

Game::Game()
    : window(sf::VideoMode(900, 650), "Tower Defense",
        sf::Style::Titlebar | sf::Style::Close)
{
    window.setFramerateLimit(60);
    srand((unsigned)time(nullptr));

    if (!font.loadFromFile("C:/Windows/Fonts/arial.ttf"))
        font.loadFromFile("arial.ttf");

    loadAssets();
    buildPath_coords();
    buildDecor();
    buildMapFires();
}

Game::~Game() {
    for (Enemy* e : enemies) delete e;
    for (Tower* t : towers)  delete t;
}

void Game::loadAssets() {
    grassTileTex.loadFromFile("assets/tiles/grass.png");
    grassTileTex.setSmooth(false);
    grassTileSprite.setTexture(grassTileTex);

    towerSpotTex.loadFromFile("assets/tiles/tower_spot.png");
    towerSpotTex.setSmooth(false);
    towerSpotSprite.setTexture(towerSpotTex);
    towerSpotSprite.setOrigin(towerSpotTex.getSize().x / 2.f,
        towerSpotTex.getSize().y / 2.f);

    treeTex1.loadFromFile("assets/tiles/tree1.png"); treeTex1.setSmooth(false);
    treeTex2.loadFromFile("assets/tiles/tree2.png"); treeTex2.setSmooth(false);
    bushTex.loadFromFile("assets/tiles/bush.png");   bushTex.setSmooth(false);

    // Campfire sheet: 192x64 = 3 frames of 64x64
    campfireSheetTex.loadFromFile("assets/tiles/campfire_sheet.png");
    campfireSheetTex.setSmooth(false);

    // Flag sheet: 192x64 = 3 frames of 64x64
    flagSheetTex.loadFromFile("assets/tiles/flag_sheet.png");
    flagSheetTex.setSmooth(false);

    // Two exit flags on either side of the exit point
    flagLeft = { 858.f, 400.f };
    flagRight = { 858.f, 480.f };

    mainMenuTex.loadFromFile("assets/ui/main_menu.png");       mainMenuTex.setSmooth(false);
    winLooseTex.loadFromFile("assets/ui/win_loose.png");       winLooseTex.setSmooth(false);
    actionPanelTex.loadFromFile("assets/ui/action_panel.png"); actionPanelTex.setSmooth(false);
}

void Game::buildPath_coords() {
    path.clear();
    path.push_back({ -30.f, 145.f });
    path.push_back({ 180.f, 145.f });
    path.push_back({ 180.f, 360.f });
    path.push_back({ 430.f, 360.f });
    path.push_back({ 430.f, 170.f });
    path.push_back({ 680.f, 170.f });
    path.push_back({ 680.f, 430.f });
    path.push_back({ 930.f, 430.f });
}

void Game::buildPath() { buildPath_coords(); }

void Game::buildDecor() {
    decors.clear();

    // type 0=tree1(white circles), type 1=tree2(green circles), type 2=bush(red circles)
    struct DecPos { float x, y; int type; };
    std::vector<DecPos> decs = {
        // Top-left area
          {40, 40, 0},
          {110, 70, 2},
          {190, 45, 1},
          // Top-middle
          {340, 35, 0},
          {430, 65, 2},
          {540, 40, 1},
          // Top-right
          {700, 45, 0},
          {790, 70, 2},
          {860, 120, 1},
          // Right side
          {870, 240, 0},
          {835, 340, 2},
          {780, 430, 1},
          // Bottom-right
          {650, 500, 0},
          {540, 525, 2},
          // Bottom-middle
          {400, 460, 1},
          {300, 435, 0},
          // Bottom-left
          {180, 510, 2},
          {80, 470, 1},
          // Left side
          {45, 350, 0},
          {70, 240, 2},
          {120, 170, 1},
          // Center fillers (light decoration only)
          {320, 260, 2},
          {560, 300, 1},
    
    };

    for (auto& d : decs) {
        if (isOnPath(d.x, d.y, 50.f)) continue;
        MapDecor md;
        sf::Texture* tex = (d.type == 0) ? &treeTex1
            : (d.type == 1) ? &treeTex2 : &bushTex;
        md.sprite.setTexture(*tex);
        md.sprite.setScale(2.f, 2.f);
        md.sprite.setOrigin(tex->getSize().x / 2.f, tex->getSize().y / 2.f);
        md.x = d.x; md.y = d.y;
        md.sprite.setPosition(d.x, d.y);
        decors.push_back(md);
    }
}

void Game::buildMapFires() {
    mapFires.clear();

    // YELLOW circles from screenshot
    std::vector<sf::Vector2f> pos = {
        // Upper area
       {250.f, 85.f},
       {545.f, 75.f},
       {760.f, 105.f},

       // Mid-left open ground
       {285.f, 235.f},

       // Mid-right open ground
       {690.f, 245.f},

       // Center-bottom open areas
       {470.f, 455.f},
       {800.f, 305.f},

       // Lower-left open ground
       {210.f, 500.f},
    };

    for (auto& p : pos) {
        if (isOnPath(p.x, p.y, 52.f)) continue;
        MapFire f;
        f.pos = p;
        f.frame = rand() % 3;
        f.timer = (rand() % 100) / 100.f;
        f.speed = 0.10f + (rand() % 5) * 0.02f;
        f.scale = 1.3f + (rand() % 4) * 0.15f;
        mapFires.push_back(f);
    }
}


void Game::resetGame() {
    for (Enemy* e : enemies) delete e;
    for (Tower* t : towers)  delete t;
    enemies.clear();
    towers.clear();
    towerSpots.clear();

    gold = 150;
    lives = 20;
    wave = 0;
    score = 0;
    gameOver = false;
    victory = false;
    waveInProgress = false;
    spawnTimer = 0.f;
    enemiesSpawnedThisWave = 0;
    enemiesToSpawn = 0;
    selectedTower = TowerType::Cannon;

    buildPath_coords();
    buildMapFires();
}

void Game::run() {
    while (window.isOpen()) {
        float dt = clock.restart().asSeconds();
        if (dt > 0.1f) dt = 0.1f;

        handleEvents();

        if (screen == GameScreen::Playing && !gameOver && !victory)
            update(dt);

        render();
    }
}

void Game::handleEvents() {
    sf::Event event;
    while (window.pollEvent(event)) {
        if (event.type == sf::Event::Closed) window.close();

        if (screen == GameScreen::MainMenu) handleMenuEvents(event);
        else if (screen == GameScreen::Playing)  handlePlayEvents(event);
        else                                     handleEndEvents(event);
    }
}

void Game::handleMenuEvents(const sf::Event& e) {
    if (e.type == sf::Event::MouseButtonPressed &&
        e.mouseButton.button == sf::Mouse::Left) {
        float mx = (float)e.mouseButton.x;
        float my = (float)e.mouseButton.y;
        if (menuPlayRect.contains(mx, my)) {
            resetGame();
            screen = GameScreen::Playing;
        }
    }
    if (e.type == sf::Event::KeyPressed &&
        e.key.code == sf::Keyboard::Return) {
        resetGame();
        screen = GameScreen::Playing;
    }
}

void Game::handlePlayEvents(const sf::Event& e) {
    if (e.type == sf::Event::KeyPressed) {
        if (e.key.code == sf::Keyboard::Num1) selectedTower = TowerType::Cannon;
        if (e.key.code == sf::Keyboard::Num2) selectedTower = TowerType::MachineGun;
        if (e.key.code == sf::Keyboard::Num3) selectedTower = TowerType::Sniper;
        if (e.key.code == sf::Keyboard::Space && !waveInProgress && wave < maxWaves)
            startWave(wave + 1);
        if (e.key.code == sf::Keyboard::Escape)
            screen = GameScreen::MainMenu;
    }

    if (e.type == sf::Event::MouseButtonPressed &&
        e.mouseButton.button == sf::Mouse::Left) {
        float mx = (float)e.mouseButton.x;
        float my = (float)e.mouseButton.y;

        if (btn1Rect.contains(mx, my)) { selectedTower = TowerType::Cannon;     return; }
        if (btn2Rect.contains(mx, my)) { selectedTower = TowerType::MachineGun; return; }
        if (btn3Rect.contains(mx, my)) { selectedTower = TowerType::Sniper;     return; }
        if (startWaveRect.contains(mx, my) && !waveInProgress && wave < maxWaves) {
            startWave(wave + 1); return;
        }

        if (my < 550.f && !isOnPath(mx, my)) {
            if (gold >= towerCost(selectedTower)) {
                placeTower(mx, my);
                towerSpots.push_back({ mx, my });
            }
        }
    }
}

void Game::handleEndEvents(const sf::Event& e) {
    if (e.type == sf::Event::MouseButtonPressed &&
        e.mouseButton.button == sf::Mouse::Left) {
        float mx = (float)e.mouseButton.x;
        float my = (float)e.mouseButton.y;
        if (endRestartRect.contains(mx, my)) { resetGame(); screen = GameScreen::Playing; }
        if (endQuitRect.contains(mx, my)) { screen = GameScreen::MainMenu; }
    }
    if (e.type == sf::Event::KeyPressed) {
        if (e.key.code == sf::Keyboard::R) { resetGame(); screen = GameScreen::Playing; }
        if (e.key.code == sf::Keyboard::Escape) { screen = GameScreen::MainMenu; }
    }
}

void Game::update(float dt) {
    // Animate exit flags
    flagTimer += dt;
    if (flagTimer >= 0.15f) {
        flagTimer = 0.f;
        flagFrame = (flagFrame + 1) % 3;
    }

    // Animate scattered fires
    for (auto& f : mapFires) {
        f.timer += dt;
        if (f.timer >= f.speed) {
            f.timer = 0.f;
            f.frame = (f.frame + 1) % 3;
        }
    }

    // Spawn enemies
    if (waveInProgress && enemiesSpawnedThisWave < enemiesToSpawn) {
        spawnTimer += dt;
        if (spawnTimer >= spawnInterval) {
            spawnTimer = 0.f;
            spawnEnemy(wave);
            enemiesSpawnedThisWave++;
        }
    }

    for (Tower* t : towers) {
        t->update(dt);
        t->attack(enemies);
    }

    for (Enemy* e : enemies) {
        e->update(dt);
        if (!e->isHpZero() && e->reachedEnd()) {
            lives--;
            score = std::max(0, score - 5);
            e->takeDamage(99999);
        }
    }

    auto rm = std::remove_if(enemies.begin(), enemies.end(),
        [&](Enemy* e) {
            if (e->isDead()) {
                gold += e->getGoldReward();
                score += e->getGoldReward() * 2;
                delete e;
                return true;
            }
            return false;
        });
    enemies.erase(rm, enemies.end());

    if (waveInProgress &&
        enemiesSpawnedThisWave >= enemiesToSpawn &&
        enemies.empty()) {
        waveInProgress = false;
        score += wave * 50;
        gold += 20 + wave * 10;
        if (wave >= maxWaves) {
            victory = true;
            screen = GameScreen::Victory;
            if (score > highScore) highScore = score;
        }
    }

    if (lives <= 0) {
        lives = 0;
        gameOver = true;
        screen = GameScreen::GameOver;
        if (score > highScore) highScore = score;
    }
}

void Game::startWave(int waveNumber) {
    wave = waveNumber;
    waveInProgress = true;
    spawnTimer = 0.f;
    enemiesSpawnedThisWave = 0;
    enemiesToSpawn = 4 + wave * 3;
    spawnInterval = std::max(0.25f, 0.8f - wave * 0.05f);
}

void Game::spawnEnemy(int waveNumber) {
    float sx = path[0].x;
    float sy = path[0].y;
    Enemy* e = nullptr;
    int roll = rand() % 10;

    if (waveNumber <= 1) {
        e = new BasicEnemy(sx, sy, &path);
    }
    else if (waveNumber <= 2) {
        e = (roll < 5) ? (Enemy*)new BasicEnemy(sx, sy, &path)
            : (Enemy*)new FastEnemy(sx, sy, &path);
    }
    else if (waveNumber <= 3) {
        if (roll < 4) e = new BasicEnemy(sx, sy, &path);
        else if (roll < 8) e = new FastEnemy(sx, sy, &path);
        else               e = new TankEnemy(sx, sy, &path);
    }
    else {
        if (roll < 3) e = new BasicEnemy(sx, sy, &path);
        else if (roll < 6) e = new FastEnemy(sx, sy, &path);
        else               e = new TankEnemy(sx, sy, &path);
    }

    e->setRenderTarget(&window);
    enemies.push_back(e);
}

void Game::placeTower(float x, float y) {
    Tower* t = nullptr;
    switch (selectedTower) {
    case TowerType::Cannon:     t = new CannonTower(x, y); break;
    case TowerType::MachineGun: t = new MachineGunTower(x, y); break;
    case TowerType::Sniper:     t = new SniperTower(x, y); break;
    }
    if (t) {
        t->setRenderTarget(&window);
        towers.push_back(t);
        gold -= towerCost(selectedTower);
    }
}

bool Game::isOnPath(float px, float py, float radius) {
    for (int i = 0; i + 1 < (int)path.size(); i++) {
        sf::Vector2f a = path[i], b = path[i + 1];
        float dx = b.x - a.x, dy = b.y - a.y;
        float len2 = dx * dx + dy * dy;
        float t = ((px - a.x) * dx + (py - a.y) * dy) / len2;
        t = std::max(0.f, std::min(1.f, t));
        float cx = a.x + t * dx - px;
        float cy = a.y + t * dy - py;
        if (cx * cx + cy * cy < radius * radius) return true;
    }
    return false;
}

void Game::render() {
    window.clear(sf::Color(34, 85, 34));

    if (screen == GameScreen::MainMenu) {
        drawMainMenu();
    }
    else if (screen == GameScreen::Playing) {
        drawBackground();
        drawPath();
        drawDecor();
        drawMapFires();
        drawExitFlags();
        for (Tower* t : towers)  t->render();
        for (Enemy* e : enemies) e->render();
        drawHUD();
        drawTowerSelector();
    }
    else if (screen == GameScreen::GameOver) {
        drawBackground();
        drawPath();
        drawDecor();
        drawMapFires();
        drawExitFlags();
        for (Tower* t : towers) t->render();
        drawGameOver();
    }
    else if (screen == GameScreen::Victory) {
        drawBackground();
        drawPath();
        drawDecor();
        drawMapFires();
        drawExitFlags();
        for (Tower* t : towers) t->render();
        drawVictory();
    }

    window.display();
}

void Game::drawBackground() {
    const float TILE = 32.f;
    const float SCALE = 2.f;
    const float TSIZE = TILE * SCALE;

    sf::Sprite tile;
    tile.setTexture(grassTileTex);
    tile.setTextureRect(sf::IntRect(0, 0, (int)TILE, (int)TILE));
    tile.setScale(SCALE, SCALE);

    for (float gy = 0; gy < 560; gy += TSIZE)
        for (float gx = 0; gx < 910; gx += TSIZE) {
            tile.setPosition(gx, gy);
            window.draw(tile);
        }
}

void Game::drawPath() {
    const float ROAD_W = 44.f;

    for (int i = 0; i + 1 < (int)path.size(); i++) {
        sf::Vector2f a = path[i], b = path[i + 1];
        float dx = b.x - a.x, dy = b.y - a.y;
        float len = std::sqrt(dx * dx + dy * dy);
        float ang = std::atan2(dy, dx) * 180.f / 3.14159f;

        sf::RectangleShape seg({ len + ROAD_W, ROAD_W });
        seg.setOrigin(ROAD_W / 2.f, ROAD_W / 2.f);
        seg.setPosition(a);
        seg.setRotation(ang);
        seg.setFillColor(sf::Color(139, 100, 60));
        seg.setOutlineColor(sf::Color(100, 72, 40));
        seg.setOutlineThickness(2.f);
        window.draw(seg);
    }
}

void Game::drawDecor() {
    for (auto& d : decors)
        window.draw(d.sprite);
}

void Game::drawMapFires() {
    sf::Sprite spr;
    spr.setTexture(campfireSheetTex);
    for (auto& f : mapFires) {
        spr.setTextureRect(sf::IntRect(f.frame * 64, 0, 64, 64));
        spr.setScale(f.scale, f.scale);
        spr.setOrigin(32.f, 64.f);
        spr.setPosition(f.pos);
        window.draw(spr);
    }
}

void Game::drawExitFlags() {
    sf::Sprite spr;
    spr.setTexture(flagSheetTex);
    spr.setTextureRect(sf::IntRect(flagFrame * 64, 0, 64, 64));
    spr.setScale(1.5f, 1.5f);
    spr.setOrigin(32.f, 64.f);

    spr.setPosition(flagLeft);
    window.draw(spr);

    spr.setPosition(flagRight);
    window.draw(spr);
}

void Game::drawHUD() {
    sf::RectangleShape hudBg({ 900.f, 100.f });
    hudBg.setPosition(0.f, 550.f);
    hudBg.setFillColor(sf::Color(15, 20, 15, 230));
    window.draw(hudBg);

    sf::RectangleShape sep({ 900.f, 2.f });
    sep.setPosition(0.f, 550.f);
    sep.setFillColor(sf::Color(80, 120, 60));
    window.draw(sep);

    drawText("GOLD: " + std::to_string(gold), 12.f, 558.f, sf::Color(255, 215, 0), 18, true);
    drawText("LIVES: " + std::to_string(lives), 12.f, 582.f, sf::Color(80, 220, 80), 18, true);
    drawText("WAVE: " + std::to_string(wave) + "/" + std::to_string(maxWaves),
        12.f, 606.f, sf::Color(120, 180, 255), 18, true);
    drawText("SCORE: " + std::to_string(score), 12.f, 627.f, sf::Color(220, 180, 255), 16);

    float bx = 155.f, by = 555.f, bw = 120.f, bh = 36.f, gap = 6.f;

    struct BtnInfo { TowerType type; sf::Color col; };
    BtnInfo btns[3] = {
        { TowerType::Cannon,     sf::Color(60, 160, 60)  },
        { TowerType::MachineGun, sf::Color(160, 120, 30) },
        { TowerType::Sniper,     sf::Color(60, 60, 180)  },
    };

    btn1Rect = { bx,               by, bw, bh };
    btn2Rect = { bx + bw + gap,    by, bw, bh };
    btn3Rect = { bx + 2 * (bw + gap),  by, bw, bh };

    sf::FloatRect* btnRects[3] = { &btn1Rect, &btn2Rect, &btn3Rect };
    const char* labels[3] = { "[1] Archer", "[2] Rapid", "[3] Sniper" };
    int costs[3] = { towerCost(TowerType::Cannon),
                                   towerCost(TowerType::MachineGun),
                                   towerCost(TowerType::Sniper) };

    for (int i = 0; i < 3; i++) {
        sf::RectangleShape btn({ btnRects[i]->width, btnRects[i]->height });
        btn.setPosition(btnRects[i]->left, btnRects[i]->top);
        bool selected = (selectedTower == btns[i].type);
        bool canAfford = (gold >= costs[i]);
        sf::Color bg = selected ? btns[i].col : sf::Color(30, 35, 30);
        if (!canAfford) bg = sf::Color(50, 30, 30);
        btn.setFillColor(bg);
        btn.setOutlineColor(selected ? sf::Color::White : sf::Color(80, 100, 80));
        btn.setOutlineThickness(selected ? 2.f : 1.f);
        window.draw(btn);
        drawText(labels[i], btnRects[i]->left + 5.f, btnRects[i]->top + 4.f,
            canAfford ? sf::Color::White : sf::Color(160, 100, 100), 13);
        drawText(std::to_string(costs[i]) + "g",
            btnRects[i]->left + 5.f, btnRects[i]->top + 19.f,
            sf::Color(255, 215, 0), 12);
    }

    float wx = 560.f;
    if (!waveInProgress && wave < maxWaves) {
        startWaveRect = { wx, 556.f, 200.f, 40.f };
        sf::RectangleShape wb({ 200.f, 40.f });
        wb.setPosition(wx, 556.f);
        wb.setFillColor(sf::Color(180, 100, 20));
        wb.setOutlineColor(sf::Color(255, 180, 50));
        wb.setOutlineThickness(2.f);
        window.draw(wb);
        drawText("SPACE / Click", wx + 12.f, 558.f, sf::Color::White, 13, true);
        drawText("Start Wave " + std::to_string(wave + 1),
            wx + 25.f, 575.f, sf::Color(255, 230, 100), 13);
    }
    else if (waveInProgress) {
        startWaveRect = { 0, 0, 0, 0 };
        drawText("Wave " + std::to_string(wave) + " in progress",
            wx + 10.f, 558.f, sf::Color(255, 160, 80), 14, true);
        drawText("Enemies: " + std::to_string((int)enemies.size()),
            wx + 30.f, 578.f, sf::Color(220, 120, 120), 13);
    }

    drawText("[ESC] Menu", wx + 5.f, 610.f, sf::Color(150, 150, 150), 12);
    drawText("Hi: " + std::to_string(highScore),
        wx + 130.f, 610.f, sf::Color(200, 180, 255), 12);
}

void Game::drawTowerSelector() {
    // handled inside drawHUD
}

void Game::drawMainMenu() {
    window.clear(sf::Color(18, 38, 18));

    sf::Sprite tile;
    tile.setTexture(grassTileTex);
    tile.setTextureRect(sf::IntRect(0, 0, 32, 32));
    tile.setScale(2.f, 2.f);
    tile.setColor(sf::Color(255, 255, 255, 60));
    for (float gy = 0; gy < 650; gy += 64)
        for (float gx = 0; gx < 900; gx += 64) {
            tile.setPosition(gx, gy);
            window.draw(tile);
        }

    sf::RectangleShape titleBg({ 500.f, 80.f });
    titleBg.setOrigin(250.f, 40.f);
    titleBg.setPosition(450.f, 160.f);
    titleBg.setFillColor(sf::Color(0, 0, 0, 160));
    titleBg.setOutlineColor(sf::Color(80, 200, 80, 180));
    titleBg.setOutlineThickness(3.f);
    window.draw(titleBg);

    drawText("TOWER DEFENSE", 230.f, 130.f, sf::Color(90, 220, 80), 48, true);
    drawText("Protect the kingdom from waves of enemies!", 300.f, 210.f, sf::Color(180, 220, 160), 16);

    if (highScore > 0)
        drawText("HIGH SCORE: " + std::to_string(highScore),
            320.f, 245.f, sf::Color(255, 215, 0), 18, true);

    sf::RectangleShape infoBox({ 500.f, 160.f });
    infoBox.setOrigin(250.f, 80.f);
    infoBox.setPosition(450.f, 360.f);
    infoBox.setFillColor(sf::Color(0, 0, 0, 140));
    infoBox.setOutlineColor(sf::Color(60, 100, 60));
    infoBox.setOutlineThickness(1.f);
    window.draw(infoBox);

    drawText("HOW TO PLAY:", 210.f, 284.f, sf::Color(255, 215, 0), 15, true);
    drawText("* Click map to place towers   [1] Archer  [2] Rapid  [3] Sniper",
        200.f, 305.f, sf::Color(200, 220, 200), 13);
    drawText("* Press SPACE or button to start each wave",
        200.f, 322.f, sf::Color(200, 220, 200), 13);
    drawText("* Earn gold by killing enemies — spend it on towers",
        200.f, 339.f, sf::Color(200, 220, 200), 13);
    drawText("* Don't let enemies reach the end!",
        200.f, 356.f, sf::Color(220, 150, 150), 13);
    drawText("* Enemies: Slime (basic)  Bee (fast)  Monster (tanky)",
        200.f, 373.f, sf::Color(200, 220, 200), 13);
    drawText("* Towers: Archer=75g  Rapid=50g  Sniper=100g",
        200.f, 390.f, sf::Color(200, 220, 200), 13);
    drawText("* Score points for kills and wave bonuses!",
        200.f, 407.f, sf::Color(200, 220, 200), 13);

    menuPlayRect = { 320.f, 440.f, 260.f, 55.f };
    sf::RectangleShape playBtn({ 260.f, 55.f });
    playBtn.setPosition(320.f, 440.f);
    playBtn.setFillColor(sf::Color(40, 140, 40));
    playBtn.setOutlineColor(sf::Color(100, 255, 100));
    playBtn.setOutlineThickness(3.f);
    window.draw(playBtn);
    drawText("PLAY GAME", 365.f, 452.f, sf::Color::White, 24, true);
    drawText("Press ENTER or click to start",
        340.f, 506.f, sf::Color(150, 200, 150), 13);
}

void Game::drawGameOver() {
    sf::RectangleShape overlay({ 900.f, 650.f });
    overlay.setFillColor(sf::Color(0, 0, 0, 180));
    window.draw(overlay);

    sf::RectangleShape panel({ 480.f, 320.f });
    panel.setOrigin(240.f, 160.f);
    panel.setPosition(450.f, 300.f);
    panel.setFillColor(sf::Color(30, 10, 10, 230));
    panel.setOutlineColor(sf::Color(200, 50, 50));
    panel.setOutlineThickness(3.f);
    window.draw(panel);

    drawText("GAME OVER", 270.f, 170.f, sf::Color(255, 60, 60), 52, true);
    drawText("You survived " + std::to_string(wave) + " waves",
        320.f, 240.f, sf::Color(200, 180, 180), 18);
    drawText("Final Score: " + std::to_string(score),
        345.f, 268.f, sf::Color(255, 215, 0), 20, true);

    if (score >= highScore && score > 0)
        drawText("NEW HIGH SCORE!", 340.f, 295.f, sf::Color(255, 100, 255), 16, true);
    else
        drawText("High Score: " + std::to_string(highScore),
            345.f, 295.f, sf::Color(200, 180, 255), 14);

    endRestartRect = { 270.f, 360.f, 180.f, 44.f };
    endQuitRect = { 460.f, 360.f, 170.f, 44.f };

    sf::RectangleShape rBtn({ 180.f, 44.f }), qBtn({ 170.f, 44.f });
    rBtn.setPosition(270.f, 360.f); rBtn.setFillColor(sf::Color(40, 130, 40));
    rBtn.setOutlineColor(sf::Color(100, 255, 100)); rBtn.setOutlineThickness(2.f);
    qBtn.setPosition(460.f, 360.f); qBtn.setFillColor(sf::Color(130, 40, 40));
    qBtn.setOutlineColor(sf::Color(255, 100, 100)); qBtn.setOutlineThickness(2.f);
    window.draw(rBtn); window.draw(qBtn);
    drawText("[R] Restart", 290.f, 372.f, sf::Color::White, 16, true);
    drawText("[ESC] Menu", 480.f, 372.f, sf::Color::White, 16, true);
}

void Game::drawVictory() {
    sf::RectangleShape overlay({ 900.f, 650.f });
    overlay.setFillColor(sf::Color(0, 0, 0, 170));
    window.draw(overlay);

    sf::RectangleShape panel({ 500.f, 340.f });
    panel.setOrigin(250.f, 170.f);
    panel.setPosition(450.f, 300.f);
    panel.setFillColor(sf::Color(10, 30, 10, 230));
    panel.setOutlineColor(sf::Color(80, 220, 80));
    panel.setOutlineThickness(3.f);
    window.draw(panel);

    drawText("VICTORY!", 310.f, 155.f, sf::Color(80, 255, 120), 56, true);
    drawText("All " + std::to_string(maxWaves) + " waves defeated!",
        315.f, 230.f, sf::Color(180, 240, 180), 20);
    drawText("Final Score: " + std::to_string(score),
        335.f, 260.f, sf::Color(255, 215, 0), 22, true);

    if (score >= highScore && score > 0)
        drawText("** NEW HIGH SCORE! **", 320.f, 292.f, sf::Color(255, 100, 255), 18, true);
    else
        drawText("High Score: " + std::to_string(highScore),
            340.f, 292.f, sf::Color(200, 180, 255), 16);

    drawText("Gold remaining: " + std::to_string(gold),
        350.f, 320.f, sf::Color(200, 200, 160), 14);
    drawText("Lives remaining: " + std::to_string(lives),
        350.f, 340.f, sf::Color(160, 220, 160), 14);

    endRestartRect = { 265.f, 380.f, 180.f, 44.f };
    endQuitRect = { 458.f, 380.f, 170.f, 44.f };

    sf::RectangleShape rBtn({ 180.f, 44.f }), qBtn({ 170.f, 44.f });
    rBtn.setPosition(265.f, 380.f); rBtn.setFillColor(sf::Color(40, 130, 40));
    rBtn.setOutlineColor(sf::Color(100, 255, 100)); rBtn.setOutlineThickness(2.f);
    qBtn.setPosition(458.f, 380.f); qBtn.setFillColor(sf::Color(80, 60, 120));
    qBtn.setOutlineColor(sf::Color(160, 130, 220)); qBtn.setOutlineThickness(2.f);
    window.draw(rBtn); window.draw(qBtn);
    drawText("[R] Play Again", 280.f, 392.f, sf::Color::White, 16, true);
    drawText("[ESC] Menu", 476.f, 392.f, sf::Color::White, 16, true);
}

int Game::towerCost(TowerType t) {
    switch (t) {
    case TowerType::Cannon:     return 75;
    case TowerType::MachineGun: return 50;
    case TowerType::Sniper:     return 100;
    }
    return 0;
}

std::string Game::towerName(TowerType t) {
    switch (t) {
    case TowerType::Cannon:     return "Archer";
    case TowerType::MachineGun: return "Rapid";
    case TowerType::Sniper:     return "Sniper";
    }
    return "";
}

void Game::drawText(const std::string& str, float x, float y,
    sf::Color col, int size, bool bold) {
    sf::Text t;
    t.setFont(font);
    t.setString(str);
    t.setCharacterSize(size);
    t.setFillColor(col);
    t.setPosition(x, y);
    if (bold) t.setStyle(sf::Text::Bold);
    window.draw(t);
}