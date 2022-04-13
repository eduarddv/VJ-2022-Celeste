#ifndef _FLAG_INCLUDE
#define _FLAG_INCLUDE


#include "Sprite.h"


// Flag is basically a Sprite that represents the flag. As such it has
// all properties it needs to track its collisions.


class Flag
{

public:


	void init(const glm::ivec2& tileMapPos, const glm::ivec2& pos, Texture& tilesheet, ShaderProgram& shaderProgram);
	void update(int deltaTime);
	void render();

	glm::ivec2 getPosition();


private:
	glm::ivec2 tileMapDispl, posFlag;
	Sprite* sprite;

};


#endif // _FLAG_INCLUDE
#pragma once

