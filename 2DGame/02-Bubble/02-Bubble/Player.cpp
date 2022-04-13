#include <cmath>
#include <iostream>
#include <GL/glew.h>
#include <GL/glut.h>
#include "Player.h"
#include "Game.h"


#define MOVE_STEP 3
#define JUMP_HEIGHT 72
#define JUMP_ANGLE_STEP 180 / 45
#define FALL_STEP int(3.14159f * JUMP_HEIGHT / 180.f * JUMP_ANGLE_STEP)
#define DASH_LENGTH_MAX 80
#define DASH_LENGTH_STEP 8

#define PLAYER_QUAD_SIZE glm::ivec2(24, 24)


enum PlayerAnims
{
	STAND_LEFT, STAND_RIGHT, MOVE_LEFT, MOVE_RIGHT, JUMP_LEFT, JUMP_RIGHT
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
		sprite->addKeyframe(STAND_LEFT, glm::vec2(0.f, 0.25f));
		
		sprite->setAnimationSpeed(STAND_RIGHT, 8);
		sprite->addKeyframe(STAND_RIGHT, glm::vec2(0.f, 0.f));
		
		sprite->setAnimationSpeed(MOVE_LEFT, 8);
		sprite->addKeyframe(MOVE_LEFT, glm::vec2(0.0f, 0.25f));
		sprite->addKeyframe(MOVE_LEFT, glm::vec2(0.25f, 0.25f));
		sprite->addKeyframe(MOVE_LEFT, glm::vec2(0.75f, 0.25f + 1.f / 4.f / 8.f));
		sprite->addKeyframe(MOVE_LEFT, glm::vec2(0.5f, 0.25f));

		sprite->setAnimationSpeed(MOVE_RIGHT, 8);
		sprite->addKeyframe(MOVE_RIGHT, glm::vec2(0.0f, 0.f));
		sprite->addKeyframe(MOVE_RIGHT, glm::vec2(0.25f, 0.f));
		sprite->addKeyframe(MOVE_RIGHT, glm::vec2(0.75f, 0.f + 1.f / 4.f / 8.f));
		sprite->addKeyframe(MOVE_RIGHT, glm::vec2(0.5f, 0.f));

		sprite->setAnimationSpeed(JUMP_LEFT, 8);
		sprite->addKeyframe(JUMP_LEFT, glm::vec2(0.75f, 0.25f + 1.f / 4.f / 8.f));

		sprite->setAnimationSpeed(JUMP_RIGHT, 8);
		sprite->addKeyframe(JUMP_RIGHT, glm::vec2(0.75f, 0.0f + 1.f / 4.f / 8.f));
		
	sprite->changeAnimation(STAND_RIGHT);
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
			orientation = (dashDir.x > 0) ? RIGHT : LEFT;
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

		bool bCollisionHorizontal = false, bCollisionUp = false, bCollisionDown = false;
		glm::vec2 posPlayerDash = (posPlayerEnd * float(dashLength) + posPlayerStart * float(DASH_LENGTH_MAX - dashLength)) / float(DASH_LENGTH_MAX);
		posPlayer.x = int(posPlayerDash.x);
		if (dashDir.x < 0) {
			if (bCollisionHorizontal |= map->collisionMoveLeft(posPlayer, PLAYER_QUAD_SIZE, &posPlayer.x)) 
			{
				bDashing = false;
			}
		}
		else if (dashDir.x > 0) {
			if (bCollisionHorizontal |= map->collisionMoveRight(posPlayer, PLAYER_QUAD_SIZE, &posPlayer.x))
			{
				bDashing = false;
			}
		}
		posPlayer.y = int(posPlayerDash.y);
		if (dashDir.y < 0) {
			if (bCollisionUp = map->collisionMoveUp(posPlayer, PLAYER_QUAD_SIZE, &posPlayer.y))
			{
				bDashing = false;
			}
		}
		else if (dashDir.y > 0) {
			if (bCollisionDown |= map->collisionMoveDown(posPlayer, PLAYER_QUAD_SIZE, &posPlayer.y, bG))
			{
				bDashing = false;
			}
		}
		// JUMP
		if (bCollisionUp || bCollisionHorizontal && !dashDir.y) {
			bJumping = true;
			jumpAngle = 90;
			startY = posPlayer.y + JUMP_HEIGHT;
		}
		else if (bCollisionHorizontal) {
			bJumping = true;
			float initialAngle = 180.f / 3.14159f * glm::acos(180.f / 3.14159f / float(JUMP_HEIGHT) * float(MOVE_STEP) / float(JUMP_ANGLE_STEP));
			float initialDisplacement = JUMP_HEIGHT * sin(3.14159f * initialAngle / 180.f);
			jumpAngle = (dashDir.y > 0) ? 180 : int(initialAngle);
			startY = posPlayer.y + ((dashDir.y > 0) ? 0 : int(initialDisplacement));
		}
		else if (!bDashing && !bCollisionDown) { // we have stopped dashing but not because of collision
			bJumping = true;
			float initialAngle = 180.f / 3.14159f * glm::acos(180.f / 3.14159f / float(JUMP_HEIGHT) * float(MOVE_STEP) / float(JUMP_ANGLE_STEP));
			float initialDisplacement = JUMP_HEIGHT * sin(3.14159f * initialAngle / 180.f);
			jumpAngle = (!dashDir.y) ? 90 : (dashDir.y > 0) ? 180 : int(initialAngle);
			startY = posPlayer.y + ((!dashDir.y) ? JUMP_HEIGHT : (dashDir.y > 0) ? 0 : int(initialDisplacement));
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

		if (orientation == LEFT) {
			if (sprite->animation() != JUMP_LEFT)
				sprite->changeAnimation(JUMP_LEFT);
		}
		else if (orientation == RIGHT) {
			if (sprite->animation() != JUMP_RIGHT)
				sprite->changeAnimation(JUMP_RIGHT);
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
			posPlayer.x -= MOVE_STEP;
			if (map->collisionMoveLeft(posPlayer, PLAYER_QUAD_SIZE, &posPlayer.x))
			{
				if (!bJumping && sprite->animation() != STAND_LEFT)
					sprite->changeAnimation(STAND_LEFT);
			}
			else {
				if (!bJumping && sprite->animation() != MOVE_LEFT)
					sprite->changeAnimation(MOVE_LEFT);
			}
			orientation = LEFT;
		}
		else if (Game::instance().getSpecialKey(GLUT_KEY_RIGHT))
		{
			posPlayer.x += MOVE_STEP;
			if (map->collisionMoveRight(posPlayer, PLAYER_QUAD_SIZE, &posPlayer.x))
			{
				if (!bJumping && sprite->animation() != STAND_RIGHT)
					sprite->changeAnimation(STAND_RIGHT);
			}
			else {
				if (!bJumping && sprite->animation() != MOVE_RIGHT)
					sprite->changeAnimation(MOVE_RIGHT);
			}
			orientation = RIGHT;
		}
		else
		{
			if (!bJumping && orientation == LEFT && sprite->animation() != STAND_LEFT)
				sprite->changeAnimation(STAND_LEFT);
			else if (!bJumping && orientation == RIGHT && sprite->animation() != STAND_RIGHT)
				sprite->changeAnimation(STAND_RIGHT);
		}

		if (bJumping)
		{
			jumpAngle += JUMP_ANGLE_STEP;
			if (jumpAngle <= 90) {
				posPlayer.y = int(startY - JUMP_HEIGHT * sin(3.14159f * jumpAngle / 180.f));
				if (map->collisionMoveUp(posPlayer, PLAYER_QUAD_SIZE, &posPlayer.y)) {
					jumpAngle = 90;
					startY = posPlayer.y + JUMP_HEIGHT;
				}
			}
			else if (jumpAngle > 90 && jumpAngle <= 180) {
				posPlayer.y = int(startY - JUMP_HEIGHT * sin(3.14159f * jumpAngle / 180.f));
				bClimbing = false;
				bJumping = !map->collisionMoveDown(posPlayer, PLAYER_QUAD_SIZE, &posPlayer.y, bG);
			}
			else if (jumpAngle > 180)
			{
				jumpAngle -= JUMP_ANGLE_STEP; // So that we don't overflow
				posPlayer.y += FALL_STEP;
				bJumping = !map->collisionMoveDown(posPlayer, PLAYER_QUAD_SIZE, &posPlayer.y, bG);
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

			if (orientation == LEFT) {
				if (sprite->animation() != JUMP_LEFT)
					sprite->changeAnimation(JUMP_LEFT);
			}
			else if (orientation == RIGHT) {
				if (sprite->animation() != JUMP_RIGHT)
					sprite->changeAnimation(JUMP_RIGHT);
			}
		}
		else
		{
			posPlayer.y += FALL_STEP;
			if (map->collisionMoveDown(posPlayer, PLAYER_QUAD_SIZE, &posPlayer.y, bG))
			{
				bCanDash = true;
				// JUMP
				if (Game::instance().getKeyBuffer('c'))
				{
					bJumping = true;
					jumpAngle = 0;
					startY = posPlayer.y;
				}
			}
			else
			{
				// FALL
				posPlayer.y -= FALL_STEP;
				bJumping = true;
				jumpAngle = 90;
				startY = posPlayer.y + JUMP_HEIGHT;
			}
		}
	}

	if (map->collisionSpike(posPlayer, PLAYER_QUAD_SIZE, bG))
	{
		spawn();
	}

	if (map->collisionBouncer(posPlayer, PLAYER_QUAD_SIZE)) {
		bJumping = true;
		bDashing = false;
		bCanDash = true;
		jumpAngle = 0;
		startY = posPlayer.y - 24;

	}

	if (map->collisionFlag(posPlayer, PLAYER_QUAD_SIZE)) {
		Game::instance().win();
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
	cerr << map->getPlayerInitTile().x << ' ' << map->getPlayerInitTile().y << ' ' << map->getTileSize() << endl;
	setPosition(glm::vec2(map->getPlayerInitTile().x * map->getTileSize(), map->getPlayerInitTile().y * map->getTileSize()));
	sprite->changeAnimation(STAND_RIGHT);
}

void Player::setTileMap(TileMap* tileMap)
{
	map = tileMap;
}

glm::ivec2& Player::getPosition()
{
	return posPlayer;
}

void Player::setJumpstate(bool resultat)
{
	bJumping = resultat;
}

void Player::setPosition(const glm::vec2 &pos)
{
	posPlayer = pos;
	sprite->setPosition(glm::vec2(float(tileMapDispl.x + posPlayer.x), float(tileMapDispl.y + posPlayer.y)));
}




