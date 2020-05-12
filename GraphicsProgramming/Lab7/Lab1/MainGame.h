#pragma once
#include <SDL\SDL.h>
#include <GL/glew.h>
#include "Display.h" 
#include "Shader.h"
#include "ShaderGeometry.h"
#include "Mesh.h"
#include "Texture.h"
#include "transform.h"
#include "Audio.h"
#include "Overlay.h"

enum class GameState{PLAY, EXIT};

class MainGame
{
public:
	MainGame();
	~MainGame();

	void run();

private:

	void initSystems();
	void processInput();
	void gameLoop();
	void drawGame();
	void playAudio(unsigned int Source, glm::vec3 pos);
	void blobEffect();
	void setADSLighting();
	void setToonLighting();
	void setRimShader();
	void Skybox();
	void linkGeoShader();
	void setCustomShader();
	void setReflectionShader();
	bool collision(glm::vec3 m1Pos, float m1Rad, glm::vec3 m2Pos, float m2Rad);

	GLuint skyboxVAO, skyboxVBO, cubemapTexture;
	vector<std::string> faces;
	Texture skybox;
	Shader shaderSkybox;

	Display _gameDisplay;
	GameState _gameState;
	Mesh mesh1;
	Mesh mesh2;
	Mesh mesh3;
	Mesh mesh4;
	Mesh mesh5;
	Texture texture;
	Texture texture1;
	Overlay overlay;
	Shader shaderPass;
	Shader shaderBlur;
	Shader shaderToon;
	Shader shaderRim;
	Shader shaderCustom;
	Shader shaderGeo;
	Shader shaderReflective;

	glm::mat4 modelView;

	Camera myCamera;
	Audio audioDevice;

	float counter;
	unsigned int whistle;
	unsigned int backGroundMusic;
};

