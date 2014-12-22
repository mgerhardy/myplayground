#pragma once

#include "BaseApplication.h"

typedef unsigned char block_t;

class TestBlockLandApplication: public BaseApplication {
private:
	Ogre::ManualObject* createCubeMesh(const Ogre::String& name, const Ogre::String& matName);
	static const int WORLD_SIZE = 16;	// We'll change these later for various test worlds
	static const int CHUNK_SIZE = 16;

	int m_ChunkID;		        // Used for uniquely naming our chunks

	block_t* m_Blocks;	        // Holds the block worlds in a [WORLD_SIZE][WORLD_SIZE][WORLD_SIZE] array

	// Read/write access method for our block world (doesn't check input)
	block_t& GetBlock(const int x, const int y, const int z) {
		return m_Blocks[x + y * WORLD_SIZE + z * WORLD_SIZE * WORLD_SIZE];
	}

	void createChunk(const int StartX, const int StartY, const int StartZ);
	void createWorldChunks();

	// Used for filling our block world
	void initWorldBlocksRandom(const int Divisor);
	void initWorldBlocksSphere();

	void createTexture(const Ogre::String& pName, const Ogre::String& pImageFilename);
	void createSolidTexture(const Ogre::String& pName);

	// Displays the world using our original "naive" method
	void displaySimpleWorld();
public:
	TestBlockLandApplication();
	virtual ~TestBlockLandApplication();

protected:
	virtual void createScene() override;
};
