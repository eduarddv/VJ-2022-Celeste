#ifndef _DESTRUCTIBLEBLOCK_INCLUDE
#define _DESTRUCTIBLEBLOCK_INCLUDE


#include "Sprite.h"


// DestructibleBlock is basically a Sprite that represents the destructible block. As such it has
// all properties it needs to track its collisions.


class DestructibleBlock
{

public:


	void init(const glm::ivec2& tileMapPos, const glm::ivec2& pos, Texture& tilesheet, ShaderProgram& shaderProgram);
	void update(int deltaTime);
	void render();

	enum State { IDLE, DESTROYING1, DESTROYING2, DESTROYED };

	glm::ivec2 getPosition();
	bool isDestroyed();

	void destroy();


private:
	State state;
	int framesSinceLastStateChange;
	glm::ivec2 tileMapDispl, posDestructibleBlock;
	Sprite* sprite;

};


#endif // _DESTRUCTIBLEBLOCK_INCLUDE
#pragma once
