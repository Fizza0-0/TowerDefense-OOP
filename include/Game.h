#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include <string>
#include "Enemy.h"
#include "Tower.h"

enum class TowerType { Cannon, MachineGun, Sniper };
enum class GameScreen { MainMenu, Playing, GameOver, Victory };

struct MapDecor {
    sf::Sprite sprite;
    float x, y;
};

struct MapFire {
    sf::Vector2f pos;
    int   frame = 0;
    float timer = 0.f;
    float speed = 0.12f;
    float scale = 1.5f;
};

class Game {
public:
    Game();
    ~Game();
    void run();

private:
    sf::RenderWindow window;
    sf::Font         font;
    sf::Clock        clock;

    GameScreen screen = GameScreen::MainMenu;

    // Background tiles
    sf::Texture grassTex;
    sf::Texture pathTex;
    sf::Texture tilesetTex;
    sf::Texture grassTileTex;
    sf::Sprite  grassTileSprite;

    // Decorations
    sf::Texture treeTex1, treeTex2, bushTex;
    std::vector<MapDecor> decors;

    // Scattered fires (campfire sheet: 192x64 = 3 frames of 64x64)
    sf::Texture campfireSheetTex;
    std::vector<MapFire> mapFires;

    // TWO exit flags only (flag sheet: 192x64 = 3 frames of 64x64)
    sf::Texture  flagSheetTex;
    int          flagFrame = 0;
    float        flagTimer = 0.f;
    sf::Vector2f flagLeft, flagRight;

    // Tower spot placeholder
    sf::Texture towerSpotTex;
    sf::Sprite  towerSpotSprite;

    // UI textures
    sf::Texture mainMenuTex;
    sf::Texture winLooseTex;
    sf::Texture buttonsTex;
    sf::Texture actionPanelTex;

    // World
    Path path;
    std::vector<Enemy*>  enemies;
    std::vector<Tower*>  towers;
    std::vector<sf::Vector2f> towerSpots;

    // Player state
    int  gold = 150;
    int  lives = 20;
    int  wave = 0;
    int  score = 0;
    int  highScore = 0;
    bool gameOver = false;
    bool victory = false;

    // Wave
    float spawnTimer = 0.f;
    float spawnInterval = 0.8f;
    int   enemiesToSpawn = 0;
    int   enemiesSpawnedThisWave = 0;
    bool  waveInProgress = false;
    int   maxWaves = 5;

    // Tower UI
    TowerType     selectedTower = TowerType::Cannon;
    sf::FloatRect btn1Rect, btn2Rect, btn3Rect;
    sf::FloatRect startWaveRect;
    sf::FloatRect menuPlayRect;
    sf::FloatRect endRestartRect;
    sf::FloatRect endQuitRect;

    // Methods
    void loadAssets();
    void buildPath();
    void buildPath_coords();
    void buildDecor();
    void buildMapFires();
    void resetGame();

    void handleEvents();
    void handleMenuEvents(const sf::Event& e);
    void handlePlayEvents(const sf::Event& e);
    void handleEndEvents(const sf::Event& e);

    void update(float dt);
    void render();

    void startWave(int waveNumber);
    void spawnEnemy(int waveNumber);
    void placeTower(float x, float y);

    void drawBackground();
    void drawPath();
    void drawDecor();
    void drawMapFires();
    void drawExitFlags();
    void drawHUD();
    void drawTowerSelector();
    void drawMainMenu();
    void drawGameOver();
    void drawVictory();

    bool        isOnPath(float x, float y, float radius = 35.f);
    int         towerCost(TowerType t);
    std::string towerName(TowerType t);
    void        drawText(const std::string& str, float x, float y,
        sf::Color col = sf::Color::White,
        int size = 18, bool bold = false);
};