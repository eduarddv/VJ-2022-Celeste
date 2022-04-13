#include <cmath>
#include <iostream>
#include <GL/glew.h>
#include <GL/glut.h>
#include <list>
#include "Bouncer.h"
#include "Game.h"


#define FRAMES_COMPRESSED 20

#define BOUNCER_QUAD_SIZE glm::ivec2(24, 24)


enum BouncerAnims
{
	IDLE, COMPRESSED
};

void Bouncer::init(const glm::ivec2& tileMapPos, const glm::ivec2& pos, Texture& tilesheet, ShaderProgram& shaderProgram)
{
	bCompressed = false;
	framesCompressed = 0;
	sprite = Sprite::createSprite(BOUNCER_QUAD_SIZE, glm::vec2(0.125f, 1.f / 6.f), &tilesheet, &shaderProgram);
	sprite->setNumberAnimations(2);

	sprite->setAnimationSpeed(IDLE, 8);
	sprite->addKeyframe(IDLE, glm::vec2(0.5f, 1.f / 3.f));

	sprite->setAnimationSpeed(COMPRESSED, 8);
	sprite->addKeyframe(COMPRESSED, glm::vec2(0.625f, 1.f / 3.f));

	sprite->changeAnimation(IDLE);
	tileMapDispl = tileMapPos;
	posBouncer = pos;
	sprite->setPosition(glm::vec2(float(tileMapDispl.x + posBouncer.x), float(tileMapDispl.y + posBouncer.y)));

}

void Bouncer::update(int deltaTime)
{
	if (bCompressed == true) {
		if (framesCompressed++ >= FRAMES_COMPRESSED) {
			bCompressed = false;
			sprite->changeAnimation(IDLE);
		}
	}
}

void Bouncer::render()
{
	sprite->render();
}

glm::ivec2 Bouncer::getPosition()
{
	return posBouncer;
}

bool Bouncer::isCompressed()
{
	return bCompressed;
}

void Bouncer::compress()
{
	bCompressed = true;
	framesCompressed = 0;
	sprite->changeAnimation(COMPRESSED);
}

