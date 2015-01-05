#include "TestBlockLandApplication.h"
#include <Ogre.h>
#include <OIS/OIS.h>
#include <iostream>

blockinfo_t g_BlockInfo[] = { { 0, "Air", Ogre::ColourValue(1.0, 1.0, 1.0) }, { 1, "Grass", Ogre::ColourValue(0.0, 0.5, 0.0) }, { 2, "Soil", Ogre::ColourValue(
		0.5, 0.5, 0.0) }, { 3, "Rock", Ogre::ColourValue(0.5, 0.5, 0.5) }, { 4, "Lava", Ogre::ColourValue(0.8, 0.0, 0.0) }, { 255, "", Ogre::ColourValue(0.0,
		0.5, 0.0) } };

TestBlockLandApplication::TestBlockLandApplication() {
	m_ChunkID = 1;
}

TestBlockLandApplication::~TestBlockLandApplication() {
	delete[] m_Blocks;
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

	++m_ChunkID;
}

Ogre::MaterialPtr TestBlockLandApplication::createSolidTexture(const Ogre::String& pName, const Ogre::ColourValue& color) {
	Ogre::MaterialPtr mat = Ogre::MaterialManager::getSingleton().create(pName, "General", true);
	Ogre::Technique* tech = mat->getTechnique(0);
	Ogre::Pass* pass = tech->getPass(0);
	Ogre::TextureUnitState* tex = pass->createTextureUnitState();
	tex->setColourOperationEx(Ogre::LBX_MODULATE, Ogre::LBS_MANUAL, Ogre::LBS_CURRENT, color);
	return mat;
}

Ogre::MaterialPtr TestBlockLandApplication::createTexture(const Ogre::String& pName, const Ogre::String& pImageFilename) {
	Ogre::MaterialPtr mat = Ogre::MaterialManager::getSingleton().create(pName, "General", true);
	Ogre::Technique* tech = mat->getTechnique(0);
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
