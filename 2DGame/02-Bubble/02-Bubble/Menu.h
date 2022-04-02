#ifndef _MENU_INCLUDE
#define _MENU_INCLUDE


#include <glm/glm.hpp>
#include "Texture.h"
#include "ShaderProgram.h"


// Main menu


class Menu
{

public:
	Menu();
	~Menu();

	void init(int screenWidth, int screenHeight);
	void render();

private:
	void initShaders();
	void prepareArrays(const glm::vec2& minCoords, ShaderProgram& program);

private:
	int screenWidth, screenHeight;

	ShaderProgram texProgram;
	glm::mat4 projection;
	Texture menuTexture;

	GLuint vao;
	GLuint vbo;
	GLint posLocation, texCoordLocation;
};


#endif // _MENU_INCLUDE

