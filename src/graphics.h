#ifndef GRAPHICS_H
#define GRAPHICS_H

#include <SFML/Graphics.hpp>
#include <queue>

// Forward declarations
class Memory;
class AddressOutOfBoundsException;

// Graphics constants
#define SCREEN_WIDTH 320
#define SCREEN_HEIGHT 240
#define TILE_SIZE 16
#define TILES_HORIZONTAL 20
#define TILES_VERTICAL 15
#define TOTAL_TILES 300
#define GRAPHICS_MEMORY_START 0xF000
#define GRAPHICS_MEMORY_END 0xFFFF

class Graphics {
private:
    Memory* memory;
    bool needsUpdate;
    bool isInitialized;

    // NEW: Keyboard input handling
    uint16_t lastKeyPressed;
    bool hasNewKeyPress;

    // SFML graphics objects
    sf::Image screenImage;
    sf::Texture screenTexture;
    sf::Sprite screenSprite;
    sf::RenderWindow window;

    // Text system (if you need these)
    sf::Font textFont;
    sf::Text displayText;
    bool textFontLoaded;

    // OLD: Legacy input handling (you might want to remove these)
    std::queue<char> inputBuffer;
    bool keyPressed;
    char lastKey;

    // Helper method to convert SFML key to ASCII/key code
    uint16_t convertSFMLKeyToCode(sf::Keyboard::Key key) const;

public:
    Graphics(Memory* mem);
    ~Graphics();

    // Core graphics methods
    bool initialize();
    bool isWindowOpen();
    void handleEvents();
    void markDirty();
    void update();
    void renderFrame();

    // NEW: Keyboard input methods (for ECALL 7)
    uint16_t getLastKeyPressed() const;
    void clearLastKeyPressed();
    bool isKeyCurrentlyPressed(sf::Keyboard::Key key) const;

    // Text display methods
    void updateTextDisplay();
    sf::Color getTextColor(uint8_t colorIndex);
    sf::Color convertPaletteColor(uint8_t paletteValue);

    // REMOVE THESE DUPLICATE/CONFLICTING METHODS:
    // char getLastKeyPressed();  // <-- REMOVE THIS LINE
    // bool isKeyPressed();       // <-- REMOVE THIS LINE if it conflicts
    // void clearKeyBuffer();     // <-- REMOVE THIS LINE if not needed
};

#endif // GRAPHICS_H