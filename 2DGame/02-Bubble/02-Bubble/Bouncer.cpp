#include <cmath>
#include <iostream>
#include <GL/glew.h>
#include <GL/glut.h>
#include <list>
#include "Bouncer.h"
#include "Game.h"


#define BOUNCER_QUAD_SIZE glm::ivec2(24, 24)


enum PlayerAnims
{
	IDLE, COMPRESSED
};
void Bouncer::init(const glm::ivec2& tileMapPos, ShaderProgram& shaderProgram)
{
	
	bCompressed = false;
	spritesheet.loadFromFile("images/MapaD.png", TEXTURE_PIXEL_FORMAT_RGBA);
	spritesheet.setMinFilter(GL_NEAREST);
	spritesheet.setMagFilter(GL_NEAREST);
	sprite = Sprite::createSprite(BOUNCER_QUAD_SIZE, glm::vec2(0.25, 0.25), &spritesheet, &shaderProgram);
	sprite->setNumberAnimations(2);

	sprite->setAnimationSpeed(IDLE, 8);
	sprite->addKeyframe(IDLE, glm::vec2(0.5f, 0.33f));

	sprite->setAnimationSpeed(COMPRESSED, 8);
	sprite->addKeyframe(COMPRESSED, glm::vec2(0.625f, 0.33f));

	sprite->changeAnimation(IDLE);
	tileMapDispl = tileMapPos;
	sprite->setPosition(glm::vec2(float(tileMapDispl.x + posBouncer.x), float(tileMapDispl.y + posBouncer.y)));

}

void Bouncer::update(int deltaTime)
{
	if (bCompressed == true) {
		sprite->changeAnimation(COMPRESSED);
	}
	else {
		bCompressed = false;
		sprite->changeAnimation(IDLE);
	}
	
	
}

void Bouncer::render()
{
	sprite->render();
}

void Bouncer::spawn(int x, int y)
{
	bCompressed = false;
	setPosition(glm::vec2(x * 24, y * 24));
	sprite->changeAnimation(IDLE);
}

void Bouncer::setState(bool state)
{
	bCompressed = state;
}

void Bouncer::setPosition(const glm::vec2& pos)
{
	posBouncer = pos;
	sprite->setPosition(glm::vec2(float(tileMapDispl.x + posBouncer.x), float(tileMapDispl.y + posBouncer.y)));
}


