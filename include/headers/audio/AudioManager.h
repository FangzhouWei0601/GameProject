#pragma once
#include <irrklang/irrKlang.h>
#include <string>
#include <unordered_map>
#include <memory>

class AudioManager {
public:
    static AudioManager& getInstance() {
        static AudioManager instance;
        return instance;
    }

    bool initialize();
    void shutdown();

    // BGM controls
    void playBGM(const std::string& name, bool loop = true);
    void stopBGM();
    void pauseBGM();
    void resumeBGM();

    // SFX controls  
    void playSFX(const std::string& name, bool loop = false);
    void stopSFX(const std::string& name);
    void stopAllSFX();

    // ambient controls
    void playAmbient(const std::string& name, float volume = 1.0f);
    void stopAmbient(const std::string& name);
    void stopAllAmbient();
    void setAmbientVolume(float volume);

    // Volume controls
    void setBGMVolume(float volume);
    void setSFXVolume(float volume);
    float getBGMVolume() const { return m_bgmVolume; }
    float getSFXVolume() const { return m_sfxVolume; }

    // Getters
    bool isBGMPlaying() const { return m_currentBGM != nullptr; }
    const std::string& getCurrentBGM() const { return m_currentBGMName; }

private:
    AudioManager();
    ~AudioManager();

    irrklang::ISoundEngine* m_soundEngine;
    irrklang::ISound* m_currentBGM;
    std::string m_currentBGMName;

    float m_bgmVolume;
    float m_sfxVolume;
    float m_ambientVolume;

    std::unordered_map<std::string, irrklang::ISoundSource*> m_sfxSources;
    std::unordered_map<std::string, irrklang::ISound*> m_ambientSounds;
};