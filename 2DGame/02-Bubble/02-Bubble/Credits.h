#ifndef _CREDITS_INCLUDE
#define _CREDITS_INCLUDE


#include <glm/glm.hpp>
#include "Texture.h"
#include "ShaderProgram.h"


// Credits screen


class Credits
{

public:
	Credits();
	~Credits();

	void init(int screenWidth, int screenHeight);
	void render();

private:
	void initShaders();
	void prepareArrays(const glm::vec2& minCoords, ShaderProgram& program);

private:
	int screenWidth, screenHeight;

	ShaderProgram texProgram;
	glm::mat4 projection;
	Texture creditsTexture;

	GLuint vao;
	GLuint vbo;
	GLint posLocation, texCoordLocation;
};


#endif // _CREDITS_INCLUDE

