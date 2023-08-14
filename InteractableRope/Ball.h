#pragma once
#include "Sphere.h"

class Ball :
	public Sphere
{
public:
	Ball();

	Ball(int sectorCount, int stackCount, float mass, float radius, glm::vec3 pos, float elasticity);

	void Update(float deltaTime);
	void AddForce(glm::vec3 forceVector, float deltaTime);
	void SetVelocity(float deltaTime);

	~Ball();
};

