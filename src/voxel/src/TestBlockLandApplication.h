#pragma once

#include "BaseApplication.h"
#include <Ogre.h>

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
	int _worldZSize;

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

	// The number of vertices in each mesh chunk
	int *_blockVertexCount;

	inline void mesh(Ogre::ManualObject* meshChunk, const Ogre::Vector3& normal, int& vertexIndex, int x1, int y1, int z1, int x2, int y2, int z2, int x3,
			int y3, int z3, int x4, int y4, int z4) const {
		meshChunk->position(x1, y1, z1);
		meshChunk->normal(normal);
		meshChunk->textureCoord(0, 1);
		meshChunk->position(x2, y2, z2);
		meshChunk->normal(normal);
		meshChunk->textureCoord(1, 1);
		meshChunk->position(x3, y3, z3);
		meshChunk->normal(normal);
		meshChunk->textureCoord(1, 0);
		meshChunk->position(x4, y4, z4);
		meshChunk->normal(normal);
		meshChunk->textureCoord(0, 0);

		meshChunk->triangle(vertexIndex, vertexIndex + 1, vertexIndex + 2);
		meshChunk->triangle(vertexIndex + 2, vertexIndex + 3, vertexIndex);

		vertexIndex += 4;
	}

	inline LightValue& getBlockLight(const int x, const int y, const int z) const {
		return getBlock(x, y, z).light;
	}

#define IDX(x, y, z) x + y * _worldXSize + z * _worldZSize * _worldXSize
	// Read/write access method (doesn't check input)
	inline Ogre::ManualObject*& getBlockChunkObject(const int x, const int y, const int z) const {
		return _blockChunkObjects[IDX(x, y, z)];
	}
	// Read/write access method for our block world (doesn't check input)
	inline Block& getBlock(const int x, const int y, const int z) const {
		return _blocks[IDX(x, y, z)];
	}

	// Read/write access method (doesn't check input)
	inline int& getBlockVertexCount(const int x, const int y, const int z) const {
		return _blockVertexCount[IDX(x, y, z)];
	}
#undef IDX

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
