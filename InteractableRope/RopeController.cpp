#include "RopeController.h"

RopeController::RopeController()
{
}

RopeController::RopeController(float gravity, int solveIterations, float rangeBetweenPoints, int pointCount)
{
	this->gravity = gravity;
	this->solveIterations = solveIterations;
	this->rangeBetweenPoints = rangeBetweenPoints;
	this->pointCount = pointCount;
}

void RopeController::Simulate(std::vector<RopeSphere*> &ropeSpheres, Ball ball, float deltaTime)
{
	for(int i = 0; i < ropeSpheres.size(); i++){
		if (!ropeSpheres[i]->locked) {
			glm::vec3 positionBeforeUpdate = ropeSpheres[i]->pos;
			glm::vec3 velocityDir = ropeSpheres[i]->pos - ropeSpheres[i]->prevPos;
			ropeSpheres[i]->velocity = velocityDir / deltaTime;
			ropeSpheres[i]->pos += velocityDir;
			ropeSpheres[i]->pos += glm::vec3(0, -1.0f, 0) * gravity * deltaTime * deltaTime * 6.0f;
			ropeSpheres[i]->prevPos = positionBeforeUpdate;
		}
	}

	for (int i = 0; i < solveIterations; i++) {
		for (int j = 0; j < ropeSpheres.size() - 1; j++) {
			if (j == 0)
				ropeSpheres[j]->pos = glm::vec3(0, 0, 0);

			CollisionControlling(ropeSpheres[j + 1], ball, deltaTime);

			glm::vec3 stickCentre = (ropeSpheres[j]->pos + ropeSpheres[j + 1]->pos) / 2.0f;
			glm::vec3 stickDir = glm::normalize((ropeSpheres[j]->pos - ropeSpheres[j + 1]->pos));
			float length = glm::distance((ropeSpheres[j]->pos - ropeSpheres[j + 1]->pos), glm::vec3(0, 0, 0));

			if (length > rangeBetweenPoints) {
				if (!ropeSpheres[j]->locked)
					ropeSpheres[j]->pos = stickCentre + stickDir * rangeBetweenPoints / 2.0f;
				if(!ropeSpheres[j + 1]->locked)
					ropeSpheres[j + 1]->pos = stickCentre - stickDir * rangeBetweenPoints / 2.0f;
			}
		}
	}
}

void RopeController::CollisionControlling(RopeSphere* &ropeSphere, Ball ball, float deltaTime)
{
	glm::vec3 diffCenters = ropeSphere->pos - ball.pos;
	float radiusSum = ropeSphere->radius + ball.radius;
	bool isCollision = glm::abs(glm::dot(diffCenters, diffCenters) <= radiusSum * radiusSum);

	if (isCollision) {
		glm::vec3 collisionNormal = ropeSphere->pos - ball.pos;
		float distanceSq = glm::distance(collisionNormal, glm::vec3(0, 0, 0));
		float penetration = radiusSum - distanceSq;

		ropeSphere->pos += 0.5f * penetration * collisionNormal;

		glm::vec3 relativeVelocity = ropeSphere->velocity - ball.velocity;
		float vDotN = glm::dot(relativeVelocity, collisionNormal);

		float modifiedVel = vDotN / (1.0f / ropeSphere->mass + 1.0f / ball.mass);
		float j1 = -(1.0f + ropeSphere->elasticity) * modifiedVel;
		ropeSphere->pos += (j1 / ropeSphere->mass * collisionNormal) * 0.5f * deltaTime;
	}
}

RopeController::~RopeController()
{

}
