#include "TestBlockLandApplication.h"
#include <OIS.h>
#include <iostream>

const BlockInfo BLOCKINFO[] = {
	{ BlockType::Air, "Air", Ogre::ColourValue(1.0, 1.0, 1.0) },
	{ BlockType::Grass, "Grass", Ogre::ColourValue(0.0, 0.5, 0.0) },
	{ BlockType::Soil, "Soil", Ogre::ColourValue(0.5, 0.3, 0.0) },
	{ BlockType::Rock, "Rock", Ogre::ColourValue(0.5, 0.5, 0.5) }
};

TestBlockLandApplication::TestBlockLandApplication() :
		_worldXSize(-1), _worldZSize(-1), _chunkID(1), _blocks(nullptr) {
}

TestBlockLandApplication::~TestBlockLandApplication() {
	delete[] _blocks;
}

void TestBlockLandApplication::initWorldBlocksCaves() {
	// Change these to increase/decrease the cave size scale
	const float delta = 0.01f;
	const float valueLimit = 0.80f;

	float nx, ny, nz;
	for (int y = 0, ny = 0.0f; y < _worldHeight; ++y, ny += delta) {
		for (int z = 0, nz = 0.0f; z < _worldZSize; ++z, nz += delta) {
			for (int x = 0, nx = 0.0f; x < _worldXSize; ++x, nx += delta) {
			//	const float value = NoiseSource.GetValue(nx, ny, nz);
			//	if (value > valueLimit)
			//		getBlock(x, y, z).type = BlockType::Air;
			}
		}
	}
}

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

				const float blockLightX = getBlockLight(x, y, z) / 255.0f;
				const float blockLightZ = blockLightX * 0.9f;
				const float blockLightY = blockLightX * 0.8f;

				BlockType blockType = defaultBlock;
				if (x > sx)
					blockType = getBlock(x - 1, y, z).type;

				const Ogre::ColourValue& color = BLOCKINFO[static_cast<int>(type)].color;
				if (blockType == BlockType::Air) {
					const Ogre::Vector3 normal(-1, 0, 0);
					mesh(meshChunk, normal, color * blockLightX, vertexIndex, x, y, z + 1, x, y + 1, z + 1, x, y + 1, z, x, y, z);
				}

				blockType = defaultBlock;
				if (x < sx + _worldXSize - 1)
					blockType = getBlock(x + 1, y, z).type;

				if (blockType == BlockType::Air) {
					const Ogre::Vector3 normal(1, 0, 0);
					mesh(meshChunk, normal, color * blockLightX, vertexIndex, x + 1, y, z, x + 1, y + 1, z, x + 1, y + 1, z + 1, x + 1, y, z + 1);
				}

				blockType = defaultBlock;
				if (y > sy)
					blockType = getBlock(x, y - 1, z).type;

				if (blockType == BlockType::Air) {
					const Ogre::Vector3 normal(0, -1, 0);
					mesh(meshChunk, normal, color * blockLightY, vertexIndex, x, y, z, x + 1, y, z, x + 1, y, z + 1, x, y, z + 1);
				}

				blockType = defaultBlock;
				if (y < sy + _worldHeight - 1)
					blockType = getBlock(x, y + 1, z).type;

				if (blockType == BlockType::Air) {
					const Ogre::Vector3 normal(0, 1, 0);
					mesh(meshChunk, normal, color * blockLightY, vertexIndex, x, y + 1, z + 1, x + 1, y + 1, z + 1, x + 1, y + 1, z, x, y + 1, z);
				}

				blockType = defaultBlock;
				if (z > sz)
					blockType = getBlock(x, y, z - 1).type;

				if (blockType == BlockType::Air) {
					const Ogre::Vector3 normal(0, 0, -1);
					mesh(meshChunk, normal, color * blockLightZ, vertexIndex, x, y + 1, z, x + 1, y + 1, z, x + 1, y, z, x, y, z);
				}

				blockType = defaultBlock;
				if (z < sz + _worldZSize - 1)
					blockType = getBlock(x, y, z + 1).type;

				if (blockType == BlockType::Air) {
					const Ogre::Vector3 normal(0, 0, 1);
					mesh(meshChunk, normal, color * blockLightZ, vertexIndex, x, y, z + 1, x + 1, y, z + 1, x + 1, y + 1, z + 1, x, y + 1, z + 1);
				}
			}
		}
	}
	meshChunk->end();

	mSceneMgr->getRootSceneNode()->createChildSceneNode()->attachObject(meshChunk);

	++_chunkID;
}

Ogre::MaterialPtr TestBlockLandApplication::createSolidTexture(const Ogre::String& pName, const Ogre::ColourValue& color) {
	Ogre::MaterialPtr mat = Ogre::MaterialManager::getSingleton().create(pName, "General");
	Ogre::Technique* tech = mat->getTechnique(0);
	tech->setLightingEnabled(true);
	Ogre::Pass* pass = tech->getPass(0);
	Ogre::TextureUnitState* tex = pass->createTextureUnitState();
	tex->setColourOperationEx(Ogre::LBX_MODULATE, Ogre::LBS_MANUAL, Ogre::LBS_CURRENT, color);
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

TArray2D<TVec4D<float>> TestBlockLandApplication::createHeightMapImage() {
	CMWC4096 rnd;
	rnd.setSeedTime();
	CImplicitFractal frac1(anl::EFractalTypes::FBM, anl::GRADIENT, anl::QUINTIC);

	frac1.setSeed(rnd.get());

	CImplicitAutoCorrect ac1(0.0, 1.0);

	ac1.setSource(&frac1);

	CRGBACompositeChannels compose1(anl::RGB);

	compose1.setRedSource(&ac1);
	compose1.setGreenSource(&ac1);
	compose1.setBlueSource(&ac1);
	compose1.setAlphaSource(1.0);

	TArray2D<TVec4D<float>> img(256, 256);

	SMappingRanges ranges;
	mapRGBA2D(anl::SEAMLESS_NONE, img ,compose1 ,ranges, 0);

	//Just for debugging purpose
	saveRGBAArray((char*)"heightmap.tga", &img);

	return img;
}

void TestBlockLandApplication::initWorldBlocksTerrain() {
	Ogre::LogManager::getSingletonPtr()->logMessage("*** initWorldBlocksTerrain: create textures ***");
	const int length = lengthof(BLOCKINFO);
	for (int i = 1; i < length; ++i) {
		const BlockInfo& info = BLOCKINFO[i];
		createSolidTexture(info.name, info.color);
	}

	TArray2D<TVec4D<float>> map = createHeightMapImage();

	_worldXSize = map.width();
	_worldZSize = map.height();

	std::ostringstream ss;
	ss << "*** initWorldBlocksTerrain: size " << _worldXSize << ":" << _worldZSize << "***";

	_blocks = new Block[_worldZSize * _worldXSize * _worldHeight];
	memset(_blocks, 0, sizeof(Block) * _worldZSize * _worldXSize * _worldHeight);

	Ogre::LogManager::getSingletonPtr()->logMessage(ss.str().c_str());

	for (int z = 0; z < _worldZSize; ++z) {
		for (int x = 0; x < _worldXSize; ++x) {
			//TODO: scale into world height
			TVec4D<float> color = map.get(x, z);
			const int height = static_cast<int>(((color.get_x() + color.get_y() + color.get_z()) / 3.0f) * 255.0f);
			for (int y = 0; y < height; ++y) {
				assert(height < _worldHeight);
				getBlock(x, y, z).type = getLayerType(height);
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
