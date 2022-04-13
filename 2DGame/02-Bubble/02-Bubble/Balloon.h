#ifndef _BALLOON_INCLUDE
#define _BALLOON_INCLUDE


#include "Sprite.h"


// Balloon is basically a Sprite that represents the balloon. As such it has
// all properties it needs to track its collisions.


class Balloon
{

public:


	void init(const glm::ivec2& tileMapPos, const glm::ivec2& pos, Texture& tilesheet, ShaderProgram& shaderProgram);
	void update(int deltaTime);
	void render();

	glm::ivec2 getPosition();
	bool isPopped();

	void pop();


private:
	bool bPopped;
	int framesPopped;
	glm::ivec2 tileMapDispl, posBalloon;
	Sprite* spriteHead, * spriteHandle;

};


#endif // _BALLOON_INCLUDE
#pragma once
