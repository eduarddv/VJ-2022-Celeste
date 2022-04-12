#include <iostream>
#include <fstream>
#include <sstream>
#include <list>
#include "TileMap.h"
#include "Bouncer.h"


using namespace std;


TileMap *TileMap::createTileMap(const string &levelFile, const glm::vec2 &minCoords, ShaderProgram &program)
{
	TileMap *map = new TileMap(levelFile, minCoords, program);
	
	return map;
}


TileMap::TileMap(const string &levelFile, const glm::vec2 &minCoords, ShaderProgram &program)
{
	bLevelWin = false; bLevelLose = false;
	loadLevel(levelFile, minCoords, program);
	prepareArrays(minCoords, program);
}

TileMap::~TileMap()
{
	if(map != NULL)
		delete map;
}


void TileMap::update(int deltaTime)
{
	currentTime += deltaTime;

	list<Bouncer*> copy = BOU;

	while (copy.empty() == false) {
		copy.front()->update(deltaTime);
		copy.pop_front();
	}
}

void TileMap::render() const
{
	list<Bouncer*> copy = BOU;

	while (copy.empty() == false) {
		copy.front()->render();
		copy.pop_front();
	}

	glEnable(GL_TEXTURE_2D);
	tilesheet.use();
	glBindVertexArray(vao);
	glEnableVertexAttribArray(posLocation);
	glEnableVertexAttribArray(texCoordLocation);
	glDrawArrays(GL_TRIANGLES, 0, 6 * mapSize.x * mapSize.y);
	glDisable(GL_TEXTURE_2D);
}

void TileMap::free()
{
	glDeleteBuffers(1, &vbo);
}

bool TileMap::loadLevel(const string &levelFile, const glm::vec2& minCoords, ShaderProgram& program)
{
	ifstream fin;
	string line, tilesheetFile;
	stringstream sstream;
	char tile;
	
	fin.open(levelFile.c_str());
	if(!fin.is_open())
		return false;
	getline(fin, line);
	if(line.compare(0, 7, "TILEMAP") != 0)
		return false;
	getline(fin, line);
	sstream.str(line);
	sstream >> mapSize.x >> mapSize.y;
	getline(fin, line);
	sstream.str(line);
	sstream >> tileSize >> blockSize;
	getline(fin, line);
	sstream.str(line);
	sstream >> playerInitTile.x >> playerInitTile.y;
	getline(fin, line);
	sstream.str(line);
	sstream >> tilesheetFile;
	tilesheet.loadFromFile(tilesheetFile, TEXTURE_PIXEL_FORMAT_RGBA);
	tilesheet.setWrapS(GL_CLAMP_TO_EDGE);
	tilesheet.setWrapT(GL_CLAMP_TO_EDGE);
	tilesheet.setMinFilter(GL_NEAREST);
	tilesheet.setMagFilter(GL_NEAREST);
	getline(fin, line);
	sstream.str(line);
	sstream >> tilesheetSize.x >> tilesheetSize.y;
	tileTexSize = glm::vec2(1.f / tilesheetSize.x, 1.f / tilesheetSize.y);
	
	map = new int[mapSize.x * mapSize.y];
	bouncermap = new int[mapSize.x * mapSize.y];
	for(int j=0; j<mapSize.y; j++)
	{
		for(int i=0; i<mapSize.x; i++)
		{
			fin.get(tile);
			if(tile == ' ')
				map[j*mapSize.x+i] = 0;
			else if (tile == 'E') {
				map[j * mapSize.x + i] = 0;
				bouncermap[j * mapSize.x + i] = 1;
			}
			else
				map[j*mapSize.x+i] = tile - int('0');
		}
		fin.get(tile);
#ifndef _WIN32
		fin.get(tile);
#endif
	}
	fin.close();
	
	return true;
}

void TileMap::prepareArrays(const glm::vec2 &minCoords, ShaderProgram &program)
{
	int tile, btile, nTiles = 0;
	glm::vec2 posTile, texCoordTile[2], halfTexel;
	vector<float> vertices;
	
	halfTexel = glm::vec2(0.5f / tilesheet.width(), 0.5f / tilesheet.height());
	for(int j=0; j<mapSize.y; j++)
	{
		for(int i=0; i<mapSize.x; i++)
		{
			btile = bouncermap[j * mapSize.x + i];
			if (btile != 0) {
				posTile = glm::vec2(minCoords.x + i * tileSize, minCoords.y + j * tileSize);
				Bouncer* l = new Bouncer();
				l->init(glm::ivec2(minCoords.x, minCoords.y), program);
				l->spawn(i, j);
				BOU.push_back(l);
			}
			
			tile = map[j * mapSize.x + i];
			if(tile != 0)
			{
				// Non-empty tile
				nTiles++;
				posTile = glm::vec2(minCoords.x + i * tileSize, minCoords.y + j * tileSize);
				texCoordTile[0] = glm::vec2(float((tile-1)%tilesheetSize.x) / tilesheetSize.x, float((tile-1)/tilesheetSize.x) / tilesheetSize.y);
				texCoordTile[1] = texCoordTile[0] + tileTexSize;
				// First triangle
				vertices.push_back(posTile.x); vertices.push_back(posTile.y);
				vertices.push_back(texCoordTile[0].x); vertices.push_back(texCoordTile[0].y);
				vertices.push_back(posTile.x + blockSize); vertices.push_back(posTile.y);
				vertices.push_back(texCoordTile[1].x); vertices.push_back(texCoordTile[0].y);
				vertices.push_back(posTile.x + blockSize); vertices.push_back(posTile.y + blockSize);
				vertices.push_back(texCoordTile[1].x); vertices.push_back(texCoordTile[1].y);
				// Second triangle
				vertices.push_back(posTile.x); vertices.push_back(posTile.y);
				vertices.push_back(texCoordTile[0].x); vertices.push_back(texCoordTile[0].y);
				vertices.push_back(posTile.x + blockSize); vertices.push_back(posTile.y + blockSize);
				vertices.push_back(texCoordTile[1].x); vertices.push_back(texCoordTile[1].y);
				vertices.push_back(posTile.x); vertices.push_back(posTile.y + blockSize);
				vertices.push_back(texCoordTile[0].x); vertices.push_back(texCoordTile[1].y);
			}
		}
	}

	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, 24 * nTiles * sizeof(float), &vertices[0], GL_STATIC_DRAW);
	posLocation = program.bindVertexAttribute("position", 2, 4*sizeof(float), 0);
	texCoordLocation = program.bindVertexAttribute("texCoord", 2, 4*sizeof(float), (void *)(2*sizeof(float)));
}

// Collision tests for axis aligned bounding boxes.
// Method collisionMoveDown also corrects Y coordinate if the box is
// already intersecting a tile below.

bool TileMap::collisionMoveLeft(const glm::ivec2 &pos, const glm::ivec2 &size, int *posX)
{
	int x, y0, y1;
	
	x = pos.x / tileSize;
	y0 = pos.y / tileSize;
	y1 = (pos.y + size.y - 1) / tileSize;
	if (pos.x < 0) {
		*posX = 0;
		return true;
	}
	for (int y = y0; y <= y1; y++)
	{
		if ((map[y * mapSize.x + x] > 0 && map[y * mapSize.x + x] <= 17) || (map[y * mapSize.x + x] > 22 && map[y * mapSize.x + x] <= 25)) {
			if (*posX - tileSize * (x + 1) > -11)
			{
				*posX = tileSize * (x + 1);
				return true;
			}
		}
	}
	
	return false;
}

bool TileMap::collisionMoveRight(const glm::ivec2& pos, const glm::ivec2& size, int *posX)
{
	int x, y0, y1;

	x = (pos.x + size.x - 1) / tileSize;
	y0 = pos.y / tileSize;
	y1 = (pos.y + size.y - 1) / tileSize;
	if ((pos.x + size.x - 1) >= mapSize.x * tileSize) {
		*posX = mapSize.x * tileSize - size.x;
		return true;
	}
	for(int y=y0; y<=y1; y++)
	{
		if ((map[y * mapSize.x + x] > 0 && map[y * mapSize.x + x] <= 17) || (map[y * mapSize.x + x] > 22 && map[y * mapSize.x + x] <= 25)) {
			if (*posX - tileSize * x + size.x <= 10)
			{
				*posX = tileSize * x - size.x;
				return true;
			}
		}
	}
	
	return false;
}

bool TileMap::collisionMoveUp(const glm::ivec2& pos, const glm::ivec2& size, int *posY)
{
	int x0, x1, y;

	x0 = pos.x / tileSize;
	x1 = (pos.x + size.x - 1) / tileSize;
	y = pos.y / tileSize;
	if (pos.y < 0) {
		*posY = 0;
		bLevelWin = true;
		return true;
	}
	for (int x = x0; x <= x1; x++)
	{
		if ((map[y * mapSize.x + x] > 0 && map[y * mapSize.x + x] <= 17) || (map[y * mapSize.x + x] > 22 && map[y * mapSize.x + x] <= 25)) {
			if (*posY - tileSize * (y+1) > -11)
			{
				*posY = tileSize * (y+1);
				return true;
			}
		}
	}

	return false;
}

bool TileMap::collisionMoveDown(const glm::ivec2& pos, const glm::ivec2& size, int* posY, const bool &bG)
{
	int x0, x1, y;

	x0 = pos.x / tileSize;
	x1 = (pos.x + size.x - 1) / tileSize;
	y = (pos.y + size.y - 1) / tileSize;
	if ((pos.y + size.y - 1) >= mapSize.y * tileSize) {
		*posY = mapSize.y * tileSize - size.y;
		bLevelLose = !bG;
		return true;
	}
	for (int x = x0; x <= x1; x++)
	{
		if ((map[y * mapSize.x + x] > 0 && map[y * mapSize.x + x] <= 17) || (map[y * mapSize.x + x] > 22 && map[y * mapSize.x + x] <= 25)) {
			if (*posY - tileSize * y + size.y <= 10)
			{
				*posY = tileSize * y - size.y;
				return true;
			}
		}
	}

	return false;
}

bool TileMap::collisionSpike(const glm::ivec2& pos, const glm::ivec2& size, const bool& bG) const
{
	// Cheat: (G)od mode
	if (bG) return false;


	int x0, x1, y0, y1;

	x0 = pos.x / tileSize;
	x1 = (pos.x + size.x - 1) / tileSize;
	y0 = pos.y / tileSize;
	y1 = (pos.y + size.y - 1) / tileSize;
	for (int x = x0; x <= x1; x++)
	{
		for (int y = y0; y <= y1; y++)
		{
			if (map[y * mapSize.x + x] + int('0') >= int('R') && map[y * mapSize.x + x] + int('0') < int('R') + 4)
			{
				return true;
			}
		}
	}

	return false;
}

bool TileMap::collisionBouncer(const glm::ivec2& pos, const glm::ivec2& size, const bool& bG) const
{
	int x0, x1, y0, y1;

	x0 = pos.x / tileSize;
	x1 = (pos.x + size.x - 1) / tileSize;
	y0 = pos.y / tileSize;
	y1 = (pos.y + size.y - 1) / tileSize;
	for (int x = x0; x <= x1; x++)
	{
		for (int y = y0; y <= y1; y++)
		{
			if (bouncermap[y * mapSize.x + x] + int('0') == 1)
			{
				return true;
			}
		}
	}

	return false;
}

bool TileMap::touchingWall(const glm::ivec2& pos, const glm::ivec2& size, const bool &bCheckRightFirst, bool *bTouchingRightFirst) const
{
	int x0, x1, y0, y1;

	x0 = pos.x / tileSize;
	x1 = (pos.x + size.x - 1) / tileSize;
	y0 = pos.y / tileSize;
	y1 = (pos.y + size.y - 1) / tileSize;
	bool bTouchingWallLeft = false, bTouchingWallRight = false;
	for (int y = y0; y <= y1; y++)
	{
		if (x0 > 0 && ((map[y * mapSize.x + x0 - 1] > 0 && map[y * mapSize.x + x0 - 1] <= 17) || (map[y * mapSize.x + x0 - 1] > 22 && map[y * mapSize.x + x0 - 1] <= 25)) && (pos.x % tileSize < 4)) {
			bTouchingWallLeft = true;
		}
		if ((x1 + 1 < mapSize.x) && ((map[y * mapSize.x + x1 + 1] > 0 && map[y * mapSize.x + x1 + 1] <= 17) || (map[y * mapSize.x + x1 + 1] > 22 && map[y * mapSize.x + x1 + 1] <= 25)) && ((pos.x + size.x - 1) % tileSize) >= tileSize - 4)
		{
			bTouchingWallRight = true;
		}
	}

	*bTouchingRightFirst = bTouchingWallRight && !(!bCheckRightFirst && bTouchingWallLeft);

	return bTouchingWallLeft || bTouchingWallRight;
}

bool TileMap::levelWin()
{
	return bLevelWin;
}

bool TileMap::levelLose()
{
	bool tmp = bLevelLose;
	bLevelLose = false;
	return tmp;
}






























