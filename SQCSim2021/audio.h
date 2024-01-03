#ifndef AUDIO_H__
#define AUDIO_H__

#ifdef _WIN32
#include <irrKlang.h>
#include <ik_ISoundSource.h>
#else
#include "./external/irrKlang-64bit-1.6.0/include/irrKlang.h"
#include "./external/irrKlang-64bit-1.6.0/include/ik_ISoundSource.h"
#endif

#include "../SQCSim-common/vector3.h"
#include "define.h"

class Audio {
private:
	irrklang::ISound* m_music;
	irrklang::ISound* m_menumusic;
	float m_mainvolume;
	float m_sfxvolume;
	bool m_music_on = true;
	std::vector<irrklang::ISound*> m_sfxes;

public:
	Audio();
	Audio(const char* music, const char* menumusic);
	~Audio();
	irrklang::ISoundEngine* m_engine;

	void Update3DAudio(Vector3f pos, Vector3f dir, Vector3f speed);


	irrklang::ISound* Create3DAudioObj(irrklang::ISound* sound, const char* name, Vector3f pos, Vector3f vel, bool is_looped, float volume);

	void Render3DAudioObj(irrklang::ISound* sound, Vector3f& pos, Vector3f& vel, float volume);

	//void PlaySong(const char* music);

	void CleanupSFX();

	void ToggleMusicState(GameState state);

	void playSound(const char* sound, float volume);

	void SetMusic(bool ison, GameState state);
	bool GetMusic();

	void PauseEngine();

	float GetMainVolume() const;
	void SetMainVolume(float volume);

	float GetMusicVolume() const;
	void SetMusicVolume(float volume);

	float GetSfxVolume() const;
	void SetSfxVolume(float volume);
};

#endif // AUDIO_H__
