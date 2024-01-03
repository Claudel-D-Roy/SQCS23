#include "renderer.h"
#include <iostream>
#include <cstring>

#include <thread>
#include <queue>

Renderer::Renderer() {
	m_meshes.Reset(nullptr);
}

Renderer::~Renderer() {
}

void Renderer::RemoveChunk(int nbReduit)
{
	for (int x = 0; x < WORLD_SIZE_X; ++x)
		for (int y = 0; y < WORLD_SIZE_Y; ++y)
		{
			Mesh* chk = nullptr;
			if (x < nbReduit)
				chk = m_meshes.Remove(x, y);
			if (y < nbReduit)
				chk = m_meshes.Remove(x, y);
			if (y > WORLD_SIZE_Y - nbReduit)
				chk = m_meshes.Remove(x, y);
			if (x > WORLD_SIZE_X - nbReduit)
				chk = m_meshes.Remove(x, y);

			// TODO: MakeDirty() les voisins pour qu'ils se redessinent.

			if (!chk)
				continue;

			delete chk;
		}


}

void Renderer::RenderWorld(World* origin, int& rendercount, const Vector3f& player_pos, const Vector3f& player_dir, Transformation world, Shader& shader, TextureAtlas& atlas) const {
	rendercount = 0;
	Vector3f angle;
	Vector3f cursor;
	Vector3f direct = player_dir;
	Vector3f pos = player_pos - direct;

	direct.y = 0;
	direct.Normalize();
	pos.y = 1;

	static Vector3<unsigned int> renderManifest[VIEW_DISTANCE * 8]; // Nombre de Chunks maximal � �tre rendus.

	//for (int dist = VIEW_DISTANCE; dist >= 0; dist -= CHUNK_SIZE_X) {
	for (int dist = 0; dist <= VIEW_DISTANCE; dist += CHUNK_SIZE_X) {
		// Configuration du radar.
		float sinus, cosinus;
		int echantillons;

		if (dist > VIEW_DISTANCE * .1f) {
			sinus = .00872653549f; // sin(1/2 degr�)
			cosinus = .99996192306; // cos(1/2 degr�)
			echantillons = 180;
		}
		//else {//if (dist > VIEW_DISTANCE * .3f) {
		//	sinus = .01151891831f; // sin(2/3 degr�)
		//	cosinus = .99993365506; // cos(2/3 degr�)
		//	echantillons = 120;
		//}
		//else if (dist > VIEW_DISTANCE * .2f) {
		//	sinus = .01745240643; // sin(1 degr�)
		//	cosinus = .99984769515; // cos(1 degr�)
		//	echantillons = 90;
		//}
		//else if (dist > VIEW_DISTANCE * .1f) {
		//	sinus = .0261769483;
		//	cosinus = .99965732497;
		//	echantillons = 60;
		//}
		else {
			sinus = .0348994967;
			cosinus = .99939082701;
			echantillons = 45;
		}

		angle.x = direct.z + direct.x;
		angle.z = direct.z - direct.x;
		angle.y = 0;
		angle.Normalize();

		for (int radar = 0; radar < echantillons; ++radar) {
			float x = angle.x;

			angle.x = angle.x * cosinus - angle.z * sinus;
			angle.z = angle.z * cosinus + x * sinus;
			angle.Normalize();

			cursor = pos - direct * CHUNK_SIZE_X * 4 + angle * dist;
			if (cursor.y >= 128.f || cursor.y >= 0.f) cursor.y = CHUNK_SIZE_Y / 4.f;


			if (origin->ChunkAt(cursor)) {
				bool valide = true;
				unsigned int chx, chy;
				origin->ChunkAt(cursor)->GetPosition(chx, chy);
				for (int index = 0; index < rendercount; ++index) // Permet de v�rifier seulement contre celles ajout�es dans la frame, et ne pas avoir � refaire l'array � chaque frame.
					if (renderManifest[index].x == chx && renderManifest[index].z == chy)
						valide = false;

				if (valide) renderManifest[rendercount++] = Vector3<unsigned int>(chx,
					(VIEW_DISTANCE - (pos - cursor).Length() * 3.f + 256.f) < 0.f ? 0 :
					(VIEW_DISTANCE - (pos - cursor).Length() * 3.f + 256.f) * 1000,
					chy);
			}
		}
	}
	shader.Use();
	atlas.Bind();
	glStencilFunc(GL_EQUAL, 1, 0x00);
	glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);

	unsigned int sx, sy, cx, cy;
	origin->GetScope(sx, sy);

	for (int index = 0; index < rendercount; ++index) {
		int chx = (renderManifest[index].x - sx) * CHUNK_SIZE_X, chy = (renderManifest[index].z - sy) * CHUNK_SIZE_Z;

		world.ApplyTranslation(chx, 0, chy);
		glLoadMatrixf(world.GetMatrix().GetInternalValues());
		float blcolor = renderManifest[index].y / (VIEW_DISTANCE / 50.f);
		glBlendColor(blcolor, blcolor, blcolor, 1.f);
		origin->ChunkAt(chx, 1, chy)->GetPosition(cx, cy);
		if (m_meshes.Get(cx - sx, cy - sy))
			m_meshes.Get(cx - sx, cy - sy)->Render();
		world.ApplyTranslation(-chx, 0, -chy);
	}
	shader.Disable();
	glStencilFunc(GL_GREATER, 1, 0xFF);
}


void Renderer::UpdateMesh(World* origin, const Vector3f& player, BlockInfo* blockinfo[BTYPE_LAST]) {
	int cx = player.x;
	int cy = player.z;
	static int frameUpdate = 2;
	int side = 0;
	int threads = 0;
	std::future<Mesh*> updateThList[THREADS_UPDATE_CHUNKS];

	unsigned int mx = 0, my = 0, sx, sy;

	origin->GetScope(sx, sy);

	if (frameUpdate > 0) --frameUpdate;

	if (!frameUpdate)
		while (side * CHUNK_SIZE_X <= VIEW_DISTANCE * 2) {
			int tx = -side, ty = -side;

			for (; tx <= side; ++tx) {
				if (frameUpdate)
					break;
				unsigned int chx = cx + tx * CHUNK_SIZE_X, chy = cy + ty * CHUNK_SIZE_Z;
				if (origin->ChunkAt(chx, 1, chy) &&
					origin->ChunkAt(chx, 1, chy)->IsDirty()) {
					origin->ChunkAt(chx, 1, chy)->GetPosition(mx, my);
					if (m_meshes.Get(mx - sx, my - sy))
						updateThList[threads++] =
						std::async(std::launch::async,
							[](Mesh* mesh, BlockInfo* blockinfo[BTYPE_LAST], World* world) {
								mesh->Update(blockinfo, world); return mesh; }, m_meshes.Get(mx - sx, my - sy), blockinfo, origin);
					else updateThList[threads++] = std::async(std::launch::async,
						[](Chunk* chunk) { return new Mesh(chunk); }, origin->ChunkAt(chx, 1, chy));
					if (threads == THREADS_UPDATE_CHUNKS) frameUpdate = FRAMES_UPDATE_CHUNKS;
				}
			}
			for (; ty <= side; ++ty) {
				if (frameUpdate)
					break;
				unsigned int chx = cx + tx * CHUNK_SIZE_X, chy = cy + ty * CHUNK_SIZE_Z;
				if (origin->ChunkAt(chx, 1, chy) &&
					origin->ChunkAt(chx, 1, chy)->IsDirty()) {
					origin->ChunkAt(chx, 1, chy)->GetPosition(mx, my);
					if (m_meshes.Get(mx - sx, my - sy))
						updateThList[threads++] =
						std::async(std::launch::async,
							[](Mesh* mesh, BlockInfo* blockinfo[BTYPE_LAST], World* world) {
								mesh->Update(blockinfo, world); return mesh; }, m_meshes.Get(mx - sx, my - sy), blockinfo, origin);
					else updateThList[threads++] = std::async(std::launch::async,
						[](Chunk* chunk) { return new Mesh(chunk); }, origin->ChunkAt(chx, 1, chy));
					if (threads == THREADS_UPDATE_CHUNKS) frameUpdate = FRAMES_UPDATE_CHUNKS;
				}
			}
			for (; tx >= -side; --tx) {
				if (frameUpdate)
					break;
				unsigned int chx = cx + tx * CHUNK_SIZE_X, chy = cy + ty * CHUNK_SIZE_Z;
				if (origin->ChunkAt(chx, 1, chy) &&
					origin->ChunkAt(chx, 1, chy)->IsDirty()) {
					origin->ChunkAt(chx, 1, chy)->GetPosition(mx, my);
					if (m_meshes.Get(mx - sx, my - sy))
						updateThList[threads++] =
						std::async(std::launch::async,
							[](Mesh* mesh, BlockInfo* blockinfo[BTYPE_LAST], World* world) {
								mesh->Update(blockinfo, world); return mesh; }, m_meshes.Get(mx - sx, my - sy), blockinfo, origin);
					else updateThList[threads++] = std::async(std::launch::async,
						[](Chunk* chunk) { return new Mesh(chunk); }, origin->ChunkAt(chx, 1, chy));
					if (threads == THREADS_UPDATE_CHUNKS) frameUpdate = FRAMES_UPDATE_CHUNKS;
				}
			}
			for (; ty >= -side; --ty) {
				if (frameUpdate)
					break;
				unsigned int chx = cx + tx * CHUNK_SIZE_X, chy = cy + ty * CHUNK_SIZE_Z;
				if (origin->ChunkAt(chx, 1, chy) &&
					origin->ChunkAt(chx, 1, chy)->IsDirty()) {
					origin->ChunkAt(chx, 1, chy)->GetPosition(mx, my);
					if (m_meshes.Get(mx - sx, my - sy))
						updateThList[threads++] =
						std::async(std::launch::async,
							[](Mesh* mesh, BlockInfo* blockinfo[BTYPE_LAST], World* world) {
								mesh->Update(blockinfo, world); return mesh; }, m_meshes.Get(mx - sx, my - sy), blockinfo, origin);
					else updateThList[threads++] = std::async(std::launch::async,
						[](Chunk* chunk) { return new Mesh(chunk); }, origin->ChunkAt(chx, 1, chy));
					if (threads == THREADS_UPDATE_CHUNKS) frameUpdate = FRAMES_UPDATE_CHUNKS;
				}
			}
			if (frameUpdate)
				break;
			++side;
		}

	if (threads > 0) {
		for (int i = 0; i < threads; ++i) {
			updateThList[i].wait();
			Mesh* mesh = updateThList[i].get();
			if (mesh->IsNew()) {
				unsigned int x, y;
				mesh->GetPosition(x, y, origin);
				m_meshes.Set(x, y, mesh);
			}
			mesh->FlushMeshToVBO();
		}
	}
}

void Renderer::RenderBooster(TextureAtlas& textureAtlas, Shader& shader, Transformation tran, Player player, Booster* booster) {
		float width = 1.f;
		float height = 1.f;

		Vector3f DiffCam = booster->GetPosition() - player.GetPosition();
		Vector3f UpCam = Vector3f(0.f, 1.f, 0.f);

		Vector3f CrossA = DiffCam.Cross(UpCam);
		Vector3f CrossB = DiffCam.Cross(CrossA);

		CrossA.Normalize();
		CrossB.Normalize();

		Vector3f playerPosition = booster->GetPosition() + Vector3f(0.f, -.75f, 0.f);

		Vector3f v2 = (playerPosition + CrossA * 0.5 * width + CrossB * 0.5 * height);
		Vector3f v1 = (playerPosition - CrossA * 0.5 * width + CrossB * 0.5 * height);
		Vector3f v3 = (playerPosition + CrossA * 0.5 * width - CrossB * 0.5 * height);
		Vector3f v4 = (playerPosition - CrossA * 0.5 * width - CrossB * 0.5 * height);

		int index;
		BOOST_TYPE type = booster->GetType();

		switch (type)
		{
		case BTYPE_HEAL:
			index = 5;
			break;
		case BTYPE_DAMAGE:
			index = 6;
			break;
		case BTYPE_SPEED:
			index = 7;
			break;
		case BTYPE_INVINCIBLE:
			index = 8;
			break;
		default:
			index = 1;
			break;
		}
		float u, v, w, h;

		shader.Use();
		textureAtlas.Bind();
		textureAtlas.TextureIndexToCoord(index, u, v, w, h);

		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glBlendEquation(GL_FUNC_ADD);

		glLoadMatrixf(tran.GetMatrix().GetInternalValues());
		glBegin(GL_QUADS);
		glTexCoord2f(u, v); glVertex3f(v1.x, v1.y, v1.z);
		glTexCoord2f(u + w, v); glVertex3f(v2.x, v2.y, v2.z);
		glTexCoord2f(u + w, v + h); glVertex3f(v3.x, v3.y, v3.z);
		glTexCoord2f(u, v + h); glVertex3f(v4.x, v4.y, v4.z);

		glEnd();

		glBlendFunc(GL_CONSTANT_COLOR, GL_ONE_MINUS_CONSTANT_COLOR);
		glBlendEquation(GL_FUNC_SUBTRACT);
		glDisable(GL_BLEND);

		shader.Disable();
}

void Renderer::RenderPlayer(Player* player, Transformation tran) const {
}

void Renderer::RenderPlayer(RemotePlayer* rplayer, const Vector3f& player_pos, const Vector3f& player_dir) const {
}

