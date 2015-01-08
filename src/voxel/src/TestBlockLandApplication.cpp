#include "TestBlockLandApplication.h"
#include <Ogre.h>
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

#define lengthof(x) (sizeof(x) / sizeof(*(x)))

TestBlockLandApplication::TestBlockLandApplication() :
		_worldXSize(-1), _worldYSize(-1), _chunkID(1), _blocks(nullptr), _lightAngle(90), _worldTime(0.0f), _updateChunkX(0), _updateChunkY(0), _updateChunkZ(0), _updateChunksCount(
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
	for (int y = 0, ny = 0; y < _worldYSize; ++y, ny += delta) {
		for (int z = 0, nz = 0; z < _worldHeight; ++z, nz += delta) {
			for (int x = 0, nx = 0; x < _worldXSize; ++x, nx += delta) {
				const float value = NoiseSource.GetValue(nx, ny, nz);
				if (value > valueLimit)
					getBlock(x, y, z).type = BlockType::Air;
			}
		}
	}
}

#if 0
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
					getBlock(x, y, z).type = LAYERS[layer].BlockID;
				}
			}
		}
	}
}
#endif

void TestBlockLandApplication::initWorldBlocksLight() {
	const LightValue deltaLight = 16;

	for (int z = 0; z < _worldHeight; ++z) {
		for (int x = 0; x < _worldXSize; ++x) {
			LightValue light = 255;

			for (int y = _worldYSize - 1; y >= 0; --y) {
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

void TestBlockLandApplication::createChunk(const int StartX, const int StartY, const int StartZ) {
	Ogre::ManualObject* meshChunk = new Ogre::ManualObject("MeshMatChunk" + Ogre::StringConverter::toString(_chunkID));

	/* Only create visible faces of chunk */
	const Block defaultBlock = { BlockType::Grass, 255 };
	const int sx = 0;
	const int sy = 0;
	const int sz = 0;

	int iVertex = 0;
	const int length = lengthof(BLOCKINFO);
	std::ostringstream ss;
	ss << "*** createChunk: " << StartX << ":" << StartY << ":" << StartZ << "***";
	Ogre::LogManager::getSingletonPtr()->logMessage(ss.str().c_str());
	for (int i = 1; i < length; ++i) {
		const BlockType type = BLOCKINFO[i].type;
		meshChunk->begin(BLOCKINFO[i].name);
		iVertex = 0;

		for (int z = StartZ; z < CHUNK_SIZE + StartZ; ++z) {
			for (int y = StartY; y < CHUNK_SIZE + StartY; ++y) {
				for (int x = StartX; x < CHUNK_SIZE + StartX; ++x) {
					const Block& block = getBlock(x, y, z);
					if (block.type != type)
						continue;

					//x-1
					Block block1 = defaultBlock;
					if (x > sx)
						block1 = getBlock(x - 1, y, z);

					if (block1.type == BlockType::Air) {
						meshChunk->position(x, y, z + 1);
						meshChunk->normal(-1, 0, 0);
						meshChunk->textureCoord(0, 1);
						meshChunk->position(x, y + 1, z + 1);
						meshChunk->normal(-1, 0, 0);
						meshChunk->textureCoord(1, 1);
						meshChunk->position(x, y + 1, z);
						meshChunk->normal(-1, 0, 0);
						meshChunk->textureCoord(1, 0);
						meshChunk->position(x, y, z);
						meshChunk->normal(-1, 0, 0);
						meshChunk->textureCoord(0, 0);

						meshChunk->triangle(iVertex, iVertex + 1, iVertex + 2);
						meshChunk->triangle(iVertex + 2, iVertex + 3, iVertex);

						iVertex += 4;
					}

					//x+1
					block1 = defaultBlock;
					if (x < sx + _worldXSize - 1)
						block1 = getBlock(x + 1, y, z);

					if (block1.type == BlockType::Air) {
						meshChunk->position(x + 1, y, z);
						meshChunk->normal(1, 0, 0);
						meshChunk->textureCoord(0, 1);
						meshChunk->position(x + 1, y + 1, z);
						meshChunk->normal(1, 0, 0);
						meshChunk->textureCoord(1, 1);
						meshChunk->position(x + 1, y + 1, z + 1);
						meshChunk->normal(1, 0, 0);
						meshChunk->textureCoord(1, 0);
						meshChunk->position(x + 1, y, z + 1);
						meshChunk->normal(1, 0, 0);
						meshChunk->textureCoord(0, 0);

						meshChunk->triangle(iVertex, iVertex + 1, iVertex + 2);
						meshChunk->triangle(iVertex + 2, iVertex + 3, iVertex);

						iVertex += 4;
					}

					//y-1
					block1 = defaultBlock;
					if (y > sy)
						block1 = getBlock(x, y - 1, z);

					if (block1.type == BlockType::Air) {
						meshChunk->position(x, y, z);
						meshChunk->normal(0, -1, 0);
						meshChunk->textureCoord(0, 1);
						meshChunk->position(x + 1, y, z);
						meshChunk->normal(0, -1, 0);
						meshChunk->textureCoord(1, 1);
						meshChunk->position(x + 1, y, z + 1);
						meshChunk->normal(0, -1, 0);
						meshChunk->textureCoord(1, 0);
						meshChunk->position(x, y, z + 1);
						meshChunk->normal(0, -1, 0);
						meshChunk->textureCoord(0, 0);

						meshChunk->triangle(iVertex, iVertex + 1, iVertex + 2);
						meshChunk->triangle(iVertex + 2, iVertex + 3, iVertex);

						iVertex += 4;
					}

					//y+1
					block1 = defaultBlock;
					if (y < sy + _worldYSize - 1)
						block1 = getBlock(x, y + 1, z);

					if (block1.type == BlockType::Air) {
						meshChunk->position(x, y + 1, z + 1);
						meshChunk->normal(0, 1, 0);
						meshChunk->textureCoord(0, 1);
						meshChunk->position(x + 1, y + 1, z + 1);
						meshChunk->normal(0, 1, 0);
						meshChunk->textureCoord(1, 1);
						meshChunk->position(x + 1, y + 1, z);
						meshChunk->normal(0, 1, 0);
						meshChunk->textureCoord(1, 0);
						meshChunk->position(x, y + 1, z);
						meshChunk->normal(0, 1, 0);
						meshChunk->textureCoord(0, 0);

						meshChunk->triangle(iVertex, iVertex + 1, iVertex + 2);
						meshChunk->triangle(iVertex + 2, iVertex + 3, iVertex);

						iVertex += 4;
					}

					//z-1
					block1 = defaultBlock;
					if (z > sz)
						block1 = getBlock(x, y, z - 1);

					if (block1.type == BlockType::Air) {
						meshChunk->position(x, y + 1, z);
						meshChunk->normal(0, 0, -1);
						meshChunk->textureCoord(0, 1);
						meshChunk->position(x + 1, y + 1, z);
						meshChunk->normal(0, 0, -1);
						meshChunk->textureCoord(1, 1);
						meshChunk->position(x + 1, y, z);
						meshChunk->normal(0, 0, -1);
						meshChunk->textureCoord(1, 0);
						meshChunk->position(x, y, z);
						meshChunk->normal(0, 0, -1);
						meshChunk->textureCoord(0, 0);

						meshChunk->triangle(iVertex, iVertex + 1, iVertex + 2);
						meshChunk->triangle(iVertex + 2, iVertex + 3, iVertex);

						iVertex += 4;
					}

					//z+1
					block1 = defaultBlock;
					if (z < sz + _worldHeight - 1)
						block1 = getBlock(x, y, z + 1);

					if (block1.type == BlockType::Air) {
						meshChunk->position(x, y, z + 1);
						meshChunk->normal(0, 0, 1);
						meshChunk->textureCoord(0, 1);
						meshChunk->position(x + 1, y, z + 1);
						meshChunk->normal(0, 0, 1);
						meshChunk->textureCoord(1, 1);
						meshChunk->position(x + 1, y + 1, z + 1);
						meshChunk->normal(0, 0, 1);
						meshChunk->textureCoord(1, 0);
						meshChunk->position(x, y + 1, z + 1);
						meshChunk->normal(0, 0, 1);
						meshChunk->textureCoord(0, 0);

						meshChunk->triangle(iVertex, iVertex + 1, iVertex + 2);
						meshChunk->triangle(iVertex + 2, iVertex + 3, iVertex);

						iVertex += 4;
					}
				}
			}
		}
		meshChunk->end();
	}

	mSceneMgr->getRootSceneNode()->createChildSceneNode()->attachObject(meshChunk);

	getBlockChunkObject(StartX / CHUNK_SIZE, StartY / CHUNK_SIZE, StartZ / CHUNK_SIZE) = meshChunk;
	getBlockVertexCount(StartX / CHUNK_SIZE, StartY / CHUNK_SIZE, StartZ / CHUNK_SIZE) = iVertex;

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
#if 0
	//This makes the world day last about 5 minutes of real time
	_worldTime += evt.timeSinceLastFrame * 0.04;
	_worldTime = fmod(_worldTime, 24.0f);

	computeWorldLightValues(_worldTime);
	updateSceneLighting();

	if (_updateChunksCount == 0)
		++_updateChunksCount;

	//Update a few chunks of the world each frame
	updateChunksFrame();
#endif
	return BaseApplication::frameEnded(evt);
}

void TestBlockLandApplication::updateChunksFrame() {
	if (_updateChunksCount <= 0)
		return;

	const int NUM_CHUNKS = _worldHeight / CHUNK_SIZE;

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
	Ogre::LogManager::getSingletonPtr()->logMessage("*** initWorldBlocksCaves ***");
	initWorldBlocksCaves();
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

void TestBlockLandApplication::initWorldBlocksSphere() {
	Ogre::LogManager::getSingletonPtr()->logMessage("*** initWorldBlocksSphere: create textures ***");
	const int length = lengthof(BLOCKINFO);
	for (int i = 1; i < length; ++i) {
		const BlockInfo& info = BLOCKINFO[i];
		createSolidTexture(info.name, info.color);
	}

	Ogre::LogManager::getSingletonPtr()->logMessage("*** initWorldBlocksSphere: load heightmap ***");
	Ogre::Image heightMap;
	heightMap.load("heightmap.png", Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);
	const Ogre::PixelBox& pb = heightMap.getPixelBox();
	_worldXSize = pb.getWidth();
	_worldYSize = pb.getHeight();
	std::ostringstream ss;
	ss << "*** initWorldBlocksSphere: size " << _worldXSize << ":" << _worldYSize << "***";
	Ogre::LogManager::getSingletonPtr()->logMessage(ss.str().c_str());
	_blocks = new Block[_worldYSize * _worldXSize * _worldHeight];
	_blockChunkObjects = new Ogre::ManualObject*[_worldYSize * _worldXSize * _worldHeight];
	_blockVertexCount = new int[_worldYSize * _worldXSize * _worldHeight];
	memset(_blocks, 0, sizeof(Block) * _worldYSize * _worldXSize * _worldHeight);
	memset(_blockVertexCount, 0, sizeof(int) * _worldYSize * _worldXSize * _worldHeight);
	memset(_blockVertexCount, 0, sizeof(Ogre::ManualObject*) * _worldYSize * _worldXSize * _worldHeight);

	for (int y = 0; y < _worldYSize; ++y) {
		for (int x = 0; x < _worldXSize; ++x) {
			const Ogre::ColourValue& color = heightMap.getColourAt(x, y, 0);
			const int height = static_cast<int>((((color.r + color.g + color.b) / 1.5f) - 1.0f) * _worldHeight / 4.0f + _worldHeight / 2.0f);
			for (int z = 0; z < height; ++z) {
				getBlock(x, y, z).type = static_cast<BlockType>((rand() % (static_cast<int>(BlockType::Max)) - static_cast<int>(BlockType::Grass)) + static_cast<int>(BlockType::Grass));
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
