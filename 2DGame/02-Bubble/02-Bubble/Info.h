#ifndef _INFO_INCLUDE
#define _INFO_INCLUDE


#include <glm/glm.hpp>
#include "Texture.h"
#include "ShaderProgram.h"


// Info screen


class Info
{

public:
	Info();
	~Info();

	void init(int screenWidth, int screenHeight);
	void render();

private:
	void initShaders();
	void prepareArrays(const glm::vec2& minCoords, ShaderProgram& program);

private:
	int screenWidth, screenHeight;

	ShaderProgram texProgram;
	glm::mat4 projection;
	Texture infoTexture;

	GLuint vao;
	GLuint vbo;
	GLint posLocation, texCoordLocation;
};


#endif // _INFO_INCLUDE

