#include "Graphics.h"
#include "Memory.h"
#include <iostream>
#include <cstring>

// Add these implementations to your Graphics.cpp file

// In the constructor, initialize the new members:
// 1. MODIFY YOUR CONSTRUCTOR to initialize the new variables:
// ADD THESE METHODS TO YOUR graphics.cpp FILE:

// 1. First, add these member variables to your Graphics constructor
// Modify your existing constructor to include the new member variable initialization:
Graphics::Graphics(Memory* mem)
    : memory(mem),
      needsUpdate(true),
      isInitialized(false),
      screenImage(),
      screenTexture(),
      screenSprite(),
      window(),
      lastKeyPressed(0),        // ADD THIS LINE
      hasNewKeyPress(false) {   // ADD THIS LINE
    std::cout << "Graphics system created, waiting for initialization..." << std::endl;
}

// 2. ADD THESE NEW METHODS at the end of your graphics.cpp file:

uint16_t Graphics::getLastKeyPressed() const {
    if (hasNewKeyPress) {
        return lastKeyPressed;
    }
    return 0; // No key pressed
}

void Graphics::clearLastKeyPressed() {
    lastKeyPressed = 0;
    hasNewKeyPress = false;
}

bool Graphics::isKeyCurrentlyPressed(sf::Keyboard::Key key) const {
    return sf::Keyboard::isKeyPressed(key);
}

uint16_t Graphics::convertSFMLKeyToCode(sf::Keyboard::Key key) const {
    // Convert SFML key codes to ASCII or custom codes
    switch (key) {
        // Letters (A-Z) - return lowercase
        case sf::Keyboard::A: return 'a';
        case sf::Keyboard::B: return 'b';
        case sf::Keyboard::C: return 'c';
        case sf::Keyboard::D: return 'd';
        case sf::Keyboard::E: return 'e';
        case sf::Keyboard::F: return 'f';
        case sf::Keyboard::G: return 'g';
        case sf::Keyboard::H: return 'h';
        case sf::Keyboard::I: return 'i';
        case sf::Keyboard::J: return 'j';
        case sf::Keyboard::K: return 'k';
        case sf::Keyboard::L: return 'l';
        case sf::Keyboard::M: return 'm';
        case sf::Keyboard::N: return 'n';
        case sf::Keyboard::O: return 'o';
        case sf::Keyboard::P: return 'p';
        case sf::Keyboard::Q: return 'q';
        case sf::Keyboard::R: return 'r';
        case sf::Keyboard::S: return 's';
        case sf::Keyboard::T: return 't';
        case sf::Keyboard::U: return 'u';
        case sf::Keyboard::V: return 'v';
        case sf::Keyboard::W: return 119;
        case sf::Keyboard::X: return 'x';
        case sf::Keyboard::Y: return 'y';
        case sf::Keyboard::Z: return 'z';

        // Numbers (0-9)
        case sf::Keyboard::Num0: return '0';
        case sf::Keyboard::Num1: return '1';
        case sf::Keyboard::Num2: return '2';
        case sf::Keyboard::Num3: return '3';
        case sf::Keyboard::Num4: return '4';
        case sf::Keyboard::Num5: return '5';
        case sf::Keyboard::Num6: return '6';
        case sf::Keyboard::Num7: return '7';
        case sf::Keyboard::Num8: return '8';
        case sf::Keyboard::Num9: return '9';

        // Special keys
        case sf::Keyboard::Space: return ' ';
        case sf::Keyboard::Enter: return '\n';
        case sf::Keyboard::Escape: return 27; // ESC
        case sf::Keyboard::Tab: return '\t';
        case sf::Keyboard::Backspace: return 8; // BS

        // Arrow keys (custom codes)
        case sf::Keyboard::Up: return 256;
        case sf::Keyboard::Down: return 257;
        case sf::Keyboard::Left: return 258;
        case sf::Keyboard::Right: return 259;

        default:
            return 0; // Unknown key
    }
}

// 3. MODIFY your existing handleEvents() method to include keyboard handling:
// Replace your existing handleEvents() method with this version:
void Graphics::handleEvents() {
    sf::Event event;
    while (window.pollEvent(event)) {
        if (event.type == sf::Event::Closed) {
            window.close();
        }

        // Handle keyboard input - capture key presses
        if (event.type == sf::Event::KeyPressed) {
            uint16_t keyCode = convertSFMLKeyToCode(event.key.code);
            if (keyCode != 0) {
                lastKeyPressed = keyCode;
                hasNewKeyPress = true;
                std::cout << "SFML Key pressed: " << (char)keyCode << " (code: " << keyCode << ")" << std::endl;
            }
        }

        if (event.type == sf::Event::KeyReleased) {
            // Optional: handle key releases if needed
            std::cout << "SFML Key released: " << event.key.code << std::endl;
        }
    }
}



Graphics::~Graphics() {
    if (window.isOpen()) {
        window.close();
    }
    std::cout << "Graphics system destroyed." << std::endl;
}

bool Graphics::initialize() {
    try {
        const int scaleFactor = 3;
        // Create SFML window with correct ZX16 resolution
        window.create(sf::VideoMode(320, 240),
                     "ZX16 Simulator Graphics");
window.hasFocus();
        // Set frame rate limit (60 FPS)
        window.setFramerateLimit(60);

        // Create the image that will hold our pixel data
        screenImage.create(SCREEN_WIDTH, SCREEN_HEIGHT, sf::Color::Black);

        // Create texture from image
        if (!screenTexture.loadFromImage(screenImage)) {
            std::cerr << "Error: Failed to create screen texture!" << std::endl;
            return false;
        }

        // Create sprite from texture
        screenSprite.setTexture(screenTexture);

        isInitialized = true;
        std::cout << "Graphics system initialized successfully!" << std::endl;
        std::cout << "Screen: " << SCREEN_WIDTH << "x" << SCREEN_HEIGHT << std::endl;
        std::cout << "Tiles: " << TILES_HORIZONTAL << "x" << TILES_VERTICAL
                  << " (" << TOTAL_TILES << " total)" << std::endl;
        std::cout << "Graphics memory: 0x" << std::hex << GRAPHICS_MEMORY_START
                  << " - 0x" << GRAPHICS_MEMORY_END << std::dec << std::endl;
        std::cout << "Tile Map: 0xF000-0xF12B (300 bytes)" << std::endl;
        std::cout << "Tile Data: 0xF200-0xF9FF (2048 bytes)" << std::endl;
        std::cout << "Color Palette: 0xFA00-0xFA0F (16 bytes)" << std::endl;

        return true;
    }
    catch (const std::exception& e) {
        std::cerr << "Error initializing graphics: " << e.what() << std::endl;
        return false;
    }
}

bool Graphics::isWindowOpen() {
    return window.isOpen();
}


void Graphics::markDirty() {
    needsUpdate = true;
}

void Graphics::update() {
    if (!isInitialized) {
        return;
    }

    // Handle window events first
    handleEvents();

    // Only render if something changed
    if (needsUpdate) {
        renderFrame();
        needsUpdate = false;
    }
}

// Convert 3-3-2 RGB palette byte to SFML Color
sf::Color Graphics::convertPaletteColor(uint8_t paletteValue) {
    // Extract RGB components using ZX16 3-3-2 format
    // Bits 7-5: Red (3 bits)
    // Bits 4-2: Green (3 bits)
    // Bits 1-0: Blue (2 bits)

    uint8_t red3   = (paletteValue >> 5) & 0x07;  // Extract bits 7-5
    uint8_t green3 = (paletteValue >> 2) & 0x07;  // Extract bits 4-2
    uint8_t blue2  = (paletteValue >> 0) & 0x03;  // Extract bits 1-0

    // Convert to 8-bit values (scale up)
    uint8_t red8   = (red3   * 255) / 7;  // Scale 3-bit to 8-bit
    uint8_t green8 = (green3 * 255) / 7;  // Scale 3-bit to 8-bit
    uint8_t blue8  = (blue2  * 255) / 3;  // Scale 2-bit to 8-bit

    return sf::Color(red8, green8, blue8);
}
void Graphics::renderFrame() {
    if (!memory) {
        std::cerr << "Error: Memory not available for graphics rendering!" << std::endl;
        return;
    }

    // Clear screen to black
    screenImage.create(SCREEN_WIDTH, SCREEN_HEIGHT, sf::Color::Black);

    static bool first_render = true;
    if (first_render) {
        std::cout << "=== FIRST RENDER DEBUG ===" << std::endl;
        first_render = false;
    }

    static int frame_count = 0;
    bool show_debug = (frame_count < 1); // Only show debug for first frame
    frame_count++;

    // Load color palette from memory (0xFA00 - 0xFA0F)
    sf::Color palette[16];
    for (int i = 0; i < 16; ++i) {
        try {
            uint8_t paletteValue = memory->load8(0xFA00 + i);
            palette[i] = convertPaletteColor(paletteValue);

            if (i < 5 && show_debug) {  // Debug first 5 palette entries only on first frame
                std::cout << "Palette[" << i << "] = 0x" << std::hex << (int)paletteValue
                          << " -> RGB(" << (int)palette[i].r << "," << (int)palette[i].g
                          << "," << (int)palette[i].b << ")" << std::endl;
            }
        } catch (const AddressOutOfBoundsException& e) {
            // Use bright default colors for testing
            switch(i) {
                case 0: palette[i] = sf::Color::Red; break;
                case 1: palette[i] = sf::Color::Blue; break;
                case 2: palette[i] = sf::Color::Green; break;
                case 3: palette[i] = sf::Color::Yellow; break;
                case 4: palette[i] = sf::Color::White; break;
                default: palette[i] = sf::Color::Black; break;
            }
            std::cout << "Palette[" << i << "] = DEFAULT RGB(" << (int)palette[i].r
                      << "," << (int)palette[i].g << "," << (int)palette[i].b << ")" << std::endl;
        }
    }

    // Render tiles - Focus on first 3x3 for debugging
    int tiles_rendered = 0;
    for (int tileY = 0; tileY < TILES_VERTICAL; ++tileY) {
        for (int tileX = 0; tileX < TILES_HORIZONTAL; ++tileX) {
            // Get tile index from tile map (0xF000 + row*20 + col)
            uint16_t mapAddr = 0xF000 + tileY * TILES_HORIZONTAL + tileX;
            uint8_t tileIndex;

            try {
                tileIndex = memory->load8(mapAddr);

                // Debug output for first 3x3 tiles only on first frame
                if (tileY < 3 && tileX < 3 && show_debug) {
                    std::cout << "Tile at (" << tileX << "," << tileY
                              << ") addr=0x" << std::hex << mapAddr
                              << " index=" << std::dec << (int)tileIndex << std::endl;
                }
            } catch (const AddressOutOfBoundsException& e) {
                if (tileY < 3 && tileX < 3 && show_debug) {
                    std::cout << "ERROR: Cannot read tile map at 0x" << std::hex << mapAddr << std::endl;
                }
                continue;
            }

            // Only render valid tiles (0-15)
            if (tileIndex >= 16) {
                continue;
            }

            tiles_rendered++;

            // Get tile data start address (0xF200 + tileIndex * 128)
            uint16_t tileStart = 0xF200 + tileIndex * 128;

            if (tileY < 3 && tileX < 3) {
                std::cout << "Rendering tile " << (int)tileIndex
                          << " from address 0x" << std::hex << tileStart << std::endl;
            }

            // Render each pixel in the 16x16 tile
            for (int py = 0; py < TILE_SIZE; ++py) {
                for (int px = 0; px < TILE_SIZE; ++px) {
                    // Calculate pixel index within tile (row-major order)
                    int pixelIndex = py * TILE_SIZE + px;

                    // Each byte contains 2 pixels (4 bits each)
                    int byteOffset = pixelIndex / 2;

                    uint8_t packed;
                    try {
                        packed = memory->load8(tileStart + byteOffset);
                    } catch (const AddressOutOfBoundsException& e) {
                        continue;
                    }

                    // Extract 4-bit color index
                    uint8_t colorIndex;
                    if (pixelIndex % 2 == 0) {
                        // Even pixel: lower 4 bits
                        colorIndex = packed & 0x0F;
                    } else {
                        // Odd pixel: upper 4 bits
                        colorIndex = (packed >> 4) & 0x0F;
                    }

                    // Debug first pixel of first tile only on first frame
                    if (tileY == 0 && tileX == 0 && py == 0 && px == 0 && show_debug) {
                        std::cout << "First pixel: packed=0x" << std::hex << (int)packed
                                  << " colorIndex=" << std::dec << (int)colorIndex << std::endl;
                    }

                    // Get color from palette
                    sf::Color color = palette[colorIndex];

                    // Calculate screen coordinates
                    int screenX = tileX * TILE_SIZE + px;
                    int screenY = tileY * TILE_SIZE + py;

                    // Set pixel in screen image
                    if (screenX < SCREEN_WIDTH && screenY < SCREEN_HEIGHT) {
                        screenImage.setPixel(screenX, screenY, color);
                    }
                }
            }
        }
    }

    if (show_debug) {
        std::cout << "Total tiles rendered: " << std::hex << tiles_rendered << std::dec << std::endl;
    }

    // Update texture and display
    screenTexture.loadFromImage(screenImage);
    window.clear();
    window.draw(screenSprite);
    window.display();

    static int frameCount = 0;
    frameCount++;
    if (frameCount % 60 == 0) {
        std::cout << "Graphics frame " << frameCount << " complete" << std::endl;
    }
}