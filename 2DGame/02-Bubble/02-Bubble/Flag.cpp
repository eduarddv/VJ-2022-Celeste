#include "Flag.h"

#include <cmath>
#include <iostream>
#include <GL/glew.h>
#include <GL/glut.h>
#include <list>
#include "Game.h"


#define FLAG_QUAD_SIZE glm::ivec2(24, 24)


enum BouncerAnims
{
	IDLE, FIRST, SECOND
};

void Flag::init(const glm::ivec2& tileMapPos, ShaderProgram& shaderProgram)
{
	spritesheet.loadFromFile("images/MapaD.png", TEXTURE_PIXEL_FORMAT_RGBA);
	spritesheet.setMinFilter(GL_NEAREST);
	spritesheet.setMagFilter(GL_NEAREST);
	sprite = Sprite::createSprite(FLAG_QUAD_SIZE, glm::vec2(0.16f, 0.16f), &spritesheet, &shaderProgram);
	sprite->setNumberAnimations(3);

	sprite->setAnimationSpeed(IDLE, 8);
	sprite->addKeyframe(IDLE, glm::vec2(0.625f, 0.875f));

	sprite->setAnimationSpeed(FIRST, 8);
	sprite->addKeyframe(FIRST, glm::vec2(0.75f, 0.875f));

	sprite->setAnimationSpeed(SECOND, 8);
	sprite->addKeyframe(SECOND, glm::vec2(0.875f, 0.875f));

	sprite->changeAnimation(IDLE);
	tileMapDispl = tileMapPos;
	sprite->setPosition(glm::vec2(float(tileMapDispl.x + posFlag.x), float(tileMapDispl.y + posFlag.y)));
}

void Flag::update(int deltaTime)
{
	if (state == 0) {
		sprite->changeAnimation(FIRST);
		state++;
	}
	else if (state == 1) {
		sprite->changeAnimation(SECOND);
		state++;
	}
	else {
		sprite->changeAnimation(IDLE);
		state = 0;
	}

}

void Flag::render()
{
	sprite->render();
}

void Flag::spawn(int x, int y)
{
	setPosition(glm::vec2(x * 24, y * 24));
	sprite->changeAnimation(IDLE);
}

void Flag::setPosition(const glm::vec2& pos)
{
	posFlag = pos;
	sprite->setPosition(glm::vec2(float(tileMapDispl.x + posFlag.x), float(tileMapDispl.y + posFlag.y)));
}
