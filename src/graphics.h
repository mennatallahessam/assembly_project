#ifndef GRAPHICS_H
#define GRAPHICS_H

#include <cstdint>
#include <string>

class graphics {
public:
    graphics();
    ~graphics();

    // Audio subsystem
    bool initAudio();
    bool isAudioAvailable() const { return audio_initialized; }
    bool playTone(uint16_t freq_hz, uint16_t duration_ms);
    void setVolume(uint8_t volume);  // 0-255
    uint8_t getVolume() const { return current_volume; }
    void stopAudio();

    // Graphics subsystem (placeholder for future expansion)
    bool initGraphics();
    bool isGraphicsAvailable() const { return graphics_initialized; }

private:
    bool audio_initialized = false;
    bool graphics_initialized = false;
    uint8_t current_volume = 128; // Default to 50% volume
    
    // SDL-specific members would go here in real implementation
    // void* audio_device;
    // SDL_AudioSpec audio_spec;
    // etc...
};

#endif
