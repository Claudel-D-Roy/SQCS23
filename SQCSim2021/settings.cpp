#include "settings.h"

Settings::Settings(Audio& audio)
    : m_audio(audio),
    m_mainVolume(0.5f),
    m_musicVolume(0.5f),
    m_sfxVolume(0.5f),
    m_resolution(FHD),
    m_fullscreen(false),
    m_brightness(0.5f),
    m_contrast(0.5f),
    m_mouseSensitivity(0.0f) {
    ApplyResolution(m_resolution);
}

void Settings::SaveFile(const std::string& filename) {
    std::ofstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Failed to open file for saving parameters" << std::endl;
        return;
    }

    file << m_mainVolume << '\n';
    file << m_musicVolume << '\n';
    file << m_sfxVolume << '\n';

    file << static_cast<int>(m_resolution) << '\n';
    file << m_fullscreen << '\n';
    file << m_brightness << '\n';
    file << m_contrast << '\n';

    file << m_mouseSensitivity << '\n';

    file.close();
}

void Settings::LoadFile(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Failed to open file for loading parameters" << std::endl;
        return;
    }

    file >> m_mainVolume;
    file >> m_musicVolume;
    file >> m_sfxVolume;

    int resolutionValue;
    file >> resolutionValue;
    m_resolution = static_cast<Resolution>(resolutionValue);

    file >> m_fullscreen;
    file >> m_brightness;
    file >> m_contrast;

    file >> m_mouseSensitivity;

    file.close();
}

float Settings::GetMainVolume() const {
    return m_mainVolume;
}

void Settings::SetMainVolume(float volume) {
    m_mainVolume = volume;
}

void Settings::GetMusicVolume() {
    m_musicVolume = m_audio.GetMusicVolume();
}

void Settings::SetMusicVolume(float volume) {
    m_musicVolume = volume;
}

float Settings::GetSfxVolume() const {
    return m_sfxVolume;
}

void Settings::SetSfxVolume(float volume) {
    m_sfxVolume = volume;
}

float Settings::GetBrightness() const {
    return m_brightness;
}

void Settings::SetBrightness(float brightness) {
    m_brightness = brightness;
}

float Settings::GetContrast() const {
    return m_contrast;
}

void Settings::SetContrast(float contrast) {
    m_contrast = contrast;
}

bool Settings::GetFullscreen() const {
    return m_fullscreen;
}

void Settings::SetFullscreen(bool fullscreen) {
    m_fullscreen = fullscreen;
}

const Resolution& Settings::GetResolution() const {
    return m_resolution;
}

void Settings::SetResolution(const Resolution& resolution) {
    m_resolution = resolution;
}

float Settings::GetMouseSensitivity() const {
    return m_mouseSensitivity;
}

void Settings::SetMouseSensitivity(float sensitivity) {
    m_mouseSensitivity = sensitivity;
}

void Settings::ApplyResolution(Resolution resolution) {
    switch (resolution) {
    case HD:
        m_rezWidth = 1280;
        m_rezHeight = 720;
        break;
    case FHD:
        m_rezWidth = 1920;
        m_rezHeight = 1080;
        break;
    case QHD:
        m_rezWidth = 2560;
        m_rezHeight = 1440;
        break;
    case UHD:
        m_rezWidth = 3840;
        m_rezHeight = 2160;
        break;
    default:
        break;
    }
}
