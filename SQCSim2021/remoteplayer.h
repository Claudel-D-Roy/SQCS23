#ifndef REMOTEPLAYER_H__
#define REMOTEPLAYER_H__
#include "../SQCSim-common/player.h"
#include "../SQCSim-common/netprotocol.h"
#include "define.h"
#include "textureatlas.h"
#include "shader.h"
#include "texture.h"
#include "openglcontext.h"
#include "vertexbuffer.h"
#include "../SQCSim-common/matrix4.h"

class RemotePlayer : public Player {
public:
	enum Anim: uint8_t { STILL = 1, RUNNING = 2, JUMPING = 4, SHOOTING = 8, POWERUP = 16,  DEAD = 32 }; // A REVOIR VOIR DEFINE.H POUR LES ANIMES
	
	RemotePlayer(netprot::PlayerInfo* pinfo);
	RemotePlayer(netprot::PlayerInfo* pinfo, const Vector3f& pos);
	~RemotePlayer();


	void Init();
	void Feed(const netprot::Output out);
	void Render(TextureAtlas& atlas, Shader& shader, Transformation tran, float elapsedTime, Player& camera);
	bool LoadTexture(Texture& texture, const std::string& filename, bool useMipmaps, bool stopOnError);
	
	void SetPosition(Vector3f pos) { m_position = pos; }

private:
	netprot::Output current, previous;
	std::map<uint64_t, netprot::Output> m_outbuf;
	netprot::PlayerInfo m_pinfo;
	float m_aminacc;
	Anim m_animstate;
	uint64_t m_team_id;
	Texture m_texture_front;

	
	

};
#endif