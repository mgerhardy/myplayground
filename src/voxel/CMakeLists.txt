cmake_minimum_required(VERSION 2.6)
PROJECT(test CXX)

set(CMAKE_MODULE_PATH /usr/lib/OGRE/cmake /usr/share/OGRE/cmake/modules /usr/local/lib/OGRE/cmake)
#set(OGRE_STATIC TRUE)

if (${CMAKE_HOST_SYSTEM_NAME} STREQUAL "Linux")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
set(CMAKE_CXX_STANDARD_LIBRARIES "-ldl -lnoise")
endif()

FIND_PACKAGE(OGRE)
FIND_PACKAGE(OIS)

INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR}/src/accidentalnoise/include ${OIS_INCLUDE_DIRS} ${OGRE_INCLUDE_DIRS} ${OGRE_Overlay_INCLUDE_DIRS} ${OGRE_RenderSystem_GLES_INCLUDE_DIRS} ${OGRE_RenderSystem_GL_INCLUDE_DIRS} ${OGRE_Plugin_OctreeSceneManager_INCLUDE_DIRS})

FILE(GLOB_RECURSE SRCS src/*.cpp src/*.h)

ADD_EXECUTABLE(test ${SRCS})
TARGET_LINK_LIBRARIES(test ${OIS_LIBRARIES} ${OGRE_LIBRARIES} ${OGRE_Overlay_LIBRARIES} ${OGRE_RenderSystem_GLES_LIBRARIES} ${OGRE_RenderSystem_GL_LIBRARIES} ${OGRE_Plugin_OctreeSceneManager_LIBRARIES})
