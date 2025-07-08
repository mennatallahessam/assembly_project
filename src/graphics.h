// graphics.h
#ifndef GRAPHICS_H
#define GRAPHICS_H

#include <cstdint>
#include <SDL2/SDL.h>

class graphics {
private:
    SDL_AudioDeviceID audio_device;
    uint8_t current_volume = SDL_MIX_MAXVOLUME; // Default to max volume

public:
    graphics();
    ~graphics();
    
    void initAudio();
    void setVolume(uint8_t volume);
    uint8_t getVolume() const;
};

#endif
