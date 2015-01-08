#include "TestBlockLandApplication.h"
#include <OIS/OIS.h>
#include <iostream>
#include <libnoise/noise.h>

const BlockInfo BLOCKINFO[] = {
	{ BlockType::Air, "Air", Ogre::ColourValue(1.0, 1.0, 1.0) },
	{ BlockType::Grass, "Grass", Ogre::ColourValue(0.0, 0.5, 0.0) },
	{ BlockType::Soil, "Soil", Ogre::ColourValue(0.5, 0.5, 0.0) },
	{ BlockType::Rock, "Rock", Ogre::ColourValue(0.5, 0.5, 0.5) }
};

struct Layer {
	BlockType blockType;
	int minLevel;
	int maxLevel;
	int seedOffset;
};

const Layer LAYERS[] = {
	{ BlockType::Grass, 0, 2, 1 },
	{ BlockType::Soil, 0, 10, 2 },
	{ BlockType::Rock, 20, 200, 3 }
};

#define lengthof(x) (sizeof(x) / sizeof(*(x)))

TestBlockLandApplication::TestBlockLandApplication() :
		_worldXSize(-1), _worldZSize(-1), _chunkID(1), _blocks(nullptr), _lightAngle(90), _worldTime(0.0f), _updateChunkX(0), _updateChunkY(0), _updateChunkZ(0), _updateChunksCount(
				0), _blockChunkObjects(nullptr), _lightColor(Ogre::ColourValue(1, 1, 1)), _fogColor(_lightColor * 0.8f), _ambientColor(_lightColor / 3.0f), _blockVertexCount(
				nullptr) {
}

TestBlockLandApplication::~TestBlockLandApplication() {
	delete[] _blocks;
	delete[] _blockChunkObjects;
	delete[] _blockVertexCount;
}

void TestBlockLandApplication::initWorldBlocksCaves() {
	// Change these to increase/decrease the cave size scale
	const float delta = 0.01f;
	const float valueLimit = 0.80f;

	noise::module::RidgedMulti NoiseSource;
	// Initialize the noise module
	NoiseSource.SetSeed(0);
	NoiseSource.SetOctaveCount(4);

	float nx, ny, nz;
	for (int y = 0, ny = 0.0f; y < _worldHeight; ++y, ny += delta) {
		for (int z = 0, nz = 0.0f; z < _worldZSize; ++z, nz += delta) {
			for (int x = 0, nx = 0.0f; x < _worldXSize; ++x, nx += delta) {
				const float value = NoiseSource.GetValue(nx, ny, nz);
				if (value > valueLimit)
					getBlock(x, y, z).type = BlockType::Air;
			}
		}
	}
}

#if 0
void TestBlockLandApplication::initWorldBlocksLayers() {
	int heightMap[_worldXSize][_worldZSize];
	infland::CLandscape LayerMaps[10];  //Hard coded because I'm lazy!
	int numLayerMaps = 0;
	int baseSeed;

	m_Landscape.SetSize(_worldXSize, _worldZSize);
	m_Landscape.CreateAltitude();
	baseSeed = m_Landscape.GetSeed();

	infland::CMap& map = m_Landscape.GetAltitudeMap();

	//Initialize our temporary height map array
	for (int z = 0; z < _worldZSize; ++z) {
		for (int x = 0; x < _worldXSize; ++x) {
			const float height = map.GetValue(x, z);
			heightMap[x][z] = height;
		}
	}

	//Create height maps for each layer
	const int length = lengthof(LAYERS);
	for (int i = 0; i < length; ++i) {
		LayerMaps[i].SetSeed(BaseSeed + LAYERS[i].SeedOffset);
		LayerMaps[i].SetSize(_worldXSize, _worldZSize);
		LayerMaps[i].CreateAltitude();
		++numLayerMaps;
	}

	int layer = 0;

	// Fill in the blocks from all layers
	for (int layer = 0; layer < numLayerMaps; ++layer) {
		infland::CMap & Map = LayerMaps[layer].GetAltitudeMap();

		for (int z = 0; z < _worldZSize; ++z) {
			for (int x = 0; x < _worldXSize; ++x) {
				if (HeightMap[x][z] <= 0)
					continue;
				int height = (Map.GetValue(x, z) + 1) / 2.0f * (LAYERS[layer].maxLevel - LAYERS[layer].minLevel) + LAYERS[layer].minLevel;

				//Don't fill the map below 0 height
				if (HeightMap[x][z] - height < 0.0f)
					height = HeightMap[x][z];
				HeightMap[x][z] -= height;

				int maxHeight = HeightMap[x][z] + height;

				for (int y = HeightMap[x][z]; y <= maxHeight; ++y) {
					getBlock(x, y, z).type = LAYERS[layer].blockType;
				}
			}
		}
	}
}
#endif

void TestBlockLandApplication::initWorldBlocksLight() {
	const LightValue deltaLight = 16;

	for (int z = 0; z < _worldZSize; ++z) {
		for (int x = 0; x < _worldXSize; ++x) {
			LightValue light = 255;

			for (int y = _worldHeight - 1; y >= 0; --y) {
				getBlockLight(x, y, z) = light;

				if (getBlock(x, y, z).type != BlockType::Air) {
					if (light >= deltaLight)
						light -= deltaLight;
					else
						light = 0;
				}
			}
		}
	}
}

void TestBlockLandApplication::createChunk(const int startX, const int startY, const int startZ) {
	Ogre::ManualObject* meshChunk = new Ogre::ManualObject("MeshMatChunk" + Ogre::StringConverter::toString(_chunkID));
	meshChunk->begin("BaseWhiteNoLighting");
	/* Only create visible faces of chunk */
	const BlockType defaultBlock = BlockType::Grass;
	const int sx = 0;
	const int sy = 0;
	const int sz = 0;

	int vertexIndex = 0;

	for (int z = startZ; z < CHUNK_SIZE + startZ; ++z) {
		for (int y = startY; y < CHUNK_SIZE + startY; ++y) {
			for (int x = startX; x < CHUNK_SIZE + startX; ++x) {
				const BlockType type = getBlock(x, y, z).type;
				if (type == BlockType::Air)
					continue;

				BlockType blockType = defaultBlock;
				if (x > sx)
					blockType = getBlock(x - 1, y, z).type;

				const Ogre::ColourValue& color = BLOCKINFO[static_cast<int>(type)].color;
				if (blockType == BlockType::Air) {
					const Ogre::Vector3 normal(-1, 0, 0);
					mesh(meshChunk, normal, color, vertexIndex, x, y, z + 1, x, y + 1, z + 1, x, y + 1, z, x, y, z);
				}

				blockType = defaultBlock;
				if (x < sx + _worldXSize - 1)
					blockType = getBlock(x + 1, y, z).type;

				if (blockType == BlockType::Air) {
					const Ogre::Vector3 normal(1, 0, 0);
					mesh(meshChunk, normal, color, vertexIndex, x + 1, y, z, x + 1, y + 1, z, x + 1, y + 1, z + 1, x + 1, y, z + 1);
				}

				blockType = defaultBlock;
				if (y > sy)
					blockType = getBlock(x, y - 1, z).type;

				if (blockType == BlockType::Air) {
					const Ogre::Vector3 normal(0, -1, 0);
					mesh(meshChunk, normal, color, vertexIndex, x, y, z, x + 1, y, z, x + 1, y, z + 1, x, y, z + 1);
				}

				blockType = defaultBlock;
				if (y < sy + _worldHeight - 1)
					blockType = getBlock(x, y + 1, z).type;

				if (blockType == BlockType::Air) {
					const Ogre::Vector3 normal(0, 1, 0);
					mesh(meshChunk, normal, color, vertexIndex, x, y + 1, z + 1, x + 1, y + 1, z + 1, x + 1, y + 1, z, x, y + 1, z);
				}

				blockType = defaultBlock;
				if (z > sz)
					blockType = getBlock(x, y, z - 1).type;

				if (blockType == BlockType::Air) {
					const Ogre::Vector3 normal(0, 0, -1);
					mesh(meshChunk, normal, color, vertexIndex, x, y + 1, z, x + 1, y + 1, z, x + 1, y, z, x, y, z);
				}

				blockType = defaultBlock;
				if (z < sz + _worldZSize - 1)
					blockType = getBlock(x, y, z + 1).type;

				if (blockType == BlockType::Air) {
					const Ogre::Vector3 normal(0, 0, 1);
					mesh(meshChunk, normal, color, vertexIndex, x, y, z + 1, x + 1, y, z + 1, x + 1, y + 1, z + 1, x, y + 1, z + 1);
				}
			}
		}
	}
	meshChunk->end();

	mSceneMgr->getRootSceneNode()->createChildSceneNode()->attachObject(meshChunk);

	getBlockChunkObject(startX / CHUNK_SIZE, startY / CHUNK_SIZE, startZ / CHUNK_SIZE) = meshChunk;
	getBlockVertexCount(startX / CHUNK_SIZE, startY / CHUNK_SIZE, startZ / CHUNK_SIZE) = vertexIndex;

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
	if (_updateChunksCount <= 0)
		return;

#if 0
	const int NUM_CHUNKSY = _worldHeight / CHUNK_SIZE;
	const int NUM_CHUNKSX = _worldXSize / CHUNK_SIZE;
	const int NUM_CHUNKSZ = _worldZSize / CHUNK_SIZE;

	for (int i = 0; i < NUM_UPDATE_CHUNKS; ++i) {
		// Ignore chunks with nothing in it
		if (getBlockVertexCount(_updateChunkX, _updateChunkY, _updateChunkZ) > 0) {
			// Remove and delete the existing mesh from the scene
			Ogre::ManualObject* pMeshChunk = getBlockChunkObject(_updateChunkX, _updateChunkY, _updateChunkZ);
			if (pMeshChunk != nullptr) {
				pMeshChunk->detachFromParent();
				delete pMeshChunk;
				getBlockChunkObject(_updateChunkX, _updateChunkY, _updateChunkZ) = nullptr;
			}

			// Create a new mesh for the chunk
			Ogre::ManualObject* meshChunk = new Ogre::ManualObject("MeshMatChunk" + Ogre::StringConverter::toString(_chunkID));
			meshChunk->setDynamic(true);
			meshChunk->begin("Combine4");
			createChunk(_updateChunkX * CHUNK_SIZE, _updateChunkY * CHUNK_SIZE, _updateChunkZ * CHUNK_SIZE);
			getBlockChunkObject(_updateChunkX, _updateChunkY, _updateChunkZ) = meshChunk;
			mSceneMgr->getRootSceneNode()->createChildSceneNode()->attachObject(meshChunk);
			++_chunkID;
		}

		// Find the next chunk to update
		++_updateChunkX;

		if (_updateChunkX >= NUM_CHUNKSX) {
			_updateChunkX = 0;
			++_updateChunkZ;

			if (_updateChunkZ >= NUM_CHUNKSZ) {
				_updateChunkZ = 0;
				++_updateChunkY;

				if (_updateChunkY >= NUM_CHUNKSY) {
					_updateChunkY = 0;
					--_updateChunksCount;
				}
			}
		}
	}
#endif
}

Ogre::MaterialPtr TestBlockLandApplication::createSolidTexture(const Ogre::String& pName, const Ogre::ColourValue& color) {
	Ogre::MaterialPtr mat = Ogre::MaterialManager::getSingleton().create(pName, "General");
	Ogre::Technique* tech = mat->getTechnique(0);
	tech->setLightingEnabled(false);
	Ogre::Pass* pass = tech->getPass(0);
	Ogre::TextureUnitState* tex = pass->createTextureUnitState();
	tex->setColourOperationEx(Ogre::LBX_MODULATE, Ogre::LBS_MANUAL, Ogre::LBS_CURRENT, color);
	return mat;
}

Ogre::MaterialPtr TestBlockLandApplication::createTexture(const Ogre::String& pName, const Ogre::String& pImageFilename) {
	Ogre::MaterialPtr mat = Ogre::MaterialManager::getSingleton().create(pName, "General");
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
	for (int z = 0; z < _worldZSize; z += CHUNK_SIZE) {
		for (int y = 0; y < _worldHeight; y += CHUNK_SIZE) {
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

	initWorldBlocksTerrain();
	Ogre::LogManager::getSingletonPtr()->logMessage("*** initWorldBlocksCaves ***");
	//initWorldBlocksCaves();
	Ogre::LogManager::getSingletonPtr()->logMessage("*** initWorldBlocksLight ***");
	initWorldBlocksLight();

	Ogre::LogManager::getSingletonPtr()->logMessage("*** createWorldChunks ***");
	createWorldChunks();

	Ogre::LogManager::getSingletonPtr()->logMessage("*** done ***");
}

void TestBlockLandApplication::computeWorldLightValues(const float worldTime) {
	//6am = SunRise
	//Light is symmetric about noon
	//4am-8am = dawn
	//4am color = (0.1, 0.1, 0.1)
	//6am color = (1, 0.6, 0.04)
	//8am color = (1, 1, 1)

	float baseWorldTime = std::fmod(worldTime, 24);

	_lightAngle = baseWorldTime / 24.0f * 360.0f - 90.0f;
	if (_lightAngle < 0)
		_lightAngle += 360.0f;

	if (baseWorldTime <= 4 || baseWorldTime >= 20) {
		_lightColor = Ogre::ColourValue(0.1f, 0.1f, 0.1f);
	} else if (baseWorldTime >= 8 && baseWorldTime <= 16) {
		_lightColor = Ogre::ColourValue(1, 1, 1);
	} else if (baseWorldTime >= 4 && baseWorldTime <= 6) {
		_lightColor.r = (baseWorldTime - 4.0f) / 2.0f * 0.9f + 0.1f;
		_lightColor.g = (baseWorldTime - 4.0f) / 2.0f * 0.5f + 0.1f;
		_lightColor.b = (baseWorldTime - 4.0f) / 2.0f * -0.06f + 0.1f;
	} else if (baseWorldTime >= 6 && baseWorldTime <= 8) {
		_lightColor.r = 1.0f;
		_lightColor.g = (baseWorldTime - 6.0f) / 2.0f * 0.4f + 0.6f;
		_lightColor.b = (baseWorldTime - 6.0f) / 2.0f * 0.96f + 0.04f;
	} else if (baseWorldTime >= 16 && baseWorldTime <= 18) {
		_lightColor.r = 1.0f;
		_lightColor.g = (18.0f - baseWorldTime) / 2.0f * 0.4f + 0.6f;
		_lightColor.b = (18.0f - baseWorldTime) / 2.0f * 0.96f + 0.04f;
	} else if (baseWorldTime >= 18 && baseWorldTime <= 20) {
		_lightColor.r = (20.0f - baseWorldTime) / 2.0f * 0.9f + 0.1f;
		_lightColor.g = (20.0f - baseWorldTime) / 2.0f * 0.5f + 0.1f;
		_lightColor.b = (20.0f - baseWorldTime) / 2.0f * -0.06f + 0.1f;
	} else	//Shouldn't get here
	{
		_lightColor = Ogre::ColourValue(1, 1, 1);
	}

	_ambientColor = _lightColor / 3.0f;
	_fogColor = _lightColor * 0.80f;
}

void TestBlockLandApplication::initWorldBlocksTerrain() {
	Ogre::LogManager::getSingletonPtr()->logMessage("*** initWorldBlocksTerrain: create textures ***");
	const int length = lengthof(BLOCKINFO);
	for (int i = 1; i < length; ++i) {
		const BlockInfo& info = BLOCKINFO[i];
		createSolidTexture(info.name, info.color);
	}

	Ogre::LogManager::getSingletonPtr()->logMessage("*** initWorldBlocksTerrain: load heightmap ***");
	Ogre::Image heightMap;
	heightMap.load("heightmap.png", Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);
	const Ogre::PixelBox& pb = heightMap.getPixelBox();
	_worldXSize = pb.getWidth();
	_worldZSize = pb.getHeight();
	std::ostringstream ss;
	ss << "*** initWorldBlocksTerrain: size " << _worldXSize << ":" << _worldZSize << "***";
	Ogre::LogManager::getSingletonPtr()->logMessage(ss.str().c_str());
	_blocks = new Block[_worldZSize * _worldXSize * _worldHeight];
	_blockChunkObjects = new Ogre::ManualObject*[_worldZSize * _worldXSize * _worldHeight];
	_blockVertexCount = new int[_worldZSize * _worldXSize * _worldHeight];
	memset(_blocks, 0, sizeof(Block) * _worldZSize * _worldXSize * _worldHeight);
	memset(_blockVertexCount, 0, sizeof(int) * _worldZSize * _worldXSize * _worldHeight);
	memset(_blockChunkObjects, 0, sizeof(Ogre::ManualObject*) * _worldZSize * _worldXSize * _worldHeight);

	for (int z = 0; z < _worldZSize; ++z) {
		for (int x = 0; x < _worldXSize; ++x) {
			const Ogre::ColourValue& color = heightMap.getColourAt(x, z, 0);
			//TODO: scale into world height
			const int height = static_cast<int>(((color.r + color.g + color.b) / 3.0f) * 256.0f);
			for (int y = 0; y < height; ++y) {
				assert(height < _worldHeight);
				getBlock(x, y, z).type = static_cast<BlockType>((rand() % (static_cast<int>(BlockType::Max) - static_cast<int>(BlockType::Grass))) + static_cast<int>(BlockType::Grass));
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
