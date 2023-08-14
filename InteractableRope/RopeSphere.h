#pragma once
#include "Sphere.h"

class RopeSphere : public Sphere
{
public:
	RopeSphere();

	RopeSphere(int sectorCount, int stackCount, float mass, float radius, glm::vec3 pos, float elasticity, bool locked);

	glm::vec3 prevPos;
	bool locked;

	~RopeSphere();
};

