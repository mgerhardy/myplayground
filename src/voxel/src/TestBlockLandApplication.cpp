#include "TestBlockLandApplication.h"
#include <Ogre.h>
#include <OIS/OIS.h>
#include <iostream>

TestBlockLandApplication::TestBlockLandApplication() {
	m_ChunkID = 1;
}

TestBlockLandApplication::~TestBlockLandApplication() {
	delete[] m_Blocks;
}

void TestBlockLandApplication::createChunk(const int StartX, const int StartY, const int StartZ) {
	Ogre::ManualObject* MeshChunk = new Ogre::ManualObject("MeshManChunk" + Ogre::StringConverter::toString(m_ChunkID));
	MeshChunk->begin("BoxColor");

	int iVertex = 0;
	block_t Block;
	block_t Block1;

	for (int z = StartZ; z < CHUNK_SIZE + StartZ; ++z) {
		for (int y = StartY; y < CHUNK_SIZE + StartY; ++y) {
			for (int x = StartX; x < CHUNK_SIZE + StartX; ++x) {
				Block = GetBlock(x, y, z);
				if (Block == 0)
					continue;

				//x-1
				Block1 = 0;
				if (x > StartX)
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
				Block1 = 0;
				if (x < StartX + CHUNK_SIZE - 1)
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
				Block1 = 0;
				if (y > StartY)
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
				Block1 = 0;
				if (y < StartY + CHUNK_SIZE - 1)
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
				Block1 = 0;
				if (z > StartZ)
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
				Block1 = 0;
				if (z < StartZ + CHUNK_SIZE - 1)
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
	mSceneMgr->getRootSceneNode()->createChildSceneNode()->attachObject(MeshChunk);

	++m_ChunkID;
}

Ogre::MaterialPtr TestBlockLandApplication::createSolidTexture(const Ogre::String& pName) {
	Ogre::MaterialPtr mat = Ogre::MaterialManager::getSingleton().create(pName, "General", true);
	Ogre::Technique* tech = mat->getTechnique(0);
	Ogre::Pass* pass = tech->getPass(0);
	Ogre::TextureUnitState* tex = pass->createTextureUnitState();
	tex->setColourOperationEx(Ogre::LBX_MODULATE, Ogre::LBS_MANUAL, Ogre::LBS_CURRENT, Ogre::ColourValue(0, 0.5, 0));
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
	Ogre::MaterialPtr mat = createSolidTexture("BoxColor");

	for (int z = 0; z < WORLD_SIZE; z += CHUNK_SIZE) {
		for (int y = 0; y < WORLD_SIZE; y += CHUNK_SIZE) {
			for (int x = 0; x < WORLD_SIZE; x += CHUNK_SIZE) {
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
				GetBlock(x, y, z) = 1;
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
