#ifndef _SCENE_INCLUDE
#define _SCENE_INCLUDE


#include <glm/glm.hpp>
#include "ShaderProgram.h"
#include "TileMap.h"
#include "Player.h"


// Scene contains all the entities of our game.
// It is responsible for updating and render them.


class Scene
{

public:
	Scene();
	~Scene();

	void init();
	void update(int deltaTime);
	void render();

private:
	void initShaders();

private:
	const std::string levelFilename[11] = { string("levels/1.txt"), string("levels/2.txt"), string("levels/3.txt"), string("levels/4.txt"), string("levels/5.txt"), string("levels/6.txt"), string("levels/7.txt"), string("levels/8.txt"), string("levels/9.txt"), string("levels/10.txt"), string("levels/Summit.txt") };
	int level;
	TileMap *map;
	Player *player;
	ShaderProgram texProgram;
	float currentTime;
	glm::mat4 projection;

};


#endif // _SCENE_INCLUDE

