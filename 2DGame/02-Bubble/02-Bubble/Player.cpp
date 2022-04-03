#include <cmath>
#include <iostream>
#include <GL/glew.h>
#include <GL/glut.h>
#include "Player.h"
#include "Game.h"


#define JUMP_ANGLE_STEP 4
#define JUMP_HEIGHT 96
#define FALL_STEP 4

#define PLAYER_QUAD_SIZE glm::ivec2(24, 24)


enum PlayerAnims
{
	STAND_LEFT, STAND_RIGHT, MOVE_LEFT, MOVE_RIGHT
};


void Player::init(const glm::ivec2 &tileMapPos, ShaderProgram &shaderProgram)
{
	bG = false;
	bJumping = false;
	spritesheet.loadFromFile("images/bub.png", TEXTURE_PIXEL_FORMAT_RGBA);
	sprite = Sprite::createSprite(PLAYER_QUAD_SIZE, glm::vec2(0.25, 0.25), &spritesheet, &shaderProgram);
	sprite->setNumberAnimations(4);
	
		sprite->setAnimationSpeed(STAND_LEFT, 8);
		sprite->addKeyframe(STAND_LEFT, glm::vec2(0.f, 0.f));
		
		sprite->setAnimationSpeed(STAND_RIGHT, 8);
		sprite->addKeyframe(STAND_RIGHT, glm::vec2(0.25f, 0.f));
		
		sprite->setAnimationSpeed(MOVE_LEFT, 8);
		sprite->addKeyframe(MOVE_LEFT, glm::vec2(0.f, 0.f));
		sprite->addKeyframe(MOVE_LEFT, glm::vec2(0.f, 0.25f));
		sprite->addKeyframe(MOVE_LEFT, glm::vec2(0.f, 0.5f));
		
		sprite->setAnimationSpeed(MOVE_RIGHT, 8);
		sprite->addKeyframe(MOVE_RIGHT, glm::vec2(0.25, 0.f));
		sprite->addKeyframe(MOVE_RIGHT, glm::vec2(0.25, 0.25f));
		sprite->addKeyframe(MOVE_RIGHT, glm::vec2(0.25, 0.5f));
		
	sprite->changeAnimation(0);
	tileMapDispl = tileMapPos;
	sprite->setPosition(glm::vec2(float(tileMapDispl.x + posPlayer.x), float(tileMapDispl.y + posPlayer.y)));
	
}

void Player::update(int deltaTime)
{
	// Cheats
	if (Game::instance().getKey('g')) {
		bG = (!bG); // Toggle (G)od mode
	}


	sprite->update(deltaTime);
	if(Game::instance().getSpecialKey(GLUT_KEY_LEFT))
	{
		if(sprite->animation() != MOVE_LEFT)
			sprite->changeAnimation(MOVE_LEFT);
		posPlayer.x -= 2;
		if (map->collisionSpike(posPlayer, PLAYER_QUAD_SIZE, bG))
		{
			spawn();
		}
		else if (map->collisionMoveLeft(posPlayer, PLAYER_QUAD_SIZE))
		{
			posPlayer.x += 2;
			sprite->changeAnimation(STAND_LEFT);
		}
	}
	else if(Game::instance().getSpecialKey(GLUT_KEY_RIGHT))
	{
		if(sprite->animation() != MOVE_RIGHT)
			sprite->changeAnimation(MOVE_RIGHT);
		posPlayer.x += 2;
		if (map->collisionSpike(posPlayer, PLAYER_QUAD_SIZE, bG))
		{
			spawn();
		}
		else if(map->collisionMoveRight(posPlayer, PLAYER_QUAD_SIZE))
		{
			posPlayer.x -= 2;
			sprite->changeAnimation(STAND_RIGHT);
		}
	}
	else
	{
		if(sprite->animation() == MOVE_LEFT)
			sprite->changeAnimation(STAND_LEFT);
		else if(sprite->animation() == MOVE_RIGHT)
			sprite->changeAnimation(STAND_RIGHT);
	}
	
	if(bJumping)
	{
		jumpAngle += JUMP_ANGLE_STEP;
		if(jumpAngle == 180)
		{
			bJumping = false;
			posPlayer.y = startY;
		}
		else
		{
			posPlayer.y = int(startY - 96 * sin(3.14159f * jumpAngle / 180.f));
			if (map->collisionSpike(posPlayer, PLAYER_QUAD_SIZE, bG))
			{
				spawn();
			}
			else if (jumpAngle <= 90) {
				if (map->collisionMoveUp(posPlayer, PLAYER_QUAD_SIZE, &posPlayer.y))
					jumpAngle = 180 - jumpAngle;
			}
			else if (jumpAngle > 90)
				bJumping = !map->collisionMoveDown(posPlayer, PLAYER_QUAD_SIZE, &posPlayer.y);
		}
		// CLIMB
		if (Game::instance().getSpecialKey(GLUT_KEY_UP) && map->touchingWall(posPlayer, PLAYER_QUAD_SIZE))
		{
			bJumping = true;
			jumpAngle = 0;
			startY = posPlayer.y;
		}
	}
	else
	{
		posPlayer.y += FALL_STEP;
		if (map->collisionSpike(posPlayer, PLAYER_QUAD_SIZE, bG))
		{
			spawn();
		}
		else if(map->collisionMoveDown(posPlayer, PLAYER_QUAD_SIZE, &posPlayer.y))
		{
			if(Game::instance().getSpecialKey(GLUT_KEY_UP))
			{
				bJumping = true;
				jumpAngle = 0;
				startY = posPlayer.y;
			}
		}
		else
		{
			// CLIMB
			if (Game::instance().getSpecialKey(GLUT_KEY_UP) && map->touchingWall(posPlayer, PLAYER_QUAD_SIZE))
			{
				bJumping = true;
				jumpAngle = 0;
				startY = posPlayer.y;
			}
		}
	}
	
	sprite->setPosition(glm::vec2(float(tileMapDispl.x + posPlayer.x), float(tileMapDispl.y + posPlayer.y)));
}

void Player::render()
{
	sprite->render();
}

void Player::spawn()
{
	bJumping = false;
	setPosition(glm::vec2(map->getPlayerInitTile().x * map->getTileSize(), map->getPlayerInitTile().y * map->getTileSize()));
	sprite->changeAnimation(STAND_RIGHT);
}

void Player::setTileMap(TileMap* tileMap)
{
	map = tileMap;
}

void Player::setPosition(const glm::vec2 &pos)
{
	posPlayer = pos;
	sprite->setPosition(glm::vec2(float(tileMapDispl.x + posPlayer.x), float(tileMapDispl.y + posPlayer.y)));
}




