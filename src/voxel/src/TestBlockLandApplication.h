#pragma once

#include "BaseApplication.h"

typedef unsigned char block_t;

class TestBlockLandApplication: public BaseApplication {
private:
	static const int WORLD_SIZE = 256;
	int WORLD_WIDTH;
	int WORLD_HEIGHT;
	static const int CHUNK_SIZE = 16;

	int m_ChunkID;		        // Used for uniquely naming our chunks

	block_t* m_Blocks;	        // Holds the block worlds in a [WORLD_SIZE][WORLD_SIZE][WORLD_SIZE] array

	// Read/write access method for our block world (doesn't check input)
	inline block_t& GetBlock(const int x, const int y, const int z) const {
		return m_Blocks[x + y * WORLD_WIDTH + z * WORLD_HEIGHT * WORLD_WIDTH];
	}

	void createChunk(const int StartX, const int StartY, const int StartZ);
	void createWorldChunks();

	// Used for filling our block world
	void initWorldBlocksSphere();

	Ogre::MaterialPtr createTexture(const Ogre::String& pName, const Ogre::String& pImageFilename);
	Ogre::MaterialPtr createSolidTexture(const Ogre::String& pName);
public:
	TestBlockLandApplication();
	virtual ~TestBlockLandApplication();

protected:
	virtual void createScene() override;
};
