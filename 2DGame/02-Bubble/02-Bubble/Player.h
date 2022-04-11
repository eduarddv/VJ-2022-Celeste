#ifndef _PLAYER_INCLUDE
#define _PLAYER_INCLUDE


#include "Sprite.h"
#include "TileMap.h"


// Player is basically a Sprite that represents the player. As such it has
// all properties it needs to track its movement, jumping, and collisions.


class Player
{

public:
	void init(const glm::ivec2 &tileMapPos, ShaderProgram &shaderProgram);
	void update(int deltaTime);
	void render();
	
	void setTileMap(TileMap *tileMap);

	void spawn();

private:
	void setPosition(const glm::vec2& pos);

private:
	bool bJumping, bDashing, bCanDash;
	bool bG; // Cheats: (G)od mode
	bool bD; // Cheats: Infinite (D)ash
	glm::ivec2 tileMapDispl, posPlayer;
	int jumpAngle, startY;
	int dashLength;
	glm::ivec2 dashDir;
	glm::vec2 posPlayerStart, posPlayerEnd;
	Texture spritesheet;
	Sprite *sprite;
	TileMap *map;

};


#endif // _PLAYER_INCLUDE


