#include <Ogre.h>
#include <OIS/OIS.h>
static const int WORLD_SIZE = 16;	// We'll change these later for various test worlds
static const int CHUNK_SIZE = 16;

int m_ChunkID;		        // Used for uniquely naming our chunks

typedef unsigned char block_t;

block_t* m_Blocks;	        // Holds the block worlds in a [WORLD_SIZE][WORLD_SIZE][WORLD_SIZE] array

// Read/write access method for our block world (doesn't check input)
block_t& GetBlock(const int x, const int y, const int z) {
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

void initWorldBlocksSphere(void) {
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

void initWorldBlocksRandom(const int Divisor) {
	srand(12345); // To keep it consistent between runs

	for (int z = 0; z < WORLD_SIZE; ++z) {
		for (int y = 0; y < WORLD_SIZE; ++y) {
			for (int x = 0; x < WORLD_SIZE; ++x) {
				GetBlock(x, y, z) = rand() % Divisor;
			}
		}
	}

}

int main() {
	Ogre::Root* root = new Ogre::Root("plugins.cfg", "ogre.cfg", "Ogre.log");

	if (!root->showConfigDialog())
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

	Ogre::MaterialPtr mat = Ogre::MaterialManager::getSingleton().create("BoxColor", "General", true);
	Ogre::Technique* tech = mat->getTechnique(0);
	Ogre::Pass* pass = tech->getPass(0);
	Ogre::TextureUnitState* tex = pass->createTextureUnitState();
	tex->setColourOperationEx(Ogre::LBX_MODULATE, Ogre::LBS_MANUAL, Ogre::LBS_CURRENT, Ogre::ColourValue(0, 0.5, 0));

	Ogre::ManualObject* testBox = createCubeMesh("TestBox1", "BoxColor");
	Ogre::SceneNode* headNode = smgr->getRootSceneNode()->createChildSceneNode();
	Ogre::MeshPtr Mesh = testBox->convertToMesh("TestBox2");
	Ogre::StaticGeometry* pGeom = new Ogre::StaticGeometry(smgr, "Boxes");
	Ogre::Entity* pEnt = smgr->createEntity("TestBox2");

	pGeom->setRegionDimensions(Ogre::Vector3(300, 300, 300));

	for (int z = 0; z < 10; ++z) {
		for (int y = 0; y < 256; ++y) {
			for (int x = 0; x < 256; ++x) {
				pGeom->addEntity(pEnt, Ogre::Vector3(x, y, z));
			}
		}
	}

	pGeom->build();

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

	delete root;
	return 0;
}
