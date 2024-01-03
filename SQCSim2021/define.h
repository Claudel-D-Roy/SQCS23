#ifndef CLI_DEFINE_H__
#define CLI_DEFINE_H__

//#define SFML_STATIC true

#include <iostream>
#include <chrono>
#include <iomanip>
#include <GL/glew.h>
#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>
#include "../SQCSim-common/define.h"

#ifdef _WIN32
#include <gl/GL.h>
#include <gl/GLU.h>
#else
#include <GL/gl.h>
#include <GL/glu.h>
#include <climits>
#endif

#define SRV_ADDR "127.0.0.1"
#define COUNTDOWN 300

#define BULLET_UPDATES_PER_FRAME 20

#define BASE_WIDTH 1280
#define BASE_HEIGHT 720


#define ANIME_PATH_JUMP		"./media/textures/AssetOtherPlayer/FinalPNGJumping/"
#define ANIME_PATH_STILL	"./media/textures/AssetOtherPlayer/FinalPNGStanding/"

//1 = jump shoot sans anim, 2 = jump shoot avec anim
#define ANIM_PATH_JSHOOT1    "./media/textures/AssetOtherPlayer/FinalPNGJumpingShooting/"
#define ANIM_PATH_JSHOOT2	"./media/textures/AssetOtherPlayer/FinalPNGJumpingShooting/ShootingJump/"
//1 = shoot sans anim, 2 = shoot avec anim
#define ANIM_PATH_SSHOOT1	"./media/textures/AssetOtherPlayer/FinalPNGShooting/"
#define ANIM_PATH_SSHOOT2	"./media/textures/AssetOtherPlayer/FinalPNGShooting/Shooting/"

#define TEXTURE_PATH        "./media/textures/"
#define SHADER_PATH		    "./media/shaders/"
#define AUDIO_PATH			"./media/audio/"
#define CHUNK_PATH			"./media/chunks/"
#define BOOSTER_TEXTURE_PATH "./media/textures/Booster/"

enum GameState { 
	MAIN_MENU, 
	SPLASH,
	LOBBY,
	OPTIONS,
	PLAY, 
	PAUSE
};

enum Resolution {
	HD = 0,		// 1280x720	 (High Definition)
	FHD,		// 1920x1080 (Full HD)
	QHD,		// 2560x1440 (Quad HD)
	UHD,		// 3840x2160 (Ultra HD)
};

#endif // DEFINE_H__
