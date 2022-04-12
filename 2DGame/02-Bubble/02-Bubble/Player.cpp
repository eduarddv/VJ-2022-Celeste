#include <cmath>
#include <iostream>
#include <GL/glew.h>
#include <GL/glut.h>
#include "Player.h"
#include "Game.h"


#define MOVE_STEP 2
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
	Orientation orientation = RIGHT;
	bJumping = false; bDashing = false; bCanDash = true; bClimbing = false;
	bG = false; bD = false;
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
	if (Game::instance().getKeyBuffer('d')) {
		bD = !bD; // Toggle Infinite (D)ash
	}


	sprite->update(deltaTime);

	// DASH
	if (Game::instance().getKeyBuffer('x') && (bCanDash || bD))
	{
		dashDir = glm::ivec2(int(Game::instance().getSpecialKey(GLUT_KEY_RIGHT)) - int(Game::instance().getSpecialKey(GLUT_KEY_LEFT)),
			int(Game::instance().getSpecialKey(GLUT_KEY_DOWN)) - int(Game::instance().getSpecialKey(GLUT_KEY_UP)));
		// If no direction is specified and we are standing still, dash in the direction we are looking towards
		if (dashDir == glm::ivec2(0, 0))
		{
			dashDir = glm::ivec2(int(orientation == RIGHT) - int(orientation == LEFT), 0);
		}
		
		if (dashDir.x) {
			orientation = (dashDir.x == 1) ? RIGHT : LEFT;
		}
		bJumping = false; bDashing = true; bCanDash = false; bClimbing = false;
		dashLength = 0;
		double dashDirCoefficient = DASH_LENGTH_MAX / glm::sqrt(1 + int(dashDir.x && dashDir.y));
		posPlayerStart = posPlayer;
		posPlayerEnd = posPlayerStart + glm::vec2(dashDirCoefficient * dashDir.x, dashDirCoefficient * dashDir.y);
	}

	if (bDashing)
	{
		dashLength += DASH_LENGTH_STEP;
		if (dashLength >= DASH_LENGTH_MAX)
		{
			bDashing = false;
		}

		glm::vec2 posPlayerDash = (posPlayerEnd * float(dashLength) + posPlayerStart * float(DASH_LENGTH_MAX - dashLength)) / float(DASH_LENGTH_MAX);
		posPlayer.x = int(posPlayerDash.x);
		if (dashDir.x < 0) {
			if (map->collisionMoveLeft(posPlayer, PLAYER_QUAD_SIZE, &posPlayer.x)) 
			{
				bDashing = false;
			}
		}
		else if (dashDir.x > 0) {
			if (map->collisionMoveRight(posPlayer, PLAYER_QUAD_SIZE, &posPlayer.x))
			{
				bDashing = false;
			}
		}
		posPlayer.y = int(posPlayerDash.y);
		if (dashDir.y < 0) {
			if (map->collisionMoveUp(posPlayer, PLAYER_QUAD_SIZE, &posPlayer.y))
			{
				bDashing = false;
			}
		}
		else if (dashDir.y > 0) {
			if (map->collisionMoveDown(posPlayer, PLAYER_QUAD_SIZE, &posPlayer.y, bG))
			{
				bDashing = false;
			}
		}
		// CLIMB
		bool bTouchingRightFirst = false;
		if (Game::instance().getKeyBuffer('c') && map->touchingWall(posPlayer, PLAYER_QUAD_SIZE, orientation == RIGHT, &bTouchingRightFirst))
		{
			bDashing = false;

			orientation = bTouchingRightFirst ? LEFT : RIGHT;
			bJumping = true; bClimbing = true;
			jumpAngle = 0;
			startY = posPlayer.y;
		}
	} 
	else
	{
		if (bClimbing) {
			if (orientation == RIGHT) {
				posPlayer.x += MOVE_STEP;
				if (map->collisionMoveRight(posPlayer, PLAYER_QUAD_SIZE, &posPlayer.x))
				{
					bClimbing = false;
				}
			}
			else if (orientation == LEFT) {
				posPlayer.x -= MOVE_STEP;
				if (map->collisionMoveLeft(posPlayer, PLAYER_QUAD_SIZE, &posPlayer.x))
				{
					bClimbing = false;
				}
			}
		}
		else if (Game::instance().getSpecialKey(GLUT_KEY_LEFT))
		{
			if (sprite->animation() != MOVE_LEFT)
				sprite->changeAnimation(MOVE_LEFT);
			posPlayer.x -= MOVE_STEP;
			if (map->collisionMoveLeft(posPlayer, PLAYER_QUAD_SIZE, &posPlayer.x))
			{
				sprite->changeAnimation(STAND_LEFT);
			}
			orientation = LEFT;
		}
		else if (Game::instance().getSpecialKey(GLUT_KEY_RIGHT))
		{
			if (sprite->animation() != MOVE_RIGHT)
				sprite->changeAnimation(MOVE_RIGHT);
			posPlayer.x += MOVE_STEP;
			if (map->collisionMoveRight(posPlayer, PLAYER_QUAD_SIZE, &posPlayer.x))
			{
				sprite->changeAnimation(STAND_RIGHT);
			}
			orientation = RIGHT;
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
			posPlayer.y = int(startY - 96 * sin(3.14159f * jumpAngle / 180.f));
			if (jumpAngle <= 90) {
				if (map->collisionMoveUp(posPlayer, PLAYER_QUAD_SIZE, &posPlayer.y))
					jumpAngle = 180 - jumpAngle;
			}
			else if (jumpAngle > 90) {
				bClimbing = false;
				bJumping = !map->collisionMoveDown(posPlayer, PLAYER_QUAD_SIZE, &posPlayer.y, bG);
			}
			if (jumpAngle >= 180)
			{
				bJumping = false;
			}

			// CLIMB
			bool bTouchingRightFirst = false;
			if (Game::instance().getKeyBuffer('c') && map->touchingWall(posPlayer, PLAYER_QUAD_SIZE, orientation == RIGHT, &bTouchingRightFirst))
			{
				orientation = bTouchingRightFirst ? LEFT : RIGHT;
				bJumping = true; bClimbing = true;
				jumpAngle = 0;
				startY = posPlayer.y;
			}
		}
		else
		{
			posPlayer.y += FALL_STEP;
			if (map->collisionMoveDown(posPlayer, PLAYER_QUAD_SIZE, &posPlayer.y, bG))
			{
				bCanDash = true;
				if (Game::instance().getKeyBuffer('c'))
				{
					bJumping = true;
					jumpAngle = 0;
					startY = posPlayer.y;
				}
			}
			else
			{
				// CLIMB
				bool bTouchingRightFirst = false;
				if (Game::instance().getKeyBuffer('c') && map->touchingWall(posPlayer, PLAYER_QUAD_SIZE, orientation == RIGHT, &bTouchingRightFirst))
				{
					orientation = bTouchingRightFirst ? LEFT : RIGHT;
					bJumping = true; bClimbing = true;
					jumpAngle = 0;
					startY = posPlayer.y;
				}
			}
		}
	}

	if (map->collisionSpike(posPlayer, PLAYER_QUAD_SIZE, bG))
	{
		spawn();
	}

	sprite->setPosition(glm::vec2(float(tileMapDispl.x + posPlayer.x), float(tileMapDispl.y + posPlayer.y)));
}

void Player::render()
{
	sprite->render();
}

void Player::spawn()
{
	bJumping = false; bDashing = false; bCanDash = true; bClimbing = false;
	orientation = RIGHT;
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




