	#include "audio.h"

Audio::Audio() {
	m_engine = irrklang::createIrrKlangDevice();
	m_engine->setDopplerEffectParameters(1);
	m_engine->setRolloffFactor(1);
	m_engine->setDefault3DSoundMinDistance(1);
	m_engine->setDefault3DSoundMaxDistance(1000);
}

Audio::Audio(const char * music, const char* menumusic) {
	m_engine = irrklang::createIrrKlangDevice();
	m_engine->setDopplerEffectParameters(1);
	m_engine->setRolloffFactor(1);
	m_engine->setDefault3DSoundMinDistance(1);
	m_engine->setDefault3DSoundMaxDistance(100);
	m_music = m_engine->play2D(music, true, true, true, irrklang::ESM_STREAMING);
	m_menumusic = m_engine->play2D(menumusic, true, true, true, irrklang::ESM_STREAMING);
	m_music->setVolume(.5);
	m_menumusic->setVolume(.5);
	m_mainvolume = 0.5f;
	m_engine->setSoundVolume(m_mainvolume);
	m_sfxvolume = 0.5f;
}

Audio::~Audio() { 
	if (m_music) m_music->drop();
	if (m_menumusic) m_menumusic->drop();
	if (m_engine) m_engine->drop();
}

void Audio::playSound(const char* name, float volume = 1.) {
	irrklang::ISound* sfx = m_engine->play2D(name, false, true);
	sfx->setVolume(volume);
	sfx->setIsPaused(false);
	m_sfxes.push_back(sfx);
}

void Audio::Update3DAudio(Vector3f pos, Vector3f dir, Vector3f vel) {
	m_engine->setListenerPosition(irrklang::vec3df(pos.x, pos.y, pos.z),
								  irrklang::vec3df(dir.x, dir.y, dir.z),
								  irrklang::vec3df(vel.x, vel.y, vel.z));
}

irrklang::ISound* Audio::Create3DAudioObj(irrklang::ISound* sound, const char* name, Vector3f pos, Vector3f vel, bool is_looped = false, float volume = 1) {
	sound = m_engine->play3D(name, irrklang::vec3df(pos.x, pos.y, pos.z), is_looped, true, true, is_looped? irrklang::ESM_STREAMING: irrklang::ESM_NO_STREAMING, true);
	sound->setVelocity(irrklang::vec3df(vel.x, vel.y, vel.z));
	sound->setVolume(volume);
	sound->setIsPaused(false);
	return sound;
}

void Audio::Render3DAudioObj(irrklang::ISound* sound, Vector3f& pos, Vector3f& vel, float volume = 1) {
	sound->setPosition(irrklang::vec3df(pos.x, pos.y, pos.z));
	sound->setVelocity(irrklang::vec3df(vel.x, vel.y, vel.z));
	sound->setVolume(volume);
}

//void Audio::PlaySong(const char* music) {
//	m_music = m_engine->play2D(music, false, false, false, irrklang::ESM_STREAMING);
//}

void Audio::CleanupSFX() {
	while (!m_sfxes.empty()) {
		irrklang::ISound* sfx = m_sfxes.back();
		if (sfx->isFinished()) {
			sfx->drop(); // drop() fait deja la job du delete sfx.
		}
		else break;
		m_sfxes.pop_back();
	}
}

void Audio::ToggleMusicState(GameState state) {
	if (m_music_on) {
		switch (state) {
		case PLAY:
			m_music->setIsPaused(false);
			m_menumusic->setIsPaused(true);
			break;
		case PAUSE:
			m_music->setIsPaused(true);
			m_menumusic->setIsPaused(true);
			break;
		default:
			m_music->setIsPaused(true);
			m_menumusic->setIsPaused(false);
			break;
		}
	}
	else {
		m_music->setIsPaused(true);
		m_menumusic->setIsPaused(true);
	}
}

void Audio::SetMusic(bool ison, GameState state) {
	m_music_on = state;
	if (!state) {
		m_music->setIsPaused(true);
		m_menumusic->setIsPaused(true);
	}
	else ToggleMusicState(state);
}

bool Audio::GetMusic() {
	return m_music_on;
}

void Audio::PauseEngine() { m_engine->setAllSoundsPaused(); }

float Audio::GetMusicVolume() const {
	return m_music->getVolume();
}

void Audio::SetMusicVolume(float volume) {
	m_music->setVolume(volume);
	m_menumusic->setVolume(volume);
}

float Audio::GetMainVolume() const {
	return m_engine->getSoundVolume();
}

void Audio::SetMainVolume(float volume) {
	m_engine->setSoundVolume(volume);
}

float Audio::GetSfxVolume() const {
	return m_sfxvolume;
}

void Audio::SetSfxVolume(float volume) {
	m_sfxvolume = volume;
}
