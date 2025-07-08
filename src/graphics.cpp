// graphics.cpp
#include "graphics.h"
#include <iostream>

graphics::graphics() {
    if (SDL_InitSubSystem(SDL_INIT_AUDIO) < 0) {
        std::cerr << "Audio init failed: " << SDL_GetError() << std::endl;
    } else {
        initAudio();
    }
}

graphics::~graphics() {
    SDL_CloseAudioDevice(audio_device);
    SDL_QuitSubSystem(SDL_INIT_AUDIO);
}

void graphics::initAudio() {
    SDL_AudioSpec desired, obtained;
    
    SDL_zero(desired);
    desired.freq = 44100;
    desired.format = AUDIO_S16SYS;
    desired.channels = 1;
    desired.samples = 2048;
    desired.callback = nullptr;
    
    audio_device = SDL_OpenAudioDevice(nullptr, 0, &desired, &obtained, 0);
    if (audio_device == 0) {
        std::cerr << "Audio device open failed: " << SDL_GetError() << std::endl;
    }
}

void graphics::setVolume(uint8_t volume) {
    current_volume = volume;
    // SDL uses 0-128 volume range
    SDL_SetAudioVolume(audio_device, (volume * 128) / 255);
}

uint8_t graphics::getVolume() const {
    return current_volume;
}
