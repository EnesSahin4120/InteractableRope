#pragma once

#include <vector>

#include <GL\glew.h>
#include <glm\glm.hpp>

#include "Mesh.h"
#include "CommonValues.h"
class Sphere
{
public:
	Sphere();

	Sphere(int sectorCount, int stackCount, float mass, float radius, glm::vec3 pos, float elasticity);

	//Physics Properties
	glm::vec3 pos;
	float mass;
	float radius;
	glm::vec3 velocity;
	float elasticity;

	GLfloat* vertices;
	unsigned int* indices;
	std::vector<GLfloat> verticesList;
	std::vector<unsigned int> indicesList;

	Mesh* mesh;

	~Sphere();

protected:
	//Mesh Parameters
	float x, y, z, xy;
	float nx, ny, nz, lengthInv;
	float s, t;

	int k1, k2;

	float sectorStep;
	float stackStep;
	float sectorAngle, stackAngle;
};

