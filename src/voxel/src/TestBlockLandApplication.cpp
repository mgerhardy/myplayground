#include "TestBlockLandApplication.h"
#include <Ogre.h>
#include <OIS/OIS.h>
#include <iostream>

blockinfo_t g_BlockInfo[] = { { 0, "Air", Ogre::ColourValue(1.0, 1.0, 1.0) }, { 1, "Grass", Ogre::ColourValue(0.0, 0.5, 0.0) }, { 2, "Soil", Ogre::ColourValue(0.5, 0.5, 0.0) }, {
		3, "Rock", Ogre::ColourValue(0.5, 0.5, 0.5) }, { 4, "Lava", Ogre::ColourValue(0.8, 0.0, 0.0) }, { 255, "", Ogre::ColourValue(0.0, 0.5, 0.0) } };

struct layer_t {
	block_t BlockID;
	int MinLevel;
	int MaxLevel;
	int SeedOffset;
};

layer_t g_Layers[] = { { 1, 0, 2, 1 },		//Grass
		{ 2, 0, 10, 2 },	//Soil
		{ 3, 20, 200, 3 },	//Rock
		{ 4, 100, 300, 4 },     //Lava
		{ 255, 0, 0, 0 }        //Must be last
};

TestBlockLandApplication::TestBlockLandApplication() {
	m_ChunkID = 1;
	m_LightAngle = 90;
	m_LightColor = Ogre::ColourValue(1, 1, 1);
	m_FogColor = m_LightColor * 0.8f;
	m_AmbientColor = m_LightColor / 3.0f;
}

TestBlockLandApplication::~TestBlockLandApplication() {
	delete[] m_Blocks;
	delete[] m_BlockLight;
}

#if 0
void TestBlockLandApplication::initWorldBlocksCaves() {
	noise::module::RidgedMulti NoiseSource;
	float nx, ny, nz;
	int x, y, z;

	// Change these to increase/decrease the cave size scale
	float delta = 0.01f;
	float ValueLimit = 0.80f;

	// Initialize the noise module
	NoiseSource.SetSeed(0);
	NoiseSource.SetOctaveCount(4);

	for (y = 0, ny = 0; y < WORLD_SIZE; ++y, ny += delta) {
		for (z = 0, nz = 0; z < WORLD_SIZE; ++z, nz += delta) {
			for (x = 0, nx = 0; x < WORLD_SIZE; ++x, nx += delta) {
				float Value = NoiseSource.GetValue(nx, ny, nz);
				if (Value > ValueLimit)
				GetBlock(x, y, z) = 0;
			}
		}
	}
}

void TestBlockLandApplication::initWorldBlocksLayers() {
	int HeightMap[WORLD_SIZE][WORLD_SIZE];
	infland::CLandscape LayerMaps[10];  //Hard coded because I'm lazy!
	int NumLayerMaps = 0;
	int BaseSeed;

	m_Landscape.SetSize(WORLD_SIZE, WORLD_SIZE);
	m_Landscape.CreateAltitude();
	BaseSeed = m_Landscape.GetSeed();

	infland::CMap& Map = m_Landscape.GetAltitudeMap();

	//Initialize our temporary height map array
	for (int z = 0; z < WORLD_SIZE; ++z) {
		for (int x = 0; x < WORLD_SIZE; ++x) {
			float Height = Map.GetValue(x, z) * WORLD_SIZE / 4 + WORLD_SIZE / 2;
			HeightMap[x][z] = Height;
		}
	}

	//Create height maps for each layer
	for (int i = 0; g_Layers[i].BlockID != BLOCK_NULL && i < 10; ++i) {
		LayerMaps[i].SetSeed(BaseSeed + g_Layers[i].SeedOffset);
		LayerMaps[i].SetSize(WORLD_SIZE, WORLD_SIZE);
		LayerMaps[i].CreateAltitude();
		++NumLayerMaps;
	}

	int Layer = 0;

	// Fill in the blocks from all layers
	for (int layer = 0; layer < NumLayerMaps; ++layer) {
		infland::CMap & Map = LayerMaps[layer].GetAltitudeMap();

		for (int z = 0; z < WORLD_SIZE; ++z) {
			for (int x = 0; x < WORLD_SIZE; ++x) {
				if (HeightMap[x][z] <= 0)
				continue;
				int Height = (Map.GetValue(x, z) + 1) / 2.0f * (g_Layers[layer].MaxLevel - g_Layers[layer].MinLevel) + g_Layers[layer].MinLevel;

				//Don't fill the map below 0 height
				if (HeightMap[x][z] - Height < 0)
				Height = HeightMap[x][z];
				HeightMap[x][z] -= Height;

				int MaxHeight = HeightMap[x][z] + Height;

				for (int y = HeightMap[x][z]; y <= MaxHeight; ++y) {
					GetBlock(x, y, z) = g_Layers[layer].BlockID;
				}
			}
		}
	}
}
#endif

void TestBlockLandApplication::initWorldBlocksLight() {
	int x, y, z;
	blocklight_t Light;
	blocklight_t DeltaLight = 16;

	for (z = 0; z < WORLD_SIZE; ++z) {
		for (x = 0; x < WORLD_SIZE; ++x) {
			Light = 255;

			for (y = WORLD_SIZE - 1; y >= 0; --y) {
				GetBlockLight(x, y, z) = Light;

				if (GetBlock(x, y, z) != 0) {
					if (Light >= DeltaLight)
						Light -= DeltaLight;
					else
						Light = 0;
				}
			}
		}
	}
}

void TestBlockLandApplication::createChunk(const int StartX, const int StartY, const int StartZ) {
	block_t LastBlock = 0;

	Ogre::ManualObject* MeshChunk = new Ogre::ManualObject("MeshMatChunk" + Ogre::StringConverter::toString(m_ChunkID));

	int iVertex = 0;
	block_t Block;
	block_t Block1;

	/* Only create visible faces of chunk */
	block_t DefaultBlock = 1;
	const int SX = 0;
	const int SY = 0;
	const int SZ = 0;

	for (int iBlock = 1; iBlock <= 4; ++iBlock) {
		MeshChunk->begin(g_BlockInfo[iBlock].Name);
		iVertex = 0;

		for (int z = StartZ; z < CHUNK_SIZE + StartZ; ++z) {
			for (int y = StartY; y < CHUNK_SIZE + StartY; ++y) {
				for (int x = StartX; x < CHUNK_SIZE + StartX; ++x) {
					Block = GetBlock(x, y, z);
					if (Block != iBlock)
						continue;

					//x-1
					Block1 = DefaultBlock;
					if (x > SX)
						Block1 = GetBlock(x - 1, y, z);

					if (Block1 == 0) {
						MeshChunk->position(x, y, z + 1);
						MeshChunk->normal(-1, 0, 0);
						MeshChunk->textureCoord(0, 1);
						MeshChunk->position(x, y + 1, z + 1);
						MeshChunk->normal(-1, 0, 0);
						MeshChunk->textureCoord(1, 1);
						MeshChunk->position(x, y + 1, z);
						MeshChunk->normal(-1, 0, 0);
						MeshChunk->textureCoord(1, 0);
						MeshChunk->position(x, y, z);
						MeshChunk->normal(-1, 0, 0);
						MeshChunk->textureCoord(0, 0);

						MeshChunk->triangle(iVertex, iVertex + 1, iVertex + 2);
						MeshChunk->triangle(iVertex + 2, iVertex + 3, iVertex);

						iVertex += 4;
					}

					//x+1
					Block1 = DefaultBlock;
					if (x < SX + WORLD_WIDTH - 1)
						Block1 = GetBlock(x + 1, y, z);

					if (Block1 == 0) {
						MeshChunk->position(x + 1, y, z);
						MeshChunk->normal(1, 0, 0);
						MeshChunk->textureCoord(0, 1);
						MeshChunk->position(x + 1, y + 1, z);
						MeshChunk->normal(1, 0, 0);
						MeshChunk->textureCoord(1, 1);
						MeshChunk->position(x + 1, y + 1, z + 1);
						MeshChunk->normal(1, 0, 0);
						MeshChunk->textureCoord(1, 0);
						MeshChunk->position(x + 1, y, z + 1);
						MeshChunk->normal(1, 0, 0);
						MeshChunk->textureCoord(0, 0);

						MeshChunk->triangle(iVertex, iVertex + 1, iVertex + 2);
						MeshChunk->triangle(iVertex + 2, iVertex + 3, iVertex);

						iVertex += 4;
					}

					//y-1
					Block1 = DefaultBlock;
					if (y > SY)
						Block1 = GetBlock(x, y - 1, z);

					if (Block1 == 0) {
						MeshChunk->position(x, y, z);
						MeshChunk->normal(0, -1, 0);
						MeshChunk->textureCoord(0, 1);
						MeshChunk->position(x + 1, y, z);
						MeshChunk->normal(0, -1, 0);
						MeshChunk->textureCoord(1, 1);
						MeshChunk->position(x + 1, y, z + 1);
						MeshChunk->normal(0, -1, 0);
						MeshChunk->textureCoord(1, 0);
						MeshChunk->position(x, y, z + 1);
						MeshChunk->normal(0, -1, 0);
						MeshChunk->textureCoord(0, 0);

						MeshChunk->triangle(iVertex, iVertex + 1, iVertex + 2);
						MeshChunk->triangle(iVertex + 2, iVertex + 3, iVertex);

						iVertex += 4;
					}

					//y+1
					Block1 = DefaultBlock;
					if (y < SY + WORLD_HEIGHT - 1)
						Block1 = GetBlock(x, y + 1, z);

					if (Block1 == 0) {
						MeshChunk->position(x, y + 1, z + 1);
						MeshChunk->normal(0, 1, 0);
						MeshChunk->textureCoord(0, 1);
						MeshChunk->position(x + 1, y + 1, z + 1);
						MeshChunk->normal(0, 1, 0);
						MeshChunk->textureCoord(1, 1);
						MeshChunk->position(x + 1, y + 1, z);
						MeshChunk->normal(0, 1, 0);
						MeshChunk->textureCoord(1, 0);
						MeshChunk->position(x, y + 1, z);
						MeshChunk->normal(0, 1, 0);
						MeshChunk->textureCoord(0, 0);

						MeshChunk->triangle(iVertex, iVertex + 1, iVertex + 2);
						MeshChunk->triangle(iVertex + 2, iVertex + 3, iVertex);

						iVertex += 4;
					}

					//z-1
					Block1 = DefaultBlock;
					if (z > SZ)
						Block1 = GetBlock(x, y, z - 1);

					if (Block1 == 0) {
						MeshChunk->position(x, y + 1, z);
						MeshChunk->normal(0, 0, -1);
						MeshChunk->textureCoord(0, 1);
						MeshChunk->position(x + 1, y + 1, z);
						MeshChunk->normal(0, 0, -1);
						MeshChunk->textureCoord(1, 1);
						MeshChunk->position(x + 1, y, z);
						MeshChunk->normal(0, 0, -1);
						MeshChunk->textureCoord(1, 0);
						MeshChunk->position(x, y, z);
						MeshChunk->normal(0, 0, -1);
						MeshChunk->textureCoord(0, 0);

						MeshChunk->triangle(iVertex, iVertex + 1, iVertex + 2);
						MeshChunk->triangle(iVertex + 2, iVertex + 3, iVertex);

						iVertex += 4;
					}

					//z+1
					Block1 = DefaultBlock;
					if (z < SZ + WORLD_SIZE - 1)
						Block1 = GetBlock(x, y, z + 1);

					if (Block1 == 0) {
						MeshChunk->position(x, y, z + 1);
						MeshChunk->normal(0, 0, 1);
						MeshChunk->textureCoord(0, 1);
						MeshChunk->position(x + 1, y, z + 1);
						MeshChunk->normal(0, 0, 1);
						MeshChunk->textureCoord(1, 1);
						MeshChunk->position(x + 1, y + 1, z + 1);
						MeshChunk->normal(0, 0, 1);
						MeshChunk->textureCoord(1, 0);
						MeshChunk->position(x, y + 1, z + 1);
						MeshChunk->normal(0, 0, 1);
						MeshChunk->textureCoord(0, 0);

						MeshChunk->triangle(iVertex, iVertex + 1, iVertex + 2);
						MeshChunk->triangle(iVertex + 2, iVertex + 3, iVertex);

						iVertex += 4;
					}
				}
			}
		}
		MeshChunk->end();
	}

	mSceneMgr->getRootSceneNode()->createChildSceneNode()->attachObject(MeshChunk);

	m_pBlockChunks[StartX / CHUNK_SIZE][StartY / CHUNK_SIZE][StartZ / CHUNK_SIZE] = MeshChunk;
	m_BlockVertexCount[StartX / CHUNK_SIZE][StartY / CHUNK_SIZE][StartZ / CHUNK_SIZE] = iVertex;

	++m_ChunkID;
}

bool TestBlockLandApplication::keyPressed(const OIS::KeyEvent &arg) {
	if (arg.key == OIS::KC_K) {
		m_WorldTime += 0.1f;   //Increase world time a bit
		if (m_WorldTime > 24.0f)
			m_WorldTime -= 24.0f;

		ComputeWorldLightValues(m_WorldTime);
		UpdateSceneLighting();

		++m_UpdateChunksCount;
	} else if (arg.key == OIS::KC_J) {
		m_WorldTime -= 0.1f; //Decrease world time a bit
		if (m_WorldTime < 0)
			m_WorldTime += 24.0f;

		ComputeWorldLightValues(m_WorldTime);
		UpdateSceneLighting();

		++m_UpdateChunksCount;
	}

	return BaseApplication::keyPressed(arg);
}

bool TestBlockLandApplication::frameEnded(const Ogre::FrameEvent &evt) {
	//This makes the world day last about 5 minutes of real time
	m_WorldTime += evt.timeSinceLastFrame * 0.04;
	m_WorldTime = fmod(m_WorldTime, 24.0f);

	ComputeWorldLightValues(m_WorldTime);
	UpdateSceneLighting();

	if (m_UpdateChunksCount == 0)
		++m_UpdateChunksCount;

	//Update a few chunks of the world each frame
	updateChunksFrame();
	return BaseApplication::frameEnded(evt);
}

void TestBlockLandApplication::updateChunksFrame() {
	int NUM_CHUNKS = WORLD_SIZE / CHUNK_SIZE;
	if (m_UpdateChunksCount <= 0)
		return;

	for (int i = 0; i < NUM_UPDATE_CHUNKS; ++i) {
		// Ignore chunks with nothing in it
		if (m_BlockVertexCount[m_UpdateChunkX][m_UpdateChunkY][m_UpdateChunkZ] > 0) {
			// Remove and delete the existing mesh from the scene
			if (m_pBlockChunks[m_UpdateChunkX][m_UpdateChunkY][m_UpdateChunkZ] != nullptr) {
				Ogre::ManualObject* pMeshChunk = m_pBlockChunks[m_UpdateChunkX][m_UpdateChunkY][m_UpdateChunkZ];
				pMeshChunk->detachFromParent();
				delete pMeshChunk;
				m_pBlockChunks[m_UpdateChunkX][m_UpdateChunkY][m_UpdateChunkZ] = nullptr;
			}

			// Create a new mesh for the chunk
			Ogre::ManualObject* MeshChunk = new Ogre::ManualObject("MeshMatChunk" + Ogre::StringConverter::toString(m_ChunkID));
			MeshChunk->setDynamic(true);
			MeshChunk->begin("Combine4");
			createChunkCombineMat(MeshChunk, m_UpdateChunkX * CHUNK_SIZE, m_UpdateChunkY * CHUNK_SIZE, m_UpdateChunkZ * CHUNK_SIZE);
			m_pBlockChunks[m_UpdateChunkX][m_UpdateChunkY][m_UpdateChunkZ] = MeshChunk;
			mSceneMgr->getRootSceneNode()->createChildSceneNode()->attachObject(MeshChunk);
			++m_ChunkID;
		}

		// Find the next chunk to update
		++m_UpdateChunkX;

		if (m_UpdateChunkX >= NUM_CHUNKS) {
			m_UpdateChunkX = 0;
			++m_UpdateChunkZ;

			if (m_UpdateChunkZ >= NUM_CHUNKS) {
				m_UpdateChunkZ = 0;
				++m_UpdateChunkY;

				if (m_UpdateChunkY >= NUM_CHUNKS) {
					m_UpdateChunkY = 0;
					--m_UpdateChunksCount;
				}
			}
		}
	}
}

Ogre::MaterialPtr TestBlockLandApplication::createSolidTexture(const Ogre::String& pName, const Ogre::ColourValue& color) {
	Ogre::MaterialPtr mat = Ogre::MaterialManager::getSingleton().create(pName, "General", true);
	Ogre::Technique* tech = mat->getTechnique(0);
	tech->setLightingEnabled(false);
	Ogre::Pass* pass = tech->getPass(0);
	Ogre::TextureUnitState* tex = pass->createTextureUnitState();
	tex->setColourOperationEx(Ogre::LBX_MODULATE, Ogre::LBS_MANUAL, Ogre::LBS_CURRENT, color);
	return mat;
}

Ogre::MaterialPtr TestBlockLandApplication::createTexture(const Ogre::String& pName, const Ogre::String& pImageFilename) {
	Ogre::MaterialPtr mat = Ogre::MaterialManager::getSingleton().create(pName, "General", true);
	Ogre::Technique* tech = mat->getTechnique(0);
	tech->setLightingEnabled(false);
	Ogre::Pass* pass = tech->getPass(0);
	Ogre::TextureUnitState* tex = pass->createTextureUnitState();

	tex->setTextureName(pImageFilename);
	tex->setNumMipmaps(4);
	tex->setTextureAnisotropy(1);
	tex->setTextureFiltering(Ogre::FO_POINT, Ogre::FO_POINT, Ogre::FO_POINT);
	return mat;
}

void TestBlockLandApplication::createWorldChunks() {
	for (int z = 0; z < WORLD_SIZE; z += CHUNK_SIZE) {
		for (int y = 0; y < WORLD_HEIGHT; y += CHUNK_SIZE) {
			for (int x = 0; x < WORLD_WIDTH; x += CHUNK_SIZE) {
				createChunk(x, y, z);
			}
		}
	}
}

void TestBlockLandApplication::createScene() {
	mSceneMgr->setSkyDome(true, "Examples/CloudySky", 2, 8, 100);
	mSceneMgr->setFog(Ogre::FOG_LINEAR, Ogre::ColourValue(0.8, 0.8, 1), 0.05, 0.0, 200);

	mCamera->setFarClipDistance(256);
	mCamera->setNearClipDistance(0.01);

	mSceneMgr->setAmbientLight(Ogre::ColourValue(0.5, 0.5, 0.5));
	Ogre::Light* l = mSceneMgr->createLight("MainLight");
	l->setPosition(20, 80, 50);

	initWorldBlocksSphere();

	createWorldChunks();
}

void TestBlockLandApplication::ComputeWorldLightValues(const float WorldTime) {
	//6am = SunRise
	//Light is symmetric about noon
	//4am-8am = dawn
	//4am color = (0.1, 0.1, 0.1)
	//6am color = (1, 0.6, 0.04)
	//8am color = (1, 1, 1)

	float BaseWorldTime = std::fmod(WorldTime, 24);

	m_LightAngle = BaseWorldTime / 24.0f * 360.0f - 90.0f;
	if (m_LightAngle < 0)
		m_LightAngle += 360.0f;

	if (BaseWorldTime <= 4 || BaseWorldTime >= 20) {
		m_LightColor = Ogre::ColourValue(0.1f, 0.1f, 0.1f);
	} else if (BaseWorldTime >= 8 && BaseWorldTime <= 16) {
		m_LightColor = Ogre::ColourValue(1, 1, 1);
	} else if (BaseWorldTime >= 4 && BaseWorldTime <= 6) {
		m_LightColor.r = (BaseWorldTime - 4.0f) / 2.0f * 0.9f + 0.1f;
		m_LightColor.g = (BaseWorldTime - 4.0f) / 2.0f * 0.5f + 0.1f;
		m_LightColor.b = (BaseWorldTime - 4.0f) / 2.0f * -0.06f + 0.1f;
	} else if (BaseWorldTime >= 6 && BaseWorldTime <= 8) {
		m_LightColor.r = 1.0f;
		m_LightColor.g = (BaseWorldTime - 6.0f) / 2.0f * 0.4f + 0.6f;
		m_LightColor.b = (BaseWorldTime - 6.0f) / 2.0f * 0.96f + 0.04f;
	} else if (BaseWorldTime >= 16 && BaseWorldTime <= 18) {
		m_LightColor.r = 1.0f;
		m_LightColor.g = (18.0f - BaseWorldTime) / 2.0f * 0.4f + 0.6f;
		m_LightColor.b = (18.0f - BaseWorldTime) / 2.0f * 0.96f + 0.04f;
	} else if (BaseWorldTime >= 18 && BaseWorldTime <= 20) {
		m_LightColor.r = (20.0f - BaseWorldTime) / 2.0f * 0.9f + 0.1f;
		m_LightColor.g = (20.0f - BaseWorldTime) / 2.0f * 0.5f + 0.1f;
		m_LightColor.b = (20.0f - BaseWorldTime) / 2.0f * -0.06f + 0.1f;
	} else	//Shouldn't get here
	{
		m_LightColor = Ogre::ColourValue(1, 1, 1);
	}

	m_AmbientColor = m_LightColor / 3.0f;
	m_FogColor = m_LightColor * 0.80f;
}

void TestBlockLandApplication::initWorldBlocksSphere() {
	for (int i = 1; g_BlockInfo[i].ID != 255; ++i) {
		createSolidTexture(g_BlockInfo[i].Name, g_BlockInfo[i].Color);
	}

	Ogre::Image heightMap;
	heightMap.load("heightmap.png", Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);
	const Ogre::PixelBox& pb = heightMap.getPixelBox();
	WORLD_WIDTH = pb.getWidth();
	WORLD_HEIGHT = pb.getHeight();
	m_Blocks = new block_t[WORLD_HEIGHT * WORLD_WIDTH * WORLD_SIZE];
	memset(m_Blocks, 0, sizeof(block_t) * WORLD_HEIGHT * WORLD_WIDTH * WORLD_SIZE);

	for (int y = 0; y < WORLD_HEIGHT; ++y) {
		for (int x = 0; x < WORLD_WIDTH; ++x) {
			const Ogre::ColourValue& color = heightMap.getColourAt(x, y, 0);
			const int height = static_cast<int>((((color.r + color.g + color.b) / 1.5f) - 1.0f) * WORLD_SIZE / 4.0f + WORLD_SIZE / 2.0f);
			for (int z = 0; z < height; ++z) {
				GetBlock(x, y, z) = (rand() % 4) + 1;
			}
		}
	}
}

#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
#define WIN32_LEAN_AND_MEAN
#include "windows.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
INT WINAPI WinMain(HINSTANCE hInst, HINSTANCE, LPSTR strCmdLine, INT)
#else
int main(int argc, char *argv[])
#endif
		{
	// Create application object
	TestBlockLandApplication app;

	try {
		app.go();
	} catch (Ogre::Exception& e) {
#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
		MessageBox(NULL, e.getFullDescription().c_str(), "An exception has occurred!", MB_OK | MB_ICONERROR | MB_TASKMODAL);
#else
		std::cerr << "An exception has occurred: " << e.getFullDescription().c_str() << std::endl;
#endif
	}

	return 0;
}

#ifdef __cplusplus
}
#endif
