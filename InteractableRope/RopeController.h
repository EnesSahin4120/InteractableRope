#pragma once

#include "RopeSphere.h"
#include "Ball.h"

#include <vector>

#include <GL\glew.h>
#include <glm\glm.hpp>

class RopeController
{
public:
	RopeController();
	RopeController(float gravity, int solveIterations, float rangeBetweenPoints, int pointCount);

	float gravity;
	int solveIterations;
	float rangeBetweenPoints;
	int pointCount;

	void Simulate(std::vector<RopeSphere*> &ropeSpheres, Ball ball, float deltaTime);
	void CollisionControlling(RopeSphere* &ropeSphere, Ball ball, float deltaTime);

	~RopeController();
};

