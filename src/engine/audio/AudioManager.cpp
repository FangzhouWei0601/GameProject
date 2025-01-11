#include <iostream>
#include "../../../include/headers/audio/AudioManager.h"
#include "../../../include/headers/resource/ResourceManager.h"

AudioManager::AudioManager()
    : m_soundEngine(nullptr)
    , m_currentBGM(nullptr)
    , m_bgmVolume(1.0f)
    , m_sfxVolume(1.0f) {
}

AudioManager::~AudioManager() {
    shutdown();
}

bool AudioManager::initialize() {
    m_soundEngine = irrklang::createIrrKlangDevice();
    if (!m_soundEngine) {
        std::cerr << "Failed to create sound engine!" << std::endl;
        return false;
    }
    return true;
}

void AudioManager::shutdown() {
    if (m_currentBGM) {
        m_currentBGM->drop();
        m_currentBGM = nullptr;
    }

    for (auto& [name, source] : m_sfxSources) {
        if (source) {
            source->drop();
        }
    }
    m_sfxSources.clear();

    if (m_soundEngine) {
        m_soundEngine->drop();
        m_soundEngine = nullptr;
    }
}

void AudioManager::playBGM(const std::string& name, bool loop) {
    auto* source = ResourceManager::getInstance().getSound(name);
    if (!source || !m_soundEngine) return;

    stopBGM();  // 停止当前BGM

    m_currentBGM = m_soundEngine->play2D(source, loop, true);
    if (m_currentBGM) {
        m_currentBGM->setVolume(m_bgmVolume);
        m_currentBGM->setIsPaused(false);
        m_currentBGMName = name;
    }
}

void AudioManager::stopBGM() {
    if (m_currentBGM) {
        m_currentBGM->stop();
        m_currentBGM->drop();
        m_currentBGM = nullptr;
        m_currentBGMName.clear();
    }
}

void AudioManager::pauseBGM() {
    if (m_currentBGM) {
        m_currentBGM->setIsPaused(true);
    }
}

void AudioManager::resumeBGM() {
    if (m_currentBGM) {
        m_currentBGM->setIsPaused(false);
    }
}

void AudioManager::playSFX(const std::string& name, bool loop) {
    auto* source = ResourceManager::getInstance().getSound(name);
    if (!source || !m_soundEngine) return;

    auto* sound = m_soundEngine->play2D(source, loop, false);
    if (sound) {
        sound->setVolume(m_sfxVolume);
    }
}

void AudioManager::stopSFX(const std::string& name) {
    auto it = m_sfxSources.find(name);
    if (it != m_sfxSources.end()) {
        if (it->second) {
            m_soundEngine->stopAllSoundsOfSoundSource(it->second);
        }
    }
}

void AudioManager::stopAllSFX() {
    if (!m_soundEngine) return;

    for (auto& [name, source] : m_sfxSources) {
        if (source) {
            m_soundEngine->stopAllSoundsOfSoundSource(source);
        }
    }
}

void AudioManager::setBGMVolume(float volume) {
    m_bgmVolume = volume;
    if (m_currentBGM) {
        m_currentBGM->setVolume(volume);
    }
}

void AudioManager::setSFXVolume(float volume) {
    m_sfxVolume = volume;
    if (m_soundEngine) {
        // 直接更新当前所有活跃的声音
        m_soundEngine->setSoundVolume(volume);
    }
}

void AudioManager::playAmbient(const std::string& name, float volume) {
    auto* source = ResourceManager::getInstance().getSound(name);
    if (!source || !m_soundEngine) return;

    // 停止已存在的相同环境音
    stopAmbient(name);

    auto* sound = m_soundEngine->play2D(source, true, true);
    if (sound) {
        sound->setVolume(volume * m_ambientVolume);
        sound->setIsPaused(false);
        m_ambientSounds[name] = sound;
    }
}

void AudioManager::stopAmbient(const std::string& name) {
    auto it = m_ambientSounds.find(name);
    if (it != m_ambientSounds.end()) {
        if (it->second) {
            it->second->stop();
            it->second->drop();
        }
        m_ambientSounds.erase(it);
    }
}

void AudioManager::stopAllAmbient() {
    for (auto& [name, sound] : m_ambientSounds) {
        if (sound) {
            sound->stop();
            sound->drop();
        }
    }
    m_ambientSounds.clear();
}

void AudioManager::setAmbientVolume(float volume) {
    m_ambientVolume = volume;
    for (auto& [name, sound] : m_ambientSounds) {
        if (sound) {
            sound->setVolume(volume);
        }
    }
}