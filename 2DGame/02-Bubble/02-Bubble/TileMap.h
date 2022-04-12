#ifndef _TILE_MAP_INCLUDE
#define _TILE_MAP_INCLUDE


#include <glm/glm.hpp>
#include "Texture.h"
#include "ShaderProgram.h"


// Class Tilemap is capable of loading a tile map from a text file in a very
// simple format (see level01.txt for an example). With this information
// it builds a single VBO that contains all tiles. As a result the render
// method draws the whole map independently of what is visible.


class TileMap
{

private:
	TileMap(const string &levelFile, const glm::vec2 &minCoords, ShaderProgram &program);

public:
	// Tile maps can only be created inside an OpenGL context
	static TileMap *createTileMap(const string &levelFile, const glm::vec2 &minCoords, ShaderProgram &program);

	~TileMap();

	void render() const;
	void free();
	
	int getTileSize() const { return tileSize; }
	glm::ivec2 getPlayerInitTile() const { return playerInitTile; }

	bool collisionMoveLeft(const glm::ivec2 &pos, const glm::ivec2 &size, int *posX);
	bool collisionMoveRight(const glm::ivec2 &pos, const glm::ivec2 &size, int *posX);
	bool collisionMoveUp(const glm::ivec2& pos, const glm::ivec2& size, int *posY);
	bool collisionMoveDown(const glm::ivec2& pos, const glm::ivec2& size, int* posY, const bool &bG);
	bool collisionSpike(const glm::ivec2& pos, const glm::ivec2& size, const bool& bG) const;
	bool touchingWall(const glm::ivec2& pos, const glm::ivec2& size, const bool& bCheckRightFirst, bool* bTouchingRightFirst) const;

	bool levelWin();
	bool levelLose();

private:
	bool loadLevel(const string &levelFile);
	void prepareArrays(const glm::vec2 &minCoords, ShaderProgram &program);

private:
	GLuint vao;
	GLuint vbo;
	GLint posLocation, texCoordLocation;
	glm::ivec2 playerInitTile, mapSize, tilesheetSize;
	int tileSize, blockSize;
	bool bLevelWin, bLevelLose;
	Texture tilesheet;
	glm::vec2 tileTexSize;
	int *map;

};


#endif // _TILE_MAP_INCLUDE


