#include "engine.h"
#include <algorithm>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <chrono>
#include <thread>
#include <queue>

// Define a structure to represent notifications
struct Notification {
	std::string message;
	float displayStartTime = 0.0f;
};

// Use a queue to manage notifications
//std::queue<Notification> notificationQueue;
// Use a vector to manage notifications
std::vector<Notification> notifications;

Engine::Engine() {}

Engine::~Engine() {
	m_world.CleanUpWorld(m_renderCount, true);
	for (int x = 0; x < WORLD_SIZE_X; ++x)
		for (int y = 0; y < WORLD_SIZE_Y; ++y)
			if (m_world.GetChunks().Get(x, y))
				m_world.GetChunks().Get(x, y)->~Chunk();
}

void Engine::Init() {
	glDisable(GL_FRAMEBUFFER_SRGB);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_STENCIL_TEST);
	glEnable(GL_BLEND);
	glEnable(GL_CULL_FACE);
	glEnable(GL_TEXTURE_2D);
	glEnable(GL_MULTISAMPLE_ARB);
	glEnable(GL_POINT_SMOOTH);
	glEnable(GL_LINE_SMOOTH);
	glEnable(GL_POLYGON_SMOOTH);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	gluPerspective(45.0f, (float)Width() / (float)Height(), 0.1f, VIEW_DISTANCE);
	glShadeModel(GL_SMOOTH);

	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
	glDisable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glBlendEquation(GL_FUNC_SUBTRACT);

	m_audio.ToggleMusicState(m_gamestate);

	if (m_istarted)
		return;
	else m_istarted = true;

	// Objet de skybox avec sa propre texture et son propre shader!
	m_skybox.Init(0.2f);
	// Objet de musique!
	
	// Array pour les balles.
	for (int x = 0; x < MAX_BULLETS; ++x) {
		m_bullets[x] = nullptr;
		m_whoosh[x] = nullptr;
	}

	m_world.GetChunks().Reset(nullptr);
	m_world.SetSeed(SEED);
}

void Engine::DeInit() {}

void Engine::LoadResource() {
	GLenum glewErr = glewInit();
	if (glewErr != GLEW_OK) {
		std::cerr << " ERREUR GLEW : " << glewGetErrorString(glewErr) << std::endl;
		abort();
	}
	LoadTexture(m_skybox.GetTexture(), TEXTURE_PATH "skybox.png", true);
	LoadTexture(m_textureCrosshair, TEXTURE_PATH "cross.bmp", true);
	LoadTexture(m_textureFont, TEXTURE_PATH "font.bmp", true);
	LoadTexture(m_textureGun, TEXTURE_PATH "gun01.png", false);
	LoadTexture(m_texturePovGun, TEXTURE_PATH "GUN.png", false);

	LoadTexture(m_textureLobbyMenu, TEXTURE_PATH "menus/backgrounds/bgLobby.png", false);
	LoadTexture(m_textureMainMenu, TEXTURE_PATH "menus/backgrounds/bgMainMenu.png", false);
	LoadTexture(m_texturePauseMenu, TEXTURE_PATH "menus/backgrounds/bgPause.png", false);
	LoadTexture(m_textureOptionsMenu, TEXTURE_PATH "menus/backgrounds/bgOptions.png", false);
	LoadTexture(m_textureSplashScreen, TEXTURE_PATH "menus/backgrounds/bgSplash.png", false);

	LoadTexture(m_textureCheck, TEXTURE_PATH "menus/others/check.png", false);
	LoadTexture(m_textureChecked, TEXTURE_PATH "menus/others/checked.png", false);

	LoadTexture(m_textureOptAudio, TEXTURE_PATH "menus/buttons/options/optAudio.png", false);
	LoadTexture(m_textureOptBack, TEXTURE_PATH "menus/buttons/options/optBack.png", false);
	LoadTexture(m_textureOptGameplay, TEXTURE_PATH "menus/buttons/options/optGameplay.png", false);
	LoadTexture(m_textureOptGraphics, TEXTURE_PATH "menus/buttons/options/optGraphics.png", false);
	LoadTexture(m_textureOptMain, TEXTURE_PATH "menus/buttons/options/optMain.png", false);
	LoadTexture(m_textureOptMusic, TEXTURE_PATH "menus/buttons/options/optMusic.png", false);
	LoadTexture(m_textureOptOptions, TEXTURE_PATH "menus/buttons/options/optOptions.png", false);
	LoadTexture(m_textureOptResolution, TEXTURE_PATH "menus/buttons/options/optResolution.png", false);
	LoadTexture(m_textureOptSensitivity, TEXTURE_PATH "menus/buttons/options/optSensitivity.png", false);
	LoadTexture(m_textureOptSfx, TEXTURE_PATH "menus/buttons/options/optSfx.png", false);

	LoadTexture(m_textureLobbyServer, TEXTURE_PATH "menus/labels/labelServer.png", false);
	LoadTexture(m_textureLobbyIdentify, TEXTURE_PATH "menus/labels/labelIdentify.png", false);
	LoadTexture(m_textureHd, TEXTURE_PATH "menus/labels/labelHd.png", false);
	LoadTexture(m_textureHd, TEXTURE_PATH "menus/labels/labelHd.png", false);
	LoadTexture(m_textureFhd, TEXTURE_PATH "menus/labels/labelFhd.png", false);
	LoadTexture(m_textureQhd, TEXTURE_PATH "menus/labels/labelQhd.png", false);
	LoadTexture(m_textureUhd, TEXTURE_PATH "menus/labels/labelUhd.png", false);
	LoadTexture(m_textureMenuTitle, TEXTURE_PATH "menus/labels/labelTitle.png", false);

	LoadTexture(m_texturePauseResume, TEXTURE_PATH "menus/buttons/pause/pauseResume.png", false);
	LoadTexture(m_texturePauseMainMenu, TEXTURE_PATH "menus/buttons/pause/pauseMainMenu.png", false);

	LoadTexture(m_textureMenuBack, TEXTURE_PATH "menus/buttons/main/mainBack.png", false);
	LoadTexture(m_textureMenuMulti, TEXTURE_PATH "menus/buttons/main/mainMulti.png", false);
	LoadTexture(m_textureMenuOptions, TEXTURE_PATH "menus/buttons/main/mainOptions.png", false);
	LoadTexture(m_textureMenuPlay, TEXTURE_PATH "menus/buttons/main/mainPlay.png", false);
	LoadTexture(m_textureMenuQuit, TEXTURE_PATH "menus/buttons/main/mainQuit.png", false);
	LoadTexture(m_textureMenuSingle, TEXTURE_PATH "menus/buttons/main/mainSingle.png", false);

	TextureAtlas::TextureIndex texDirtIndex = m_textureAtlas.AddTexture(TEXTURE_PATH "metal2.png"); //0
	TextureAtlas::TextureIndex texIceIndex = m_textureAtlas.AddTexture(TEXTURE_PATH "metal3.png"); //1
	TextureAtlas::TextureIndex texGrassIndex = m_textureAtlas.AddTexture(TEXTURE_PATH "grass.png"); //2
	TextureAtlas::TextureIndex texMetalIndex = m_textureAtlas.AddTexture(TEXTURE_PATH "dirt.png"); //3
	TextureAtlas::TextureIndex texGreenGrassIndex = m_textureAtlas.AddTexture(TEXTURE_PATH "greengrass.png"); //4
	TextureAtlas::TextureIndex texBoostHeal = m_textureAtlas.AddTexture(BOOSTER_TEXTURE_PATH "BoosterVert.png"); //5
	TextureAtlas::TextureIndex texBoostDmg = m_textureAtlas.AddTexture(BOOSTER_TEXTURE_PATH "BoosterRouge.png"); //6
	TextureAtlas::TextureIndex texBoostSpd = m_textureAtlas.AddTexture(BOOSTER_TEXTURE_PATH "BoosterBleu.png"); //7
	TextureAtlas::TextureIndex texBoostInv = m_textureAtlas.AddTexture(BOOSTER_TEXTURE_PATH "BoosterJaune.png"); //8

	//AJOUTER LES TEXTURES DANS L'ORDRE DE L'ÉNUM 

	//STILL//STANDING
	TextureAtlas::TextureIndex StillFront = m_animeAtlas.AddTexture(ANIME_PATH_STILL "BlueFrontRight.png"); //0
	TextureAtlas::TextureIndex StillQuarterFrontLeft = m_animeAtlas.AddTexture(ANIME_PATH_STILL "BlueLeft.png"); //1
	TextureAtlas::TextureIndex StillQuarterFrontRight = m_animeAtlas.AddTexture(ANIME_PATH_STILL "BlueRight.png"); //2
	TextureAtlas::TextureIndex StillProfiltLeft = m_animeAtlas.AddTexture(ANIME_PATH_STILL "BlueProfilLeft.png"); //3
	TextureAtlas::TextureIndex StillProfiltRight = m_animeAtlas.AddTexture(ANIME_PATH_STILL "BlueProfilRight.png"); //4
	TextureAtlas::TextureIndex StillQuarterBackLeft = m_animeAtlas.AddTexture(ANIME_PATH_STILL "BlueLeftBack.png"); //5
	TextureAtlas::TextureIndex StillQuarterBackRight = m_animeAtlas.AddTexture(ANIME_PATH_STILL "BlueRightBack.png"); //6 
	TextureAtlas::TextureIndex StillBack = m_animeAtlas.AddTexture(ANIME_PATH_STILL "BlueBackRight.png"); //7

	//SHOOTINGSTILL SANS TIRER
	TextureAtlas::TextureIndex StillFrontShoot = m_animeAtlas.AddTexture(ANIM_PATH_SSHOOT1 "BlueFrontRightShootingRight.png"); ////9
	TextureAtlas::TextureIndex StillQuarterFrontLeftShoot = m_animeAtlas.AddTexture(ANIM_PATH_SSHOOT1 "BlueFrontRightShootingRight.png"); ////10
	TextureAtlas::TextureIndex StillQuarterFrontRightShoot = m_animeAtlas.AddTexture(ANIM_PATH_SSHOOT1 "BlueRightShootingRight.png"); ////11
	TextureAtlas::TextureIndex StillProfiltLeftShoot = m_animeAtlas.AddTexture(ANIM_PATH_SSHOOT1 "BlueProfilShootingLeft.png"); ////12
	TextureAtlas::TextureIndex StillProfiltRightShoot = m_animeAtlas.AddTexture(ANIM_PATH_SSHOOT1 "BlueProfilShootingRight.png"); ////13
	TextureAtlas::TextureIndex StillQuarterBackLeftShoot = m_animeAtlas.AddTexture(ANIM_PATH_SSHOOT1 "BlueBackLeftShootingLeft.png"); ////14
	TextureAtlas::TextureIndex StillQuarterBackRightShoot = m_animeAtlas.AddTexture(ANIM_PATH_SSHOOT1 "BlueBackRightShootingRight.png"); ////15
	TextureAtlas::TextureIndex StillBackShoot = m_animeAtlas.AddTexture(ANIM_PATH_SSHOOT1 "BlueShootingBackRight.png"); ////16

	//SHOOTINGSTILL TIRER
	TextureAtlas::TextureIndex StillFrontShootFire = m_animeAtlas.AddTexture(ANIM_PATH_SSHOOT2  "BlueFrontRightShootingRightShoot1.png"); ////17
	TextureAtlas::TextureIndex StillQuarterFrontLeftFire = m_animeAtlas.AddTexture(ANIM_PATH_SSHOOT2  "BlueLeftShootingLeftShoot1.png"); ////18
	TextureAtlas::TextureIndex StillQuarterFrontRightShootFire = m_animeAtlas.AddTexture(ANIM_PATH_SSHOOT2  "BlueRightShootingRightShoot1.png"); ////19
	TextureAtlas::TextureIndex StillProfiltLeftShootFire = m_animeAtlas.AddTexture(ANIM_PATH_SSHOOT2  "BlueProfilShootingLeftShoot1.png"); ////20
	TextureAtlas::TextureIndex StillProfiltRightShootFire = m_animeAtlas.AddTexture(ANIM_PATH_SSHOOT2  "BlueProfilShootingRightShoot1.png"); ////21 
	TextureAtlas::TextureIndex StillQuarterBackLeftShootFire = m_animeAtlas.AddTexture(ANIM_PATH_SSHOOT2  "BlueBackLeftShootingLeftShoot1.png"); ////22
	TextureAtlas::TextureIndex StillQuarterBackRightShootFire = m_animeAtlas.AddTexture(ANIM_PATH_SSHOOT2  "BlueBackRightShootingRightShoot1.png"); ////23
	TextureAtlas::TextureIndex StillBackShootFire = m_animeAtlas.AddTexture(ANIM_PATH_SSHOOT2  "BlueShootingBackRightShoot1.png"); ////24

	//JUMP
	TextureAtlas::TextureIndex JumpFront = m_animeAtlas.AddTexture(ANIME_PATH_JUMP "BlueFrontJumpRight.png"); ////25
	TextureAtlas::TextureIndex JumpQuarterFrontLeft = m_animeAtlas.AddTexture(ANIME_PATH_JUMP "BlueLeftFrontJumpLeft.png"); ////26
	TextureAtlas::TextureIndex JumpQuarterFrontRight = m_animeAtlas.AddTexture(ANIME_PATH_JUMP "BlueRightFrontJumpRight.png"); ////27
	TextureAtlas::TextureIndex JumpProfiltLeft = m_animeAtlas.AddTexture(ANIME_PATH_JUMP "BlueProfilJumpLeft.png"); ////28
	TextureAtlas::TextureIndex JumpProfiltRight = m_animeAtlas.AddTexture(ANIME_PATH_JUMP "BlueProfilJumpRight.png"); ////29
	TextureAtlas::TextureIndex JumpQuarterBackLeft = m_animeAtlas.AddTexture(ANIME_PATH_JUMP "BlueLeftBackJumpLeft.png"); ////30
	TextureAtlas::TextureIndex JumpQuarterBackRight = m_animeAtlas.AddTexture(ANIME_PATH_JUMP "BlueRightBackJumpRight.png"); ////31
	TextureAtlas::TextureIndex JumpBack = m_animeAtlas.AddTexture(ANIME_PATH_JUMP "BlueBackJumpRight.png"); ////32

	//SHOOTINGJUMP SANS TIRER  
	TextureAtlas::TextureIndex JumpFrontShoot = m_animeAtlas.AddTexture(ANIM_PATH_JSHOOT1  "BlueFrontJumpRightShootingRight.png"); ////33
	TextureAtlas::TextureIndex JumpQuarterFrontLeftShoot = m_animeAtlas.AddTexture(ANIM_PATH_JSHOOT1  "BlueFrontLeftJumpLeftShootingLeft.png"); ////34
	TextureAtlas::TextureIndex JumpQuarterFrontRightShoot = m_animeAtlas.AddTexture(ANIM_PATH_JSHOOT1  "BlueFrontRightJumpRightShootingRight.png"); ////35
	TextureAtlas::TextureIndex JumpProfiltLeftShoot = m_animeAtlas.AddTexture(ANIM_PATH_JSHOOT1  "BlueProfilLeftJumpLeftShootingLeft.png");  ////36
	TextureAtlas::TextureIndex JumpProfiltRightShoot = m_animeAtlas.AddTexture(ANIM_PATH_JSHOOT1  "BluerProfilRightJumprightShootingRight.png"); ////37
	TextureAtlas::TextureIndex JumpQuarterBackLeftShoot = m_animeAtlas.AddTexture(ANIM_PATH_JSHOOT1  "BlueBackLeftJumpLeftShootingLeft.png"); ////38
	TextureAtlas::TextureIndex JumpQuarterBackRightShoot = m_animeAtlas.AddTexture(ANIM_PATH_JSHOOT1  "BlueBackRightJumpRightShootingRight.png"); ////39
	TextureAtlas::TextureIndex JumpBackShoot = m_animeAtlas.AddTexture(ANIM_PATH_JSHOOT1  "BlueBackJumpRightShootingRight.png"); ////40

	//SHOOTINGJUMP TIRER
	TextureAtlas::TextureIndex JumpFrontShootFire = m_animeAtlas.AddTexture(ANIM_PATH_JSHOOT2  "BlueFrontJumpRightShootingRightShoot1.png"); ////41
	TextureAtlas::TextureIndex JumpQuarterFrontLeftShootFire = m_animeAtlas.AddTexture(ANIM_PATH_JSHOOT2  "BlueFrontLeftJumpLeftShootingLeftShoot1.png"); ////42
	TextureAtlas::TextureIndex JumpQuarterFrontRightShootFire = m_animeAtlas.AddTexture(ANIM_PATH_JSHOOT2  "BlueFrontRightJumpRightShootingRightShoot1.png"); ////43
	TextureAtlas::TextureIndex JumpProfiltLeftShootFire = m_animeAtlas.AddTexture(ANIM_PATH_JSHOOT2  "BlueProfilLeftJumpLeftShootingLeftShoot1.png"); ////44
	TextureAtlas::TextureIndex JumpProfiltRightShootFire = m_animeAtlas.AddTexture(ANIM_PATH_JSHOOT2  "BluerProfilRightJumprightShootingRightShoot1.png"); ////45
	TextureAtlas::TextureIndex JumpQuarterBackLeftShootFire = m_animeAtlas.AddTexture(ANIM_PATH_JSHOOT2  "BlueBackLeftJumpLeftShootingLeftShoot1.png"); ////46
	TextureAtlas::TextureIndex JumpQuarterBackRightShootFire = m_animeAtlas.AddTexture(ANIM_PATH_JSHOOT2  "BlueBackRightJumpRightShootingRightShoot1.png"); ////47
	TextureAtlas::TextureIndex JumpBackShootFire = m_animeAtlas.AddTexture(ANIM_PATH_JSHOOT2  "BlueBackJumpRightShootingRightShoot1.png"); ////48

	if (!m_animeAtlas.Generate(TEXTURE_SIZE, false)) {
		std::cout << " Unable to generate texture atlas ..." << std::endl;
		abort();
	}

	if (!m_textureAtlas.Generate(TEXTURE_SIZE, false)) {
		std::cout << " Unable to generate texture atlas ..." << std::endl;
		abort();
	}

	float u, v, s;
	m_textureAtlas.TextureIndexToCoord(texDirtIndex, u, v, s, s);
	m_blockinfo[BTYPE_DIRT] = new BlockInfo(BTYPE_DIRT, "Dirt", u, v, s, 1);
	m_textureAtlas.TextureIndexToCoord(texGrassIndex, u, v, s, s);
	m_blockinfo[BTYPE_GRASS] = new BlockInfo(BTYPE_GRASS, "Grass", u, v, s, 1);
	m_textureAtlas.TextureIndexToCoord(texMetalIndex, u, v, s, s);
	m_blockinfo[BTYPE_METAL] = new BlockInfo(BTYPE_METAL, "Metal", u, v, s, 1);
	m_textureAtlas.TextureIndexToCoord(texIceIndex, u, v, s, s);
	m_blockinfo[BTYPE_ICE] = new BlockInfo(BTYPE_ICE, "Ice", u, v, s, 1);
	m_textureAtlas.TextureIndexToCoord(texGreenGrassIndex, u, v, s, s);
	m_blockinfo[BTYPE_GREENGRASS] = new BlockInfo(BTYPE_GREENGRASS, "GreenGrass", u, v, s, 1);
	m_textureAtlas.TextureIndexToCoord(texBoostHeal, u, v, s, s);
	m_boostinfo[BTYPE_HEAL] = new BoostInfo(BTYPE_HEAL, "Heal", u, v, s, 1);
	m_textureAtlas.TextureIndexToCoord(texBoostDmg, u, v, s, s);
	m_boostinfo[BTYPE_DAMAGE] = new BoostInfo(BTYPE_DAMAGE, "Dmg", u, v, s, 1);
	m_textureAtlas.TextureIndexToCoord(texBoostSpd, u, v, s, s);
	m_boostinfo[BTYPE_SPEED] = new BoostInfo(BTYPE_SPEED, "Spd", u, v, s, 1);
	m_textureAtlas.TextureIndexToCoord(texIceIndex, u, v, s, s);
	m_boostinfo[BTYPE_INVINCIBLE] = new BoostInfo(BTYPE_INVINCIBLE, "Inv", u, v, s, 1);

	m_animeAtlas.TextureIndexToCoord(0, u, v, s, s);

	std::cout << " Loading and compiling shaders ..." << std::endl;
	if (!m_shader01.Load(SHADER_PATH "shader01.vert", SHADER_PATH "shader01.frag", true)) {
		std::cout << " Failed to load shader " << std::endl;
		exit(1);
	}

	if (!m_skybox.GetShader().Load(SHADER_PATH "skybox.vert", SHADER_PATH "skybox.frag", true)) {
		std::cout << " Failed to load shader " << std::endl;
		exit(1);
	}

	// Settings
	m_mainvolume = m_audio.GetMainVolume();
	m_options.SetMainVolume(m_mainvolume);

	m_musicvolume = m_audio.GetMusicVolume();
	m_options.SetMusicVolume(m_musicvolume);

	m_sfxvolume = m_audio.GetSfxVolume();
	m_options.SetSfxVolume(m_sfxvolume);

	m_sensitivity = m_player.GetSensitivity();
	m_options.SetMouseSensitivity(m_sensitivity);
}

void Engine::UnloadResource() {}

void Engine::InstantDamage() {
	m_player.InflictDamage(0.10f);
	m_damage = false;
}

void Engine::SystemNotification(std::string systemLog) {
	std::string message = "";

	message = systemLog;

	DisplayNotification(message);
}

void Engine::KillNotification(Player killer, Player killed) {
	std::string message = "";

	message = killed.GetUsername() + " killed by -> " + killer.GetUsername();

	DisplayNotification(message);
}

void Engine::DisplayNotification(std::string message) {

	if (message.length() > 45) {
		message = message.substr(0, 45);
	}
	// Create a new notification and add it to the queue
	Notification newNotification;
	newNotification.message = message;
	newNotification.displayStartTime = m_time;

	notifications.push_back(newNotification);
}

// Add a method to process the notification queue
void Engine::ProcessNotificationQueue() {
	//PrintText(fPosX, fUsernamePosY, ss.str(), 1.5f);
	//float fPosX = (Width() / 100.0f) * scaleX;
	//float fPosY = Height() - (Height() * 0.05) * scaleY;

	m_textureFont.Bind();
	float xOffset = Width() * 0.66f;
	float yOffset = Height() * 0.83f;

	for (auto it = notifications.begin(); it != notifications.end(); ) {
		float timeSinceDisplay = m_time - it->displayStartTime;

		float y = yOffset - (20.0f * (it - notifications.begin()));

		glDisable(GL_STENCIL_TEST);
		glDisable(GL_DEPTH_TEST);

		glBlendFunc(GL_SRC_ALPHA, GL_ONE);
		glBlendEquation(GL_FUNC_ADD);

		glMatrixMode(GL_PROJECTION);
		glPushMatrix();

		glLoadIdentity();
		glOrtho(0, Width(), 0, Height(), -1, 1);

		glMatrixMode(GL_MODELVIEW);
		glPushMatrix();

		PrintText(xOffset, y, it->message);

		glBlendFunc(GL_CONSTANT_COLOR, GL_ONE_MINUS_CONSTANT_COLOR);
		glBlendEquation(GL_FUNC_SUBTRACT);

		glEnable(GL_STENCIL_TEST);
		glEnable(GL_DEPTH_TEST);

		glMatrixMode(GL_PROJECTION);
		glPopMatrix();

		glMatrixMode(GL_MODELVIEW);
		glPopMatrix();

		if (timeSinceDisplay >= 4.0f) {
			it = notifications.erase(it);
		}
		else {
			++it;
		}
	}
}

void Engine::DisplayCrosshair() {
	m_textureCrosshair.Bind();
	static const int crossSize = 32;
	glLoadIdentity();
	glTranslated(Width() / 2 - crossSize / 2, Height() / 2 - crossSize / 2, 0);
	glBegin(GL_QUADS);
	glTexCoord2f(0, 0); glVertex2i(0, 0);
	glTexCoord2f(1, 0); glVertex2i(crossSize, 0);
	glTexCoord2f(1, 1); glVertex2i(crossSize, crossSize);
	glTexCoord2f(0, 1); glVertex2i(0, crossSize);
	glEnd();
}

void Engine::DisplayCurrentItem() {}

void Engine::DisplayHud(int timer) {
	glBindTexture(GL_TEXTURE_2D, 0);
	glLoadIdentity();
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glClear(GL_STENCIL_BUFFER_BIT);

	float itemBackgroundWidthProportion = 0.25f;
	float itemBackgroundHeightProportion = 0.175f;
	float itemBackgroundWidth = Width() * itemBackgroundWidthProportion;
	float itemBackgroundHeight = Height() * itemBackgroundHeightProportion;
	float itemBackgroundXOffset = Width() * 0.05f;
	float itemBackgroundYOffset = Height() * 0.6f;
	float itemBackgroundXPos = itemBackgroundXOffset;
	float itemBackgroundYPos = Height() - itemBackgroundHeight - itemBackgroundYOffset;

	// Selected item background
	glColor4f(1.0f, 1.0f, 1.0f, 0.2f);
	glBegin(GL_QUADS);
	glVertex2f(itemBackgroundXPos, itemBackgroundYPos);
	glVertex2f(itemBackgroundXPos + itemBackgroundWidth, itemBackgroundYPos);
	glVertex2f(itemBackgroundXPos + itemBackgroundWidth, itemBackgroundYPos + itemBackgroundHeight);
	glVertex2f(itemBackgroundXPos, itemBackgroundYPos + itemBackgroundHeight);
	glEnd();

	// HP Bar
	float playerHp = m_player.GetHP();
	if (playerHp < 0.)
		playerHp = 0;
	float facteurOmbrage = m_displayInfo ? 0.5f : 1.0f;

	float hpBarWidthProportion = 0.25f;
	float hpBarHeightProportion = 0.045f;
	float hpBarWidth = Width() * hpBarWidthProportion;
	float hpBarHeight = Height() * hpBarHeightProportion;
	float hpBarXOffset = Width() * 0.05f;
	float hpBarYOffset = Height() * 0.7f;
	float hpBarYPos = Height() - hpBarHeight - hpBarYOffset;

	// HP Bar Background
	glColor3f(1.0f * facteurOmbrage, 1.0f * facteurOmbrage, 1.0f * facteurOmbrage);
	glBegin(GL_QUADS);
	glVertex2f(itemBackgroundXPos, hpBarYPos - hpBarHeight);
	glVertex2f(itemBackgroundXPos + itemBackgroundWidth, hpBarYPos - hpBarHeight);
	glVertex2f(itemBackgroundXPos + itemBackgroundWidth, hpBarYPos);
	glVertex2f(itemBackgroundXPos, hpBarYPos);
	glEnd();

	//TODO: Associer avec mechanique de vie du joueur

	// Barre HP
	glColor3f(0.0f * facteurOmbrage, 1.0f * facteurOmbrage, 0.0f * facteurOmbrage);
	glBegin(GL_QUADS);
	glVertex2f(itemBackgroundXPos, hpBarYPos - hpBarHeight);
	glVertex2f(itemBackgroundXPos + itemBackgroundWidth * playerHp, hpBarYPos - hpBarHeight);
	glVertex2f(itemBackgroundXPos + itemBackgroundWidth * playerHp, hpBarYPos);
	glVertex2f(itemBackgroundXPos, hpBarYPos);
	glEnd();

	// Equip Bar
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glColor3f(1.0f * facteurOmbrage, 1.0f * facteurOmbrage, 1.0f * facteurOmbrage);

	float equipWidthProportion = 0.8f;
	float equipHeightProportion = 0.7f;
	float equipWidth = itemBackgroundWidth * equipWidthProportion;
	float equipHeight = itemBackgroundHeight * equipHeightProportion;
	float equipXOffset = itemBackgroundXPos + (itemBackgroundWidth - equipWidth) * 0.1f;
	float equipYOffset = itemBackgroundYPos + (itemBackgroundHeight - equipHeight) * 0.75f;

	glTranslatef(equipXOffset, equipYOffset, 0);

	m_textureGun.Bind();
	glBegin(GL_QUADS);
	glTexCoord2f(0, 0); glVertex2f(0, 0);
	glTexCoord2f(1, 0); glVertex2f(equipWidth, 0);
	glTexCoord2f(1, 1); glVertex2f(equipWidth, equipHeight);
	glTexCoord2f(0, 1); glVertex2f(0, equipHeight);
	glEnd();

	// Username
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE);
	glColor3f(1.0f, 1.0f, 1.0f);

	m_textureFont.Bind();
	std::ostringstream ss;
	float fUsernamePosY = hpBarYPos - (hpBarHeight * 2);

	ss.str("");
	ss << m_player.GetUsername();
	PrintText(itemBackgroundXPos, fUsernamePosY, ss.str(), 1.5f);

	ss.str("");
	ss << m_player.GetHP() * 100 << "%";
	PrintText(itemBackgroundXPos * 5.25f, fUsernamePosY, ss.str(), 1.5f);

	float countdownXOffset = Width() * 0.2f;
	float countdownYOffset = Height() * 0.1f;
	float countdownXPos = Width() - countdownXOffset;
	float countdownYPos = Height() - countdownYOffset;

	// Countdown
	ss.str("");
	ss << "Time: " << (int)(timer / 60) << ":" << std::setw(2) << std::setfill('0') << timer % 60;
	PrintText(countdownXPos, countdownYPos, ss.str(), 2.0f);
}

void Engine::DrawHud(float elapsedTime, BlockType bloc) {
	// Setter le blend function, tout ce qui sera noir sera transparent
	glDisable(GL_STENCIL_TEST);
	glDisable(GL_DEPTH_TEST);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE);
	glBlendEquation(GL_FUNC_ADD);

	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	glOrtho(0, Width(), 0, Height(), -1, 1);

	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();

	int timer = GetCountdown(elapsedTime);
	/*for (int i = 1; i < WORLD_SIZE_X; i++) {
		if (timer <= COUNTDOWN - m_timerReductionChunk * i) {
			m_world.RemoveChunk(m_nbReductionChunk * i);
			m_renderer.RemoveChunk(m_nbReductionChunk * i);
		}
	}*/
	if (m_keyK) {
		SystemNotification(m_messageNotification);
		m_keyK = false;
	}
	if (m_keyL) {

		KillNotification(m_player, m_player);
		m_keyL = false;
	}

	if (m_displayInfo) {
		DisplayInfo(elapsedTime, bloc);
	}

	if (m_displayHud) {
		DisplayHud(timer);
	}

	if (m_displayCrosshair) {
		DisplayCrosshair();
	}

	glBlendFunc(GL_CONSTANT_COLOR, GL_ONE_MINUS_CONSTANT_COLOR);
	glBlendEquation(GL_FUNC_SUBTRACT);

	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();

	glEnable(GL_STENCIL_TEST);
	glEnable(GL_DEPTH_TEST);
}

void Engine::DisplayPovGun() {
	// Setter le blend function, tout ce qui sera noir sera transparent
	glDisable(GL_STENCIL_TEST);
	glDisable(GL_DEPTH_TEST);

	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glBlendEquation(GL_FUNC_ADD);

	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	glOrtho(0, Width(), 0, Height(), -1, 1);

	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();

	float baseXOffsetPercentage = 0.4958;
	float baseWidthPercentage = 0.4688;
	float baseHeightPercentage = 0.5787;

	float xTranslation = baseXOffsetPercentage * Width();
	float quadWidth = baseWidthPercentage * Width();
	float quadHeight = baseHeightPercentage * Height();

	m_texturePovGun.Bind();
	glTranslated(xTranslation, 0, 0);
	glBegin(GL_QUADS);
	glTexCoord2f(0, 0);	glVertex2i(0, 0);
	glTexCoord2f(1, 0);	glVertex2i(quadWidth, 0);
	glTexCoord2f(1, 1);	glVertex2i(quadWidth, quadHeight);
	glTexCoord2f(0, 1);	glVertex2i(0, quadHeight);
	glEnd();

	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();

	glEnable(GL_STENCIL_TEST);
	glEnable(GL_DEPTH_TEST);
}

void Engine::PrintText(float x, float y, const std::string& t, float charSizeMultiplier) {
	float windowWidth = static_cast<float>(Width());
	float windowHeight = static_cast<float>(Height());

	float posX = x * windowWidth;
	float posY = y * windowHeight;

	float baseCharSize = 20 + (24 - 20) * (windowWidth - 1600) / (1920 - 1600);
	float charSize = baseCharSize * charSizeMultiplier;

	glPushMatrix();
	glLoadIdentity();
	glTranslated(x, y, 0);

	for (unsigned int i = 0; i < t.length(); ++i) {
		float left = (float)((t[i] - 32) % 16) / 16.f;
		float top = (float)((t[i] - 32) / 16) / 16.f;
		top += 0.5f;

		glBegin(GL_QUADS);
		glTexCoord2f(left, 1.f - top - .0625f);          glVertex2f(0, 0);
		glTexCoord2f(left + .0625f, 1.f - top - .0625f); glVertex2f(charSize, 0);
		glTexCoord2f(left + .0625f, 1.f - top);          glVertex2f(charSize, charSize);
		glTexCoord2f(left, 1.f - top);                   glVertex2f(0, charSize);
		glEnd();

		glTranslated(0.5555f * charSize, 0, 0);
	}
	glPopMatrix();
}

int Engine::GetFps(float elapsedTime) const { return 1 / elapsedTime; }

int Engine::GetCountdown(float elapsedTime) {
	return m_countdown;
}

int Engine::GetOptionsChoice() {
	return m_selectedOption;
}

bool Engine::StartMultiplayerGame() {
	bool ok = true;
	if (!m_conn.Init()) {
		if (!m_conn.Connect(m_serverAddr.c_str(), m_username)) {
			// setup jeu en reseau.
			std::cout << "ID recu du serveur: " << std::to_string(m_conn.getId()) << "!" << std::endl;
			std::cout << "Seed recu du serveur: " << std::to_string(m_conn.getSeed()) << "!" << std::endl;
			m_player = Player(m_conn.getOrigin().position);

			for (auto& [key, player] : m_conn.m_players)
				m_players[key] = new RemotePlayer(&player);

			m_world.SetSeed(m_conn.getSeed());
			//m_world.SetSeed(9370707);
			m_networkgame = true;
			m_player.m_username = m_username;
		}
		else {
			std::cout << "Erreur de connexion." << std::endl;
			ok = false;
		}
	}
	else {
		std::cout << "Erreur de creation de socket." << std::endl;
		ok = false;
	}
	m_world.BuildWorld();
	return ok;
}

void Engine::DisplayInfo(float elapsedTime, BlockType bloc) {
	m_textureFont.Bind();
	std::ostringstream ss;

	float marginX = Width() * 0.01;
	float marginY = Height() * 0.05;

	float fPosX = marginX;
	float fPosY = Height() - marginY;

	float charSize = 20 + (24 - 20) * (Width() - 1600) / (1920 - 1600);

	ss << " Fps : " << GetFps(elapsedTime);
	PrintText(fPosX, fPosY, ss.str());
	ss.str("");
	fPosY -= charSize;

	ss << " Rendered Chunks : " << m_renderCount;
	PrintText(fPosX, fPosY, ss.str());
	ss.str("");
	fPosY -= charSize;

	ss << " To-Be-Deleted Chunks : " << m_world.GettbDeleted();
	PrintText(fPosX, fPosY, ss.str());
	ss.str("");
	fPosY -= charSize;

	float fPosYJump = Height() * 0.09;
	fPosY = fPosYJump;
	fPosY -= charSize;

	ss << " Velocity  : " << m_player.GetVelocity();
	PrintText(fPosX, fPosY, ss.str());
	ss.str("");
	fPosY -= charSize;

	ss << " Player Position : " << m_player.GetPosition();
	PrintText(fPosX, fPosY, ss.str());
	ss.str("");
	fPosY -= charSize;

	Vector3f pos = Vector3f(m_otherplayerpos.x + CHUNK_SIZE_X * WORLD_SIZE_X / 2, m_otherplayerpos.y, m_otherplayerpos.z + CHUNK_SIZE_Z * WORLD_SIZE_Y / 2);
	ss << " Remote Position  : " << pos;
	PrintText(fPosX, fPosY, ss.str());
	ss.str("");
	fPosY -= charSize;

	PrintText(fPosX, fPosYJump, ss.str());
}

void Engine::DisplaySplashScreen() {
	glDisable(GL_LIGHTING);
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_STENCIL_TEST);

	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	glOrtho(-Width() / 2, Width() / 2, -Height() / 2, Height() / 2, -1, 1);

	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();

	int imageWidth = Width();
	int imageHeight = Height();

	m_textureSplashScreen.Bind();
	glBegin(GL_QUADS);
	glTexCoord2f(0, 0);	glVertex2i(-imageWidth / 2, -imageHeight / 2);
	glTexCoord2f(1, 0); glVertex2i(imageWidth / 2, -imageHeight / 2);
	glTexCoord2f(1, 1);	glVertex2i(imageWidth / 2, imageHeight / 2);
	glTexCoord2f(0, 1);	glVertex2i(-imageWidth / 2, imageHeight / 2);
	glEnd();

	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();
}

void Engine::DisplayLobbyMenu(float elapsedTime) {
	GLint viewport[4];
	glGetIntegerv(GL_VIEWPORT, viewport);

	glDisable(GL_STENCIL_TEST);
	glDisable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);

	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	glOrtho(0, Width(), 0, Height(), -1, 1);

	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();

	glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
	m_textureLobbyMenu.Bind();
	glBegin(GL_QUADS);
	glTexCoord2f(0, 0); glVertex2i(0, 0);
	glTexCoord2f(1, 0); glVertex2i(Width(), 0);
	glTexCoord2f(1, 1); glVertex2i(Width(), Height());
	glTexCoord2f(0, 1); glVertex2i(0, Height());
	glEnd();


	if (m_settingUsername) {
		SetPlayerUsername(elapsedTime);
	}
	else if (m_settingServer) {
		SetServerAddress(elapsedTime);
	}

	glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
	m_textureOptBack.Bind();
	glBegin(GL_QUADS);
	glTexCoord2f(0, 0); glVertex2i(Width() * 0.675, Height() * 0.15);
	glTexCoord2f(1, 0); glVertex2i(Width() * 0.9, Height() * 0.15);
	glTexCoord2f(1, 1); glVertex2i(Width() * 0.9, Height() * 0.225);
	glTexCoord2f(0, 1); glVertex2i(Width() * 0.675, Height() * 0.225);
	glEnd();

	glViewport(viewport[0], viewport[1], viewport[2], viewport[3]);

	glEnable(GL_STENCIL_TEST);
	glEnable(GL_DEPTH_TEST);
	glDisable(GL_BLEND);

	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();
}

void Engine::SetPlayerUsername(float elapsedTime) {
	glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
	m_textureLobbyIdentify.Bind();
	glBegin(GL_QUADS);
	glTexCoord2f(0, 0); glVertex2i(Width() * 0.6, Height() * 0.75);
	glTexCoord2f(1, 0); glVertex2i(Width() * 0.975, Height() * 0.75);
	glTexCoord2f(1, 1); glVertex2i(Width() * 0.975, Height() * 0.95);
	glTexCoord2f(0, 1); glVertex2i(Width() * 0.6, Height() * 0.95);
	glEnd();

	glBlendFunc(GL_SRC_ALPHA, GL_ONE);
	HandlePlayerInput(elapsedTime);
	glDisable(GL_BLEND);
}

void Engine::SetServerAddress(float elapsedTime) {
	glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
	m_textureLobbyServer.Bind();
	glBegin(GL_QUADS);
	glTexCoord2f(0, 0); glVertex2i(Width() * 0.6, Height() * 0.75);
	glTexCoord2f(1, 0); glVertex2i(Width() * 0.975, Height() * 0.75);
	glTexCoord2f(1, 1); glVertex2i(Width() * 0.975, Height() * 0.95);
	glTexCoord2f(0, 1); glVertex2i(Width() * 0.6, Height() * 0.95);
	glEnd();

	glBlendFunc(GL_SRC_ALPHA, GL_ONE);
	HandlePlayerInput(elapsedTime);
	glDisable(GL_BLEND);

}

void Engine::DisplayPauseMenu(float elapsedTime) {
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	glOrtho(0, Width(), 0, Height(), -1, 1);

	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();

	m_texturePauseMenu.Bind();
	glBegin(GL_QUADS);
	glTexCoord2f(0, 0); glVertex2i(0, 0);
	glTexCoord2f(1, 0); glVertex2i(Width(), 0);
	glTexCoord2f(1, 1); glVertex2i(Width(), Height());
	glTexCoord2f(0, 1); glVertex2i(0, Height());
	glEnd();

	glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
	m_texturePauseResume.Bind();
	glBegin(GL_QUADS);
	glTexCoord2f(0, 0); glVertex2i(Width() * 0.025f, Height() * 0.75);
	glTexCoord2f(1, 0); glVertex2i(Width() * 0.33f, Height() * 0.75);
	glTexCoord2f(1, 1); glVertex2i(Width() * 0.33f, Height() * 0.95);
	glTexCoord2f(0, 1); glVertex2i(Width() * 0.025f, Height() * 0.95);
	glEnd();

	m_texturePauseMainMenu.Bind();
	glBegin(GL_QUADS);
	glTexCoord2f(0, 0); glVertex2i(Width() * 0.67f, Height() * 0.75);
	glTexCoord2f(1, 0); glVertex2i(Width() * 0.9975f, Height() * 0.75);
	glTexCoord2f(1, 1); glVertex2i(Width() * 0.9975f, Height() * 0.95);
	glTexCoord2f(0, 1); glVertex2i(Width() * 0.67f, Height() * 0.95);
	glEnd();

	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();
}

void Engine::DisplayMainMenu() {
	GLint viewport[4];
	glGetIntegerv(GL_VIEWPORT, viewport);

	glDisable(GL_STENCIL_TEST);
	glDisable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glBlendEquation(GL_FUNC_ADD);

	glMatrixMode(GL_PROJECTION);
	glPushMatrix();

	glLoadIdentity();
	glOrtho(0, Width(), 0, Height(), -1, 1);

	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();

	// Background
	m_textureMainMenu.Bind();
	glBegin(GL_QUADS);
	glTexCoord2f(0.0f, 0.0f); glVertex2f(0.0f, 0.0f);
	glTexCoord2f(1.0f, 0.0f); glVertex2f(Width(), 0.0f);
	glTexCoord2f(1.0f, 1.0f); glVertex2f(Width(), Height());
	glTexCoord2f(0.0f, 1.0f); glVertex2f(0.0f, Height());
	glEnd();

	// Title
	float centerX = Width() * 0.5f;
	float centerY = Height() * 0.5f;

	glColor4f(1.0f, 0.5f, 0.0f, 1.0f);
	m_textureMenuTitle.Bind();
	glBegin(GL_QUADS);
	glTexCoord2f(0.0f, 0.0f); glVertex2f(centerX + Width() * 0.05, centerY + Height() * 0.05);
	glTexCoord2f(1.0f, 0.0f); glVertex2f(centerX + Width() * 0.45, centerY + Height() * 0.05);
	glTexCoord2f(1.0f, 1.0f); glVertex2f(centerX + Width() * 0.45, centerY + Height() * 0.45);
	glTexCoord2f(0.0f, 1.0f); glVertex2f(centerX + Width() * 0.05, centerY + Height() * 0.45);
	glEnd();

	DrawButtonBackgrounds(centerX, centerY, 3);

	if (m_gamestate == GameState::MAIN_MENU) {
		if (m_selectedPlayOptions) {
			DrawSingleMultiButtons(centerX, centerY);
		}
		else {
			DrawMainMenuButtons(centerX, centerY);
		}
	}

	glColor4f(1.0f, 1.0f, 1.0f, 1.0f);

	glViewport(viewport[0], viewport[1], viewport[2], viewport[3]);

	glEnable(GL_STENCIL_TEST);
	glEnable(GL_DEPTH_TEST);

	glMatrixMode(GL_PROJECTION);
	glPopMatrix();

	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();

	ShowCursor();
}

void Engine::DrawMainMenuButtons(float centerX, float centerY) {
	m_textureMenuPlay.Bind();
	glColor4f(0.75f, 0.05f, 0.0f, 1.0f);
	glBegin(GL_QUADS);
	glTexCoord2f(0.0f, 0.0f); glVertex2f(centerX + Width() * 0.16, centerY - Height() * 0.065);
	glTexCoord2f(1.0f, 0.0f); glVertex2f(centerX + Width() * 0.34, centerY - Height() * 0.065);
	glTexCoord2f(1.0f, 1.0f); glVertex2f(centerX + Width() * 0.34, centerY - Height() * 0.01);
	glTexCoord2f(0.0f, 1.0f); glVertex2f(centerX + Width() * 0.16, centerY - Height() * 0.01);
	glEnd();

	m_textureMenuOptions.Bind();
	glColor4f(0.75f, 0.05f, 0.0f, 1.0f);
	glBegin(GL_QUADS);
	glTexCoord2f(0.0f, 0.0f); glVertex2f(centerX + Width() * 0.08, centerY - Height() * 0.165);
	glTexCoord2f(1.0f, 0.0f); glVertex2f(centerX + Width() * 0.42, centerY - Height() * 0.165);
	glTexCoord2f(1.0f, 1.0f); glVertex2f(centerX + Width() * 0.42, centerY - Height() * 0.11);
	glTexCoord2f(0.0f, 1.0f); glVertex2f(centerX + Width() * 0.08, centerY - Height() * 0.11);
	glEnd();

	m_textureMenuQuit.Bind();
	glColor4f(0.75f, 0.05f, 0.0f, 1.0f);
	glBegin(GL_QUADS);
	glTexCoord2f(0.0f, 0.0f); glVertex2f(centerX + Width() * 0.16, centerY - Height() * 0.265);
	glTexCoord2f(1.0f, 0.0f); glVertex2f(centerX + Width() * 0.34, centerY - Height() * 0.265);
	glTexCoord2f(1.0f, 1.0f); glVertex2f(centerX + Width() * 0.34, centerY - Height() * 0.21);
	glTexCoord2f(0.0f, 1.0f); glVertex2f(centerX + Width() * 0.16, centerY - Height() * 0.21);
	glEnd();
}

void Engine::DrawSingleMultiButtons(float centerX, float centerY) {
	m_textureMenuSingle.Bind();
	glColor4f(0.75f, 0.05f, 0.0f, 1.0f);
	glBegin(GL_QUADS);
	glTexCoord2f(0.0f, 0.0f); glVertex2f(centerX + Width() * 0.06, centerY - Height() * 0.065);
	glTexCoord2f(1.0f, 0.0f); glVertex2f(centerX + Width() * 0.44, centerY - Height() * 0.065);
	glTexCoord2f(1.0f, 1.0f); glVertex2f(centerX + Width() * 0.44, centerY - Height() * 0.01);
	glTexCoord2f(0.0f, 1.0f); glVertex2f(centerX + Width() * 0.06, centerY - Height() * 0.01);
	glEnd();

	m_textureMenuMulti.Bind();
	glColor4f(0.75f, 0.05f, 0.0f, 1.0f);
	glBegin(GL_QUADS);
	glTexCoord2f(0.0f, 0.0f); glVertex2f(centerX + Width() * 0.08, centerY - Height() * 0.165);
	glTexCoord2f(1.0f, 0.0f); glVertex2f(centerX + Width() * 0.42, centerY - Height() * 0.165);
	glTexCoord2f(1.0f, 1.0f); glVertex2f(centerX + Width() * 0.42, centerY - Height() * 0.11);
	glTexCoord2f(0.0f, 1.0f); glVertex2f(centerX + Width() * 0.08, centerY - Height() * 0.11);
	glEnd();

	m_textureMenuBack.Bind();
	glColor4f(0.75f, 0.05f, 0.0f, 1.0f);
	glBegin(GL_QUADS);
	glTexCoord2f(0.0f, 0.0f); glVertex2f(centerX + Width() * 0.16, centerY - Height() * 0.265);
	glTexCoord2f(1.0f, 0.0f); glVertex2f(centerX + Width() * 0.34, centerY - Height() * 0.265);
	glTexCoord2f(1.0f, 1.0f); glVertex2f(centerX + Width() * 0.34, centerY - Height() * 0.21);
	glTexCoord2f(0.0f, 1.0f); glVertex2f(centerX + Width() * 0.16, centerY - Height() * 0.21);
	glEnd();
}

void Engine::DrawButtonBackgrounds(float centerX, float centerY, int iterations) {
	glColor4f(1.0f, 1.0f, 1.0f, 0.5f);
	glBindTexture(GL_TEXTURE_2D, 0);

	float bottomOffset = 0.075f;
	float topOffset = 0.0f;
	float distanceBetween = 0.1f;

	for (int i = 0; i < iterations; i++) {
		glBegin(GL_QUADS);
		glTexCoord2f(0.0f, 0.0f); glVertex2f(centerX + Width() * 0.05, centerY - Height() * bottomOffset);
		glTexCoord2f(1.0f, 0.0f); glVertex2f(centerX + Width() * 0.45, centerY - Height() * bottomOffset);
		glTexCoord2f(1.0f, 1.0f); glVertex2f(centerX + Width() * 0.45, centerY - Height() * topOffset);
		glTexCoord2f(0.0f, 1.0f); glVertex2f(centerX + Width() * 0.05, centerY - Height() * topOffset);
		glEnd();

		bottomOffset += distanceBetween;
		topOffset += distanceBetween;
	}
}

void Engine::DisplayOptionsMenu() {
	static const int sTitle = 400;
	static const int sButton = 225;

	float centerX = Width() * 0.5f;
	float centerY = Height() * 0.5f;

	GLint viewport[4];
	glGetIntegerv(GL_VIEWPORT, viewport);

	glDisable(GL_STENCIL_TEST);
	glDisable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glBlendEquation(GL_FUNC_ADD);

	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	glOrtho(0, Width(), 0, Height(), -1, 1);

	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();

	m_textureOptionsMenu.Bind();
	glBegin(GL_QUADS);
	glTexCoord2f(0, 0);	glVertex2i(0, 0);
	glTexCoord2f(1, 0);	glVertex2i(Width(), 0);
	glTexCoord2f(1, 1);	glVertex2i(Width(), Height());
	glTexCoord2f(0, 1); glVertex2i(0, Height());
	glEnd();

	glColor4f(1.0f, 0.0f, 1.0f, 1.0f);
	m_textureOptOptions.Bind();
	glBegin(GL_QUADS);
	glTexCoord2f(0, 0);	glVertex2i(centerX - Width() * 0.175, centerY + Height() * 0.35);
	glTexCoord2f(1, 0);	glVertex2i(centerX + Width() * 0.175, centerY + Height() * 0.35);
	glTexCoord2f(1, 1);	glVertex2i(centerX + Width() * 0.175, centerY + Height() * 0.45);
	glTexCoord2f(0, 1); glVertex2i(centerX - Width() * 0.175, centerY + Height() * 0.45);
	glEnd();

	if (m_selectedOption == 0) {
		glColor4f(0.5f, 1.0f, 0.0f, 1.0f);
	}
	m_textureOptAudio.Bind();
	glBegin(GL_QUADS);
	glTexCoord2f(0, 0);	glVertex2i(centerX - Width() * 0.35, centerY + Height() * 0.2);
	glTexCoord2f(1, 0);	glVertex2i(centerX - Width() * 0.2, centerY + Height() * 0.2);
	glTexCoord2f(1, 1);	glVertex2i(centerX - Width() * 0.2, centerY + Height() * 0.25);
	glTexCoord2f(0, 1);	glVertex2i(centerX - Width() * 0.35, centerY + Height() * 0.25);
	glEnd();

	glColor4f(1.0f, 0.0f, 1.0f, 1.0f);
	if (m_selectedOption == 1) {
		glColor4f(0.5f, 1.0f, 0.0f, 1.0f);
	}
	m_textureOptGraphics.Bind();
	glBegin(GL_QUADS);
	glTexCoord2f(0, 0);	glVertex2i(centerX - Width() * 0.4, centerY + Height() * 0.05);
	glTexCoord2f(1, 0);	glVertex2i(centerX - Width() * 0.2, centerY + Height() * 0.05);
	glTexCoord2f(1, 1);	glVertex2i(centerX - Width() * 0.2, centerY + Height() * 0.1);
	glTexCoord2f(0, 1);	glVertex2i(centerX - Width() * 0.4, centerY + Height() * 0.1);
	glEnd();

	glColor4f(1.0f, 0.0f, 1.0f, 1.0f);
	if (m_selectedOption == 2) {
		glColor4f(0.5f, 1.0f, 0.0f, 1.0f);
	}
	m_textureOptGameplay.Bind();
	glBegin(GL_QUADS);
	glTexCoord2f(0, 0);	glVertex2i(centerX - Width() * 0.375, centerY - Height() * 0.1);
	glTexCoord2f(1, 0);	glVertex2i(centerX - Width() * 0.2, centerY - Height() * 0.1);
	glTexCoord2f(1, 1);	glVertex2i(centerX - Width() * 0.2, centerY - Height() * 0.05);
	glTexCoord2f(0, 1);	glVertex2i(centerX - Width() * 0.375, centerY - Height() * 0.05);
	glEnd();

	// Séparateur
	glColor4f(0.0f, 0.0f, 0.0f, 1.0f);
	glBindTexture(GL_TEXTURE_2D, 0);
	glBegin(GL_QUADS);
	glTexCoord2f(0, 0);	glVertex2i(centerX - Width() * 0.175, centerY - Height() * 0.1);
	glTexCoord2f(1, 0);	glVertex2i(centerX - Width() * 0.18, centerY - Height() * 0.1);
	glTexCoord2f(1, 1);	glVertex2i(centerX - Width() * 0.18, centerY + Height() * 0.25);
	glTexCoord2f(0, 1);	glVertex2i(centerX - Width() * 0.175, centerY + Height() * 0.25);
	glEnd();

	glColor4f(1.0f, 0.0f, 1.0f, 1.0f);
	m_textureOptBack.Bind();
	glBegin(GL_QUADS);
	glTexCoord2f(0, 0);	glVertex2i(centerX - Width() * 0.075, centerY - Height() * 0.25);
	glTexCoord2f(1, 0);	glVertex2i(centerX + Width() * 0.075, centerY - Height() * 0.25);
	glTexCoord2f(1, 1);	glVertex2i(centerX + Width() * 0.075, centerY - Height() * 0.2);
	glTexCoord2f(0, 1); glVertex2i(centerX - Width() * 0.075, centerY - Height() * 0.2);
	glEnd();

	glColor4f(1.0f, 1.0f, 1.0f, 1.0f);

	if (m_selectedOption == 0) {
		DisplayAudioMenu(centerX, centerY);
	}
	else if (m_selectedOption == 1) {
		DisplayGraphicsMenu(centerX, centerY);
	}
	else if (m_selectedOption == 2) {
		DisplayGameplayMenu(centerX, centerY);
	}

	glViewport(viewport[0], viewport[1], viewport[2], viewport[3]);

	glEnable(GL_STENCIL_TEST);
	glEnable(GL_DEPTH_TEST);

	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();
}

void Engine::DisplayAudioMenu(float centerX, float centerY) {
	float minBar = centerX - Width() * 0.15;
	float maxBar = centerX + Width() * 0.3;
	float percentPosX = centerX + Width() * 0.31f;

	glColor4f(1.0f, 1.0f, 0.0f, 1.0f);
	m_textureOptMain.Bind();
	glBegin(GL_QUADS);
	glTexCoord2f(0, 0);	glVertex2i(centerX - Width() * 0.15, centerY + Height() * 0.215);
	glTexCoord2f(1, 0);	glVertex2i(centerX - Width() * 0.075, centerY + Height() * 0.215);
	glTexCoord2f(1, 1);	glVertex2i(centerX - Width() * 0.075, centerY + Height() * 0.25);
	glTexCoord2f(0, 1);	glVertex2i(centerX - Width() * 0.15, centerY + Height() * 0.25);
	glEnd();

	float principalBottom = centerY + Height() * 0.165f;
	float principalTop = centerY + Height() * 0.2f;

	DrawSliderBackground(centerX, centerY, minBar, maxBar, principalBottom, principalTop);
	DrawSlider(centerX, centerY, m_mainvolume, minBar, maxBar, principalBottom, principalTop);
	DisplayBarPercentValue(centerX, centerY, percentPosX, principalBottom, minBar, maxBar, m_mainvolume);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glBlendEquation(GL_FUNC_ADD);

	glColor4f(1.0f, 1.0f, 0.0f, 1.0f);
	m_textureOptMusic.Bind();
	glBegin(GL_QUADS);
	glTexCoord2f(0, 0);	glVertex2i(centerX - Width() * 0.15, centerY + Height() * 0.09);
	glTexCoord2f(1, 0);	glVertex2i(centerX - Width() * 0.05, centerY + Height() * 0.09);
	glTexCoord2f(1, 1);	glVertex2i(centerX - Width() * 0.05, centerY + Height() * 0.125);
	glTexCoord2f(0, 1);	glVertex2i(centerX - Width() * 0.15, centerY + Height() * 0.125);
	glEnd();

	float musiqueBottom = centerY + Height() * 0.04f;
	float musiqueTop = centerY + Height() * 0.075f;

	DrawSliderBackground(centerX, centerY, minBar, maxBar, musiqueBottom, musiqueTop);
	DrawSlider(centerX, centerY, m_musicvolume, minBar, maxBar, musiqueBottom, musiqueTop);
	DisplayBarPercentValue(centerX, centerY, percentPosX, musiqueBottom, minBar, maxBar, m_musicvolume);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glBlendEquation(GL_FUNC_ADD);

	glColor4f(1.0f, 1.0f, 0.0f, 1.0f);
	m_textureOptSfx.Bind();
	glBegin(GL_QUADS);
	glTexCoord2f(0, 0);	glVertex2i(centerX - Width() * 0.15, centerY - Height() * 0.035);
	glTexCoord2f(1, 0);	glVertex2i(centerX - Width() * 0.075, centerY - Height() * 0.035);
	glTexCoord2f(1, 1);	glVertex2i(centerX - Width() * 0.075, centerY - Height() * 0.00001);
	glTexCoord2f(0, 1);	glVertex2i(centerX - Width() * 0.15, centerY - Height() * 0.00001);
	glEnd();

	float effectsBottom = centerY - Height() * 0.085f;
	float effectsTop = centerY - Height() * 0.05f;

	DrawSliderBackground(centerX, centerY, minBar, maxBar, effectsBottom, effectsTop);
	DrawSlider(centerX, centerY, m_sfxvolume, minBar, maxBar, effectsBottom, effectsTop);
	DisplayBarPercentValue(centerX, centerY, percentPosX, effectsBottom, minBar, maxBar, m_sfxvolume);

	glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
}

void Engine::DisplayGraphicsMenu(float centerX, float centerY) {
	glColor4f(1.0f, 1.0f, 0.0f, 1.0f);
	m_textureOptResolution.Bind();
	glBegin(GL_QUADS);
	glTexCoord2f(0, 0);	glVertex2i(centerX - Width() * 0.15, centerY + Height() * 0.215);
	glTexCoord2f(1, 0);	glVertex2i(centerX, centerY + Height() * 0.215);
	glTexCoord2f(1, 1);	glVertex2i(centerX, centerY + Height() * 0.25);
	glTexCoord2f(0, 1);	glVertex2i(centerX - Width() * 0.15, centerY + Height() * 0.25);
	glEnd();

	glColor4f(1.0f, 1.0f, 1.0f, 0.5f);
	glBindTexture(GL_TEXTURE_2D, 0);
	glBegin(GL_QUADS);
	glTexCoord2f(0, 0);	glVertex2i(Width() * 0.34, Height() * 0.46);
	glTexCoord2f(1, 0);	glVertex2i(Width() * 0.6, Height() * 0.46);
	glTexCoord2f(1, 1);	glVertex2i(Width() * 0.6, Height() * 0.67);
	glTexCoord2f(0, 1);	glVertex2i(Width() * 0.34, Height() * 0.67);
	glEnd();

	glColor4f(0.0f, 0.0f, 0.0f, 1.0f);
	m_textureHd.Bind();
	glBegin(GL_QUADS);
	glTexCoord2f(0, 0);	glVertex2i(centerX - Width() * 0.15, centerY + Height() * 0.125);
	glTexCoord2f(1, 0);	glVertex2i(centerX + Width() * 0.05, centerY + Height() * 0.125);
	glTexCoord2f(1, 1);	glVertex2i(centerX + Width() * 0.05, centerY + Height() * 0.15);
	glTexCoord2f(0, 1);	glVertex2i(centerX - Width() * 0.15, centerY + Height() * 0.15);
	glEnd();

	m_textureFhd.Bind();
	glBegin(GL_QUADS);
	glTexCoord2f(0, 0);	glVertex2i(centerX - Width() * 0.15, centerY + Height() * 0.075);
	glTexCoord2f(1, 0);	glVertex2i(centerX, centerY + Height() * 0.075);
	glTexCoord2f(1, 1);	glVertex2i(centerX, centerY + Height() * 0.1);
	glTexCoord2f(0, 1);	glVertex2i(centerX - Width() * 0.15, centerY + Height() * 0.1);
	glEnd();

	m_textureQhd.Bind();
	glBegin(GL_QUADS);
	glTexCoord2f(0, 0);	glVertex2i(centerX - Width() * 0.15, centerY + Height() * 0.025);
	glTexCoord2f(1, 0);	glVertex2i(centerX, centerY + Height() * 0.025);
	glTexCoord2f(1, 1);	glVertex2i(centerX, centerY + Height() * 0.05);
	glTexCoord2f(0, 1);	glVertex2i(centerX - Width() * 0.15, centerY + Height() * 0.05);
	glEnd();

	m_textureUhd.Bind();
	glBegin(GL_QUADS);
	glTexCoord2f(0, 0);	glVertex2i(centerX - Width() * 0.15, centerY - Height() * 0.025);
	glTexCoord2f(1, 0);	glVertex2i(centerX + Width() * 0.015, centerY - Height() * 0.025);
	glTexCoord2f(1, 1);	glVertex2i(centerX + Width() * 0.015, centerY);
	glTexCoord2f(0, 1);	glVertex2i(centerX - Width() * 0.15, centerY);
	glEnd();

	glColor4f(1.0f, 1.0f, 1.0f, 1.0f);

	float heightRatioBottom = 0.125f;
	float heightRatioTop = 0.15;

	// Checkboxes
	for (int i = 0; i < 4; i++) {
		if (static_cast<int>(m_resolution) == i) {
			m_textureChecked.Bind();
		}
		else {
			m_textureCheck.Bind();
		}

		glBegin(GL_QUADS);
		glTexCoord2f(0, 0);	glVertex2i(centerX + Width() * 0.075, centerY + Height() * heightRatioBottom);
		glTexCoord2f(1, 0);	glVertex2i(centerX + Width() * 0.09, centerY + Height() * heightRatioBottom);
		glTexCoord2f(1, 1);	glVertex2i(centerX + Width() * 0.09, centerY + Height() * heightRatioTop);
		glTexCoord2f(0, 1);	glVertex2i(centerX + Width() * 0.075, centerY + Height() * heightRatioTop);
		glEnd();

		heightRatioBottom -= 0.05f;
		heightRatioTop -= 0.05f;
	}

	glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
}

void Engine::Render(float elapsedTime) {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
	m_audio.CleanupSFX();

	if (m_gamestate == GameState::SPLASH) {
		if (m_splashTime > 0.0f) {
			DisplaySplashScreen();
		}
		else {
			m_gamestate = GameState::MAIN_MENU;
		}

		m_splashTime -= elapsedTime;
		return;
	}

	if (m_gamestate == GameState::MAIN_MENU) {
		DisplayMainMenu();
		return;
	}

	if (m_gamestate == GameState::OPTIONS) {
		DisplayOptionsMenu();
		return;
	}

	if (m_gamestate == GameState::LOBBY) {
		DisplayLobbyMenu(elapsedTime);
		if (m_multiReady) {
			if (StartMultiplayerGame()) {
				std::cout << "Starting multiplayer game reached" << std::endl;
				m_gamestate = GameState::PLAY;
				m_audio.ToggleMusicState(m_gamestate);
				m_startTime = std::chrono::high_resolution_clock::now();
			}
			else {
				std::cout << "Cannot reach server." << std::endl;
				m_gamestate = GameState::MAIN_MENU;
			}
		}
		else if (m_singleReady) {
			m_gamestate = GameState::PLAY;
			m_audio.ToggleMusicState(m_gamestate);
			m_startTime = std::chrono::high_resolution_clock::now();
		}
		return;
	}

	if (m_gamestate == GameState::PAUSE) {
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();

		glDisable(GL_STENCIL_TEST);
		glDisable(GL_DEPTH_TEST);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		DisplayPauseMenu(elapsedTime);
		ShowCursor();
		return;
	}

	if (m_gamestate == GameState::PLAY) {
		HideCursor();
		CenterMouse();

		static irrklang::ISound* step; // Pour les sons de pas.
		static float pollTime = 0;
		static float bulletTime = 0;
		static BlockType bloc = 1;

		if (elapsedTime > 0.1f) return;

		pollTime += elapsedTime;

		Transformation all;
		Transformation skybox;
		Transformation remotePlayer;

		Vector3f vstep;

		// Transformations initiales
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();

		if (bulletTime > 0.f) bulletTime -= elapsedTime;
		if (bulletTime < 0.f) bulletTime = 0.f;

		static bool leftright = false;
		if (pollTime >= .005f) {
			Player::Sound snd = m_player.ApplyPhysics(m_player.GetInput(m_keyW, m_keyS, m_keyA, m_keyD, m_keySpace, (bloc == BTYPE_LAST && bulletTime <= 0.f && m_mouseL), elapsedTime), &m_world, elapsedTime);
			switch (snd) {
			case Player::Sound::STEP:
				if (leftright)
					vstep = Vector3f(m_player.GetPosition().x + m_player.GetDirection().z, m_player.GetPosition().y - 1.7f, m_player.GetPosition().z + m_player.GetDirection().x);
				else vstep = Vector3f(m_player.GetPosition().x - m_player.GetDirection().z, m_player.GetPosition().y - 1.7f, m_player.GetPosition().z - m_player.GetDirection().x);
				m_audio.Create3DAudioObj(step, AUDIO_PATH "step.wav", vstep, m_player.GetVelocity(), false, m_sfxvolume);
				leftright = !leftright;
				break;
			case Player::Sound::FALL:
				m_audio.Create3DAudioObj(step, AUDIO_PATH "hit.wav", m_player.GetPosition(), m_player.GetVelocity(), false, m_sfxvolume);
				break;
			default: break;
			}
			m_audio.Update3DAudio(m_player.GetPOV(), m_player.GetDirection(), m_player.GetVelocity()); // Ajustement du positionnement 3D avec les coordonnees du joueur et
			// son vecteur de velocite (pour l'effet Doppler)
			pollTime = 0;
		}
		uint64_t boostid = m_player.TakeBooster(m_boosters, elapsedTime);

		m_boost_manifest.insert(boostid);

		m_player.ApplyTransformation(all);

		m_player.ApplyTransformation(skybox, false); // Version d'ApplyTransformation qui ne tient compte que de la rotation 
		// (donc l'objet ne bouge pas relativement au joueur, ce qui est pratique pour une skybox!).
		m_player.ApplyTransformation(remotePlayer, true, false);

		m_mouseWU = m_mouseWD = m_key1 = m_key2 = false;
		netprot::ChunkMod* cmod = nullptr;
		if (!m_player.AmIDead() && m_mouseL) {
			if (bulletTime <= 0.f) {
				for (int x = 0; x < MAX_BULLETS; ++x)  // Ajouter une balle dans l'array (aussi connu sous le nom de "faire pow pow").
					if (!m_bullets[x]) {
						m_bullets[x] = new Bullet(m_player.GetPOV() + m_player.GetDirection(), m_player.GetDirection());
						break;
					}
					else if (x == MAX_BULLETS - 1) { // S'il y a pas d'espace dans l'array, prendre la place de la première balle de l'array.
						m_bullets[0]->~Bullet();
						m_bullets[0] = new Bullet(m_player.GetPOV() + m_player.GetDirection(), m_player.GetDirection());
					}
				bulletTime = BULLET_TIME;
				m_audio.Create3DAudioObj(m_powpow, AUDIO_PATH "pow.wav", m_player.GetPOV(), m_player.GetDirection() * 10, false, m_sfxvolume);
				if (m_flash) { // Coupe le rendering et affiche un frame blanc, pour simuler un flash.
					glClearColor(.8f, .8f, .8f, 1.f);
					glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
					glClearColor(0.f, 0.f, 0.f, 1.f);
					return;
				}
			}
		}
		else if (m_mouseR)
			cmod = m_world.ChangeBlockAtCursor(BTYPE_METAL, m_player.GetPosition(), m_player.GetDirection(), m_block, m_networkgame);

		static netprot::ChunkMod** wat = &m_chunkmod;
		for (int x = 0; x < MAX_BULLETS; ++x) { // Array de bullets en jeu.
			if (m_bullets[x]) {
				for (int b = 0; b < BULLET_UPDATES_PER_FRAME; ++b) {
					if (m_bullets[x]->Update(&m_world, elapsedTime, BULLET_UPDATES_PER_FRAME, m_players, m_networkgame ? wat : nullptr)) {
						m_bullets[x]->~Bullet();

						if (m_chunkmod) {
							m_chunkmod_manifest.push_back(std::move(m_chunkmod));
							m_chunkmod = nullptr;
						}
						m_bullets[x] = nullptr;
						break;
					}
				}
			}
		}
		m_world.Update(m_bullets, m_player.GetPosition(), m_blockinfo);
		m_renderer.UpdateMesh(&m_world, m_player.GetPosition(), m_blockinfo);

		if (m_isSkybox) m_renderer.RenderWorld(&m_world, m_renderCount, m_player.GetPosition(), m_player.GetDirection(), all, m_shader01, m_textureAtlas);

		glDisable(GL_BLEND);
		m_renderer.RenderWorld(&m_world, m_renderCount, m_player.GetPosition(), m_player.GetDirection(), all, m_shader01, m_textureAtlas);
		glEnable(GL_BLEND);

		if (m_isSkybox) m_skybox.Render(skybox);

		ProcessNotificationQueue();
		if (m_damage) {
			InstantDamage();
		}
		static bool died = false;
		if ((m_player.GetPosition().y < -1.7f || m_player.AmIDead()) && !died) {
			m_audio.Create3DAudioObj(m_scream, AUDIO_PATH "scream.wav", m_player.GetPOV(), m_player.GetVelocity(), false, m_sfxvolume);
			died = true;
			std::string user = m_player.m_username.append(" (Dead)");
			m_player = Player(Vector3f(.5, CHUNK_SIZE_Y + 1.7f, .5), 0, 0);
			m_player.m_username = user;
			m_player.InflictDamage(-m_player.GetHP());
		}

		m_time += elapsedTime;

		if (m_networkgame) { // Pour se gerer le paquet.
			static bool has_synced = false;
			using namespace std::chrono;
			using namespace netprot;
			Timestamp tstamp = duration_cast<milliseconds>(high_resolution_clock::now() - m_startTime).count();
			static Timestamp last = 0;
			Input input;
			Sync sync;
			uint64_t id = m_conn.getId();
			static std::vector<char*> lsPck;
			static int sync_acc = 0, cmod_acc = 0;

			if (cmod)
				m_chunkmod_manifest.emplace_back(cmod);

			if (last == 0)
				last = tstamp;

			sync_acc += tstamp - last;
			cmod_acc += tstamp - last;
			last = tstamp;

			if (sync_acc >= SYNC_ACC) {
				sync_acc -= SYNC_ACC;
				sync.sid = id;
				sync.timestamp = tstamp;
				sync.position = m_player.GetPositionAbs();
				sync.hp = m_player.GetHP();
				if (!has_synced) {
					has_synced = true;
					sendPackTo<Sync>(m_conn.m_sock_udp, &sync, &m_bufout, &m_conn.m_srvsockaddr);
				}
				m_syncs[sync.timestamp] = sync;
			}

			if (cmod_acc >= CMOD_ACC) {
				while (cmod_acc >= CMOD_ACC)
					cmod_acc -= CMOD_ACC;
				if (!m_chunkmod_manifest.empty()) {
					ChunkMod* cmod = m_chunkmod_manifest.front();
					m_chunkmod_manifest.pop_front();
					m_world.ChangeBlockAtPosition(cmod->old_b_type, cmod->pos);
					delete cmod;
				}
			}

			input.sid = id;
			input.direction = m_player.GetDirection();
			input.timestamp = tstamp;
			input.keys.forward = m_keyW;
			input.keys.backward = m_keyS;
			input.keys.left = m_keyA;
			input.keys.right = m_keyD;
			input.keys.jump = m_keySpace;
			input.keys.block = m_mouseR;
			input.keys.shoot = m_mouseL;

			sendPackTo<Input>(m_conn.m_sock_udp, &input, &m_bufout, &m_conn.m_srvsockaddr);

			recvPacks(m_conn.m_sock_udp, &m_buf, &lsPck);
			char* prevptr = nullptr;
			Chat chat;
			for (auto& pck : lsPck) { // We could make a few threads out of this.
				Sync sync; Output out; ChunkMod cmod; BulletAdd bull; PickupMod pmod;
				if (!prevptr)
					prevptr = m_buf.ptr;
				uint32_t bsize = m_buf.len - (pck - prevptr);
				prevptr = pck;
				switch (getType(pck, 1)) {
					using enum PACKET_TYPE;
				case PICKUPMOD:
					if (Deserialize(&pmod, pck, &bsize)) {
						if (m_boosters.count(pmod.id)) {
							Booster* boost = m_boosters.at(pmod.id);
							if (m_boost_manifest.count(boost->GetId())) {
								std::string str = "Picked up ";
								if (pmod.boost.damage)
									str.append("Damage Booster");
								else if (pmod.boost.hp)
									str.append("Healing");
								else if (pmod.boost.invincible)
									str.append("Invincibility");
								else break;
								SystemNotification(str);
							}
							boost->SetAvailability(pmod.available);
						}
						else {
							BOOST_TYPE btype;
							if (pmod.boost.damage)
								btype = BOOST_TYPE::BTYPE_DAMAGE;
							else if (pmod.boost.hp)
								btype = BOOST_TYPE::BTYPE_HEAL;
							else if (pmod.boost.invincible)
								btype = BOOST_TYPE::BTYPE_INVINCIBLE;
							else break;

							Booster* boost = new Booster(pmod.pos, btype, pmod.id);

							boost->SetAvailability(pmod.available);

							m_boosters[pmod.id] = boost;
						}
					}
					break;
				case SYNC:
					if (Deserialize(&sync, pck, &bsize)) {
						if (sync.sid != m_conn.getId()) {
							SystemNotification("syncsid be no good.");
							break;
						}
						if (sync.timestamp == m_conn.getId()) {
							m_player.SetHP(sync.hp);
						}
						if (m_syncs.count(sync.timestamp)) {
							Sync comp = m_syncs[sync.timestamp];
							m_otherplayerpos = sync.position;
	
							m_player.SetHP(sync.hp);

							Vector3f diff = sync.position - comp.position;

							if (diff.y < 1.)
								diff.y = 0;

							Boosts boost = sync.boost;

							m_player.boostdamage = boost.damage;
							m_player.boostinvincible = boost.invincible;

							if (diff.Length() > 10.) {
								m_player.Move(-diff);
							}

							if (diff.Length() > 1.5) {
								diff.Normalize();
								m_player.Move(-diff);
							}

							m_countdown = sync.timer;

							m_syncs.erase(sync.timestamp);
						}
					}
					break;
				case OUTPUT:
					if (Deserialize(&out, pck, &bsize)) {
						if (!m_players.contains(out.id)) {
							SystemNotification(std::to_string(out.id).append(" is id no good."));
							break;
						}
						RemotePlayer* rt = static_cast<RemotePlayer*>(m_players[out.id]);
						rt->Feed(out);
						if (rt->AmIDead() && !m_deadplayers.count(out.id)) {
							m_audio.Create3DAudioObj(m_scream, AUDIO_PATH "scream.wav", m_player.GetPOV(), m_player.GetVelocity(), false, m_sfxvolume);
							m_deadplayers.insert(out.id);
						}
					}
					break;
				case CHUNKMOD:
					if (Deserialize(&cmod, pck, &bsize)) {
						if (!std::erase_if(m_chunkmod_manifest, // Efface le chunkmod du manifeste s'il est dedans et reset le countdown, sinon fait la modification.
							[cmod](ChunkMod* c) {
								return cmod.pos == c->pos &&
									cmod.b_type == c->b_type &&
									cmod.old_b_type == c->old_b_type;
							}))
							m_world.ChangeBlockAtPosition(cmod.b_type, cmod.pos);
						else cmod_acc = 0;
					}
					else SystemNotification("cmod iznogoud.");
					break;
				case BULLET:
					if (Deserialize(&bull, pck, &bsize)) {
						Bullet* bult = new Bullet(bull.pos, bull.dir, bull.id, false);
						for (int x = 0; x < MAX_BULLETS; ++x)  // Ajouter une balle dans l'array (aussi connu sous le nom de "faire pow pow").
							if (!m_bullets[x]) {
								m_bullets[x] = bult;
								break;
							}
							else if (x == MAX_BULLETS - 1) { // S'il y a pas d'espace dans l'array, prendre la place de la première balle de l'array.
								m_bullets[0]->~Bullet();
								m_bullets[0] = bult;
								break;
							}
						m_audio.Create3DAudioObj(m_powpow, AUDIO_PATH "pow.wav", bull.pos, bull.dir, false, m_sfxvolume);
					}
					else SystemNotification("Bullet is kraput.");
					break;
				case CHAT:
					if (Deserialize(&chat, pck, &bsize))
						SystemNotification(chat.mess);
					else SystemNotification("Chat iznogoud.");
					break;
				default:
					SystemNotification("packet be no good.");
					break;
				}
			}
			lsPck.clear();

			glDisable(GL_CULL_FACE);
			for (auto& [key, player] : m_players) {
				RemotePlayer* rt = static_cast<RemotePlayer*>(player);
				glClear(GL_STENCIL_BUFFER_BIT);
				rt->Render(m_animeAtlas, m_shader01, all, elapsedTime, m_player);
			}
			for (auto& [key, booster] : m_boosters) {
				if (booster->GetAvailability()) {
					glClear(GL_STENCIL_BUFFER_BIT);
					m_renderer.RenderBooster(m_textureAtlas, m_shader01, all, m_player, booster);
				}
			}
			glEnable(GL_CULL_FACE);
		}
		else {
			if (m_resetcountdown) {
				m_nbReductionChunk = 4;
				m_timerReductionChunk = 30;
				m_countdown = m_time + COUNTDOWN;
				m_resetcountdown = false;
			}
			if (!m_stopcountdown) {
				m_countdown = COUNTDOWN - (int)m_time;
			}
		}

		DrawHud(elapsedTime, bloc);
		DisplayPovGun();
	}
}

void Engine::DisplayGameplayMenu(float centerX, float centerY) {
	float minBar = centerX - Width() * 0.15;
	float maxBar = centerX + Width() * 0.3;
	float percentPosX = centerX + Width() * 0.31f;

	glColor4f(1.0f, 1.0f, 0.0f, 1.0f);
	m_textureOptSensitivity.Bind();
	glBegin(GL_QUADS);
	glTexCoord2f(0, 0);	glVertex2i(centerX - Width() * 0.15, centerY + Height() * 0.215);
	glTexCoord2f(1, 0);	glVertex2i(centerX, centerY + Height() * 0.215);
	glTexCoord2f(1, 1);	glVertex2i(centerX, centerY + Height() * 0.25);
	glTexCoord2f(0, 1);	glVertex2i(centerX - Width() * 0.15, centerY + Height() * 0.25);
	glEnd();

	float sensibleBottom = centerY + Height() * 0.165f;
	float sensibleTop = centerY + Height() * 0.2f;

	DrawSliderBackground(centerX, centerY, minBar, maxBar, sensibleBottom, sensibleTop);
	DrawSlider(centerX, centerY, m_sensitivity, minBar, maxBar, sensibleBottom, sensibleTop);
	DisplayBarPercentValue(centerX, centerY, percentPosX, sensibleBottom, minBar, maxBar, m_sensitivity);

	glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
}

void Engine::DrawSliderBackground(float centerX, float centerY, float minVal, float maxVal, float bottomSideValue, float topSideValue) {
	glColor4f(0.0f, 0.0f, 0.0f, 1.0f);
	glBindTexture(GL_TEXTURE_2D, 0);
	glBegin(GL_QUADS);
	glTexCoord2f(0, 0);	glVertex2i(minVal, bottomSideValue);
	glTexCoord2f(1, 0);	glVertex2i(maxVal, bottomSideValue);
	glTexCoord2f(1, 1);	glVertex2i(maxVal, topSideValue);
	glTexCoord2f(0, 1);	glVertex2i(minVal, topSideValue);
	glEnd();
}

void Engine::DisplayBarPercentValue(float centerX, float centerY, float posX, float posY, float minVal, float maxVal, float value) {
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE);

	if (m_selectedOption == 0) {
		if (value < 0.0f) {
			value = 0.0f;
		}
	}
	else {
		if (value < 0.1f) {
			value = 0.1f;
		}
	}
	
	if (value > 1.0f) {
		value = 1.0f;
	}

	float percentage = value * 100;

	m_textureFont.Bind();
	std::ostringstream ss;
	ss.str("");
	ss << std::fixed << std::setprecision(2);
	ss << percentage << "%";
	glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
	PrintText(posX, posY, ss.str(), 2.0f);

	glDisable(GL_BLEND);
}

void Engine::DrawSlider(float centerX, float centerY, float value, float minVal, float maxVal, float bottomSideValue, float topSideValue) {
	float barLength = maxVal - minVal;

	if (m_selectedOption == 0) {
		if (value < 0.0f) {
			value = 0.0f;
		}
	}
	else {
		if (value < 0.1f) {
			value = 0.1f;
		}
	}

	if (value > 1.0f) {
		value = 1.0f;
	}

	float scaledLength = minVal + value * barLength;

	if (value >= 0.0f && value <= 1.0f) {
		glColor4f(0.0f, 1.0f, 0.0f, 1.0f);
		glBegin(GL_QUADS);
		glTexCoord2f(0, 0);	glVertex2i(minVal, bottomSideValue);
		glTexCoord2f(1, 0);	glVertex2i(scaledLength, bottomSideValue);
		glTexCoord2f(1, 1);	glVertex2i(scaledLength, topSideValue);
		glTexCoord2f(0, 1);	glVertex2i(minVal, topSideValue);
		glEnd();
	}
}

void Engine::KeyPressEvent(unsigned char key) {
	if (m_gamestate == LOBBY) {
		m_inputChar = SimulateKeyboard(key);
		return;
	}
	switch (key) {
	case 0: // A - Gauche
		if (!m_keyA) {
			m_keyA = true;
		}
		break;
	case 3: // D - Droite
		if (!m_keyD) {
			m_keyD = true;
		}
		break;
	case 18: // S - Reculer
		if (!m_keyS) {
			m_keyS = true;
		}
		break;
	case 22: // W - Avancer
		if (!m_keyW) {
			m_keyW = true;
		}
		break;
	case 36: // ESC - Quitter
		if (m_gamestate == GameState::PLAY) {
			m_gamestate = GameState::PAUSE;
		}
		else if (m_gamestate == GameState::PAUSE) {
			m_gamestate = GameState::PLAY;
		}
		m_audio.ToggleMusicState(m_gamestate);
		break;
	case 57: // Space - Sauter
		if (!m_keySpace) {
			m_keySpace = true;
		}
		break;
	case 94: // F10 - Plein �cran
		IsFullscreen() ? SetFullscreen(false) : SetFullscreen(true);
		//SetFullscreen(!IsFullscreen());
		break;
	case 2: // C - Ignorer
		break;
	case 5: // F - Ignorer
		break;
	case 10: // K - Debugging DisplayNotification()
		m_keyK = true;
		m_messageNotification = "notifications systeme peuvent etre affichees";
		break;
	case 11: // L - Debugging DisplayNotification()
		m_keyL = true;
		break;
	case 6: // G - Ignorer
		break;
	case 12: // M - Ignorer
		break;
	case 7: // H - Ignorer
		break;
	case 8: // I - Ignorer
		break;
	case 9: // J - InstantDamage
		m_damage = true;
	case 15: // P - Ignorer
		break;
	case 17: // R - Ignorer
		break;
	case 19: // T - Ignorer
		break;
	case 24: // Y - Ignorer
		break;
	case 27: // 1
		break;
	case 28: // 2
		break;
	case 255: // Fn - Ignorer
		break;
	default:
		std::cout << "Unhandled key: " << (int)key << std::endl;
	}
}

void Engine::KeyReleaseEvent(unsigned char key) {
	if (m_gamestate == LOBBY) {
		if (key == 38) {
			m_keyShift = false;
		}
		return;
	}
	switch (key) {
	case 0: // A - Stop gauche
		m_keyA = false;
		break;
	case 2: // C - Toggle crosshair
		m_displayCrosshair = !m_displayCrosshair;
		break;
	case 3: // D - Stop droite
		m_keyD = false;
		break;
	case 5: // F - Toggle flash
		m_flash = !m_flash;
		break;
	case 6: // G - Toggle Stop Countdown
		m_stopcountdown = !m_stopcountdown;
		std::cout << "STOP COUNTDOWN " << (m_stopcountdown ? "enabled" : "disabled") << std::endl;
		break;
	case 7: // H - Toggle HUD
		m_displayHud = !m_displayHud;
		std::cout << "DISPLAY HUD " << (m_displayHud ? "enabled" : "disabled") << std::endl;
		break;
	case 8: // I - Toggle render data
		m_displayInfo = !m_displayInfo;
		std::cout << "DISPLAY INFO " << (m_displayInfo ? "enabled" : "disabled") << std::endl;
		break;
	case 10: // K
		m_keyK = false;
		break;
	case 11: // L - Debugging DisplayNotification()
		m_keyL = false;
		break;
	case 12: // M - Toggle music
		m_audio.SetMusic(!m_audio.GetMusic(), m_gamestate);
		break;
	case 15:
		for (int x = 0; x < MAX_BULLETS; ++x)  // Ajouter une balle dans l'array (aussi connu sous le nom de "faire pow pow").
			if (!m_bullets[x]) {
				m_bullets[x] = new Bullet(m_player.GetPOV() - Vector3f(1.f, 0.f, 1.f), Vector3f(1.f, 0.f, 1.f));
				break;
			}
			else if (x == MAX_BULLETS - 1) { // S'il y a pas d'espace dans l'array, prendre la place de la premi�re balle de l'array.
				m_bullets[0]->~Bullet();
				m_bullets[0] = new Bullet(m_player.GetPOV() - Vector3f(1.f, 0.f, 1.f), Vector3f(1.f, 0.f, 1.f));
			}
		break;
	case 17: // R - Toggle skybox
		m_isSkybox = !m_isSkybox;
		break;
	case 18: // S - Stop reculer
		m_keyS = false;
		break;
	case 19: // T -Reset countdown
		m_player.Put(m_otherplayerpos);
		break;
	case 22: // W - Stop avancer
		m_keyW = false;
		break;
	case 24: // Y - Wireframe
		m_wireframe = !m_wireframe;
		if (m_wireframe)
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		else
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		break;
	case 27: // 1
		m_key1 = true;
		break;
	case 28: // 2
		m_key2 = true;
		break;
	case 38: // Shift
		m_keyShift = false;
		break;
	case 57: // Espace - Stop sauter
		m_keySpace = false;
		break;
	case 58: // Enter - comfirm input
		m_keyEnter = false;
		break;
	case 59: // backspace - remove char
		m_keyBackspace = false;
		break;
	}
}

char Engine::SimulateKeyboard(unsigned char key) {
	switch (key) {
	case 0: // A
		if (!m_keyShift) { m_inputChar = 'a'; }
		else { m_inputChar = 'A'; }
		break;
	case 1: // B
		if (!m_keyShift) { m_inputChar = 'b'; }
		else { m_inputChar = 'B'; }
		break;
	case 2: // C
		if (!m_keyShift) { m_inputChar = 'c'; }
		else { m_inputChar = 'C'; }
		break;
	case 3: // D
		if (!m_keyShift) { m_inputChar = 'd'; }
		else { m_inputChar = 'D'; }
		break;
	case 4: // E
		if (!m_keyShift) { m_inputChar = 'e'; }
		else { m_inputChar = 'E'; }
		break;
	case 5: // F
		if (!m_keyShift) { m_inputChar = 'f'; }
		else { m_inputChar = 'F'; }
		break;
	case 6: // G
		if (!m_keyShift) { m_inputChar = 'g'; }
		else { m_inputChar = 'G'; }
		break;
	case 7: // H
		if (!m_keyShift) { m_inputChar = 'h'; }
		else { m_inputChar = 'H'; }
		break;
	case 8: // I
		if (!m_keyShift) { m_inputChar = 'i'; }
		else { m_inputChar = 'I'; }
		break;
	case 9: // J
		if (!m_keyShift) { m_inputChar = 'j'; }
		else { m_inputChar = 'J'; }
		break;
	case 10: // K
		if (!m_keyShift) { m_inputChar = 'k'; }
		else { m_inputChar = 'K'; }
		break;
	case 11: // L
		if (!m_keyShift) { m_inputChar = 'l'; }
		else { m_inputChar = 'L'; }
		break;
	case 12: // M
		if (!m_keyShift) { m_inputChar = 'm'; }
		else { m_inputChar = 'M'; }
		break;
	case 13: // N
		if (!m_keyShift) { m_inputChar = 'n'; }
		else { m_inputChar = 'N'; }
		break;
	case 14: // O
		if (!m_keyShift) { m_inputChar = 'o'; }
		else { m_inputChar = 'O'; }
		break;
	case 15: // P
		if (!m_keyShift) { m_inputChar = 'p'; }
		else { m_inputChar = 'P'; }
		break;
	case 16: // Q
		if (!m_keyShift) { m_inputChar = 'q'; }
		else { m_inputChar = 'Q'; }
		break;
	case 17: // R
		if (!m_keyShift) { m_inputChar = 'r'; }
		else { m_inputChar = 'R'; }
		break;
	case 18: // S
		if (!m_keyShift) { m_inputChar = 's'; }
		else { m_inputChar = 'S'; }
		break;
	case 19: // T
		if (!m_keyShift) { m_inputChar = 't'; }
		else { m_inputChar = 'T'; }
		break;
	case 20: // U
		if (!m_keyShift) { m_inputChar = 'u'; }
		else { m_inputChar = 'U'; }
		break;
	case 21: // V
		if (!m_keyShift) { m_inputChar = 'v'; }
		else { m_inputChar = 'V'; }
		break;
	case 22: // W
		if (!m_keyShift) { m_inputChar = 'w'; }
		else { m_inputChar = 'W'; }
		break;
	case 23: // X
		if (!m_keyShift) { m_inputChar = 'x'; }
		else { m_inputChar = 'X'; }
		break;
	case 24: // Y
		if (!m_keyShift) { m_inputChar = 'y'; }
		else { m_inputChar = 'Y'; }
		break;
	case 25: // Z
		if (!m_keyShift) { m_inputChar = 'z'; }
		else { m_inputChar = 'Z'; }
		break;
	case 26: // 0
		if (!m_keyShift) { m_inputChar = '0'; }
		else { m_inputChar = ')'; }
		break;
	case 27: // 1
		if (!m_keyShift) { m_inputChar = '1'; }
		else { m_inputChar = '!'; }
		break;
	case 28: // 2
		if (!m_keyShift) { m_inputChar = '2'; }
		else { m_inputChar = '\"'; }
		break;
	case 29: // 3
		if (!m_keyShift) { m_inputChar = '3'; }
		else { m_inputChar = '/'; }
		break;
	case 30: // 4
		if (!m_keyShift) { m_inputChar = '4'; }
		else { m_inputChar = '$'; }
		break;
	case 31: // 5
		if (!m_keyShift) { m_inputChar = '5'; }
		else { m_inputChar = '%'; }
		break;
	case 32: // 6
		if (!m_keyShift) { m_inputChar = '6'; }
		else { m_inputChar = '?'; }
		break;
	case 33: // 7
		if (!m_keyShift) { m_inputChar = '7'; }
		else { m_inputChar = '&'; }
		break;
	case 34: // 8
		if (!m_keyShift) { m_inputChar = '8'; }
		else { m_inputChar = '*'; }
		break;
	case 35: // 9
		if (!m_keyShift) { m_inputChar = '9'; }
		else { m_inputChar = ')'; }
		break;
	case 38: // SHIFT
		m_keyShift = true;
		break;
	case 50: // . (Period)
		m_inputChar = '.';
		break;
	case 56: // _ (Underscore)
		if (!m_keyShift) { m_inputChar = '-'; }
		else { m_inputChar = '_'; }
		break;
	case 57: // SPACE
		m_inputChar = ' ';
		break;
	case 58: // ENTER
		m_keyEnter = true;
		break;
	case 59: // BACKSPACE
		m_keyBackspace = true;
		break;
	default:
		m_invalidChar = true;
		break;
	}

	if ((int)key != 38 && !m_invalidChar) {
		m_charChanged = true;
	}

	m_invalidChar = false;
	return m_inputChar;
}

void Engine::HandlePlayerInput(float elapsedTime) {
	static float lobbyTime = 0.0f;
	lobbyTime += elapsedTime;
	float onInterval = 0.5f;
	float offInterval = 1.0f;

	m_textureFont.Bind();
	std::ostringstream ss;

	glColor4f(1.0f, 1.0f, 1.0f, 1.0f);

	if (m_charChanged) {
		if (m_keyBackspace) {
			if (!m_currentInputString.empty()) {
				m_currentInputString.pop_back();
			}
			m_keyBackspace = false;
		}
		else if (m_keyEnter) {
			if (m_settingUsername) {
				if (!m_currentInputString.empty() &&
					m_currentInputString.size() > 1 &&
					m_currentInputString.size() < 26) {
					m_username = m_currentInputString;
					m_player.SetUsername(m_username);
					m_currentInputString = "";
					m_settingUsername = false;

					if (m_selectedMultiPlayer) {
						m_settingServer = true;
					}
					else if (m_selectedSinglePlayer) {
						m_singleReady = true;
					}
				}
			}
			else if (m_settingServer) {
				if (!m_currentInputString.empty()) {
					m_serverAddr = m_currentInputString;
					m_currentInputString = "";
					m_settingServer = false;
					m_multiReady = true;
				}
			}
			m_keyEnter = false;
		}
		else if (m_settingUsername && m_currentInputString.size() < 26) {
			m_currentInputString += m_inputChar;
		}
		else if (m_settingServer && m_currentInputString.size() < 15) {
			m_currentInputString += m_inputChar;
		}
	}

	ss << m_currentInputString;
	m_charChanged = false;
	
	ss << (lobbyTime < onInterval ? "_" : (lobbyTime > onInterval && lobbyTime < offInterval) ? " " : (lobbyTime = 0.0f, ""));
	PrintText(Width() * 0.6f, Height() * 0.4f, ss.str(), 2.0f);
}

void Engine::MouseMoveEvent(int x, int y) {
	if (m_gamestate == GameState::PLAY) {
		m_player.TurnLeftRight(x - (Width() / 2), m_sensitivity);
		m_player.TurnTopBottom(y - (Height() / 2), m_sensitivity);

		// Centrer la souris seulement si elle n'est pas d�j� centr�e
		// Il est n�cessaire de faire la v�rification pour �viter de tomber
		// dans une boucle infinie o� l'appel � CenterMouse g�n�re un
		// MouseMoveEvent, qui rapelle CenterMouse qui rapelle un autre
		// MouseMoveEvent, etc
		if (x == (Width() / 2) && y == (Height() / 2))
			return;
	}
	else if (m_gamestate == GameState::MAIN_MENU) {
		DisplayMainMenu();
	}
	else if (m_gamestate == GameState::OPTIONS) {
		float centerX = Width() * 0.5f;
		float centerY = Height() * 0.5f;
		float leftBar = centerX - Width() * 0.15f;
		float rightBar = centerX + Width() * 0.3f;
		float barLength = rightBar - leftBar;

		float normalizedPosition = (x - leftBar) / barLength;

		if (m_selectedOption == 0 && m_selectedOptAudioMainBar) {
			m_mainvolume = normalizedPosition;
			m_audio.SetMainVolume(m_mainvolume);
		}
		else if (m_selectedOption == 0 && m_selectedOptAudioMusicBar) {
			m_musicvolume = normalizedPosition;
			m_audio.SetMusicVolume(m_musicvolume);
		}
		else if (m_selectedOption == 0 && m_selectedOptAudioSfxBar) {
			m_sfxvolume = normalizedPosition;
			m_audio.SetSfxVolume(m_sfxvolume);
		}
		else if (m_selectedOption == 2 && m_selectedGameplaySensitivityBar) {
			m_sensitivity = normalizedPosition;
		}
	}
}

void Engine::MousePressEvent(const MOUSE_BUTTON& button, int x, int y) {
	m_mousemx = x;
	m_mousemy = y;
	float centerX = Width() * 0.5f;
	float centerY = Height() * 0.5f;

	if (m_gamestate == GameState::PLAY) {
		switch (button) {
		case MOUSE_BUTTON_LEFT:
			m_mouseL = true;
			break;
		case MOUSE_BUTTON_RIGHT:
			m_mouseR = true;
			break;
		case MOUSE_BUTTON_MIDDLE:
			m_mouseC = true;
			break;
		case MOUSE_BUTTON_WHEEL_UP:
			m_mouseWU = true;
			break;
		case MOUSE_BUTTON_WHEEL_DOWN:
			m_mouseWD = true;
			break;
		case MOUSE_BUTTON_NONE: break;
		}
	}
	else if (m_gamestate == GameState::MAIN_MENU) {

		float leftButton = centerX + Width() * 0.045f;
		float rightButton = centerX + Width() * 0.4455f;

		float topFirst = centerY + Height() * 0.073f;
		float bottomFirst = centerY;

		float topSecond = centerY + Height() * 0.177f;
		float bottomSecond = centerY + Height() * 0.105f;

		float topThird = centerY + Height() * 0.275f;
		float bottomThird = centerY + Height() * 0.198f;

		if (x > leftButton && x < rightButton && y > bottomFirst && y < topFirst) {
			m_audio.playSound(AUDIO_PATH "snap.wav", m_sfxvolume);
			if (m_selectedPlayOptions) {
				m_gamestate = GameState::LOBBY;
				m_selectedSinglePlayer = true;
				m_selectedMultiPlayer = false;
				m_settingUsername = true;
			}
			else {
				m_selectedPlayOptions = true;
			}
		}
		else if (x > leftButton && x < rightButton && y > bottomSecond && y < topSecond) {
			m_audio.playSound(AUDIO_PATH "snap.wav", m_sfxvolume);
			if (m_selectedPlayOptions) {
				m_gamestate = GameState::LOBBY;
				m_selectedMultiPlayer = true;
				m_selectedSinglePlayer = false;
				m_settingUsername = true;
			}
			else {
				m_gamestate = GameState::OPTIONS;
			}
		}
		else if (x > leftButton && x < rightButton && y > bottomThird && y < topThird) {
			m_audio.playSound(AUDIO_PATH "snap.wav", m_sfxvolume);
			if (m_selectedPlayOptions) {
				m_selectedPlayOptions = false;
			}
			else {
				Stop();
			}
		}
	}
	else if (m_gamestate == GameState::OPTIONS) {
		float leftAudio = centerX - Width() * 0.35f;
		float rightAudio = centerX - Width() * 0.2f;
		float topAudio = (Height() * 0.25f) + ((Height() * 0.25f) - (Height() * 0.2f));
		float bottomAudio = (Height() * 0.2f) + ((Height() * 0.25f) - (Height() * 0.2f));

		float leftGraph = centerX - Width() * 0.4f;
		float rightGraph = centerX - Width() * 0.2f;
		float topGraph = centerY - Height() * 0.05f;
		float bottomGraph = centerY - Height() * 0.1f;

		float leftGameplay = centerX - Width() * 0.375f;
		float rightGameplay = centerX - Width() * 0.2f;
		float topGameplay = centerY + Height() * 0.1f;
		float bottomGameplay = centerY + Height() * 0.05f;

		float leftBack = centerX - Width() * 0.075f;
		float rightBack = centerX + Width() * 0.075f;
		float topBack = centerY + Height() * 0.25f;
		float bottomBack = centerY + Height() * 0.2f;

		if (x > leftAudio && x < rightAudio && y > bottomAudio && y < topAudio) {
			m_audio.playSound(AUDIO_PATH "snap.wav", m_sfxvolume);
			m_selectedOption = 0; // Volume
		}
		else if (x > leftGraph && x < rightGraph && y > bottomGraph && y < topGraph) {
			m_audio.playSound(AUDIO_PATH "snap.wav", m_sfxvolume);
			m_selectedOption = 1; // Graphics
		}
		else if (x > leftGameplay && x < rightGameplay && y > bottomGameplay && y < topGameplay) {
			m_audio.playSound(AUDIO_PATH "snap.wav", m_sfxvolume);
			m_selectedOption = 2; // Gameplay
		}
		else if (x > leftBack && x < rightBack && y > bottomBack && y < topBack) {
			m_audio.playSound(AUDIO_PATH "snap.wav", m_sfxvolume);
			m_gamestate = GameState::MAIN_MENU;
		}

		// Audio
		float leftBar = centerX - Width() * 0.15f;
		float rightBar = centerX + Width() * 0.3f;
		float barLength = rightBar - leftBar;

		float topBarPrincipal = centerY - Height() * 0.165f;
		float bottomBarPrincipal = centerY - Height() * 0.2f;

		float topBarMusique = centerY - Height() * 0.0415f;
		float bottomBarMusique = centerY - Height() * 0.075f;

		float topBarEffets = centerY + Height() * 0.085f;
		float bottomBarEffets = centerY + Height() * 0.05f;

		float normalizedPosition = (x - leftBar) / barLength;

		if (m_selectedOption == 0 && x > leftBar && x < rightBar && y > bottomBarPrincipal && y < topBarPrincipal) {
			m_selectedOptAudioMainBar = true;
			m_mainvolume = normalizedPosition;
			m_audio.SetMainVolume(m_mainvolume);
			m_audio.playSound(AUDIO_PATH "snap.wav", m_sfxvolume);
		}
		else if (m_selectedOption == 0 && x > leftBar && x < rightBar && y > bottomBarMusique && y < topBarMusique) {
			m_selectedOptAudioMusicBar = true;
			m_musicvolume = normalizedPosition;
			m_audio.SetMusicVolume(m_musicvolume);
			m_audio.playSound(AUDIO_PATH "snap.wav", m_sfxvolume);
		}
		else if (m_selectedOption == 0 && x > leftBar && x < rightBar && y > bottomBarEffets && y < topBarEffets) {
			m_selectedOptAudioSfxBar = true;
			m_sfxvolume = normalizedPosition;
			m_audio.SetSfxVolume(m_sfxvolume);
			m_audio.playSound(AUDIO_PATH "snap.wav", m_sfxvolume);
		}

		// Resolution
		float leftChk = centerX + Width() * 0.075f;
		float rightChk = centerX + Width() * 0.09f;

		float topChkHD = centerY - Height() * 0.125f;
		float bottomChkHD = centerY - Height() * 0.155f;

		float topChkFHD = centerY - Height() * 0.0725f;
		float bottomChkFHD = centerY - Height() * 0.1f;

		float topChkQHD = centerY - Height() * 0.025f;
		float bottomChkQHD = centerY - Height() * 0.05f;

		float topChkUHD = centerY + Height() * 0.025f;
		float bottomChkUHD = centerY;

		if (m_selectedOption == 1) {
			if (x > leftChk && x < rightChk && y > bottomChkHD && y < topChkHD) {
				m_audio.playSound(AUDIO_PATH "snap.wav", m_sfxvolume);
				ChangeResolution(HD);
			}
			else if (x > leftChk && x < rightChk && y > bottomChkFHD && y < topChkFHD) {
				m_audio.playSound(AUDIO_PATH "snap.wav", m_sfxvolume);
				ChangeResolution(FHD);
			}
			else if (x > leftChk && x < rightChk && y > bottomChkQHD && y < topChkQHD) {
				m_audio.playSound(AUDIO_PATH "snap.wav", m_sfxvolume);
				ChangeResolution(QHD);
			}
			else if (x > leftChk && x < rightChk && y > bottomChkUHD && y < topChkUHD) {
				m_audio.playSound(AUDIO_PATH "snap.wav", m_sfxvolume);
				ChangeResolution(UHD);
			}
		}

		if (normalizedPosition < 0.1f) {
			normalizedPosition = 0.1f;
		}
		else if (normalizedPosition > 1.0f) {
			normalizedPosition = 1.0f;
		}

		// Gameplay
		if (m_selectedOption == 2 && x > leftBar && x < rightBar && y > bottomBarPrincipal && y < topBarPrincipal) {
			m_audio.playSound(AUDIO_PATH "snap.wav", m_sfxvolume);
			m_selectedGameplaySensitivityBar = true;
			m_sensitivity = normalizedPosition;
			m_player.SetSensitivity(m_sensitivity);
		}
	}
	else if (m_gamestate == GameState::PAUSE) {
		float leftResume = centerX - Width() * 0.4735f;
		float rightResume = centerX - Width() * 0.1765f;
		float topResume = centerY - Height() * 0.271f;
		float bottomResume = centerY - Height() * 0.4415f;

		float leftMainMenu = centerX + Width() * 0.17f;
		float rightMainMenu = centerX + Width() * 0.4875f;
		float topMainMenu = centerY - Height() * 0.255f;
		float bottomMainMenu = centerY - Height() * 0.4415f;

		if (x > leftResume && x < rightResume && y > bottomResume && y < topResume) {
			m_audio.playSound(AUDIO_PATH "snap.wav", m_sfxvolume);
			m_gamestate = GameState::PLAY;
			m_audio.ToggleMusicState(m_gamestate);
		}
		else if (x > leftMainMenu && x < rightMainMenu && y > bottomMainMenu && y < topMainMenu) {
			m_audio.playSound(AUDIO_PATH "snap.wav", m_sfxvolume);
			m_gamestate = GameState::MAIN_MENU;
			m_selectedPlayOptions = false;
			m_selectedSinglePlayer = false;
			m_selectedMultiPlayer = false;
			m_singleReady = false;
			m_multiReady = false;
			m_audio.ToggleMusicState(m_gamestate);
			if (m_networkgame) {
				if (m_conn.m_sock_udp)
					closesocket(m_conn.m_sock_udp);
				if (m_conn.m_sock_udp)
					closesocket(m_conn.m_sock_udp);
			}
		}
	}
	else if (m_gamestate == GameState::LOBBY) {
		float leftBack = centerX + Width() * 0.178f;
		float rightBack = centerX + Width() * 0.397f;
		float topBack = centerY + Height() * 0.346f;
		float bottomBack = centerY + Height() * 0.275f;

		if (leftBack && x < rightBack && y > bottomBack && y < topBack) {
			m_audio.playSound(AUDIO_PATH "snap.wav", m_sfxvolume);
			m_gamestate = GameState::MAIN_MENU;
		}
	}
}

void Engine::MouseReleaseEvent(const MOUSE_BUTTON& button, int x, int y) {
	float centerX = Width() * 0.5f;
	float centerY = Height() * 0.5f;
	float leftBar = centerX - Width() * 0.15f;
	float rightBar = centerX + Width() * 0.3f;
	float barLength = rightBar - leftBar;

	float normalizedPosition = (x - leftBar) / barLength;

	if (normalizedPosition > 1.0f) {
		normalizedPosition = 1.0f;
	}

	switch (button) {
	case MOUSE_BUTTON_LEFT:
		if (m_selectedOption == 0) {
			if (normalizedPosition < 0.0f) {
				normalizedPosition = 0.0f;
			}

			if (m_selectedOptAudioMainBar) {
				m_mainvolume = normalizedPosition;
				m_audio.SetMainVolume(m_mainvolume);
			}
			else if (m_selectedOptAudioMusicBar) {
				m_musicvolume = normalizedPosition;
				m_audio.SetMusicVolume(m_musicvolume);
			}
			else if (m_selectedOptAudioSfxBar) {
				m_sfxvolume = normalizedPosition;
				m_audio.SetSfxVolume(m_sfxvolume);
			}
		}
		else {
			if (m_selectedGameplaySensitivityBar) {
				if (normalizedPosition < 0.1f) {
					normalizedPosition = 0.1f;
				}

				m_sensitivity = normalizedPosition;
				m_player.SetSensitivity(m_sensitivity);
			}
		}
		m_mouseL = false;
		m_block = false;
		m_selectedOptAudioMainBar = false;
		m_selectedOptAudioMusicBar = false;
		m_selectedOptAudioSfxBar = false;
		m_selectedGameplaySensitivityBar = false;
		break;
	case MOUSE_BUTTON_RIGHT:
		m_mouseR = false;
		m_block = false;
		break;
	case MOUSE_BUTTON_MIDDLE:
		m_mouseC = false;
		break;
	case MOUSE_BUTTON_WHEEL_UP:
		m_mouseWU = false;
		break;
	case MOUSE_BUTTON_WHEEL_DOWN:
		m_mouseWD = false;
		break;
	case MOUSE_BUTTON_NONE: break;
	}
}

bool Engine::LoadTexture(Texture& texture, const std::string& filename, bool useMipmaps, bool stopOnError) {
	texture.Load(filename, useMipmaps);
	if (!texture.IsValid()) {
		std::cerr << "Unable to load texture (" << filename << ")" << std::endl;
		if (stopOnError)
			Stop();

		return false;
	}

	return true;
}

void Engine::ChangeResolution(Resolution resolution) {
	DeInit();

	switch (resolution) {
		case HD:
			OpenglContext::InitWindow(1280, 720);
			m_resolution = HD;
			break;
		case FHD:
			OpenglContext::InitWindow(1920, 1080);
			m_resolution = FHD;
			break;
		case QHD:
			OpenglContext::InitWindow(2560, 1440);
			m_resolution = QHD;
			break;
		case UHD:
			OpenglContext::InitWindow(3840, 2160);
			m_resolution = UHD;
			break;
	}

	Init();
}


