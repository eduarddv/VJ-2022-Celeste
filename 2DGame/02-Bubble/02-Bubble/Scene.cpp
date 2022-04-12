#include <iostream>
#include <cmath>
#include <fstream>
#include <sstream>
#include <glm/gtc/matrix_transform.hpp>
#include "Scene.h"
#include "Game.h"
#include "Bouncer.h"



#define SCREEN_X 16
#define SCREEN_Y 16

#define INIT_PLAYER_X_TILES 2
#define INIT_PLAYER_Y_TILES 12


Scene::Scene()
{
	map = NULL;
	player = NULL;
}

Scene::~Scene()
{
	if(map != NULL)
		delete map;
	if(player != NULL)
		delete player;
}


void Scene::init()
{
	initShaders();
	level = 0;
	map = TileMap::createTileMap(levelFilename[level], glm::vec2(SCREEN_X, SCREEN_Y), texProgram);
	bouncers = searchBouncers(levelFilename[level], glm::vec2(SCREEN_X, SCREEN_Y), texProgram);
	player = new Player();
	player->init(glm::ivec2(SCREEN_X, SCREEN_Y), texProgram);
	player->setTileMap(map);
	player->spawn();
	
	projection = glm::ortho(0.f, float(SCREEN_WIDTH - 1), float(SCREEN_HEIGHT - 1), 0.f);
	currentTime = 0.0f;
}

void Scene::update(int deltaTime)
{
	currentTime += deltaTime;
	player->update(deltaTime);

	list<Bouncer*> copy = bouncers;

	while (copy.empty() == false) {
		copy.front()->update(deltaTime);
		copy.pop_front();
	}

	if (map->levelLose()) {
		player->spawn();
	}
	
	bool bChangeLevel = false;
	if (map->levelWin()) {
		level++;
		bChangeLevel = true;
	}
	// Cheats - 1,...,9,0,p (Level Skip)
	for (char c = '1'; c <= '9'; c++) {
		if (Game::instance().getKeyBuffer(c)) {
			level = c - '1';
			bChangeLevel = true;
		}
	}
	if (Game::instance().getKeyBuffer('0')) {
		level = 9;
		bChangeLevel = true;
	}
	if (Game::instance().getKeyBuffer('p')) {
		level = 10;
		bChangeLevel = true;
	}

	if (bChangeLevel) {
		delete map;
		map = TileMap::createTileMap(levelFilename[level], glm::vec2(SCREEN_X, SCREEN_Y), texProgram);
		player->setTileMap(map);
		player->spawn();
	}
}

void Scene::render()
{
	glm::mat4 modelview;

	texProgram.use();
	texProgram.setUniformMatrix4f("projection", projection);
	texProgram.setUniform4f("color", 1.0f, 1.0f, 1.0f, 1.0f);
	modelview = glm::mat4(1.0f);
	texProgram.setUniformMatrix4f("modelview", modelview);
	texProgram.setUniform2f("texCoordDispl", 0.f, 0.f);
	map->render();
	player->render();
}

Player Scene::getPlayer()
{
	return Player();
}

list<Bouncer*> Scene::searchBouncers(const string& levelFile, const glm::vec2& minCoords, ShaderProgram& program)
{
	list<Bouncer*> Bouncers;
	Bouncer* l;

	ifstream fin;
	string line, tilesheetFile;
	stringstream sstream;
	char tile;

	glm::ivec2 playerInitTile, mapSize, tilesheetSize;
	int tileSize, blockSize;

	fin.open(levelFile.c_str());
	getline(fin, line);
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
	getline(fin, line);
	sstream.str(line);

	for (int j = 0; j < mapSize.y; j++)
	{
		for (int i = 0; i < mapSize.x; i++)
		{
			fin.get(tile);
			if (tile == 'E') {
				l = new Bouncer;
				l->init(glm::ivec2(minCoords.x, minCoords.y), program);
				l->setTileMap(map);
				l->spawn();
				Bouncers.push_back(l);
			}

		}
		fin.get(tile);
#ifndef _WIN32
		fin.get(tile);
#endif
	}
	fin.close();

	return Bouncers;
}

void Scene::initShaders()
{
	Shader vShader, fShader;

	vShader.initFromFile(VERTEX_SHADER, "shaders/texture.vert");
	if(!vShader.isCompiled())
	{
		cout << "Vertex Shader Error" << endl;
		cout << "" << vShader.log() << endl << endl;
	}
	fShader.initFromFile(FRAGMENT_SHADER, "shaders/texture.frag");
	if(!fShader.isCompiled())
	{
		cout << "Fragment Shader Error" << endl;
		cout << "" << fShader.log() << endl << endl;
	}
	texProgram.init();
	texProgram.addShader(vShader);
	texProgram.addShader(fShader);
	texProgram.link();
	if(!texProgram.isLinked())
	{
		cout << "Shader Linking Error" << endl;
		cout << "" << texProgram.log() << endl << endl;
	}
	texProgram.bindFragmentOutput("outColor");
	vShader.free();
	fShader.free();
}


