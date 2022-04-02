#include <GL/glew.h>
#include <GL/glut.h>
#include "Game.h"


void Game::init()
{
	bPlay = true;
	state = MENU;
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	menu.init(SCREEN_WIDTH, SCREEN_HEIGHT);
	scene.init();
}

bool Game::update(int deltaTime)
{
	switch (state)
	{
	case MENU: {

	} break;
	case GAME: {
		scene.update(deltaTime);
	} break;
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
	case GAME: {
		scene.render();
	} break;
	}
}

void Game::keyPressed(int key)
{
	if(key == 27) // Escape code
		bPlay = false;
	if (key == ' ')
		state = GAME;
	keys[key] = true;
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

bool Game::getKey(int key) const
{
	return keys[key];
}

bool Game::getSpecialKey(int key) const
{
	return specialKeys[key];
}





