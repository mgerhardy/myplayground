#pragma once

#include "BaseApplication.h"

typedef unsigned char block_t;
typedef unsigned char blocklight_t;

struct blockinfo_t {
	block_t ID;
	char Name[64];
	Ogre::ColourValue Color;
};

class TestBlockLandApplication: public BaseApplication {
private:
	static const int WORLD_SIZE = 256;
	int WORLD_WIDTH;
	int WORLD_HEIGHT;
	static const int CHUNK_SIZE = 16;

	int m_ChunkID;		        // Used for uniquely naming our chunks

	block_t* m_Blocks;	        // Holds the block worlds in a [WORLD_SIZE][WORLD_SIZE][WORLD_SIZE] array

	Ogre::ColourValue m_LightColor;
	Ogre::ColourValue m_AmbientColor;
	Ogre::ColourValue m_FogColor;
	float m_LightAngle;

	float m_WorldTime;
	int m_UpdateChunkX;              //The current chunk index we're updating
	int m_UpdateChunkY;
	int m_UpdateChunkZ;
	int m_UpdateChunksCount;        //Keeps track of whether to update chunks or not
	static const int NUM_UPDATE_CHUNKS = 16;     //Number of chunks to update each frame

	//Stores the mesh object for each chunk
	Ogre::ManualObject** m_pBlockChunks;

	//The number of vertices in each mesh chunk
	int *m_BlockVertexCount;

	// Read/write access method for our block world (doesn't check input)
	inline block_t& GetBlock(const int x, const int y, const int z) const {
		return m_Blocks[x + y * WORLD_WIDTH + z * WORLD_HEIGHT * WORLD_WIDTH];
	}

	blocklight_t* m_BlockLight;

	block_t& GetBlockLight(const int x, const int y, const int z) {
		return m_BlockLight[x + y * WORLD_WIDTH + z * WORLD_HEIGHT * WORLD_WIDTH];
	}

	void initWorldBlocksLight();
	void createChunk(const int StartX, const int StartY, const int StartZ);
	void createWorldChunks();
	void updateChunksFrame();

	// Used for filling our block world
	void initWorldBlocksSphere();

	bool frameEnded(const Ogre::FrameEvent &evt) override;
	bool keyPressed(const OIS::KeyEvent &arg) override;
	void ComputeWorldLightValues(const float WorldTime);
	Ogre::MaterialPtr createTexture(const Ogre::String& pName, const Ogre::String& pImageFilename);
	Ogre::MaterialPtr createSolidTexture(const Ogre::String& pName, const Ogre::ColourValue& color);
public:
	TestBlockLandApplication();
	virtual ~TestBlockLandApplication();

protected:
	virtual void createScene() override;
};
