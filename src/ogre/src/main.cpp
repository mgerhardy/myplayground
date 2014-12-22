#include <Ogre.h>
#include <OIS/OIS.h>

static const int WORLD_SIZE = 16;	// We'll change these later for various test worlds
static const int CHUNK_SIZE = 16;

static int m_ChunkID;		        // Used for uniquely naming our chunks

typedef unsigned char block_t;

static block_t* m_Blocks;	        // Holds the block worlds in a [WORLD_SIZE][WORLD_SIZE][WORLD_SIZE] array

// Read/write access method for our block world (doesn't check input)
static block_t& GetBlock(const int x, const int y, const int z) {
	return m_Blocks[x + y * WORLD_SIZE + z * WORLD_SIZE * WORLD_SIZE];
}

static Ogre::ManualObject* createCubeMesh(const Ogre::String& name, const Ogre::String& matName) {
	Ogre::ManualObject* cube = new Ogre::ManualObject(name);
	cube->begin(matName);

	cube->position(0.5f, -0.5f, 1.0f);
	cube->normal(0.408248f, -0.816497f, 0.408248f);
	cube->textureCoord(1, 0);
	cube->position(-0.5f, -0.5f, 0.0f);
	cube->normal(-0.408248f, -0.816497f, -0.408248f);
	cube->textureCoord(0, 1);
	cube->position(0.5f, -0.5f, 0.0f);
	cube->normal(0.666667f, -0.333333f, -0.666667f);
	cube->textureCoord(1, 1);
	cube->position(-0.5f, -0.5f, 1.0f);
	cube->normal(-0.666667f, -0.333333f, 0.666667f);
	cube->textureCoord(0, 0);
	cube->position(0.5f, 0.5f, 1.0f);
	cube->normal(0.666667f, 0.333333f, 0.666667f);
	cube->textureCoord(1, 0);
	cube->position(-0.5, -0.5, 1.0);
	cube->normal(-0.666667f, -0.333333f, 0.666667f);
	cube->textureCoord(0, 1);
	cube->position(0.5, -0.5, 1.0);
	cube->normal(0.408248, -0.816497, 0.408248f);
	cube->textureCoord(1, 1);
	cube->position(-0.5, 0.5, 1.0);
	cube->normal(-0.408248, 0.816497, 0.408248);
	cube->textureCoord(0, 0);
	cube->position(-0.5, 0.5, 0.0);
	cube->normal(-0.666667, 0.333333, -0.666667);
	cube->textureCoord(0, 1);
	cube->position(-0.5, -0.5, 0.0);
	cube->normal(-0.408248, -0.816497, -0.408248);
	cube->textureCoord(1, 1);
	cube->position(-0.5, -0.5, 1.0);
	cube->normal(-0.666667, -0.333333, 0.666667);
	cube->textureCoord(1, 0);
	cube->position(0.5, -0.5, 0.0);
	cube->normal(0.666667, -0.333333, -0.666667);
	cube->textureCoord(0, 1);
	cube->position(0.5, 0.5, 0.0);
	cube->normal(0.408248, 0.816497, -0.408248);
	cube->textureCoord(1, 1);
	cube->position(0.5, -0.5, 1.0);
	cube->normal(0.408248, -0.816497, 0.408248);
	cube->textureCoord(0, 0);
	cube->position(0.5, -0.5, 0.0);
	cube->normal(0.666667, -0.333333, -0.666667);
	cube->textureCoord(1, 0);
	cube->position(-0.5, -0.5, 0.0);
	cube->normal(-0.408248, -0.816497, -0.408248);
	cube->textureCoord(0, 0);
	cube->position(-0.5, 0.5, 1.0);
	cube->normal(-0.408248, 0.816497, 0.408248);
	cube->textureCoord(1, 0);
	cube->position(0.5, 0.5, 0.0);
	cube->normal(0.408248, 0.816497, -0.408248);
	cube->textureCoord(0, 1);
	cube->position(-0.5, 0.5, 0.0);
	cube->normal(-0.666667, 0.333333, -0.666667);
	cube->textureCoord(1, 1);
	cube->position(0.5, 0.5, 1.0);
	cube->normal(0.666667, 0.333333, 0.666667);
	cube->textureCoord(0, 0);

	cube->triangle(0, 1, 2);
	cube->triangle(3, 1, 0);
	cube->triangle(4, 5, 6);
	cube->triangle(4, 7, 5);
	cube->triangle(8, 9, 10);
	cube->triangle(10, 7, 8);
	cube->triangle(4, 11, 12);
	cube->triangle(4, 13, 11);
	cube->triangle(14, 8, 12);
	cube->triangle(14, 15, 8);
	cube->triangle(16, 17, 18);
	cube->triangle(16, 19, 17);
	cube->end();

	return cube;
}

static void initWorldBlocksSphere(void) {
	for (int z = 0; z < WORLD_SIZE; ++z) {
		for (int y = 0; y < WORLD_SIZE; ++y) {
			for (int x = 0; x < WORLD_SIZE; ++x) {
				if (sqrt(
						(float) (x - WORLD_SIZE / 2) * (x - WORLD_SIZE / 2) + (y - WORLD_SIZE / 2) * (y - WORLD_SIZE / 2)
								+ (z - WORLD_SIZE / 2) * (z - WORLD_SIZE / 2)) < WORLD_SIZE / 2)
					GetBlock(x, y, z) = 1;
			}
		}
	}
}

static void initWorldBlocksRandom(const int Divisor) {
	srand(12345); // To keep it consistent between runs

	for (int z = 0; z < WORLD_SIZE; ++z) {
		for (int y = 0; y < WORLD_SIZE; ++y) {
			for (int x = 0; x < WORLD_SIZE; ++x) {
				GetBlock(x, y, z) = rand() % Divisor;
			}
		}
	}

}

static Ogre::ManualObject* createChunk(const int StartX, const int StartY, const int StartZ) {
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

	++m_ChunkID;

	return MeshChunk;
}

static void createWorldChunks(Ogre::SceneManager* smgr) {
	//std::vector<int> VertexArray;
	Ogre::MaterialPtr mat = Ogre::MaterialManager::getSingleton().create("BoxColor", "General", true);
	Ogre::Technique* tech = mat->getTechnique(0);
	Ogre::Pass* pass = tech->getPass(0);
	Ogre::TextureUnitState* tex = pass->createTextureUnitState();
	tex->setColourOperationEx(Ogre::LBX_MODULATE, Ogre::LBS_MANUAL, Ogre::LBS_CURRENT, Ogre::ColourValue(0, 0.5, 0));

	//Ogre::ManualObject* MeshChunk = new Ogre::ManualObject("MeshManChunk");
	//MeshChunk->begin("BoxColor");

	for (int z = 0; z < WORLD_SIZE; z += CHUNK_SIZE) {
		for (int y = 0; y < WORLD_SIZE; y += CHUNK_SIZE) {
			for (int x = 0; x < WORLD_SIZE; x += CHUNK_SIZE) {
				Ogre::ManualObject* MeshChunk = createChunk(x, y, z);
				smgr->getRootSceneNode()->createChildSceneNode()->attachObject(MeshChunk);
			}
		}
	}

}

int main(int argc, char *argv[]) {
	Ogre::Root* root = new Ogre::Root("plugins.cfg", "ogre.cfg", "Ogre.log");

	if (!root->restoreConfig() && !root->showConfigDialog())
		return -1;

	Ogre::ConfigFile cf;
	cf.load("resources.cfg");

	Ogre::ConfigFile::SectionIterator seci = cf.getSectionIterator();
	while (seci.hasMoreElements()) {
		const Ogre::String& secName = seci.peekNextKey();
		Ogre::ConfigFile::SettingsMultiMap *settings = seci.getNext();
		Ogre::ConfigFile::SettingsMultiMap::iterator i;
		for (i = settings->begin(); i != settings->end(); ++i) {
			const Ogre::String& typeName = i->first;
			const Ogre::String& archName = i->second;
			Ogre::ResourceGroupManager::getSingleton().addResourceLocation(archName, typeName, secName);
		}
	}

	Ogre::ResourceGroupManager::getSingleton().initialiseAllResourceGroups();

	m_Blocks = new block_t[WORLD_SIZE * WORLD_SIZE * WORLD_SIZE + 16000];
	memset(m_Blocks, 0, sizeof(block_t) * WORLD_SIZE * WORLD_SIZE * WORLD_SIZE);
	initWorldBlocksSphere();
	m_ChunkID = 1;

	Ogre::RenderWindow* window = root->initialise(true);
	Ogre::SceneManager* smgr = root->createSceneManager(Ogre::ST_GENERIC);

	createWorldChunks(smgr);

	Ogre::Camera* mCamera = smgr->createCamera("MainCamera");
	Ogre::Viewport* mViewport = window->addViewport(mCamera);

	// Position it at 500 in Z direction
	mCamera->setPosition(Ogre::Vector3(0, 0, 80));
	// Look back along -Z
	mCamera->lookAt(Ogre::Vector3(0, 0, -300));
	mCamera->setNearClipDistance(5);

	mViewport->setBackgroundColour(Ogre::ColourValue(0, 0, 0));
	mCamera->setAspectRatio(Ogre::Real(mViewport->getActualWidth()) / Ogre::Real(mViewport->getActualHeight()));

	smgr->setAmbientLight(Ogre::ColourValue(0.5, 0.5, 0.5));
	Ogre::Light* l = smgr->createLight("MainLight");
	l->setPosition(20, 80, 50);

	//Input Stuff
	size_t windowHnd = 0;
	window->getCustomAttribute("WINDOW", &windowHnd);
	OIS::InputManager* im = OIS::InputManager::createInputSystem(windowHnd);
	OIS::Keyboard* keyboard = static_cast<OIS::Keyboard*>(im->createInputObject(OIS::OISKeyboard, true));

	while (1) {
		Ogre::WindowEventUtilities::messagePump();

		keyboard->capture();

		if (keyboard->isKeyDown(OIS::KC_ESCAPE))
			break;

		if (root->renderOneFrame() == false)
			break;
	}

	im->destroyInputObject(keyboard);
	im->destroyInputSystem(im);
	im = 0;

	delete[] m_Blocks;
	delete root;
	return 0;
}
