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
	sprite->addKeyframe(IDLE, glm::vec2(0.5, 0.33));

	sprite->setAnimationSpeed(COMPRESSED, 8);
	sprite->addKeyframe(COMPRESSED, glm::vec2(0.625, 0.33));

	sprite->changeAnimation(IDLE);
	tileMapDispl = tileMapPos;
	sprite->setPosition(glm::vec2(float(tileMapDispl.x + posBouncer.x), float(tileMapDispl.y + posBouncer.y)));

}

void Bouncer::update(int deltaTime)
{
	if (Game::instance().getScene().getPlayer().getPosition().x == posBouncer.x && Game::instance().getScene().getPlayer().getPosition().y == posBouncer.y && deltaTime%10==0) {
		if (bCompressed == false) {
			bCompressed = true;
			sprite->changeAnimation(COMPRESSED);
		}
		else {
			bCompressed = false;
			sprite->changeAnimation(IDLE);
			Game::instance().getScene().getPlayer().setJumpstate(true);
		}
	}
	
}

void Bouncer::render()
{
	sprite->render();
}

void Bouncer::setTileMap(TileMap* tileMap)
{
	map = tileMap;
}

void Bouncer::spawn()
{
	bCompressed = false;
	setPosition(glm::vec2(map->getPlayerInitTile().x * map->getTileSize(), map->getPlayerInitTile().y * map->getTileSize()));
	sprite->changeAnimation(IDLE);
}

void Bouncer::setPosition(const glm::vec2& pos)
{
	posBouncer = pos;
	sprite->setPosition(glm::vec2(float(tileMapDispl.x + posBouncer.x), float(tileMapDispl.y + posBouncer.y)));
}
