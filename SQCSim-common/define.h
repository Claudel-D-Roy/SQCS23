#ifndef DEFINE_H__
#define DEFINE_H__

#include <iostream>
#include <chrono>

#define SRV_PORT 10000
#define CLI_PORT 10001

#define BUFFER_LENGTH 1500

#define CHUNK_SIZE_X  6
#define CHUNK_SIZE_Y  64
#define CHUNK_SIZE_Z  6
#define MAX_SELECTION_DISTANCE 5
#define SEED 0
#define COUNTDOWN 300

#define WORLD_SIZE_X 64
#define WORLD_SIZE_Y 64

#define FRAMES_RENDER_CHUNKS 1
#define FRAMES_UPDATE_CHUNKS 1
#define FRAMES_DELETE_CHUNKS 1

#define THREADS_GENERATE_CHUNKS 8
#define THREADS_UPDATE_CHUNKS 6
#define THREADS_DELETE_CHUNKS 3

#define VIEW_DISTANCE 512 // Si les chunks arr�tent de s'afficher pendant une game et qu'il y a un access violation quand tu quitte, il faut augmenter ce chiffre.
#define TEXTURE_SIZE 512
#define MAX_BULLETS 512

#define NB_BOOST 2
#define TIME_SPEED_BOOST 10		//secondes
#define TIME_DAMAGE_BOOST 10	//secondes
#define TIME_INVINCIBLE_BOOST 4	//secondes
#define STRENGTH_SPEED_BOOST 1000	//Pourcentage
#define BULLET_TIME .2 //secondes

#define SYNC_ACC 600  // ms
#define CMOD_ACC 1000  // ms

typedef uint8_t BlockType;
enum BLOCK_TYPE { BTYPE_AIR, BTYPE_DIRT, BTYPE_GRASS, BTYPE_METAL, BTYPE_ICE, BTYPE_GREENGRASS, BTYPE_LAST };
typedef uint8_t BoostType;
enum BOOST_TYPE { BTYPE_SPEED, BTYPE_HEAL, BTYPE_DAMAGE, BTYPE_INVINCIBLE, BTYPE_BOOST_LAST };

//anim 
enum ANIM_TYPE { STILL = 0, SHOOTING = 8, JUMPING = 16, JUMPINGSHOOTING = 24, DEAD = 32, TYPE_LAST = 40};
enum ANIM_POS {FRONT, QUARTER_FRONT_LEFT, QUATER_FRONT_RIGHT, PROFIL_LEFT, PROFIL_RIGHT, QUARTER_BACK_LEFT, QUARTER_BACK_RIGHT, BACK , POS_LAST};

typedef uint64_t Timestamp;

#ifdef _WIN32

#pragma comment(lib,"wsock32.lib") // Pour pouvoir faire fonctionner le linker sans le vcxproject
#pragma comment(lib,"ws2_32.lib")

#include <ws2tcpip.h>
#include <Windows.h>
#include <cstdio>
#include <ctime>

#define poll WSAPoll
#define flag_t u_long
#define addrlen_t int
#define ioctl ioctlsocket
#define SOCK_NONBLOCK FIONBIO
#define strcpy strcpy_s
#define popen _popen
#define pclose _pclose

#else // Pas _WIN32

#include <unistd.h>
#include <time.h>
#include <stdio.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <cstring>
#include <poll.h>

#define flag_t unsigned int
#define addrlen_t unsigned int
#define SOCKET int
#define INVALID_SOCKET -1
#define closesocket close

#endif // _WIN32


#endif // DEFINE_H__
