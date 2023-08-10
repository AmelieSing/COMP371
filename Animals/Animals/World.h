#pragma once

#include <list>
#include <vector>

class Model;
class Quad;

class World {
public:
	World();
	~World();

	void Update(float dt);
	void Draw(int textureID, int shaderProgram);

private:
	std::list<Quad*> models;
	
	float gravity;
	float friction;

	float spawnTimer;

	const float SPAWN_PERIOD = 0.25;
};

float GetRandomFloat(float min, float max);

