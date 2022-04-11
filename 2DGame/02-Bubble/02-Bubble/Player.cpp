#include <cmath>
#include <iostream>
#include <GL/glew.h>
#include <GL/glut.h>
#include "Player.h"
#include "Game.h"


#define JUMP_ANGLE_STEP 4
#define DASH_LENGTH_STEP 8
#define DASH_LENGTH_MAX 15 * DASH_LENGTH_STEP
#define JUMP_HEIGHT 96
#define FALL_STEP 4

#define PLAYER_QUAD_SIZE glm::ivec2(24, 24)


enum PlayerAnims
{
	STAND_LEFT, STAND_RIGHT, MOVE_LEFT, MOVE_RIGHT, JUMP_RIGHT, JUMP_LEFT
};


void Player::init(const glm::ivec2 &tileMapPos, ShaderProgram &shaderProgram)
{
	bG = false;
	bJumping = false; bDashing = false; bCanDash = true;
	spritesheet.loadFromFile("images/ProtaC.png", TEXTURE_PIXEL_FORMAT_RGBA);
	spritesheet.setMinFilter(GL_NEAREST);
	spritesheet.setMagFilter(GL_NEAREST);
	sprite = Sprite::createSprite(PLAYER_QUAD_SIZE, glm::vec2(0.25, 0.25), &spritesheet, &shaderProgram);
	sprite->setNumberAnimations(6);
	
		sprite->setAnimationSpeed(STAND_LEFT, 8);
		sprite->addKeyframe(STAND_LEFT, glm::vec2(0.f, 0.25));
		
		sprite->setAnimationSpeed(STAND_RIGHT, 8);
		sprite->addKeyframe(STAND_RIGHT, glm::vec2(0.f, 0.f));
		
		sprite->setAnimationSpeed(MOVE_LEFT, 8);
		sprite->addKeyframe(MOVE_LEFT, glm::vec2(0.25, 0.25));
		sprite->addKeyframe(MOVE_LEFT, glm::vec2(0.5, 0.25));
		sprite->addKeyframe(MOVE_LEFT, glm::vec2(0.75, 0.25));
		
		sprite->setAnimationSpeed(MOVE_RIGHT, 8);
		sprite->addKeyframe(MOVE_RIGHT, glm::vec2(0.25, 0.f));
		sprite->addKeyframe(MOVE_RIGHT, glm::vec2(0.5, 0.f));
		sprite->addKeyframe(MOVE_RIGHT, glm::vec2(0.75, 0.f));

		sprite->setAnimationSpeed(JUMP_RIGHT, 8);
		sprite->addKeyframe(JUMP_RIGHT, glm::vec2(0.f, 0.5));
		sprite->addKeyframe(JUMP_RIGHT, glm::vec2(0.25, 0.5));
		sprite->addKeyframe(JUMP_RIGHT, glm::vec2(0.5, 0.5));

		sprite->setAnimationSpeed(JUMP_LEFT, 8);
		sprite->addKeyframe(JUMP_LEFT, glm::vec2(0.f, 0.75f));
		sprite->addKeyframe(JUMP_LEFT, glm::vec2(0.25, 0.75f));
		sprite->addKeyframe(JUMP_LEFT, glm::vec2(0.5, 0.75f));
		
	sprite->changeAnimation(0);
	tileMapDispl = tileMapPos;
	sprite->setPosition(glm::vec2(float(tileMapDispl.x + posPlayer.x), float(tileMapDispl.y + posPlayer.y)));
	
}

void Player::update(int deltaTime)
{
	// Cheats
	if (Game::instance().getKeyBuffer('g')) {
		bG = !bG; // Toggle (G)od mode
	}


	sprite->update(deltaTime);

	// DASH
	if (Game::instance().getKey('x') && bCanDash)
	{
		dashDir = glm::ivec2(int(Game::instance().getSpecialKey(GLUT_KEY_RIGHT)) - int(Game::instance().getSpecialKey(GLUT_KEY_LEFT)),
			int(Game::instance().getSpecialKey(GLUT_KEY_DOWN)) - int(Game::instance().getSpecialKey(GLUT_KEY_UP)));
		// If no direction is specified and we are standing still, dash in the direction we are looking towards
		if (dashDir == glm::ivec2(0, 0))
		{
			dashDir == glm::ivec2(int(sprite->animation() == STAND_RIGHT) + int(sprite->animation() == MOVE_RIGHT)
				- int(sprite->animation() == STAND_LEFT) - int(sprite->animation() == MOVE_LEFT), 0);
		}
		// If dashDir is 0, don't do anything
		if (dashDir != glm::ivec2(0, 0))
		{
			bJumping = false; bDashing = true; bCanDash = false;
			dashLength = 0;
			double dashDirCoefficient = DASH_LENGTH_MAX / glm::sqrt(1 + int(dashDir.x && dashDir.y));
			posPlayerStart = posPlayer;
			posPlayerEnd = posPlayerStart + glm::vec2(dashDirCoefficient * dashDir.x, dashDirCoefficient * dashDir.y);
		}
	}

	if (bDashing)
	{
		dashLength += DASH_LENGTH_STEP;
		if (dashLength == DASH_LENGTH_MAX)
		{
			bDashing = false;
		}

		glm::vec2 posPlayerDash = (posPlayerEnd * float(dashLength) + posPlayerStart * float(DASH_LENGTH_MAX - dashLength)) / float(DASH_LENGTH_MAX);
		glm::ivec2 posPlayerCurrent = posPlayer;
		posPlayer.x = int(posPlayerDash.x);
		if (map->collisionSpike(posPlayer, PLAYER_QUAD_SIZE, bG))
		{
			spawn();
		}
		else if (dashDir.x < 0) {
			if (map->collisionMoveLeft(posPlayer, PLAYER_QUAD_SIZE)) 
			{
				posPlayer.x = int(posPlayerCurrent.x);
				bDashing = false;
			}
		}
		else if (dashDir.x > 0) {
			if (map->collisionMoveRight(posPlayer, PLAYER_QUAD_SIZE))
			{
				posPlayer.x = int(posPlayerCurrent.x);
				bDashing = false;
			}
		}
		posPlayer.y = int(posPlayerDash.y);
		if (map->collisionSpike(posPlayer, PLAYER_QUAD_SIZE, bG))
		{
			spawn();
		}
		else if (dashDir.y < 0) {
			if (map->collisionMoveLeft(posPlayer, PLAYER_QUAD_SIZE))
			{
				posPlayer.y = int(posPlayerCurrent.y);
				bDashing = false;
			}
		}
		else if (dashDir.y > 0) {
			if (map->collisionMoveRight(posPlayer, PLAYER_QUAD_SIZE))
			{
				posPlayer.y = int(posPlayerCurrent.y);
				bDashing = false;
			}
		}
		// CLIMB
		if (Game::instance().getKey('c') && map->touchingWall(posPlayer, PLAYER_QUAD_SIZE))
		{
			bDashing = false;

			bJumping = true;
			jumpAngle = 0;
			startY = posPlayer.y;
		}
	} 
	else
	{
		if (Game::instance().getSpecialKey(GLUT_KEY_LEFT))
		{
			if (sprite->animation() != MOVE_LEFT)
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
		else if (Game::instance().getSpecialKey(GLUT_KEY_RIGHT))
		{
			if (sprite->animation() != MOVE_RIGHT)
				sprite->changeAnimation(MOVE_RIGHT);
			posPlayer.x += 2;
			if (map->collisionSpike(posPlayer, PLAYER_QUAD_SIZE, bG))
			{
				spawn();
			}
			else if (map->collisionMoveRight(posPlayer, PLAYER_QUAD_SIZE))
			{
				posPlayer.x -= 2;
				sprite->changeAnimation(STAND_RIGHT);
			}
		}
		else
		{
			if (sprite->animation() == MOVE_LEFT)
				sprite->changeAnimation(STAND_LEFT);
			else if (sprite->animation() == MOVE_RIGHT)
				sprite->changeAnimation(STAND_RIGHT);
		}

		if (bJumping)
		{
			jumpAngle += JUMP_ANGLE_STEP;
			if (jumpAngle == 180)
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
			if (Game::instance().getKey('c') && map->touchingWall(posPlayer, PLAYER_QUAD_SIZE))
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
			else if (map->collisionMoveDown(posPlayer, PLAYER_QUAD_SIZE, &posPlayer.y))
			{
				bCanDash = true;
				if (Game::instance().getKey('c'))
				{
					bJumping = true;
					jumpAngle = 0;
					startY = posPlayer.y;
				}
			}
			else
			{
				// CLIMB
				if (Game::instance().getKey('c') && map->touchingWall(posPlayer, PLAYER_QUAD_SIZE))
				{
					bJumping = true;
					jumpAngle = 0;
					startY = posPlayer.y;
				}
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
	bJumping = false; bDashing = false; bCanDash = true;
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




