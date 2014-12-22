#pragma once

#include <OgreCamera.h>
#include <OgreEntity.h>
#include <OgreLogManager.h>
#include <OgreRoot.h>
#include <OgreViewport.h>
#include <OgreSceneManager.h>
#include <OgreRenderWindow.h>
#include <OgreConfigFile.h>
#include <OgreOverlaySystem.h>
#include <OgreWindowEventUtilities.h>
#include <OgreTextureManager.h>
#include <OgreMaterialManager.h>

#include <OISEvents.h>
#include <OISInputManager.h>
#include <OISKeyboard.h>
#include <OISMouse.h>

#include "SdkTrays.h"
#include "SdkCameraMan.h"

#ifdef OGRE_STATIC_LIB
#  define OGRE_STATIC_GL
#  if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
#    define OGRE_STATIC_Direct3D9
// D3D10 will only work on vista, so be careful about statically linking
#    if OGRE_USE_D3D10
#      define OGRE_STATIC_Direct3D10
#    endif
#  endif
//#  define OGRE_STATIC_BSPSceneManager
//#  define OGRE_STATIC_ParticleFX
#  define OGRE_STATIC_OctreeSceneManager
#  include "OgreStaticPluginLoader.h"
#endif

class BaseApplication: public Ogre::FrameListener,
		public Ogre::WindowEventListener,
		public OIS::KeyListener,
		public OIS::MouseListener,
		OgreBites::SdkTrayListener {
public:
	BaseApplication();
	virtual ~BaseApplication();

	virtual void go();

protected:
	virtual bool setup();
	virtual bool configure();
	virtual void chooseSceneManager();
	virtual void createCamera();
	virtual void createFrameListener();
	virtual void createScene() = 0; // Override me!
	virtual void destroyScene();
	virtual void createViewports();
	virtual void setupResources();
	virtual void createResourceListener();
	virtual void loadResources();
	virtual bool frameRenderingQueued(const Ogre::FrameEvent& evt);

	virtual bool keyPressed(const OIS::KeyEvent &arg);
	virtual bool keyReleased(const OIS::KeyEvent &arg);
	virtual bool mouseMoved(const OIS::MouseEvent &arg);
	virtual bool mousePressed(const OIS::MouseEvent &arg, OIS::MouseButtonID id);
	virtual bool mouseReleased(const OIS::MouseEvent &arg, OIS::MouseButtonID id);

	// Adjust mouse clipping area
	virtual void windowResized(Ogre::RenderWindow* rw);
	// Unattach OIS before window shutdown (very important under Linux)
	virtual void windowClosed(Ogre::RenderWindow* rw);

	Ogre::Root* mRoot;
	Ogre::Camera* mCamera;
	Ogre::SceneManager* mSceneMgr;
	Ogre::RenderWindow* mWindow;
	Ogre::String mResourcesCfg;
	Ogre::String mPluginsCfg;

	Ogre::OverlaySystem* mOverlaySystem;

	// OgreBites
	OgreBites::InputContext mInputContext;
	OgreBites::SdkTrayManager* mTrayMgr;
	OgreBites::SdkCameraMan* mCameraMan;     	// Basic camera controller
	OgreBites::ParamsPanel* mDetailsPanel;   	// Sample details panel
	bool mCursorWasVisible;	// Was cursor visible before dialog appeared?
	bool mShutDown;

	//OIS Input devices
	OIS::InputManager* mInputManager;
	OIS::Mouse* mMouse;
	OIS::Keyboard* mKeyboard;

	// Added for Mac compatibility
	Ogre::String m_ResourcePath;

#ifdef OGRE_STATIC_LIB
	Ogre::StaticPluginLoader m_StaticPluginLoader;
#endif
};
