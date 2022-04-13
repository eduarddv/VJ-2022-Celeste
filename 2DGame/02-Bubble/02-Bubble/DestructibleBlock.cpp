#include <cmath>
#include <iostream>
#include <GL/glew.h>
#include <GL/glut.h>
#include <list>
#include "DestructibleBlock.h"
#include "Game.h"


#define FRAMES_DESTROYING1 15
#define FRAMES_DESTROYING2 15
#define FRAMES_DESTROYED 120

#define DESTRUCTIBLEBLOCK_QUAD_SIZE glm::ivec2(24, 24)


enum DestructibleBlockAnims
{
	IDLE, DESTROYING1, DESTROYING2
};

void DestructibleBlock::init(const glm::ivec2& tileMapPos, const glm::ivec2& pos, Texture& tilesheet, ShaderProgram& shaderProgram)
{
	state = IDLE;
	framesSinceLastStateChange = 0;
	sprite = Sprite::createSprite(DESTRUCTIBLEBLOCK_QUAD_SIZE, glm::vec2(0.125f, 1.f / 6.f), &tilesheet, &shaderProgram);
	sprite->setNumberAnimations(3);

	sprite->setAnimationSpeed(IDLE, 8);
	sprite->addKeyframe(IDLE, glm::vec2(0.75f, 1.f / 3.f));

	sprite->setAnimationSpeed(DESTROYING1, 8);
	sprite->addKeyframe(DESTROYING1, glm::vec2(0.875f, 1.f / 3.f));

	sprite->setAnimationSpeed(DESTROYING2, 8);
	sprite->addKeyframe(DESTROYING2, glm::vec2(0.f, 0.5f));

	sprite->changeAnimation(DestructibleBlockAnims::IDLE);
	tileMapDispl = tileMapPos;
	posDestructibleBlock = pos;
	sprite->setPosition(glm::vec2(float(tileMapDispl.x + posDestructibleBlock.x), float(tileMapDispl.y + posDestructibleBlock.y)));

}

void DestructibleBlock::update(int deltaTime)
{
	sprite->update(deltaTime);

	switch (state)
	{
	case IDLE: {
	} break;
	case DESTROYING1: {
		if (framesSinceLastStateChange++ >= FRAMES_DESTROYING1) {
			state = DESTROYING2;
			framesSinceLastStateChange = 0;
			sprite->changeAnimation(DestructibleBlockAnims::DESTROYING2);
		}
	} break;
	case DESTROYING2: {
		if (framesSinceLastStateChange++ >= FRAMES_DESTROYING2) {
			state = DESTROYED;
			framesSinceLastStateChange = 0;
			// In render() we won't render the sprite
		}
	} break;
	case DESTROYED: {
		if (framesSinceLastStateChange++ >= FRAMES_DESTROYED) {
			state = IDLE;
			framesSinceLastStateChange = 0;
			sprite->changeAnimation(DestructibleBlockAnims::IDLE);
		}
	} break;
	}
}

void DestructibleBlock::render()
{
	if (state != DESTROYED) {
		sprite->render();
	}
}

glm::ivec2 DestructibleBlock::getPosition()
{
	return posDestructibleBlock;
}

bool DestructibleBlock::isDestroyed()
{
	return state == DESTROYED;
}

void DestructibleBlock::destroy()
{
	if (state == IDLE) {
		state = DESTROYING1;
		framesSinceLastStateChange = 0;
		sprite->changeAnimation(DestructibleBlockAnims::DESTROYING1);
	}
}

