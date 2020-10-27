#ifndef __GLOBALS__
#define __GLOBALS__
#pragma once

// Warning disabled ---
#pragma warning( disable : 4577 ) // Warning that exceptions are disabled
#pragma warning( disable : 4530 )

#include <windows.h>
#include <stdio.h>
#include <string>
#include <vector>
#include <assert.h>

#define CAP(n) ((n <= 0.0f) ? n=0.0f : (n >= 1.0f) ? n=1.0f : n=n)

#define DEGTORAD 0.0174532925199432957f
#define RADTODEG 57.295779513082320876f
#define PI acos(-1)

#define LIBRARY_PATH "Library/"
#define FOLDERS_PATH "Library/Folders/"
#define MESHES_PATH "Library/Meshes/"
#define MATERIALS_PATH "Library/Materials/"
#define TEXTURES_PATH "Library/Textures/"
#define MODELS_PATH "Library/Models/"
#define ANIMATIONS_PATH "Library/Animations/"
#define BONES_PATH "Library/Bones/"
#define PARTICLES_PATH "Library/ParticleSystems/"
#define SHADERS_PATH "Library/Shaders/"
#define SCENES_PATH "Library/Scenes/"
#define OBLIGUED_PATH "Assets/"

// Deletes an array of buffers
#define RELEASE_ARRAY( x )\
	{\
       if( x != nullptr )\
       {\
           delete[] x;\
	       x = nullptr;\
		 }\
	 }

typedef unsigned int uint;
typedef unsigned __int64 uint64;

enum class update_status
{
	UPDATE_CONTINUE = 1,
	UPDATE_STOP,
	UPDATE_ERROR
};

enum class EVENT_ENUM
{
	NULL_EVENT,
	CHANGE_WINDOW_WIDTH,
	CHANGE_WINDOW_HEIGHT,
	WINDOW_RESIZE,
	FULLSCREEN,
	RESIZABLE_WINDOW,
	BORDERLESS_WINDOW,
	FULLDESKTOP_WINDOW,
	SCREEN_BRIGHTNESS,

	FILE_DROPPED,

	DEFAULT_TEXTURE_LOADED,

	CREATE_CUBE,
	CREATE_SPHERE,
	CREATE_PYRAMID,
	CREATE_CYLINDER

};

// Configuration -----------
#define SCREEN_WIDTH 1600
#define SCREEN_HEIGHT 900
#define SCREEN_SIZE 1
#define WIN_FULLSCREEN false
#define WIN_RESIZABLE true
#define WIN_BORDERLESS false
#define WIN_FULLSCREEN_DESKTOP false

#endif