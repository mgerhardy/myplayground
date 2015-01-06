#include "TestBlockLandApplication.h"
#include <Ogre.h>
#include <OIS/OIS.h>
#include <iostream>

const BlockInfo BLOCKINFO[] = {
	{ BlockType::Air, "Air", Ogre::ColourValue(1.0, 1.0, 1.0) },
	{ BlockType::Grass, "Grass", Ogre::ColourValue(0.0, 0.5, 0.0) },
	{ BlockType::Soil, "Soil", Ogre::ColourValue(0.5, 0.5, 0.0) },
	{ BlockType::Rock, "Rock", Ogre::ColourValue(0.5, 0.5, 0.5) }
};

struct Layer {
	BlockType BlockID;
	int MinLevel;
	int MaxLevel;
	int SeedOffset;
};

const Layer LAYERS[] = {
	{ BlockType::Grass, 0, 2, 1 },
	{ BlockType::Soil, 0, 10, 2 },
	{ BlockType::Rock, 20, 200, 3 }
};

TestBlockLandApplication::TestBlockLandApplication() :
		_worldXSize(-1), _worldYSize(-1), _chunkID(1), _blocks(nullptr), _lightAngle(90), _worldTime(0.0f), _updateChunkX(0), _updateChunkY(0), _updateChunkZ(0), _updateChunksCount(
				0), _blockChunkObjects(nullptr), _lightColor(Ogre::ColourValue(1, 1, 1)), _fogColor(_lightColor * 0.8f), _ambientColor(_lightColor / 3.0f), _blockVertexCount(
				nullptr) {
}

TestBlockLandApplication::~TestBlockLandApplication() {
	delete[] _blocks;
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

	for (y = 0, ny = 0; y < _worldHeight; ++y, ny += delta) {
		for (z = 0, nz = 0; z < _worldHeight; ++z, nz += delta) {
			for (x = 0, nx = 0; x < _worldHeight; ++x, nx += delta) {
				float Value = NoiseSource.GetValue(nx, ny, nz);
				if (Value > ValueLimit)
				GetBlock(x, y, z) = 0;
			}
		}
	}
}

void TestBlockLandApplication::initWorldBlocksLayers() {
	int HeightMap[_worldHeight][_worldHeight];
	infland::CLandscape LayerMaps[10];  //Hard coded because I'm lazy!
	int NumLayerMaps = 0;
	int BaseSeed;

	m_Landscape.SetSize(_worldHeight, _worldHeight);
	m_Landscape.CreateAltitude();
	BaseSeed = m_Landscape.GetSeed();

	infland::CMap& Map = m_Landscape.GetAltitudeMap();

	//Initialize our temporary height map array
	for (int z = 0; z < _worldHeight; ++z) {
		for (int x = 0; x < _worldHeight; ++x) {
			float Height = Map.GetValue(x, z) * _worldHeight / 4 + _worldHeight / 2;
			HeightMap[x][z] = Height;
		}
	}

	//Create height maps for each layer
	for (int i = 0; LAYERS[i].BlockID != BLOCK_NULL && i < 10; ++i) {
		LayerMaps[i].SetSeed(BaseSeed + LAYERS[i].SeedOffset);
		LayerMaps[i].SetSize(_worldHeight, _worldHeight);
		LayerMaps[i].CreateAltitude();
		++NumLayerMaps;
	}

	int Layer = 0;

	// Fill in the blocks from all layers
	for (int layer = 0; layer < NumLayerMaps; ++layer) {
		infland::CMap & Map = LayerMaps[layer].GetAltitudeMap();

		for (int z = 0; z < _worldHeight; ++z) {
			for (int x = 0; x < _worldHeight; ++x) {
				if (HeightMap[x][z] <= 0)
				continue;
				int Height = (Map.GetValue(x, z) + 1) / 2.0f * (LAYERS[layer].MaxLevel - LAYERS[layer].MinLevel) + LAYERS[layer].MinLevel;

				//Don't fill the map below 0 height
				if (HeightMap[x][z] - Height < 0)
				Height = HeightMap[x][z];
				HeightMap[x][z] -= Height;

				int MaxHeight = HeightMap[x][z] + Height;

				for (int y = HeightMap[x][z]; y <= MaxHeight; ++y) {
					GetBlock(x, y, z) = LAYERS[layer].BlockID;
				}
			}
		}
	}
}
#endif

void TestBlockLandApplication::initWorldBlocksLight() {
	int x, y, z;
	LightValue Light;
	LightValue DeltaLight = 16;

	for (z = 0; z < _worldHeight; ++z) {
		for (x = 0; x < _worldHeight; ++x) {
			Light = 255;

			for (y = _worldHeight - 1; y >= 0; --y) {
				getBlockLight(x, y, z) = Light;

				if (getBlock(x, y, z) != 0) {
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
	Block LastBlock = 0;

	Ogre::ManualObject* MeshChunk = new Ogre::ManualObject("MeshMatChunk" + Ogre::StringConverter::toString(_chunkID));

	int iVertex = 0;
	Block Block;
	Block Block1;

	/* Only create visible faces of chunk */
	Block DefaultBlock = 1;
	const int SX = 0;
	const int SY = 0;
	const int SZ = 0;

	for (int iBlock = 1; iBlock <= 4; ++iBlock) {
		MeshChunk->begin(BLOCKINFO[iBlock].name);
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
					if (x < SX + _worldXSize - 1)
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
					if (y < SY + _worldYSize - 1)
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
					if (z < SZ + _worldHeight - 1)
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

	_blockChunkObjects[StartX / CHUNK_SIZE][StartY / CHUNK_SIZE][StartZ / CHUNK_SIZE] = MeshChunk;
	_blockVertexCount[StartX / CHUNK_SIZE][StartY / CHUNK_SIZE][StartZ / CHUNK_SIZE] = iVertex;

	++_chunkID;
}

bool TestBlockLandApplication::keyPressed(const OIS::KeyEvent &arg) {
	if (arg.key == OIS::KC_K) {
		_worldTime += 0.1f;   //Increase world time a bit
		if (_worldTime > 24.0f)
			_worldTime -= 24.0f;

		computeWorldLightValues(_worldTime);
		updateSceneLighting();

		++_updateChunksCount;
	} else if (arg.key == OIS::KC_J) {
		_worldTime -= 0.1f; //Decrease world time a bit
		if (_worldTime < 0)
			_worldTime += 24.0f;

		computeWorldLightValues(_worldTime);
		updateSceneLighting();

		++_updateChunksCount;
	}

	return BaseApplication::keyPressed(arg);
}

void TestBlockLandApplication::updateSceneLighting() {

}

bool TestBlockLandApplication::frameEnded(const Ogre::FrameEvent &evt) {
	//This makes the world day last about 5 minutes of real time
	_worldTime += evt.timeSinceLastFrame * 0.04;
	_worldTime = fmod(_worldTime, 24.0f);

	computeWorldLightValues(_worldTime);
	updateSceneLighting();

	if (_updateChunksCount == 0)
		++_updateChunksCount;

	//Update a few chunks of the world each frame
	updateChunksFrame();
	return BaseApplication::frameEnded(evt);
}

void TestBlockLandApplication::updateChunksFrame() {
	int NUM_CHUNKS = _worldHeight / CHUNK_SIZE;
	if (_updateChunksCount <= 0)
		return;

	for (int i = 0; i < NUM_UPDATE_CHUNKS; ++i) {
		// Ignore chunks with nothing in it
		if (_blockVertexCount[_updateChunkX][_updateChunkY][_updateChunkZ] > 0) {
			// Remove and delete the existing mesh from the scene
			if (_blockChunkObjects[_updateChunkX][_updateChunkY][_updateChunkZ] != nullptr) {
				Ogre::ManualObject* pMeshChunk = _blockChunkObjects[_updateChunkX][_updateChunkY][_updateChunkZ];
				pMeshChunk->detachFromParent();
				delete pMeshChunk;
				_blockChunkObjects[_updateChunkX][_updateChunkY][_updateChunkZ] = nullptr;
			}

			// Create a new mesh for the chunk
			Ogre::ManualObject* MeshChunk = new Ogre::ManualObject("MeshMatChunk" + Ogre::StringConverter::toString(_chunkID));
			MeshChunk->setDynamic(true);
			MeshChunk->begin("Combine4");
			createChunkCombineMat(MeshChunk, _updateChunkX * CHUNK_SIZE, _updateChunkY * CHUNK_SIZE, _updateChunkZ * CHUNK_SIZE);
			_blockChunkObjects[_updateChunkX][_updateChunkY][_updateChunkZ] = MeshChunk;
			mSceneMgr->getRootSceneNode()->createChildSceneNode()->attachObject(MeshChunk);
			++_chunkID;
		}

		// Find the next chunk to update
		++_updateChunkX;

		if (_updateChunkX >= NUM_CHUNKS) {
			_updateChunkX = 0;
			++_updateChunkZ;

			if (_updateChunkZ >= NUM_CHUNKS) {
				_updateChunkZ = 0;
				++_updateChunkY;

				if (_updateChunkY >= NUM_CHUNKS) {
					_updateChunkY = 0;
					--_updateChunksCount;
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
	for (int z = 0; z < _worldHeight; z += CHUNK_SIZE) {
		for (int y = 0; y < _worldYSize; y += CHUNK_SIZE) {
			for (int x = 0; x < _worldXSize; x += CHUNK_SIZE) {
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

void TestBlockLandApplication::computeWorldLightValues(const float WorldTime) {
	//6am = SunRise
	//Light is symmetric about noon
	//4am-8am = dawn
	//4am color = (0.1, 0.1, 0.1)
	//6am color = (1, 0.6, 0.04)
	//8am color = (1, 1, 1)

	float BaseWorldTime = std::fmod(WorldTime, 24);

	_lightAngle = BaseWorldTime / 24.0f * 360.0f - 90.0f;
	if (_lightAngle < 0)
		_lightAngle += 360.0f;

	if (BaseWorldTime <= 4 || BaseWorldTime >= 20) {
		_lightColor = Ogre::ColourValue(0.1f, 0.1f, 0.1f);
	} else if (BaseWorldTime >= 8 && BaseWorldTime <= 16) {
		_lightColor = Ogre::ColourValue(1, 1, 1);
	} else if (BaseWorldTime >= 4 && BaseWorldTime <= 6) {
		_lightColor.r = (BaseWorldTime - 4.0f) / 2.0f * 0.9f + 0.1f;
		_lightColor.g = (BaseWorldTime - 4.0f) / 2.0f * 0.5f + 0.1f;
		_lightColor.b = (BaseWorldTime - 4.0f) / 2.0f * -0.06f + 0.1f;
	} else if (BaseWorldTime >= 6 && BaseWorldTime <= 8) {
		_lightColor.r = 1.0f;
		_lightColor.g = (BaseWorldTime - 6.0f) / 2.0f * 0.4f + 0.6f;
		_lightColor.b = (BaseWorldTime - 6.0f) / 2.0f * 0.96f + 0.04f;
	} else if (BaseWorldTime >= 16 && BaseWorldTime <= 18) {
		_lightColor.r = 1.0f;
		_lightColor.g = (18.0f - BaseWorldTime) / 2.0f * 0.4f + 0.6f;
		_lightColor.b = (18.0f - BaseWorldTime) / 2.0f * 0.96f + 0.04f;
	} else if (BaseWorldTime >= 18 && BaseWorldTime <= 20) {
		_lightColor.r = (20.0f - BaseWorldTime) / 2.0f * 0.9f + 0.1f;
		_lightColor.g = (20.0f - BaseWorldTime) / 2.0f * 0.5f + 0.1f;
		_lightColor.b = (20.0f - BaseWorldTime) / 2.0f * -0.06f + 0.1f;
	} else	//Shouldn't get here
	{
		_lightColor = Ogre::ColourValue(1, 1, 1);
	}

	_ambientColor = _lightColor / 3.0f;
	_fogColor = _lightColor * 0.80f;
}

void TestBlockLandApplication::initWorldBlocksSphere() {
	for (BlockType i = BlockType::Air; BLOCKINFO[i].type != BlockType::Max; ++i) {
		createSolidTexture(BLOCKINFO[i].name, BLOCKINFO[i].color);
	}

	Ogre::Image heightMap;
	heightMap.load("heightmap.png", Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);
	const Ogre::PixelBox& pb = heightMap.getPixelBox();
	_worldXSize = pb.getWidth();
	_worldYSize = pb.getHeight();
	_blocks = new Block[_worldYSize * _worldXSize * _worldHeight];
	memset(_blocks, 0, sizeof(Block) * _worldYSize * _worldXSize * _worldHeight);

	for (int y = 0; y < _worldYSize; ++y) {
		for (int x = 0; x < _worldXSize; ++x) {
			const Ogre::ColourValue& color = heightMap.getColourAt(x, y, 0);
			const int height = static_cast<int>((((color.r + color.g + color.b) / 1.5f) - 1.0f) * _worldHeight / 4.0f + _worldHeight / 2.0f);
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
