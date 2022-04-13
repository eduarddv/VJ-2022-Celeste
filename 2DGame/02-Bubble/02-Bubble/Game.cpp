#include <GL/glew.h>
#include <GL/glut.h>
#include "Game.h"


void Game::init()
{
	// OpenAL setup
	// Generate an AL Buffer
	alGenBuffers(1, &uiBuffer);

	// Load Wave file into OpenAL Buffer
	if (!ALFWLoadWaveToBuffer((char*)ALFWaddMediaPath(BACKGROUND_MUSIC_FILE), uiBuffer))
	{
		ALFWprintf("Failed to load %s\n", ALFWaddMediaPath(BACKGROUND_MUSIC_FILE));
	}

	// Generate a Source to playback the Buffer
	alGenSources(1, &uiSource);

	// Attach Source to Buffer
	alSourcei(uiSource, AL_BUFFER, uiBuffer);

	// Initialize iState
	alGetSourcei(uiSource, AL_SOURCE_STATE, &iState);


	bPlay = true;
	state = MENU;
	bS = false; bLastFrameWasUpdate = true;
	lastFrameDeltaTime = 0;
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	menu.init(SCREEN_WIDTH, SCREEN_HEIGHT);
	info.init(SCREEN_WIDTH, SCREEN_HEIGHT);
	scene.init();
	credits.init(SCREEN_WIDTH, SCREEN_HEIGHT);
	keysBuffer['1'] = true;
}

bool Game::update(int deltaTime)
{
	// Cheats: Half (S)peed
	if (getKeyBuffer('s')) {
		bS = !bS;
		bLastFrameWasUpdate = true;
		lastFrameDeltaTime = 0;
	}

	switch (state)
	{
	case MENU: {
		if (getKeyBuffer('i')) {
			state = INFO;
			break;
		}
		else if (getKeyBuffer(' ')) {
			state = GAME;

		}
		else break;
	}
	case GAME: {
		if (bS) { // Cheats: Half (S)peed
			if (bLastFrameWasUpdate) {
				lastFrameDeltaTime = deltaTime;
			}
			else {
				scene.update((deltaTime + lastFrameDeltaTime) / 2);
				lastFrameDeltaTime = 0;
			}
			bLastFrameWasUpdate = !bLastFrameWasUpdate;
		}
		else {
			scene.update(deltaTime);
		}
	} break;

	case INFO: {
		if (getKeyBuffer('m')) {
			state = MENU;
		}
	} break;

	case CREDITS: {
	} break;
	}

	// OpenAL: background music
	alGetSourcei(uiSource, AL_SOURCE_STATE, &iState);
	if (iState != AL_PLAYING) {
		alSourcePlay(uiSource);
	}

	return bPlay;
}

void Game::render()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	switch (state)
	{
	case MENU: {
		menu.render();
	} break;
	case INFO: {
		info.render();
	} break;
	case GAME: {
		scene.render();
	} break;
	case CREDITS: {
		credits.render();
	} break;
	}
}

void Game::cleanup()
{
	// OpennAL: clean up by deleting Source(s) and Buffer(s)
	alSourceStop(uiSource);
	alDeleteSources(1, &uiSource);
	alDeleteBuffers(1, &uiBuffer);
}

void Game::keyPressed(int key)
{
	if(key == 27) // Escape code
		bPlay = false;
	keys[key] = true;
	keysBuffer[key] = true;
}

void Game::keyReleased(int key)
{
	keys[key] = false;
}

void Game::specialKeyPressed(int key)
{
	specialKeys[key] = true;
}

void Game::specialKeyReleased(int key)
{
	specialKeys[key] = false;
}

void Game::mouseMove(int x, int y)
{
}

void Game::mousePress(int button)
{
}

void Game::mouseRelease(int button)
{
}

void Game::win()
{	
	state = CREDITS;
}

bool Game::getKey(int key) const
{
	return keys[key];
}

bool Game::getKeyBuffer(int key)
{
	bool b = keysBuffer[key];
	keysBuffer[key] = false;
	return b;
}

bool Game::getSpecialKey(int key) const
{
	return specialKeys[key];
}

Scene Game::getScene()
{
	return Scene();
}





