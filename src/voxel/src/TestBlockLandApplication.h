#pragma once

#include "BaseApplication.h"
#include <Ogre.h>
#include "accidentalnoise/include/anl.h"

using namespace anl;

enum class BlockType {
	Air, Grass, Soil, Rock, Max
};

typedef unsigned char LightValue;
#define lengthof(x) (sizeof(x) / sizeof(*(x)))

struct Block {
	BlockType type;
	LightValue light;
};

struct BlockInfo {
	BlockType type;
	const char* name;
	Ogre::ColourValue color;
};

struct Layer {
	BlockType blockType;
	int minLevel;
	int maxLevel;
	int seedOffset;
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

	inline void mesh(Ogre::ManualObject* meshChunk, const Ogre::Vector3& normal, const Ogre::ColourValue& color, int& vertexIndex, int x1, int y1, int z1, int x2, int y2, int z2, int x3,
			int y3, int z3, int x4, int y4, int z4) const {
		meshChunk->position(x1, y1, z1);
		meshChunk->normal(normal);
		meshChunk->textureCoord(0, 1);
		meshChunk->colour(color);

		meshChunk->position(x2, y2, z2);
		meshChunk->normal(normal);
		meshChunk->textureCoord(1, 1);
		meshChunk->colour(color);

		meshChunk->position(x3, y3, z3);
		meshChunk->normal(normal);
		meshChunk->textureCoord(1, 0);
		meshChunk->colour(color);

		meshChunk->position(x4, y4, z4);
		meshChunk->normal(normal);
		meshChunk->textureCoord(0, 0);
		meshChunk->colour(color);

		meshChunk->triangle(vertexIndex, vertexIndex + 1, vertexIndex + 2);
		meshChunk->triangle(vertexIndex + 2, vertexIndex + 3, vertexIndex);

		vertexIndex += 4;
	}

	inline LightValue& getBlockLight(const int x, const int y, const int z) const {
		return getBlock(x, y, z).light;
	}

#define IDX(x, y, z) x + y * _worldXSize + z * _worldZSize * _worldXSize
	// Read/write access method for our block world (doesn't check input)
	inline Block& getBlock(const int x, const int y, const int z) const {
		return _blocks[IDX(x, y, z)];
	}
#undef IDX

	TArray2D<TVec4D<float>> createHeightMapImage();
	void initWorldBlocksLight();
	void createChunk(const int StartX, const int StartY, const int StartZ);
	void createWorldChunks();

	void initWorldBlocksTerrain();
	void initWorldBlocksCaves();

	inline BlockType getLayerType(int height) const {
		static const Layer LAYERS[] = {
			{ BlockType::Grass, 0, 80, 1 },
			{ BlockType::Soil, 81, 190, 2 },
			{ BlockType::Rock, 191, 255, 3 }
		};
		static const int length = lengthof(LAYERS);
		for (int i = 0; i < length; ++i) {
			if (height >= LAYERS[i].minLevel && height <= LAYERS[i].maxLevel)
				return LAYERS[i].blockType;
		}
		return BlockType::Grass;
	}

	Ogre::MaterialPtr createSolidTexture(const Ogre::String& pName, const Ogre::ColourValue& color);
public:
	TestBlockLandApplication();
	virtual ~TestBlockLandApplication();

protected:
	virtual void createScene() override;
};
