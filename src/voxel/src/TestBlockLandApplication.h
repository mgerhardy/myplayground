#pragma once

#include "BaseApplication.h"

enum class BlockType {
	Air, Grass, Soil, Rock, Max
};

typedef unsigned char LightValue;

struct Block {
	BlockType type;
	LightValue light;
};

struct BlockInfo {
	BlockType type;
	const char* name;
	Ogre::ColourValue color;
};

class TestBlockLandApplication: public BaseApplication {
private:
	static const int NUM_UPDATE_CHUNKS = 16;
	static const int CHUNK_SIZE = 16;
	static const int _worldHeight = 256;
	int _worldXSize;
	int _worldYSize;

	// Used for uniquely naming our chunks
	int _chunkID;

	Block* _blocks;

	Ogre::ColourValue _lightColor;
	Ogre::ColourValue _ambientColor;
	Ogre::ColourValue _fogColor;
	float _lightAngle;

	float _worldTime;
	// The current chunk index we're updating
	int _updateChunkX;
	int _updateChunkY;
	int _updateChunkZ;
	// Keeps track of whether to update chunks or not
	int _updateChunksCount;
	// Number of chunks to update each frame

	// Stores the mesh object for each chunk
	Ogre::ManualObject** _blockChunkObjects;

	// Read/write access method (doesn't check input)
	inline Ogre::ManualObject*& getBlockChunkObject(const int x, const int y, const int z) const {
		return _blockChunkObjects[x + y * _worldXSize + z * _worldYSize * _worldXSize];
	}

	// The number of vertices in each mesh chunk
	int *_blockVertexCount;

	// Read/write access method for our block world (doesn't check input)
	inline Block& getBlock(const int x, const int y, const int z) const {
		return _blocks[x + y * _worldXSize + z * _worldYSize * _worldXSize];
	}

	// Read/write access method (doesn't check input)
	inline int& getBlockVertexCount(const int x, const int y, const int z) const {
		return _blockVertexCount[x + y * _worldXSize + z * _worldYSize * _worldXSize];
	}

	inline LightValue& getBlockLight(const int x, const int y, const int z) const {
		return getBlock(x, y, z).light;
	}

	void initWorldBlocksLight();
	void createChunk(const int StartX, const int StartY, const int StartZ);
	void createWorldChunks();
	void updateChunksFrame();

	void updateSceneLighting();

	// Used for filling our block world
	void initWorldBlocksSphere();
	void initWorldBlocksCaves();

	bool frameEnded(const Ogre::FrameEvent &evt) override;
	bool keyPressed(const OIS::KeyEvent &arg) override;
	void computeWorldLightValues(const float WorldTime);
	Ogre::MaterialPtr createTexture(const Ogre::String& pName, const Ogre::String& pImageFilename);
	Ogre::MaterialPtr createSolidTexture(const Ogre::String& pName, const Ogre::ColourValue& color);
public:
	TestBlockLandApplication();
	virtual ~TestBlockLandApplication();

protected:
	virtual void createScene() override;
};
