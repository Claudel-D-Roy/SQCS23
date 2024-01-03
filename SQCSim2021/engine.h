#ifndef ENGINE_H__
#define ENGINE_H__

#include <algorithm>
#include <chrono>
#include <cmath>
#include <unordered_map>
#include <set>
#include "../SQCSim-common/array2d.h"
#include "../SQCSim-common/blockinfo.h"
#include "../SQCSim-common/boostinfo.h"
#include "../SQCSim-common/bullet.h"
#include "../SQCSim-common/chunk.h"
#include "../SQCSim-common/world.h"
#include "../SQCSim-common/transformation.h"
#include "../SQCSim-common/player.h"
#include "../SQCSim-common/booster.h"
#include "define.h"
#include "openglcontext.h"
#include "texture.h"
#include "shader.h"
#include "skybox.h"
#include "audio.h"
#include "textureatlas.h"
#include "connector.h"
#include "renderer.h"
#include "remoteplayer.h"
#include "settings.h"

class Engine : public OpenglContext {
public:
    Engine();
    virtual ~Engine();



    virtual void Init();
    virtual void DeInit();
    virtual void LoadResource();
    virtual void UnloadResource();
    virtual void Render(float elapsedTime);
    virtual void KeyPressEvent(unsigned char key);
    virtual void KeyReleaseEvent(unsigned char key);
    virtual void MouseMoveEvent(int x, int y);
    virtual void MousePressEvent(const MOUSE_BUTTON& button, int x, int y);
    virtual void MouseReleaseEvent(const MOUSE_BUTTON& button, int x, int y);

private:
    int GetFps(float elapsedTime) const;
    int GetCountdown(float elapsedTime);
    int GetOptionsChoice();
    bool StartMultiplayerGame();

    bool LoadTexture(Texture& texture, const std::string& filename, bool useMipmaps = true, bool stopOnError = true);
    void ChangeResolution(Resolution resolution);

    void InstantDamage();
    void SystemNotification(std::string systemLog);
    void KillNotification(Player killer, Player killed);

    void DisplayNotification(std::string message);

    void DisplayCrosshair();
    void DisplayPovGun();
    void DisplayCurrentItem();
    void DisplayHud(int timer);
    void DrawHud(float elapsedTime, BlockType bloc);
    void DisplayInfo(float elapsedTime, BlockType bloc);

    void DisplaySplashScreen();

    void DisplayMainMenu();
    void DrawButtonBackgrounds(float centerX, float centerY, int iterations);
    void DrawMainMenuButtons(float centerX, float centerY);
    void DrawSingleMultiButtons(float centerX, float centerY);

    void DisplayLobbyMenu(float elapsedTime);
    void SetPlayerUsername(float elapsedTime);
    void SetServerAddress(float elapsedTime);

    void DisplayPauseMenu(float elapsedTime);

    void DisplayOptionsMenu();
    void DisplayAudioMenu(float centerX, float centerY);
    void DisplayGraphicsMenu(float centerX, float centerY);
    void DisplayGameplayMenu(float centerX, float centerY);
    void DrawSliderBackground(float centerX, float centerY, float minVal, float maxVal, float bottomSideValue, float topSideValue);
    void DisplayBarPercentValue(float centerX, float centerY, float posX, float posY, float minVal, float maxVal, float value);
    void DrawSlider(float centerX, float centerY, float value, float minVal, float maxVal, float bottomSideValue, float topSideValue);

    void PrintText(float x, float y, const std::string& t, float charSizeMultiplier = 1.0f);
    void ProcessNotificationQueue();
    char SimulateKeyboard(unsigned char key);
    void HandlePlayerInput(float elapsedTime);

    Audio m_audio = Audio(AUDIO_PATH "music01.wav", AUDIO_PATH "menumusic01.wav");
    irrklang::ISound* m_powpow, * m_scream;
    irrklang::ISound* m_whoosh[MAX_BULLETS];

    Bullet* m_bullets[MAX_BULLETS];

    //Menu
    Vector3f m_otherplayerpos = Vector3f(999, 999, 999);

    World m_world = World();
    Player m_player = Player(Vector3f(.5f, CHUNK_SIZE_Y + 1.8f, .5f));

    Renderer m_renderer = Renderer();

    BlockInfo* m_blockinfo[BTYPE_LAST];
    BoostInfo* m_boostinfo[BTYPE_BOOST_LAST];
    GameState m_gamestate = GameState::SPLASH;

    Shader m_shader01;

    Skybox m_skybox;

    TextureAtlas m_textureAtlas = TextureAtlas(BTYPE_LAST);
    TextureAtlas m_animeAtlas = TextureAtlas(TYPE_LAST + POS_LAST);

    TextureAtlas::TextureIndex texBoostHeal;

    Texture m_textureCrosshair;
    Texture m_textureFont;
    Texture m_textureGun;
    Texture m_texturePovGun;
    Texture m_textureSkybox;

    Texture m_textureLobbyMenu;
    Texture m_textureMainMenu;
    Texture m_textureOptionsMenu;
    Texture m_texturePauseMenu;
    Texture m_textureSplashScreen;

    Texture m_textureHd;
    Texture m_textureFhd;
    Texture m_textureQhd;
    Texture m_textureUhd;

    Texture m_textureLobbyServer;
    Texture m_textureLobbyIdentify;
    Texture m_textureCheck;
    Texture m_textureChecked;

    Texture m_texturePauseResume;
    Texture m_texturePauseMainMenu;

    Texture m_textureOptAudio;
    Texture m_textureOptBack;
    Texture m_textureOptGameplay;
    Texture m_textureOptGraphics;
    Texture m_textureOptMain;
    Texture m_textureOptMusic;
    Texture m_textureOptOptions;
    Texture m_textureOptResolution;
    Texture m_textureOptSensitivity;
    Texture m_textureOptSfx;

    Texture m_textureMenuBack;
    Texture m_textureMenuMulti;
    Texture m_textureMenuOptions;
    Texture m_textureMenuPlay;
    Texture m_textureMenuQuit;
    Texture m_textureMenuSingle;
    Texture m_textureMenuTitle;

    Settings m_options = Settings(m_audio);

    Resolution m_resolution = HD;

    float m_splashTime = 2.0f;
    float m_scale;
    float m_time = 0;

    float m_titleX = 0;
    float m_titleY = 0;



    int m_renderCount = 0;
    int m_countdown = COUNTDOWN;
    int m_nbReductionChunk = 4;
    int m_timerReductionChunk = 30;

    float m_mainvolume;
    float m_musicvolume;
    float m_sfxvolume;
    float m_sensitivity;

    int m_selectedOption = 0;

    bool m_selectedOptAudioMainBar = false;
    bool m_selectedOptAudioMusicBar = false;
    bool m_selectedOptAudioSfxBar = false;
    bool m_selectedGameplaySensitivityBar = false;

    bool m_damage = false;

    bool m_wireframe = false;
    bool m_isSkybox = true;
    bool m_block = false;
    bool m_flash = true;
    bool m_displayCrosshair = true;
    bool m_displayHud = true;
    bool m_displayInfo = true;
    bool m_resetcountdown = false;
    bool m_soloMultiChoiceMade = false;
    bool m_stopcountdown = false;

    bool m_selectedPlayOptions = false;
    bool m_selectedOptions = false;
    bool m_selectedQuit = false;

    std::string m_currentInputString;
    std::string m_username;
    std::string m_serverAddr;

    char m_inputChar = 0;
    bool m_invalidChar = false;
    bool m_charChanged = false;
    bool m_settingUsername = false;
    bool m_settingServer = false;
    bool m_selectedSinglePlayer = false;
    bool m_selectedMultiPlayer = false;
    bool m_singleReady = false;
    bool m_multiReady = false;


    bool m_key1 = false;
    bool m_key2 = false;

    bool m_keyK = false;
    bool m_keyL = false;
    bool m_keyW = false;
    bool m_keyA = false;
    bool m_keyS = false;
    bool m_keyD = false;
    bool m_keyEnter = false;
    bool m_keySpace = false;
    bool m_keyShift = false;
    bool m_keyBackspace = false;

    bool m_mouseL = false;
    bool m_mouseR = false;
    bool m_mouseC = false;
    bool m_mouseWU = false;
    bool m_mouseWD = false;

    float m_mousemx = 0;
    float m_mousemy = 0;

    bool m_networkgame = false;

    Connector m_conn;
    std::deque<netprot::ChunkMod*> m_chunkmod_manifest;
    std::chrono::high_resolution_clock::time_point m_startTime;
    std::unordered_map<uint64_t, Player*> m_players;
    std::set<uint64_t> m_deadplayers;
    netprot::Buffer m_buf, m_bufout;
    netprot::ChunkMod* m_chunkmod = nullptr;
    std::unordered_map<uint64_t, Booster*> m_boosters;
    std::set<uint64_t> m_boost_manifest;

    std::unordered_map<uint64_t, netprot::Sync> m_syncs;
    std::string m_messageNotification = "";
};

#endif // ENGINE_H__