#ifndef GRAPHICS_H
#define GRAPHICS_H

#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <SFML/Main.hpp>
#include <SFML/System.hpp>

#include <vector>
#include <cstdint>
#include "Memory.h"

class graphics {
private:
    // Screen dimensions
    static const int SCREEN_WIDTH = 320;
    static const int SCREEN_HEIGHT = 240;
    static const int TILE_SIZE = 16;
    static const int TILES_HORIZONTAL = 20;
    static const int TILES_VERTICAL = 15;
    static const int TOTAL_TILES = 300;

    // Memory addresses
    static const uint16_t TILE_MAP_ADDRESS = 0xF000;
    static const uint16_t TILE_DEFINITIONS_ADDRESS = 0xF200;
    static const uint16_t COLOR_PALETTE_ADDRESS = 0xFA00;

    // SFML objects
    sf::RenderWindow* window;
    sf::Image screenImage;
    sf::Texture screenTexture;
    sf::Sprite screenSprite;

    // Color palette (16 colors)
    std::vector<sf::Color> colorPalette;

    // Graphics enabled flag
    bool graphicsEnabled;

    // Tile data cache for performance
    std::vector<std::vector<uint8_t>> tilePixels; // 16 tiles, each with 256 pixels

public:
    graphics();
    ~graphics();

    // Initialize graphics system
    bool initializeGraphics();

    // Update graphics from memory
    void updateGraphics(const Memory& mem);

    // Render frame
    void renderFrame();

    // Check if graphics window is open
    bool isGraphicsWindowOpen() const;

    // Handle graphics events
    void handleGraphicsEvents();

    // Enable/disable graphics
    void setGraphicsEnabled(bool enabled);
    bool isGraphicsEnabled() const;

    // Color palette functions
    void updateColorPalette(const Memory& mem);
    void displayColorPalette() const;
    sf::Color getColor(uint8_t colorIndex) const;

    // Tile functions
    void updateTileDefinitions(const Memory& mem);
    void renderTiles(const Memory& mem);

    // Debug functions
    void drawColorPaletteTest();
    void drawTileTest(uint8_t tileIndex);

    // Test function to set palette colors
    void setTestPalette(Memory& mem);
};

#endif // GRAPHICS_H
