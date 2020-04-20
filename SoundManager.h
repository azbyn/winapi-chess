#pragma once

#include "core/WaveSound.h"

#include <array>

class SoundManager {
public:
    constexpr static uint8_t MinVolume = 0;
    constexpr static uint8_t MaxVolume = 255;
    
    constexpr static int MinNiceVolume = 0;
    constexpr static int MaxNiceVolume = 10;
    static SoundManager& instance() {
        static SoundManager i;
        return i;
    }

    static bool getSfxOn();
    static void setSfxOn(bool val);
    static void toggleSfxOn();

    //the ui wants nicer values like 0 to 10
    static int getNiceVolume();
    static void setNiceVolume(int val);


    static void playPieceCapture();
    static void playPieceMove();
    static void playSelected();
    static void playInvalid();
    static void playLeftRightMove();

private:
    SoundManager();
    static uint8_t getVolume();
    static void setVolume(uint8_t val);
    static bool shouldPlay();

    std::array<core::WaveSound<200>, 2> wavs;
    int index;
    core::WaveSound<200>& getWav();
    bool sfxOn;
    uint8_t volume;
};
