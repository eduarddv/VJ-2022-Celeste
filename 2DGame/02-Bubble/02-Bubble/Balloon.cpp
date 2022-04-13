#include <cmath>
#include <iostream>
#include <GL/glew.h>
#include <GL/glut.h>
#include <list>
#include "Balloon.h"
#include "Game.h"


#define FRAMES_POPPED 120

#define BALLOON_PIECE_QUAD_SIZE glm::ivec2(24, 24)


enum BalloonAnims
{
	IDLE
};

void Balloon::init(const glm::ivec2& tileMapPos, const glm::ivec2& pos, Texture& tilesheet, ShaderProgram& shaderProgram)
{
	bPopped = false;
	framesPopped = 0;
	spriteHead = Sprite::createSprite(BALLOON_PIECE_QUAD_SIZE, glm::vec2(0.125f, 1.f / 6.f), &tilesheet, &shaderProgram);
	spriteHandle = Sprite::createSprite(BALLOON_PIECE_QUAD_SIZE, glm::vec2(0.125f, 1.f / 6.f), &tilesheet, &shaderProgram);
	spriteHead->setNumberAnimations(1);
	spriteHandle->setNumberAnimations(1);

	spriteHead->setAnimationSpeed(IDLE, 2);
	spriteHead->addKeyframe(IDLE, glm::vec2(0.625f, 0.5f));

	spriteHandle->setAnimationSpeed(IDLE, 2);
	spriteHandle->addKeyframe(IDLE, glm::vec2(0.75f, 0.5f));
	spriteHandle->addKeyframe(IDLE, glm::vec2(0.875f, 0.5f));

	spriteHead->changeAnimation(IDLE);
	spriteHandle->changeAnimation(IDLE);
	tileMapDispl = tileMapPos;
	posBalloon = pos;
	spriteHead->setPosition(glm::vec2(float(tileMapDispl.x + posBalloon.x), float(tileMapDispl.y + posBalloon.y)));
	spriteHandle->setPosition(glm::vec2(float(tileMapDispl.x + posBalloon.x), float(tileMapDispl.y + posBalloon.y + BALLOON_PIECE_QUAD_SIZE.y)));

}

void Balloon::update(int deltaTime)
{
	spriteHead->update(deltaTime);
	spriteHandle->update(deltaTime);

	if (bPopped == true) {
		if (framesPopped++ >= FRAMES_POPPED) {
			bPopped = false;
		}
	}
}

void Balloon::render()
{
	if (!bPopped) { // Draw only when not popped
		spriteHead->render();
		spriteHandle->render();
	}
}

glm::ivec2 Balloon::getPosition()
{
	return posBalloon;
}

bool Balloon::isPopped()
{
	return bPopped;
}

void Balloon::pop()
{
	bPopped = true;
	framesPopped = 0;
}

