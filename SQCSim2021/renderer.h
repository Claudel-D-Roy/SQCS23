#ifndef RENDERER_H__
#define RENDERER_H__
#include <future>
#include <thread>
#include "../SQCSim-common/world.h"
#include "../SQCSim-common/transformation.h"
#include "define.h"
#include "mesh.h"
#include "textureatlas.h"
#include "shader.h"
#include "remoteplayer.h"
#include "openglcontext.h"
#include "vertexbuffer.h"
#include "../SQCSim-common/matrix4.h"

class Renderer {
private:
	Array2d<Mesh*> m_meshes = Array2d<Mesh*>(WORLD_SIZE_X, WORLD_SIZE_Y);

	TextureAtlas* m_playertext = nullptr;
	Shader* m_playershader = nullptr;
	bool test = true;

public:
	Renderer();
	~Renderer();

	void RemoveChunk(int nbReduit);

	void UpdateMesh(World* origin, const Vector3f& player, BlockInfo* blockinfo[BTYPE_LAST]);

	void RenderBooster(TextureAtlas& textureAtlas, Shader& shader, Transformation tran, Player player, Booster* booster);

	void RenderWorld(World* origin, int& rendercount, const Vector3f& player_pos, const Vector3f& player_dir, Transformation world, Shader& shader, TextureAtlas& atlas) const;
	void RenderPlayer(Player* player, Transformation tran) const;
	void RenderPlayer(RemotePlayer* rplayer, const Vector3f& player_pos, const Vector3f& player_dir) const;
};
#endif