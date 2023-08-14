#include "RopeSphere.h"

RopeSphere::RopeSphere() : Sphere()
{
}

RopeSphere::RopeSphere(int sectorCount, int stackCount, float mass, float radius, glm::vec3 pos, float elasticity, bool locked) : Sphere(sectorCount, stackCount, mass, radius, pos, elasticity)
{
	this->locked = locked;
}

RopeSphere::~RopeSphere()
{
}
