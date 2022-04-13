#ifndef _FLAG_INCLUDE
#define _FLAG_INCLUDE


#include "Sprite.h"


// Player is basically a Sprite that represents the player. As such it has
// all properties it needs to track its movement, jumping, and collisions.


class Flag
{

public:


	void init(const glm::ivec2& tileMapPos, ShaderProgram& shaderProgram);
	void update(int deltaTime);
	void render();


	void spawn(int x, int y);


private:
	void setPosition(const glm::vec2& pos);

private:
	int state;
	glm::ivec2 tileMapDispl, posFlag;
	int startY;
	Texture spritesheet;
	Sprite* sprite;

};


#endif // _FLAG_INCLUDE
#pragma once

