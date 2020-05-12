#include "MainGame.h"
#include "Camera.h"
#include <iostream>
#include <string>


Transform transform;

MainGame::MainGame()
{
	_gameState = GameState::PLAY;
	Display* _gameDisplay = new Display(); //new display
}

MainGame::~MainGame() 
{
}

void MainGame::run()
{
	initSystems(); 
	gameLoop();
}

void MainGame::initSystems()
{
	_gameDisplay.initDisplay(); 
	whistle = audioDevice.loadSound("..\\res\\bang.wav");
	backGroundMusic = audioDevice.loadSound("..\\res\\background.wav");
	texture.init("..\\res\\bricks.jpg"); 
	texture1.init("..\\res\\water.jpg"); 

	shaderPass.init("..\\res\\shaderRim.vert","..\\res\\shaderRim.frag");
	shaderBlur.init("..\\res\\shaderBlur.vert", "..\\res\\shaderBlur.frag");
	shaderToon.init("..\\res\\shaderToon.vert", "..\\res\\shaderToon.frag");
	shaderRim.init("..\\res\\shaderRim.vert", "..\\res\\shaderRim.frag");
	shaderSkybox.init("..\\res\\shaderSkyBox.vert", "..\\res\\shaderSkyBox.frag");
	shaderGeo.initGeo("..\\res\\geoShader.vert", "..\\res\\geoShader.geom", "..\\res\\geoShader.frag");
	shaderCustom.init("..\\res\\toonFogCombine.vert", "..\\res\\toonFogCombine.frag");
	shaderReflective.init("..\\res\\shaderReflection.vert", "..\\res\\shaderReflection.frag");
	

	vector<std::string>faces
	{
		"..\\res\\skybox\\right.jpg",
		"..\\res\\skybox\\left.jpg",
		"..\\res\\skybox\\top.jpg",
		"..\\res\\skybox\\bottom.jpg",
		"..\\res\\skybox\\front.jpg",
		"..\\res\\skybox\\back.jpg"
	};

	cubemapTexture = skybox.loadCubemap(faces); //Load the cubemap using "faces" into cubemapTextures

	float skyboxVertices[] = {
		// positions          
		-6.0f,  6.0f, -6.0f,
		-6.0f, -6.0f, -6.0f,
		6.0f, -6.0f, -6.0f,
		6.0f, -6.0f, -6.0f,
		6.0f,  6.0f, -6.0f,
		-6.0f,  6.0f, -6.0f,

		-6.0f, -6.0f,  6.0f,
		-6.0f, -6.0f, -6.0f,
		-6.0f,  6.0f, -6.0f,
		-6.0f,  6.0f, -6.0f,
		-6.0f,  6.0f,  6.0f,
		-6.0f, -6.0f,  6.0f,

		6.0f, -6.0f, -6.0f,
		6.0f, -6.0f,  6.0f,
		6.0f,  6.0f,  6.0f,
		6.0f,  6.0f,  6.0f,
		6.0f,  6.0f, -6.0f,
		6.0f, -6.0f, -6.0f,

		-6.0f, -6.0f,  6.0f,
		-6.0f,  6.0f,  6.0f,
		6.0f,  6.0f,  6.0f,
		6.0f,  6.0f,  6.0f,
		6.0f, -6.0f,  6.0f,
		-6.0f, -6.0f,  6.0f,

		-6.0f,  6.0f, -6.0f,
		6.0f,  6.0f, -6.0f,
		6.0f,  6.0f,  6.0f,
		6.0f,  6.0f,  6.0f,
		-6.0f,  6.0f,  6.0f,
		-6.0f,  6.0f, -6.0f,

		-6.0f, -6.0f, -6.0f,
		-6.0f, -6.0f,  6.0f,
		6.0f, -6.0f, -6.0f,
		6.0f, -6.0f, -6.0f,
		-6.0f, -6.0f,  6.0f,
		6.0f, -6.0f,  6.0f
	};

	//use openGL functionality to generate & bind data into buffers
	glGenVertexArrays(1, &skyboxVAO);
	glGenBuffers(1, &skyboxVBO);
	glBindVertexArray(skyboxVAO);
	glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);


	overlay.init("..\\res\\brick.jpg");

	mesh1.loadModel("..\\res\\Skull.obj");
	mesh2.loadModel("..\\res\\chicken.obj");
	mesh3.loadModel("..\\res\\newGun.obj");
	mesh4.loadModel("..\\res\\Urn.obj");
	mesh5.loadModel("..\\res\\monkey3.obj");

	
	myCamera.initCamera(glm::vec3(0, 0, -10.0), 70.0f, (float)_gameDisplay.getWidth()/_gameDisplay.getHeight(), 0.01f, 1000.0f);

	counter = 1.0f;
}



void MainGame::gameLoop()
{
	while (_gameState != GameState::EXIT)
	{
		processInput();
		drawGame();
		collision(mesh1.getSpherePos(), mesh1.getSphereRadius(), mesh2.getSpherePos(), mesh2.getSphereRadius());
		playAudio(backGroundMusic, glm::vec3(0.0f,0.0f,0.0f));
	}
}

void MainGame::processInput()
{
	SDL_Event evnt;

	while(SDL_PollEvent(&evnt)) //get and process events
	{
		switch (evnt.type)
		{
			case SDL_QUIT:
				_gameState = GameState::EXIT;
				break;
		}
	}
	
}

void MainGame::playAudio(unsigned int Source, glm::vec3 pos)
{
	
	ALint state; 
	alGetSourcei(Source, AL_SOURCE_STATE, &state);

	
	if (AL_PLAYING != state)
	{
		//audioDevice.playSound(Source, pos);
	}
}

void MainGame::setADSLighting()
{
	modelView = transform.GetModel() * myCamera.GetView();
	
	shaderPass.setMat4("ModelViewMatrix", modelView);
	shaderPass.setMat4("ProjectionMatrix", myCamera.GetProjection()); 
	
	glm::mat4 normalMatrix = transpose(inverse(modelView));
	
	shaderPass.setMat4("NormalMatrix", normalMatrix);

	shaderPass.setVec4("Position", glm::vec4(10.0,10.0,10.0,1.0));
	shaderPass.setVec3("Intensity", glm::vec3(0.0, 0.0, 0.0));

	shaderPass.setVec3("ka", glm::vec3(0.5, 0.5, 0.5));
	shaderPass.setVec3("kd", glm::vec3(0.5, 0.5, 0.5));
	shaderPass.setVec3("ks", glm::vec3(0.5, 0.5, 0.5));

	shaderPass.setFloat("Shininess", 0.5);
}

void MainGame::setToonLighting()
{
	shaderToon.setVec3("lightDir", glm::vec3(0.5, 0.5, 0.5));
}

void MainGame::setCustomShader()
{
	//shaderCustom.setMat4("u_vm", myCamera.GetView());
	//shaderCustom.setMat4("u_pm", myCamera.GetViewProjection());
	//shaderCustom.setVec3("fogColor", glm::vec3(0.5f, 0.0f, 0.0f));
	shaderCustom.setFloat("minDist", 0.0f);
	shaderCustom.setFloat("maxDist", 20.0f);
	shaderCustom.setVec3("lightDir", glm::vec3(0.5f, 0.5f, 0.5f));
	//shaderCustom.setVec3("IVD", myCamera.getPos() - myCamera.GetForward()); //Rim Lighting
	//shaderCustom.setVec3("normal", 1.0f, 0.0f, 0.0f);
	shaderCustom.setVec3("fogColor", glm::vec3(1, 0.8, 0.0));
	//shaderCustom.setVec3("CoolColor", glm::vec3(0, 0, 0.6));
	//shaderCustom.setFloat("DiffuseCool", 0.45f);
	//shaderCustom.setFloat("DiffuseWarm", 0.45f);
	//shaderCustom.setVec3("LightPosition", glm::vec3(0, 10, 4));
	//shaderCustom.setVec3("SurfaceColor", glm::vec3(1, 0.75, 0.75));
	//shaderCustom.setVec3("WarmColor", glm::vec3(0.6, 0.6, 0));
	//shaderCustom.setVec4("InnerColor", glm::vec4(0.2f, 0.5f, 0.75f, 1));
	//shaderCustom.setVec4("OuterColor", glm::vec4(0.5f, 0.8f, 0.35f, 1));
	//shaderCustom.setFloat("RadiusInner", 0.54f);
	//shaderCustom.setFloat("RadiusOuter", 0.67f);
	//shaderCustom.setMat4("Proj", myCamera.GetProjection());
	//shaderCustom.setMat3("View", myCamera.GetView());
	//shaderCustom.setVec3("lightDirection", glm::vec3(0.3f, 0.7f, 0.5f));
	//shaderCustom.setMat4("u_MVPMatrix", myCamera.GetViewProjection());
	//shaderCustom.setMat4("u_MVMatrix", myCamera.GetView());
	//shaderCustom.setVec3("u_LightPos", glm::vec3(0.5f, 0.5f, 0.5f));

}

void MainGame::setReflectionShader()
{
	shaderReflective.setVec3("cameraPos", myCamera.getPos());
	//shaderReflective.setMat4("view", myCamera.GetView());
	//shaderReflective.setMat4("projection", myCamera.GetProjection());
	shaderReflective.setMat4("model", modelView);
	
}


void MainGame::setRimShader()
{
	shaderRim.setMat4("u_vm", myCamera.GetView());
	shaderRim.setMat4("u_pm", myCamera.GetProjection());
}


void MainGame::blobEffect()
{
	GLuint blockIndex = glGetUniformBlockIndex(shaderBlur.getProgram(), "BlobSettings");

	GLint blockSize;
	glGetActiveUniformBlockiv(shaderBlur.getProgram(), blockIndex,
		GL_UNIFORM_BLOCK_DATA_SIZE, &blockSize); //get information about blobsettings and save it in blockSize

	GLubyte * blockBuffer = (GLubyte *)malloc(blockSize); //allocates the requested memory and returns a pointer to it.

														  // Query for the offsets of each block variable
	const GLchar *names[] = { "InnerColor", "OuterColor",
		"RadiusInner", "RadiusOuter" };

	GLuint indices[4];
	glGetUniformIndices(shaderBlur.getProgram(), 4, names, indices); // glGetUniformIndices retrieves the indices of a number of uniforms within program

	GLint offset[4];
	glGetActiveUniformsiv(shaderBlur.getProgram(), 4, indices, GL_UNIFORM_OFFSET, offset); //Returns information about several active uniform variables for the specified program object

	GLfloat outerColor[] = { 0.0f, 0.0f, 0.0f, 0.0f };
	GLfloat innerColor[] = { 1.0f, 1.0f, 0.75f, 1.0f };

	GLfloat innerRadius = 0.0f, outerRadius = 3.0f;

	memcpy(blockBuffer + offset[0], innerColor,
		4 * sizeof(GLfloat)); //destination, source, no of bytes. 
	memcpy(blockBuffer + offset[1], outerColor,
		4 * sizeof(GLfloat));
	memcpy(blockBuffer + offset[2], &innerRadius,
		sizeof(GLfloat));
	memcpy(blockBuffer + offset[3], &outerRadius,
		sizeof(GLfloat));

	GLuint uboHandle;

	glGenBuffers(1, &uboHandle);

	glBindBuffer(GL_UNIFORM_BUFFER, uboHandle);
	glBufferData(GL_UNIFORM_BUFFER, blockSize, blockBuffer,
		GL_DYNAMIC_DRAW); //creates and initializes a buffer object's data store - targer, size, data, usage

	glBindBufferBase(GL_UNIFORM_BUFFER, blockIndex, uboHandle); // bind a buffer object to an indexed buffer target - trager, index, buffer
}

void MainGame::Skybox()
{
	glDepthFunc(GL_LEQUAL);  // change depth function so depth test passes when values are equal to depth buffer's content
	shaderSkybox.Bind();
	shaderSkybox.setInt("skybox", 0);
	//view = glm::mat4(glm::mat3(camera.GetViewMatrix())); // remove translation from the view matrix
	shaderSkybox.setMat4("view", myCamera.GetView());
	shaderSkybox.setMat4("projection", myCamera.GetProjection());
	// skybox cube
	glBindVertexArray(skyboxVAO);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
	glDrawArrays(GL_TRIANGLES, 0, 36);
	glBindVertexArray(0);
	glDepthFunc(GL_LESS); // set depth function back to default
}

bool MainGame::collision(glm::vec3 m1Pos, float m1Rad, glm::vec3 m2Pos, float m2Rad)
{
	float distance = glm::sqrt((m2Pos.x - m1Pos.x) * (m2Pos.x - m1Pos.x) + (m2Pos.y - m1Pos.y) * (m2Pos.y - m1Pos.y) + (m2Pos.z - m1Pos.z) * (m2Pos.z - m1Pos.z));

	if (distance < (m1Rad + m2Rad))
	{
		//audioDevice.setlistener(myCamera.getPos(), m1Pos); //add bool to mesh
		//playAudio(whistle, m1Pos);
		return true;
	}
	else
	{
		return false;
	}
}

void MainGame::linkGeoShader()
{
	//shader.setMat4("u_vm", myCamera.GetView());
	//shader.setMat4("u_pm", myCamera.GetViewProjection());
	//shader.setVec3("fogColor", glm::vec3(0.0f, 0.0f, 0.0f));
	//shader.setFloat("minDist", 0.0f);
	//shader.setFloat("maxDist", 20.0f);
	//shader.setVec3("lightDir", glm::vec3(0.0f, 1.0f, 0.0f));
	//shader.setVec3("IVD", myCamera.getPos()- myCamera.GetForward()); //Rim Lighting
	//shader.setVec3("normal", 1.0f, 0.0f, 0.0f);
	shaderGeo.setFloat("time", counter);
}

void MainGame::drawGame()
{
	_gameDisplay.clearDisplay(0.0f, 0.0f, 0.0f, 1.0f);
	//setCustomShader();
	//setReflectionShader();
	linkGeoShader();

	Skybox();	

	transform.SetPos(glm::vec3(sinf(counter), 0.5, 0.0));
	transform.SetRot(glm::vec3(0.0, counter * 5, 0.0));
	transform.SetScale(glm::vec3(2.8, 2.8, 2.8));
	
	shaderRim.Bind();
	//setReflectionShader();
	setRimShader();
	shaderRim.Update(transform, myCamera);	
	mesh1.draw();	

	transform.SetPos(glm::vec3(-sinf(counter), -1.0, -sinf(counter)*5));
	transform.SetRot(glm::vec3(0.0, counter * 5, 0.0));
	transform.SetScale(glm::vec3(3.0, 3.0, 3.0));
	
	shaderToon.Bind();
	setToonLighting();
	//setReflectionShader();
	shaderToon.Update(transform, myCamera);
	mesh2.draw();

	transform.SetPos(glm::vec3(1.5, 0, sinf(counter) * 7.5));
	transform.SetRot(glm::vec3(0.0, counter * 5, 0.0));
	transform.SetScale(glm::vec3(1.0, 1.0, 1.0));

	shaderCustom.Bind();
	//setToonLighting();
    setCustomShader();
	shaderCustom.Update(transform, myCamera);
	mesh4.draw();

	transform.SetPos(glm::vec3(8, -3, 7));
	transform.SetRot(glm::vec3(0, 10, 0));
	transform.SetScale(glm::vec3(1.0, 1.0, 1.0));

	shaderReflective.Bind();
	setReflectionShader();
	shaderReflective.Update(transform, myCamera);
	mesh5.draw();

	transform.SetPos(glm::vec3(-sinf(counter), -sinf(counter), -sinf(counter)));
	transform.SetRot(glm::vec3(0.0, counter * 5, 0.0));
	transform.SetScale(glm::vec3(0.2, 0.2, 0.2));
	
	
	shaderGeo.Bind();		
	shaderGeo.Update(transform, myCamera);	
	texture1.Bind(0);
	mesh3.draw();
	mesh3.updateSphereData(*transform.GetPos(), 0.62f);	  

	

	counter = counter + 0.01f;

	glEnableClientState(GL_COLOR_ARRAY); 
	glEnd();

	_gameDisplay.swapBuffer();
} 