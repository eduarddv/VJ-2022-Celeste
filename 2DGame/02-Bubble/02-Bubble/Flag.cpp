#include "Flag.h"

#include <cmath>
#include <iostream>
#include <GL/glew.h>
#include <GL/glut.h>
#include <list>
#include "Game.h"


#define FLAG_QUAD_SIZE glm::ivec2(24, 24)


enum FlagAnims
{
	IDLE
};

void Flag::init(const glm::ivec2& tileMapPos, const glm::ivec2& pos, Texture& tilesheet, ShaderProgram& shaderProgram)
{
	sprite = Sprite::createSprite(FLAG_QUAD_SIZE, glm::vec2(0.125f, 1.f / 6.f), &tilesheet, &shaderProgram);
	sprite->setNumberAnimations(1);

	sprite->setAnimationSpeed(IDLE, 8);
	sprite->addKeyframe(IDLE, glm::vec2(0.625f, 5.f / 6.f));
	sprite->addKeyframe(IDLE, glm::vec2(0.75f, 5.f / 6.f));
	sprite->addKeyframe(IDLE, glm::vec2(0.875f, 5.f / 6.f));

	sprite->changeAnimation(IDLE);
	tileMapDispl = tileMapPos;
	posFlag = pos;
	sprite->setPosition(glm::vec2(float(tileMapDispl.x + posFlag.x), float(tileMapDispl.y + posFlag.y)));
}

void Flag::update(int deltaTime)
{
	sprite->update(deltaTime);
}

void Flag::render()
{
	sprite->render();
}

glm::ivec2 Flag::getPosition()
{
	return posFlag;
}
