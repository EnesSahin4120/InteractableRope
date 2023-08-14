#define STB_IMAGE_IMPLEMENTATION

#include <stdio.h>
#include <string.h>
#include <cmath>
#include <vector>

#include <GL\glew.h>
#include <GLFW\glfw3.h>

#include <glm\glm.hpp>
#include <glm\gtc\matrix_transform.hpp>
#include <glm\gtc\type_ptr.hpp>

#include "CommonValues.h"

#include "Window.h"
#include "Mesh.h"
#include "Shader.h"
#include "Camera.h"
#include "Texture.h"
#include "DirectionalLight.h"
#include "SpotLight.h"
#include "Material.h"
#include "RopeSphere.h"
#include "Ball.h"
#include "RopeController.h"
#include "Line.h"

#include "Skybox.h"

GLuint uniformProjection = 0, uniformModel = 0, uniformView = 0, uniformEyePosition = 0,
uniformSpecularIntensity = 0, uniformShininess = 0,
uniformDirectionalLightTransform = 0, uniformOmniLightPos = 0, uniformFarPlane = 0;

Window mainWindow;
std::vector<Mesh*> environmentMeshes;

Shader gameShader;
Shader lineShader;
Shader directionalShadowShader;
Shader omniShadowShader;

Camera camera;

Texture basketballTexture;
Texture footballTexture;
Texture groundTexture;
Texture hangerTexture;

Material gameMaterial;

DirectionalLight mainLight;
SpotLight spotLights[MAX_SPOT_LIGHTS];

Skybox skybox;

std::vector<Line*> lines;
std::vector<RopeSphere*> ropeSpheres;
RopeController ropeController = RopeController(10, 5, 1, 12);
Ball ball;

unsigned int spotLightCount = 0;
unsigned int pointLightCount = 0;

GLfloat deltaTime = 0.0f;
GLfloat lastTime = 0.0f;

// Vertex Shader
static const char* vertexShader = "Shaders/shader.vert";

// Fragment Shader
static const char* fragmentShader = "Shaders/shader.frag";

// Object Vertex Shader 
static const char* lineVertexShader = "Shaders/line.vert";

// Object Fragment Shader
static const char* lineFragmentShader = "Shaders/line.frag";

void CreateObjects()
{
	for (int i = 0; i < ropeController.pointCount; i++) 
	{
		bool locked = i == 0 ? true : false;
		RopeSphere *ropeSphere = new RopeSphere(20, 20, 0.5f, 0.3f, glm::vec3(0, - ropeController.rangeBetweenPoints * i, 0), 0.5f, locked);
		ropeSpheres.push_back(ropeSphere);
	}

	unsigned int planeShapeIndices[] = {
		0, 2, 1,
		1, 2, 3
	};

	GLfloat groundVertices[] = {
		-10.0f, 0.0f, -10.0f,	0.0f, 0.0f,		0.0f, -1.0f, 0.0f,
		10.0f, 0.0f, -10.0f,	1.0f, 0.0f,	    0.0f, -1.0f, 0.0f,
		-10.0f, 0.0f, 10.0f,	0.0f, 1.0f,	    0.0f, -1.0f, 0.0f,
		10.0f, 0.0f, 10.0f,		1.0f, 1.0f,	    0.0f, -1.0f, 0.0f
	};

	GLfloat hangerVertices[] = {
		-1.0f, 0.0f, -1.0f,	    0.0f, 0.0f,		    0.0f, -1.0f, 0.0f,
		1.0f, 0.0f, -1.0f,	    0.125f, 0.0f,	    0.0f, -1.0f, 0.0f,
		-1.0f, 0.0f, 1.0f,	    0.0f, 0.125f,	    0.0f, -1.0f, 0.0f,
		1.0f, 0.0f, 1.0f,	    0.125f, 0.125f,	    0.0f, -1.0f, 0.0f
	};

	Mesh *groundMesh = new Mesh();
	groundMesh->CreateMesh(groundVertices, planeShapeIndices, 32, 6);
	environmentMeshes.push_back(groundMesh);

	Mesh *hangerMesh = new Mesh();
	hangerMesh->CreateMesh(hangerVertices, planeShapeIndices, 32, 6);
	environmentMeshes.push_back(hangerMesh);
}

void UpdateLines(std::vector<Line*> &lines) {
	lines.clear();

	for (int i = 0; i < ropeSpheres.size() - 1; i++) {
		Line *currentLine = new Line();
		GLfloat lineVertices[] = {
			ropeSpheres[i]->pos.x, ropeSpheres[i]->pos.y, ropeSpheres[i]->pos.z,
			ropeSpheres[i + 1]->pos.x, ropeSpheres[i + 1]->pos.y, ropeSpheres[i + 1]->pos.z
		};
		currentLine->CreateLine(lineVertices, 6);
		lines.push_back(currentLine);
	}
}

void CreateShaders()
{
	gameShader = Shader();
	gameShader.CreateFromFiles(vertexShader, fragmentShader);
	
	lineShader = Shader();
	lineShader.CreateFromFiles(lineVertexShader, lineFragmentShader);

	directionalShadowShader.CreateFromFiles("Shaders/directional_shadow_map.vert", "Shaders/directional_shadow_map.frag");
	omniShadowShader.CreateFromFiles("Shaders/omni_shadow_map.vert", "Shaders/omni_shadow_map.geom", "Shaders/omni_shadow_map.frag");
}

void RenderScene()
{
	glm::mat4 model(1.0f);

	model = glm::mat4(1.0f);
	model = glm::translate(model, glm::vec3(0.0f, -15.0f, 0.0f));
	model = glm::scale(model, glm::vec3(3.5f, 1.0f, 3.5f));
	glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
	groundTexture.UseTexture();
	gameMaterial.UseMaterial(uniformSpecularIntensity, uniformShininess);
	environmentMeshes[0]->RenderMesh();

	model = glm::mat4(1.0f);
	model = glm::translate(model, glm::vec3(0.0f, 0.3f, 0.0f));
	glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
	hangerTexture.UseTexture();
	gameMaterial.UseMaterial(uniformSpecularIntensity, uniformShininess);
	environmentMeshes[1]->RenderMesh();

	model = glm::mat4(1.0f);
	model = glm::translate(model, ball.pos);
	glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
	basketballTexture.UseTexture();
	gameMaterial.UseMaterial(uniformSpecularIntensity, uniformShininess);
	ball.mesh->RenderMesh();

	for (int i = 0; i < ropeSpheres.size(); i++) {
		model = glm::mat4(1.0f);
		model = glm::translate(model, ropeSpheres[i]->pos);
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		footballTexture.UseTexture();
		gameMaterial.UseMaterial(uniformSpecularIntensity, uniformShininess);
		ropeSpheres[i]->mesh->RenderMesh();
	}
}

void DirectionalShadowMapPass(DirectionalLight* light)
{
	directionalShadowShader.UseShader();

	glViewport(0, 0, light->getShadowMap()->GetShadowWidth(), light->getShadowMap()->GetShadowHeight());

	light->getShadowMap()->Write();
	glClear(GL_DEPTH_BUFFER_BIT);

	uniformModel = directionalShadowShader.GetModelLocation();
	directionalShadowShader.SetDirectionalLightTransform(&light->CalculateLightTransform());

	directionalShadowShader.Validate();

	RenderScene();

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void OmniShadowMapPass(PointLight* light)
{
	omniShadowShader.UseShader();

	glViewport(0, 0, light->getShadowMap()->GetShadowWidth(), light->getShadowMap()->GetShadowHeight());

	light->getShadowMap()->Write();
	glClear(GL_DEPTH_BUFFER_BIT);

	uniformModel = omniShadowShader.GetModelLocation();
	uniformOmniLightPos = omniShadowShader.GetOmniLightPosLocation();
	uniformFarPlane = omniShadowShader.GetFarPlaneLocation();

	glUniform3f(uniformOmniLightPos, light->GetPosition().x, light->GetPosition().y, light->GetPosition().z);
	glUniform1f(uniformFarPlane, light->GetFarPlane());
	omniShadowShader.SetLightMatrices(light->CalculateLightTransform());

	omniShadowShader.Validate();

	RenderScene();

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void RenderPass(glm::mat4 viewMatrix, glm::mat4 projectionMatrix)
{
	glViewport(0, 0, 1366, 768);

	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	skybox.DrawSkybox(viewMatrix, projectionMatrix);

	gameShader.UseShader();

	uniformModel = gameShader.GetModelLocation();
	uniformProjection = gameShader.GetProjectionLocation();
	uniformView = gameShader.GetViewLocation();
	uniformModel = gameShader.GetModelLocation();
	uniformEyePosition = gameShader.GetEyePositionLocation();
	uniformSpecularIntensity = gameShader.GetSpecularIntensityLocation();
	uniformShininess = gameShader.GetShininessLocation();

	glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projectionMatrix));
	glUniformMatrix4fv(uniformView, 1, GL_FALSE, glm::value_ptr(viewMatrix));
	glUniform3f(uniformEyePosition, camera.getCameraPosition().x, camera.getCameraPosition().y, camera.getCameraPosition().z);

	gameShader.SetDirectionalLight(&mainLight);
	gameShader.SetSpotLights(spotLights, spotLightCount, 3 + pointLightCount, pointLightCount);
	gameShader.SetDirectionalLightTransform(&mainLight.CalculateLightTransform());

	mainLight.getShadowMap()->Read(GL_TEXTURE2);
	gameShader.SetTexture(1);
	gameShader.SetDirectionalShadowMap(2);

	glm::vec3 lowerLight = camera.getCameraPosition();
	lowerLight.y -= 0.3f;
	spotLights[0].SetFlash(lowerLight, camera.getCameraDirection());

	gameShader.Validate();

	RenderScene();

	uniformModel = lineShader.GetModelLocation();
	uniformProjection = lineShader.GetProjectionLocation();
	uniformView = lineShader.GetViewLocation();
	lineShader.UseShader();

	glm::mat4 model(1.0f);

	model = glm::mat4(1.0f);
	lineShader.UseShader();
	glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
	glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projectionMatrix));
	glUniformMatrix4fv(uniformView, 1, GL_FALSE, glm::value_ptr(viewMatrix));

	for (int i = 0; i < lines.size(); i++)
		lines[i]->RenderLine();
}

int main()
{
	mainWindow = Window(1366, 768); 
	mainWindow.Initialise();

	CreateObjects();
	CreateShaders();

	camera = Camera(glm::vec3(-7.0f, 2.0f, 14.0f), glm::vec3(0.0f, 1.0f, 0.0f), -60.0f, 0.0f, 5.0f, 0.5f);

	basketballTexture = Texture("Textures/basketballTexture.jpg");
	basketballTexture.LoadTexture();
	footballTexture = Texture("Textures/footballTexture.jpg");
	footballTexture.LoadTexture();
	groundTexture = Texture("Textures/woodTexture.jpg");
	groundTexture.LoadTexture();
	hangerTexture = Texture("Textures/hangerTexture.jpg");
	hangerTexture.LoadTexture();

	gameMaterial = Material(0.3f, 4);

	mainLight = DirectionalLight(2048, 2048,
		1.0f, 1.0f, 1.0f,
		0.1f, 0.9f,
		10.0f, -12.0f, -18.5f);

	spotLights[0] = SpotLight(1024, 1024,
		0.01f, 100.0f,
		1.0f, 1.0f, 1.0f,
		0.5f, 0.8f,
		0.0f, 0.0f, 0.0f,
		0.0f, -1.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		35.0f);
	spotLightCount++;

	std::vector<std::string> skyboxFaces;
	skyboxFaces.push_back("Textures/Skybox/DesertRHDrt.jpg");
	skyboxFaces.push_back("Textures/Skybox/DesertRHDlf.jpg");
	skyboxFaces.push_back("Textures/Skybox/DesertRHDup.jpg");
	skyboxFaces.push_back("Textures/Skybox/DesertRHDdn.jpg");
	skyboxFaces.push_back("Textures/Skybox/DesertRHDbk.jpg");
	skyboxFaces.push_back("Textures/Skybox/DesertRHDft.jpg");

	skybox = Skybox(skyboxFaces);
	ball = Ball(20, 20, 5, 2, glm::vec3(180, -6.5f, -0.5f), 0.5f);
	ball.AddForce(glm::vec3(-6000.0f, 0, 0), 0.02f);

	GLuint uniformProjection = 0, uniformModel = 0, uniformView = 0, uniformEyePosition = 0,
		uniformSpecularIntensity = 0, uniformShininess = 0;
	glm::mat4 projection = glm::perspective(glm::radians(60.0f), (GLfloat)mainWindow.getBufferWidth() / mainWindow.getBufferHeight(), 0.1f, 100.0f);

	while (!mainWindow.getShouldClose())
	{
		GLfloat now = glfwGetTime();
		deltaTime = now - lastTime; 
		lastTime = now;

		glfwPollEvents();

		camera.keyControl(mainWindow.getsKeys(), deltaTime);
		camera.mouseControl(mainWindow.getXChange() * deltaTime, mainWindow.getYChange() * deltaTime);
		
		ropeController.Simulate(ropeSpheres, ball, deltaTime);
		ball.Update(deltaTime);
		UpdateLines(lines);

		DirectionalShadowMapPass(&mainLight);
		for (size_t i = 0; i < spotLightCount; i++)
		{
			OmniShadowMapPass(&spotLights[i]);
		}
		RenderPass(camera.calculateViewMatrix(), projection);

		mainWindow.swapBuffers();
	}

	return 0;
}