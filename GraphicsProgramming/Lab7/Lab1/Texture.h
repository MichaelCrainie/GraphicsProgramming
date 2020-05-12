#pragma once
#include <string>
#include <GL\glew.h>
#include <vector>
#include <cassert>
#include <iostream>

class Texture
{
public:
	Texture();	

	void Bind(unsigned int unit); // bind upto 32 textures
	void init(const std::string& fileName);
	~Texture();	
	GLint getTexHandler() { return textureHandler; }
	GLuint textureHandler;	
	GLuint loadCubemap(std::vector<std::string> faces);


protected:
private:

	
};

