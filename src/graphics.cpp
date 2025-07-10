#include "graphics.h"
#include <iostream>
#include <iomanip>

graphics::graphics() : window(nullptr), graphicsEnabled(false) {
    // Initialize color palette with default colors
    colorPalette.resize(16);
    tilePixels.resize(16);
    
    // Initialize each tile with 256 pixels
    for (int i = 0; i < 16; i++) {
        tilePixels[i].resize(256, 0); // 16x16 = 256 pixels per tile
    }
    
    // Set default palette
    colorPalette[0] = sf::Color::Black;
    colorPalette[1] = sf::Color::Blue;
    colorPalette[2] = sf::Color::Green;
    colorPalette[3] = sf::Color::Cyan;
    colorPalette[4] = sf::Color::Red;
    colorPalette[5] = sf::Color::Magenta;
    colorPalette[6] = sf::Color::Yellow;
    colorPalette[7] = sf::Color::White;
    colorPalette[8] = sf::Color(64, 64, 64);
    colorPalette[9] = sf::Color(0, 0, 128);
    colorPalette[10] = sf::Color(0, 128, 0);
    colorPalette[11] = sf::Color(0, 128, 128);
    colorPalette[12] = sf::Color(128, 0, 0);
    colorPalette[13] = sf::Color(128, 0, 128);
    colorPalette[14] = sf::Color(128, 128, 0);
    colorPalette[15] = sf::Color(192, 192, 192);
}

graphics::~graphics() {
    if (window) {
        delete window;
    }
}

bool graphics::initializeGraphics() {
    try {
        // Create window
        window = new sf::RenderWindow(sf::VideoMode(SCREEN_WIDTH * 2, SCREEN_HEIGHT * 2), 
                                     "ZX16 Simulator Graphics");
        window->setFramerateLimit(60);
        
        // Initialize screen image
        screenImage.create(SCREEN_WIDTH, SCREEN_HEIGHT, sf::Color::Black);
        screenTexture.create(SCREEN_WIDTH, SCREEN_HEIGHT);
        screenSprite.setTexture(screenTexture);
        screenSprite.setScale(2.0f, 2.0f); // Scale up for visibility
        
        graphicsEnabled = true;
        
        std::cout << "ZX16 Graphics System initialized successfully" << std::endl;
        std::cout << "Screen: " << SCREEN_WIDTH << "x" << SCREEN_HEIGHT << " pixels" << std::endl;
        std::cout << "Tiles: " << TILES_HORIZONTAL << "x" << TILES_VERTICAL << " grid" << std::endl;
        
        return true;
    } catch (const std::exception& e) {
        std::cerr << "Failed to initialize graphics: " << e.what() << std::endl;
        graphicsEnabled = false;
        return false;
    }
}

void graphics::updateColorPalette(const Memory& mem) {
    try {
        for (int i = 0; i < 16; i++) {
            uint8_t colorByte = mem.load8(COLOR_PALETTE_ADDRESS + i);
            
            // Extract RGB values according to ZX16 specification
            uint8_t red = (colorByte >> 5) & 0x07;    // bits 5-7
            uint8_t green = (colorByte >> 2) & 0x07;  // bits 2-4
            uint8_t blue = colorByte & 0x03;          // bits 0-1
            
            // Scale to 0-255 range
            red = (red * 255) / 7;
            green = (green * 255) / 7;
            blue = (blue * 255) / 3;
            
            colorPalette[i] = sf::Color(red, green, blue);
        }
    } catch (const AddressOutOfBoundsException& e) {
        std::cerr << "Error reading color palette: " << e.what() << std::endl;
    }
}

void graphics::updateTileDefinitions(const Memory& mem) {
    try {
        for (int tileIndex = 0; tileIndex < 16; tileIndex++) {
            uint16_t tileAddress = TILE_DEFINITIONS_ADDRESS + (tileIndex * 128);
            
            // Each tile is 16x16 pixels, stored as 4-bit values packed into bytes
            for (int pixelPair = 0; pixelPair < 128; pixelPair++) {
                uint8_t packedPixels = mem.load8(tileAddress + pixelPair);
                
                // Extract two 4-bit pixel values
                uint8_t pixel1 = packedPixels & 0x0F;        // Lower 4 bits
                uint8_t pixel2 = (packedPixels >> 4) & 0x0F; // Upper 4 bits
                
                // Store in tile cache
                tilePixels[tileIndex][pixelPair * 2] = pixel1;
                tilePixels[tileIndex][pixelPair * 2 + 1] = pixel2;
            }
        }
    } catch (const AddressOutOfBoundsException& e) {
        std::cerr << "Error reading tile definitions: " << e.what() << std::endl;
    }
}

void graphics::renderTiles(const Memory& mem) {
    try {
        // Clear screen
        screenImage.create(SCREEN_WIDTH, SCREEN_HEIGHT, sf::Color::Black);
        
        // Render each tile position
        for (int tileY = 0; tileY < TILES_VERTICAL; tileY++) {
            for (int tileX = 0; tileX < TILES_HORIZONTAL; tileX++) {
                // Get tile index from tile map
                uint16_t tileMapIndex = tileY * TILES_HORIZONTAL + tileX;
                uint8_t tileIndex = mem.load8(TILE_MAP_ADDRESS + tileMapIndex);
                
                // Clamp tile index to valid range
                if (tileIndex >= 16) {
                    tileIndex = 0;
                }
                
                // Render this tile
                for (int pixelY = 0; pixelY < TILE_SIZE; pixelY++) {
                    for (int pixelX = 0; pixelX < TILE_SIZE; pixelX++) {
                        int pixelIndex = pixelY * TILE_SIZE + pixelX;
                        uint8_t colorIndex = tilePixels[tileIndex][pixelIndex];
                        
                        // Calculate screen position
                        int screenX = tileX * TILE_SIZE + pixelX;
                        int screenY = tileY * TILE_SIZE + pixelY;
                        
                        // Set pixel color
                        screenImage.setPixel(screenX, screenY, getColor(colorIndex));
                    }
                }
            }
        }
    } catch (const AddressOutOfBoundsException& e) {
        std::cerr << "Error rendering tiles: " << e.what() << std::endl;
    }
}

void graphics::updateGraphics(const Memory& mem) {
    if (!graphicsEnabled || !window) return;
    
    // Update color palette from memory
    updateColorPalette(mem);
    
    // Update tile definitions from memory
    updateTileDefinitions(mem);
    
    // Render tiles to screen
    renderTiles(mem);
    
    // Update texture
    screenTexture.update(screenImage);
}

void graphics::renderFrame() {
    if (!graphicsEnabled || !window) return;
    
    window->clear();
    window->draw(screenSprite);
    window->display();
}

bool graphics::isGraphicsWindowOpen() const {
    return window && window->isOpen();
}

void graphics::handleGraphicsEvents() {
    if (!window) return;
    
    sf::Event event;
    while (window->pollEvent(event)) {
        if (event.type == sf::Event::Closed) {
            window->close();
        }
        
        // Add keyboard shortcuts for testing
        if (event.type == sf::Event::KeyPressed) {
            switch (event.key.code) {
                case sf::Keyboard::Key::P:
                    displayColorPalette();
                    break;
                case sf::Keyboard::Key::T:
                    drawColorPaletteTest();
                    break;
                default:
                    break;
            }
        }
    }
}

void graphics::setGraphicsEnabled(bool enabled) {
    graphicsEnabled = enabled;
}

bool graphics::isGraphicsEnabled() const {
    return graphicsEnabled;
}

sf::Color graphics::getColor(uint8_t colorIndex) const {
    if (colorIndex >= 16) {
        return colorPalette[0]; // Default to black
    }
    return colorPalette[colorIndex];
}

void graphics::displayColorPalette() const {
    std::cout << "\n=== ZX16 Color Palette ===" << std::endl;
    for (int i = 0; i < 16; i++) {
        sf::Color color = colorPalette[i];
        std::cout << "Color " << std::setw(2) << i << ": RGB(" 
                  << std::setw(3) << (int)color.r << ", " 
                  << std::setw(3) << (int)color.g << ", " 
                  << std::setw(3) << (int)color.b << ")" << std::endl;
    }
    std::cout << "==========================" << std::endl;
}

void graphics::drawColorPaletteTest() {
    const int colorBlockSize = 40;
    
    // Clear screen
    screenImage.create(SCREEN_WIDTH, SCREEN_HEIGHT, sf::Color::Black);
    
    // Draw color palette as 4x4 grid
    for (int i = 0; i < 16; i++) {
        int blockX = (i % 4) * colorBlockSize;
        int blockY = (i / 4) * colorBlockSize;
        
        sf::Color color = colorPalette[i];
        
        // Draw block
        for (int dy = 0; dy < colorBlockSize && (blockY + dy) < SCREEN_HEIGHT; dy++) {
            for (int dx = 0; dx < colorBlockSize && (blockX + dx) < SCREEN_WIDTH; dx++) {
                screenImage.setPixel(blockX + dx, blockY + dy, color);
            }
        }
    }
    
    // Update texture
    screenTexture.update(screenImage);
}

void graphics::setTestPalette(Memory& mem) {
    // Set up a test color palette in memory
    const uint8_t testColors[16] = {
        0x00, // 0: Black (000|000|00)
        0x03, // 1: Blue (000|000|11)
        0x1C, // 2: Green (000|111|00)
        0x1F, // 3: Cyan (000|111|11)
        0xE0, // 4: Red (111|000|00)
        0xE3, // 5: Magenta (111|000|11)
        0xFC, // 6: Yellow (111|111|00)
        0xFF, // 7: White (111|111|11)
        0x49, // 8: Dark Gray (010|010|01)
        0x02, // 9: Dark Blue (000|000|10)
        0x0C, // 10: Dark Green (000|011|00)
        0x0E, // 11: Dark Cyan (000|011|10)
        0x60, // 12: Dark Red (011|000|00)
        0x62, // 13: Dark Magenta (011|000|10)
        0x6C, // 14: Dark Yellow (011|011|00)
        0xDB  // 15: Light Gray (110|110|11)
    };
    
    try {
        for (int i = 0; i < 16; i++) {
            mem.store8(COLOR_PALETTE_ADDRESS + i, testColors[i]);
        }
        std::cout << "Test color palette loaded into memory" << std::endl;
    } catch (const AddressOutOfBoundsException& e) {
        std::cerr << "Error setting test palette: " << e.what() << std::endl;
    }
}

// Example integration with your main.cpp
/* Add this to your main.cpp:

#include "graphics.h"

int main() {
    // ... your existing code ...
    
    Decoder decoder;
    Registers regs;
    Memory mem;
    graphics gfx;  // Your graphics object already exists
    Ecalls ecalls;
    ALU alu;
    
    // Initialize graphics system
    if (gfx.initializeGraphics()) {
        std::cout << "Graphics system ready" << std::endl;
        
        // Set up test palette
        gfx.setTestPalette(mem);
        
        // Test the color palette
        gfx.updateGraphics(mem);
        gfx.drawColorPaletteTest();
        gfx.renderFrame();
        
        std::cout << "Press 'P' to display palette info, 'T' to test colors" << std::endl;
    }
    
    // ... rest of your simulation loop ...
    
    while (!halted) {
        // ... your instruction execution ...
        
        // Update graphics if enabled
        if (gfx.isGraphicsEnabled() && gfx.isGraphicsWindowOpen()) {
            gfx.handleGraphicsEvents();
            gfx.updateGraphics(mem);
            gfx.renderFrame();
        }
    }
    
    return 0;
}

*/