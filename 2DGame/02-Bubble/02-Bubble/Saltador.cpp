#include <cmath>
#include <iostream>
#include <GL/glew.h>
#include <GL/glut.h>
#include "Saltador.h"
#include "Game.h"


#define JUMP_HEIGHT 126


#define SALTADOR_QUAD_SIZE glm::ivec2(24, 24)


enum SaltadorAnims
{
	IDLE, COMPRIMIDO
};


void Saltador::init(const glm::ivec2& tileMapPos, ShaderProgram& shaderProgram)
{
	bComprimido = false;
	spritesheet.loadFromFile("images/MapaD.png", TEXTURE_PIXEL_FORMAT_RGBA);
	spritesheet.setMinFilter(GL_NEAREST);
	spritesheet.setMagFilter(GL_NEAREST);
	sprite = Sprite::createSprite(SALTADOR_QUAD_SIZE, glm::vec2(0.25, 0.25), &spritesheet, &shaderProgram);
	sprite->setNumberAnimations(2);

	sprite->setAnimationSpeed(IDLE, 8);
	sprite->addKeyframe(IDLE, glm::vec2(0.5, 0.33));

	sprite->setAnimationSpeed(COMPRIMIDO, 8);
	sprite->addKeyframe(COMPRIMIDO, glm::vec2(0.625, 0.33));

	sprite->changeAnimation(IDLE);
	tileMapDispl = tileMapPos;
	sprite->setPosition(glm::vec2(float(tileMapDispl.x + posSaltador.x), float(tileMapDispl.y + posSaltador.y)));

}

void Saltador::update(int deltaTime)
{
	if (Game::instance().getScene().getPlayer().getPosition().x == posSaltador.x && Game::instance().getScene().getPlayer().getPosition().x == posSaltador.y == (posSaltador.y + 1) && bComprimido == false) {
		bComprimido = true;
		sprite->changeAnimation(COMPRIMIDO);
	}
	else if (Game::instance().getScene().getPlayer().getPosition().x == posSaltador.x && Game::instance().getScene().getPlayer().getPosition().y == (posSaltador.y + 1) && bComprimido) {
		bComprimido = false;
		sprite->changeAnimation(IDLE);
	}
	else if (Game::instance().getScene().getPlayer().getPosition().x == posSaltador.x && Game::instance().getScene().getPlayer().getPosition().y == posSaltador.y && bComprimido) {
		bComprimido = false;
		sprite->changeAnimation(IDLE);
	}
}

void Saltador::render()
{
	sprite->render();
}

void Saltador::setTileMap(TileMap* tileMap)
{
	map = tileMap;
}

void Saltador::spawn(int x, int y)
{
	bComprimido = false;
	setPosition(glm::vec2(x * map->getTileSize(), y * map->getTileSize()));
	sprite->changeAnimation(IDLE);
}

void Saltador::setPosition(const glm::vec2& pos)
{
	posSaltador = pos;
	sprite->setPosition(glm::vec2(float(tileMapDispl.x + posSaltador.x), float(tileMapDispl.y + posSaltador.y)));
}
