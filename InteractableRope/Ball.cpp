#include "Ball.h"

Ball::Ball() : Sphere()
{
}

Ball::Ball(int sectorCount, int stackCount, float mass, float radius, glm::vec3 pos, float elasticity) : Sphere(sectorCount, stackCount, mass, radius, pos, elasticity)
{
}

void Ball::Update(float deltaTime)
{
	SetVelocity(deltaTime);
}

void Ball::AddForce(glm::vec3 forceVector, float deltaTime)
{
	glm::vec3 accelerationVector = forceVector / mass;
	velocity += accelerationVector * deltaTime;
}

void Ball::SetVelocity(float deltaTime)
{
	pos += velocity * deltaTime;
}

Ball::~Ball()
{
}
