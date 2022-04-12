#ifndef _SALTADOR_INCLUDE
#define _SALTADOR_INCLUDE


#include "Sprite.h"
#include "TileMap.h"


// Saltador is basically an entity that represents the bouncing mechanism. As such it has
// all properties it needs to track its state, jumping, and collisions.


class Saltador
{

public:
	void init(const glm::ivec2& tileMapPos, ShaderProgram& shaderProgram);
	void update(int deltaTime);
	void render();

	void setTileMap(TileMap* tileMap);

	void spawn(int x, int y);

private:
	void setPosition(const glm::vec2& pos);

private:
	bool bComprimido;
	glm::ivec2 tileMapDispl, posSaltador;
	int startY;
	glm::vec2 posSaltadorStart, posSaltadorEnd;
	Texture spritesheet;
	Sprite* sprite;
	TileMap* map;

};


#endif // _SALTADOR_INCLUDE