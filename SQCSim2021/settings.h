#ifndef SETTINGS_H__
#define SETTINGS_H__

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <map>

#include "define.h"
#include "audio.h"

class Settings {
public:
	Settings(Audio& audio);

	void SaveFile(const std::string& filename);
	void LoadFile(const std::string& filename);

	// Audio
	float GetMainVolume() const;
	void SetMainVolume(float volume);

	void GetMusicVolume();
	void SetMusicVolume(float volume);

	float GetSfxVolume() const;
	void SetSfxVolume(float volume);

	// Graphic
	float GetBrightness() const;
	void SetBrightness(float brightness);

	float GetContrast() const;
	void SetContrast(float contrast);

	bool GetFullscreen() const;
	void SetFullscreen(bool fullscreen);

	const Resolution& GetResolution() const;
	void SetResolution(const Resolution& resolution);

	// Gameplay
	float GetMouseSensitivity() const;
	void SetMouseSensitivity(float sensitivity);

	void ApplyResolution(Resolution resolution);

private:

	Audio& m_audio;
	// Audio
	float m_mainVolume;
	float m_musicVolume;
	float m_sfxVolume;

	// Graphic
	Resolution m_resolution;
	bool m_fullscreen;
	int m_rezWidth;
	int m_rezHeight;
	float m_brightness;
	float m_contrast;

	// Gameplay
	float m_mouseSensitivity;
};


#endif // PARAMETERS_H