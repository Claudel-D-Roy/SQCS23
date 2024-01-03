#include "chunk.h"
#include "world.h"
#include <random>


Chunk::Chunk(unsigned int x, unsigned int y, int64_t seed) : m_posX(x), m_posY(y) {
	//std::ostringstream pos; // V�rifie l'existence d'un fichier .chunk avec sa position.
	//pos << CHUNK_PATH << x << '_' << y << ".chunk";
	//std::ifstream input(pos.str(), std::fstream::binary);

	//if (input.fail()) {
	OpenSimplexNoise::Noise simplex = OpenSimplexNoise::Noise(seed);
	int ratio = 0;

	ratio = x * y % 7;
	m_blocks.Reset(BTYPE_AIR);

#pragma region  Montagnes et Grass des montagnes
	for (int ix = 0; ix < CHUNK_SIZE_X; ++ix)
		for (int iz = 0; iz < CHUNK_SIZE_Z; ++iz) {
			float xnoiz, ynoiz;
			xnoiz = (double)(ix + x * CHUNK_SIZE_X) / 4796.;
			ynoiz = (double)(iz + y * CHUNK_SIZE_Z) / 4796.;
			double height = 0;
			for (int x = 0; x < 39; ++x) {
				height += simplex.eval(xnoiz, ynoiz);
				height *= .79;
				xnoiz *= 1.1305;
				ynoiz *= 1.1305;
			}
			height = height * 2000. * simplex.eval((double)(ix + x * CHUNK_SIZE_X) / 512., (double)(iz + y * CHUNK_SIZE_Z) / 512.);
			height /= (CHUNK_SIZE_Y / 1.9);
			height += 15.;
			for (int iy = 0; iy <= (int)height % CHUNK_SIZE_Y; ++iy) {
				if (iy < 20)
				{
					//std::cout << "" << ynoiz << std::endl;
					SetBlock(ix, iy, iz, BTYPE_GRASS, nullptr);
				}
				else if (iy == 20 || iy == 21) {
					double fractionalPart = ynoiz - static_cast<int>(ynoiz);

					if (iy == 20) {
						if (fractionalPart < 0.3) {
							SetBlock(ix, iy, iz, BTYPE_GRASS, nullptr);
						}
						else {
							SetBlock(ix, iy, iz, BTYPE_METAL, nullptr);
						}
					}
					else if (iy == 21) {
						if (fractionalPart < 0.6) {
							SetBlock(ix, iy, iz, BTYPE_GRASS, nullptr);
						}
						else {
							SetBlock(ix, iy, iz, BTYPE_METAL, nullptr);
						}
					}
				}
				else
				{
					SetBlock(ix, iy, iz, BTYPE_METAL, nullptr);
				}
			}
		}
#pragma endregion

#pragma region Lacs
	for (int ix = 0; ix < CHUNK_SIZE_X; ++ix)
		for (int iz = 0; iz < CHUNK_SIZE_Z; ++iz) {
			for (int iy = 0; iy < 13; ++iy) {
				if (iy < 5 && GetBlock(ix, iy, iz) == BTYPE_AIR) {
					SetBlock(ix, iy, iz, BTYPE_ICE, nullptr);
				}
				else if (iy >= 5 && GetBlock(ix, iy, iz) == BTYPE_AIR) {
					SetBlock(ix, iy, iz, BTYPE_ICE, nullptr);
				}
			}
		}
#pragma endregion

	//int rnd = rand() % 15;
	if (ratio == 1)
		for (int ix = 0; ix < CHUNK_SIZE_X; ++ix) // structure
			for (int iz = 0; iz < CHUNK_SIZE_Z; ++iz) {
				for (int iy = 0; iy < 14; ++iy) {
					if (ix == 3)
						if (GetBlock(ix, iy, iz) == BTYPE_AIR)
							Structure(ix, iy, iz, 2);
				}
			}
	if (ratio == 3)
		for (int ix = 0; ix < CHUNK_SIZE_Z; ++ix) // structure
			for (int iz = 0; iz < CHUNK_SIZE_X; ++iz) {
				for (int iy = 0; iy < 14; ++iy) {
					if (iz == 4)
						if (GetBlock(ix, iy, iz) == BTYPE_AIR)
							Structure(ix, iy, iz, 1);
				}
			}
	if (ratio == 5)
		for (int ix = 0; ix < CHUNK_SIZE_X; ++ix) // structure
			for (int iz = 0; iz < CHUNK_SIZE_Z; ++iz) {
				for (int iy = 0; iy < 14; ++iy) {
					if (ix == 3)
						if (GetBlock(ix, iy, iz) == BTYPE_AIR)
							Structure(ix, iy, iz, 2);
				}
			}





#pragma region Arbre
	double valeurRnd = 0;
	int treeheight = 10;
	int lastTreeX = -1;
	int lastTreeZ = -1;

	int minDistanceBetweenTrees = 10; // Définir la distance minimale entre les arbres


	for (int ix = 0; ix < CHUNK_SIZE_X; ++ix) {
		for (int iz = 0; iz < CHUNK_SIZE_Z; ++iz) {
			if (GetBlock(ix, 0, iz) != BTYPE_ICE) {
				float xnoiz = (double)(ix + x * CHUNK_SIZE_X) / 4796.;
				float ynoiz = (double)(iz + y * CHUNK_SIZE_Z) / 4796.;
				double height = 0;
				for (int i = 0; i < 39; ++i) {
					height += simplex.eval(xnoiz, ynoiz);
					height *= .79;
					xnoiz *= 1.1305;
					ynoiz *= 1.1305;
				}
				height = height * 2000. * simplex.eval((double)(ix + x * CHUNK_SIZE_X) / 512., (double)(iz + y * CHUNK_SIZE_Z) / 512.);
				height /= (CHUNK_SIZE_Y / 1.9);
				height += 15.;
				if (GetBlock(ix, (int)height, iz) == BTYPE_GRASS || (GetBlock(ix, (int)height, iz) == BTYPE_METAL)) {
					valeurRnd = simplex.eval(xnoiz, ynoiz);
					int distanceThreshold = 20;
					// Vérifie si l'emplacement n'est pas à l'intérieur des lacs
					bool isInsideLake = false;
					for (int iy = 0; iy < 13; ++iy) {
						if (GetBlock(ix, iy, iz) == BTYPE_ICE) {
							isInsideLake = true;
							break;
						}
					}
					if (!isInsideLake && ((valeurRnd > -0.4 && valeurRnd < -0.38) || (valeurRnd > -0.35 && valeurRnd < -0.31)
						|| (valeurRnd > 0.3 && valeurRnd < 0.32) || (valeurRnd > 0.37 && valeurRnd < 0.39))
						) {
						if (lastTreeX == -1 || abs(ix - lastTreeX) > minDistanceBetweenTrees || abs(iz - lastTreeZ) > minDistanceBetweenTrees) {
							if (valeurRnd < 0.1)
								treeheight = 10;
							else {
								treeheight = valeurRnd * 20;
								if (treeheight < 5)
									treeheight = 5;
							}
							PlaceTree(ix, height, iz, treeheight);
							lastTreeX = ix;
							lastTreeZ = iz;
						}
					}
				}
			}
		}
	}
#pragma endregion








	//else {
	//	input.seekg(0, std::ios_base::end);
	//	int size = input.tellg();
	//	input.seekg(0, std::ios_base::beg);

	//	char data[CHUNK_SIZE_X * CHUNK_SIZE_Y * CHUNK_SIZE_Z];
	//	input.read(data, size);
	//	input.close();

	//	for (int ix = 0; ix < CHUNK_SIZE_X; ++ix)
	//		for (int iz = 0; iz < CHUNK_SIZE_Z; ++iz)
	//			for (int iy = 0; iy < CHUNK_SIZE_Y; ++iy)
	//				m_blocks.Set(ix, iy, iz, data[ix + (iz * CHUNK_SIZE_X) + (iy * CHUNK_SIZE_Z * CHUNK_SIZE_X)]);
	//}*/




	//for (int ix = 0; ix < CHUNK_SIZE_X; ++ix) // Collines
//	for (int iz = 0; iz < CHUNK_SIZE_Z; ++iz) {
//		float xnoiz, ynoiz;
//		xnoiz = (double)(ix + x * CHUNK_SIZE_X) / 512.;
//		ynoiz = (double)(iz + y * CHUNK_SIZE_Z) / 512.;
//		float height = simplex.eval(xnoiz, ynoiz) * 50.f;// +1.f;
//		for (int iy = 0; iy <= (int)height % CHUNK_SIZE_Y; ++iy) {
//			if (iy < 10 && GetBlock(ix, iy, iz) == BTYPE_AIR) {
//				SetBlock(ix, iy, iz, BTYPE_METAL, nullptr); // Collines
//			}
//			else if (iy >= 10 && GetBlock(ix, iy, iz) == BTYPE_AIR) {
//				SetBlock(ix, iy, iz, BTYPE_GRASS, nullptr); // Grass des collines
//			}
//		}
//	}

}

void Chunk::PlaceTree(int x, int y, int z, int height) {
	// Vérifie si les coordonnées x, y, z sont dans les limites du chunk
	if (x < 0 || x >= CHUNK_SIZE_X || y < 0 || y >= CHUNK_SIZE_Y || z < 0 || z >= CHUNK_SIZE_Z) {
		// Coordonnées hors limites du chunk, sortie anticipée pour éviter tout accès non valide
		return;
	}

	// Place la tige de l'arbre
	for (int iy = 0; iy < height + 1; ++iy) {
		if (y + iy < CHUNK_SIZE_Y) { // Vérifie si la hauteur est à l'intérieur des limites du chunk
			SetBlock(x, y + iy, z, BTYPE_DIRT, nullptr);
		}
	}

	// Place les feuilles de l'arbre
	int foliageHeight = height / 2;
	for (int dy = 0; dy < foliageHeight; ++dy) {
		for (int dx = -4; dx <= 4; ++dx) {
			for (int dz = -4; dz <= 4; ++dz) {
				// Vérifie que les coordonnées se trouvent à l'intérieur des limites du chunk
				if (x + dx >= 0 && x + dx < CHUNK_SIZE_X && y + height + dy >= 0 && y + height + dy < CHUNK_SIZE_Y &&
					z + dz >= 0 && z + dz < CHUNK_SIZE_Z) {
					// Vérifie si le bloc est à une distance acceptable du centre des feuilles pour les placer
					double distanceSquared = dx * dx + dy * dy + dz * dz;
					if (distanceSquared < 20) {
						SetBlock(x + dx, y + height + dy, z + dz, BTYPE_GREENGRASS, nullptr);
					}
				}
			}
		}
	}
}





Chunk::~Chunk() {
	/*if (m_isModified) {
		char data[CHUNK_SIZE_X * CHUNK_SIZE_Y * CHUNK_SIZE_Z];

		for (int x = 0; x < CHUNK_SIZE_X; ++x)
			for (int z = 0; z < CHUNK_SIZE_Z; ++z)
				for (int y = 0; y < CHUNK_SIZE_Y; ++y)
					data[x + (z * CHUNK_SIZE_X) + (y * CHUNK_SIZE_Z * CHUNK_SIZE_X)] = (char)GetBlock(x, y, z);

		std::ostringstream pos;
		pos << CHUNK_PATH << m_posX << '_' << m_posY << ".chunk";

		std::ofstream output(pos.str(), std::fstream::binary);
		output.write(data, sizeof(data));
		output.close();
	}*/
}

void Chunk::RemoveBlock(int x, int y, int z, World* world) {
	m_blocks.Set(x, y, z, BTYPE_AIR);
	CheckNeighbors(x, y, world);
	m_isDirty = true;
}

void Chunk::SetBlock(int x, int y, int z, BlockType type, World* world) {
	m_blocks.Set(x, y, z, type);
	if (world) CheckNeighbors(x, z, world); // Si nullptr, ne pas v�rifier les chunks voisines.
	m_isDirty = true;
}

BlockType Chunk::GetBlock(int x, int y, int z) { return m_blocks.Get(x, y, z); }

void Chunk::CheckNeighbors(unsigned int x, unsigned int z, World* world) {
	unsigned int cx, cy;

	world->GetScope(cx, cy);

	if (x == 0 && m_posX - cx >= 0 &&
		world->ChunkAt((m_posX - cx - 1) * CHUNK_SIZE_X, 1, (m_posY - cy) * CHUNK_SIZE_Z))
		world->ChunkAt((m_posX - cx - 1) * CHUNK_SIZE_X, 1, (m_posY - cy) * CHUNK_SIZE_Z)->MakeDirty();
	else if (x == CHUNK_SIZE_X - 1 && m_posX - cx < WORLD_SIZE_X &&
		world->ChunkAt((m_posX - cx + 1) * CHUNK_SIZE_X, 1, (m_posY - cy) * CHUNK_SIZE_Z))
		world->ChunkAt((m_posX - cx + 1) * CHUNK_SIZE_X, 1, (m_posY - cy) * CHUNK_SIZE_Z)->MakeDirty();

	if (z == 0 && m_posY - cy >= 0 &&
		world->ChunkAt((m_posX - cx) * CHUNK_SIZE_X, 1, (m_posY - cy - 1) * CHUNK_SIZE_Z))
		world->ChunkAt((m_posX - cx) * CHUNK_SIZE_X, 1, (m_posY - cy - 1) * CHUNK_SIZE_Z)->MakeDirty();
	else if (z == CHUNK_SIZE_X - 1 && m_posY - cy < WORLD_SIZE_Y &&
		world->ChunkAt((m_posX - cx) * CHUNK_SIZE_X, 1, (m_posY - cy + 1) * CHUNK_SIZE_Z))
		world->ChunkAt((m_posX - cx) * CHUNK_SIZE_X, 1, (m_posY - cy + 1) * CHUNK_SIZE_Z)->MakeDirty();
}

void Chunk::GetPosition(unsigned int& x, unsigned int& y) const { x = m_posX; y = m_posY; }





bool Chunk::IsDirty() const { return m_isDirty; }

void Chunk::MakeDirty() { m_isDirty = true; }

void Chunk::MakeClean() { m_isDirty = false; }

void Chunk::MakeModified() { m_isModified = true; }


void Chunk::Structure(int x, int y, int z,int height)
{
	for (int i = 0; i < height; i++)
	{
		SetBlock(x, i + y, z, BTYPE_GRASS, nullptr);
	}
}
