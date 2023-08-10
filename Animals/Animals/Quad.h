#pragma once
#include "Model.h"

class Quad : public Model {
public:
	Quad(int vao, int vertexCount, int shaderProgram);

	bool expired() const;
	virtual void Update(float dt) override;
	virtual void Draw(int textureID, int shaderProgram) override;

	static GLuint makeQuadVAO(int& vertexCount);
	

private:
	float age, opacity;

	const glm::vec3 SCALE_I = glm::vec3(0, 0, 0);
	const glm::vec3 SCALE_F = glm::vec3(2, 2, 2);
	const float OPACITY_I = 1;
	const float OPACITY_F = 0;
	const float LIFETIME = 5;  // in seconds

	static GLuint quadVAO;
	static GLuint particleTexture;
	static int textureWidth;

	static float linear(float t);
	static float quadratic(float t);
	static float root(float t);
	static float smooth(float t);
};
