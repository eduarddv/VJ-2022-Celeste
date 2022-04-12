#ifndef _ENTITY_INCLUDE
#define _ENTITY_INCLUDE


#include <glm/glm.hpp>
#include "Texture.h"
#include "ShaderProgram.h"


class Entity
{

private:
	TileMap(const string& levelFile, const glm::vec2& minCoords, ShaderProgram& program);

public:
	// Tile maps can only be created inside an OpenGL context
	static TileMap* createTileMap(const string& levelFile, const glm::vec2& minCoords, ShaderProgram& program);

	~TileMap();

	void render() const;
	void free();

	int getTileSize() const { return tileSize; }
	glm::ivec2 getPlayerInitTile() const { return playerInitTile; }

	bool collisionMoveLeft(const glm::ivec2& pos, const glm::ivec2& size) const;
	bool collisionMoveRight(const glm::ivec2& pos, const glm::ivec2& size) const;
	bool collisionMoveUp(const glm::ivec2& pos, const glm::ivec2& size, int* posY);
	bool collisionMoveDown(const glm::ivec2& pos, const glm::ivec2& size, int* posY) const;
	bool collisionSpike(const glm::ivec2& pos, const glm::ivec2& size, const bool& bG) const;
	bool touchingWall(const glm::ivec2& pos, const glm::ivec2& size) const;

	bool levelWin();

private:
	bool loadLevel(const string& levelFile);
	void prepareArrays(const glm::vec2& minCoords, ShaderProgram& program);

private:
	GLuint vao;
	GLuint vbo;
	GLint posLocation, texCoordLocation;
	glm::ivec2 playerInitTile, mapSize, tilesheetSize;
	int tileSize, blockSize;
	bool bLevelWin;
	Texture tilesheet;
	glm::vec2 tileTexSize;
	int* map;

};


#endif // _TILE_MAP_INCLUDE
#pragma once
