#pragma once

#include "Utils.h"
#include <vector>

#ifdef _MSC_VER
#pragma comment(lib, "winmm.lib")
#endif

namespace core {

template <size_t DurationMs,
          // size_t SampleRate = 8000,
          size_t SampleRate = 44100,
          size_t NumChannels = 1,
          size_t BitsPerSample = 8>
class WaveSound {
public:
    constexpr static size_t NumSamples = size_t(DurationMs * SampleRate / 1000);
    // the same as subChunk2Size
    constexpr static size_t ByteSize =
        NumSamples * NumChannels * BitsPerSample/8;

private:
    // from https://stackoverflow.com/questions/1451606
    // The order here is important
    DWORD chunkID;       // 0x46464952 "RIFF" in little endian
    DWORD chunkSize;     // 4 + (8 + subChunk1Size) + (8 + subChunk2Size)
    DWORD format;        // 0x45564157 "WAVE" in little endian

    DWORD subChunk1ID;   // 0x20746d66 "fmt " in little endian
    DWORD subChunk1Size; // 16 for PCM
    WORD  audioFormat;   // 1 for PCM, 3 for EEE floating point, 7 for mu-law
    WORD  numChannels;   // 1 for mono, 2 for stereo
    DWORD sampleRate;    // 8000, 22050, 44100, etc...
    DWORD byteRate;      // sampleRate * numChannels * bitsPerSample/8
    WORD  blockAlign;    // numChannels * bitsPerSample/8
    WORD  bitsPerSample; // number of bits (8 for 8 bits, etc...)

    DWORD subChunk2ID;   // 0x61746164 "data" in little endian
    DWORD subChunk2Size; // numSamples * numChannels * bitsPerSample/8

    uint8_t data[ByteSize];

public:

    WaveSound() {
        chunkID = 0x46464952; // "RIFF" in little endian
        subChunk1Size = 16; // 16 for PCM
        format  = 0x45564157; // "WAVE" in little endian

        subChunk1ID = 0x20746d66; // "fmt " in little endian
        audioFormat = 1; // 1 for PCM, 3 fot EEE floating point, 7 for μ-law
        numChannels = NumChannels; // 1 for mono, 2 for stereo
        sampleRate = SampleRate; // 8000, 22050, 44100, etc...
        bitsPerSample = BitsPerSample; // number of bits (8 for 8 bits, etc...)

        blockAlign = NumChannels * BitsPerSample/8;
        byteRate = SampleRate * NumChannels * BitsPerSample/8;

        subChunk2ID = 0x61746164; // "data" in little endian
        subChunk2Size = ByteSize; //(this is the actual data size in bytes)
        chunkSize = 4 + (8 + subChunk1Size) + (8 + subChunk2Size);
    }

    void play(bool async = true) const {
        auto flags = SND_MEMORY | (async ? SND_ASYNC : 0);
        if (!::PlaySound((LPCWSTR)this, nullptr, flags))
            throw core::WinapiError("PlaySound failed");
    }
    void stop(bool async = true) const {
        auto flags = SND_MEMORY | (async ? SND_ASYNC : 0);
        if (!::PlaySound(nullptr, nullptr, flags))
            throw core::WinapiError("PlaySound failed");
    }
    struct Component {
        enum class Type {
            Square, Sin, Noise
        };
        Type type;
        uint8_t volume;
        size_t delay;
        size_t t;
        Component(Type type, int volume = 255, size_t frequency = 440,
             double offset = 0)
                : type(type), volume(volume),
                  delay(SampleRate / (frequency * 2)),
                  t(size_t(delay * offset-1)) {}

        uint8_t getVal() {
            if (++t >= delay*2)
                t = 0;
            switch (type) {
            case Component::Type::Noise:
                return rand() % volume;
                break;
            case Component::Type::Sin:
                return (t < delay) ? ((volume * t) / delay)
                                   : (volume * (2 * delay - t)) / delay;
            case Component::Type::Square:
                return (t < delay) ? 0 : volume;
            }
            return 0;
        }
    };
    void initSound(std::initializer_list<Component> components,
                   size_t t0 = ByteSize/5, size_t t1 = (2*ByteSize) / 5) {
        std::vector comps =components;
        auto getVal = [&] () -> uint8_t {
            uint8_t res = 0;
            for (auto& w : comps)
                res += w.getVal();
            return res;
        };
        size_t i = 0;
        for (; i < t0; ++i)
            data[i] = uint8_t((getVal() *i) / t0);
        for (; i < t1; ++i)
            data[i] = getVal();
        size_t dur = ByteSize - t1;
        for (; i < ByteSize; ++i)
            data[i] = uint8_t((getVal() * (ByteSize - i)) / dur);
    }
};

} // namespace core
