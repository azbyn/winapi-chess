#include "SoundManager.h"

#include <cmath>

static uint8_t toNormalVolume(int val) {
    return (val * SoundManager::MaxVolume) / SoundManager::MaxNiceVolume;
}

SoundManager::SoundManager() {
    sfxOn = false;// true;
    volume = toNormalVolume(5);
    // setNiceVolume(5);
}

core::WaveSound<200>& SoundManager::getWav() {
    index = !index;
    return wavs[index];
}
bool SoundManager::shouldPlay() {
    return instance().sfxOn && instance().volume;
}

bool SoundManager::getSfxOn() {
    return instance().sfxOn;
}
void SoundManager::setSfxOn(bool val) {
    instance().sfxOn = val;
}
void SoundManager::toggleSfxOn() {
    instance().sfxOn = !getSfxOn();
}

int SoundManager::getNiceVolume() {
    return std::round(double(getVolume() * MaxNiceVolume) / MaxVolume);
}
void SoundManager::setNiceVolume(int val) {
    return setVolume(toNormalVolume(val));
}

uint8_t SoundManager::getVolume() {
    return instance().volume;
}
void SoundManager::setVolume(uint8_t val) {
    instance().volume = val;
}

using Type = core::WaveSound<200>::Component::Type;
void SoundManager::playPieceCapture() {
    std::cout << "playCapture\n";
    if (!shouldPlay()) return;
    auto& w = instance().getWav();
    w.initSound({
            {Type::Noise,  getVolume()/2},
            {Type::Square, getVolume()/2, 160},
        });
    w.play();
}

void SoundManager::playPieceMove() {
    std::cout << "playPieceMove\n";
    if (!shouldPlay()) return;
    auto& w = instance().getWav();

    w.initSound({
            {Type::Noise, getVolume()/2 },
        });
    w.play();
}
void SoundManager::playSelected() {
    std::cout << "playSelected\n";
    if (!shouldPlay()) return;
    auto& w = instance().getWav();

    w.initSound({
            {Type::Square, getVolume()/2, 160},
        });
    w.play();
}

void SoundManager::playInvalid() {
    std::cout << "playInvalid\n";
    if (!shouldPlay()) return;
    auto& w = instance().getWav();
    w.initSound({
            {Type::Square, getVolume()/2, 110},
        });
    w.play();
}

void SoundManager::playLeftRightMove() {
    std::cout << "playLeftRightMove\n";
    playSelected();
}
