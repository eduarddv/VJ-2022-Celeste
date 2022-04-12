#ifndef _BOUNCER_INCLUDE
#define _BOUNCER_INCLUDE


#include "Sprite.h"


// Player is basically a Sprite that represents the player. As such it has
// all properties it needs to track its movement, jumping, and collisions.


class Bouncer
{

public:


	void init(const glm::ivec2& tileMapPos, ShaderProgram& shaderProgram);
	void update(int deltaTime);
	void render();


	void spawn(int x, int y);
	void setState(bool state);


private:
	void setPosition(const glm::vec2& pos);

private:
	bool bCompressed;
	glm::ivec2 tileMapDispl, posBouncer;
	int startY;
	Texture spritesheet;
	Sprite* sprite;

};


#endif // _BOUNCER_INCLUDE
#pragma once
