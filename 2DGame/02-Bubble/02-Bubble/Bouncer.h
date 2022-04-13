#ifndef _BOUNCER_INCLUDE
#define _BOUNCER_INCLUDE


#include "Sprite.h"


// Bouncer is basically a Sprite that represents the bouncer. As such it has
// all properties it needs to track its collisions.


class Bouncer
{

public:


	void init(const glm::ivec2& tileMapPos, const glm::ivec2& pos, Texture& tilesheet, ShaderProgram& shaderProgram);
	void update(int deltaTime);
	void render();

	glm::ivec2 getPosition();
	bool isCompressed();

	void compress();


private:
	bool bCompressed;
	int framesCompressed;
	glm::ivec2 tileMapDispl, posBouncer;
	Sprite* sprite;

};


#endif // _BOUNCER_INCLUDE
#pragma once
