#include "Chunk.hpp"
#include "Game.hpp"
#include "Texture.hpp"
#include "../src/CommonMinimal.hpp"
#include "LocationComponent.hpp"
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <algorithm>
#include <memory>
#include <stdlib.h>
#include <time.h>
#include "SimplexNoise.hpp"

void Game::init() {
	//Sahil: You can comment this out
	chunks = new Chunk[numChunks];

}

void Game::renderScene(glm::mat4 modelViewMatrix, glm::mat4 projectionMatrix) {
	//Move camera to the position of the player
	float xPos = 0.0f, yPos = -10.0f, zPos = -300.0f;
	glm::mat4 baseModelViewMatrix = glm::translate(modelViewMatrix, glm::vec3(xPos, yPos, zPos));

	//per cube
	for (int i = 0; i < numChunks; ++i) {
		if (!(chunks[i].isVisible() && chunks[i].isRenderable()))
			continue;

		std::shared_ptr<LocationComponent> locationComponent = chunks[i].getLocationComponent();
		glm::vec3 pos = locationComponent->getPosition();

		modelViewMatrix = glm::translate(baseModelViewMatrix, glm::vec3(pos.x, pos.y, pos.z));

		//Get the renderable component and bind in the shader
		std::shared_ptr<RenderableComponent> renderableComponent = chunks[i].getRenderableComponent();
		std::shared_ptr<Shader> objectShader = renderableComponent->getShader();

		if (!objectShader)
			continue;

		renderableComponent->bindShader();

		renderableComponent->setProjectionMatrix(projectionMatrix);
		renderableComponent->setModelviewMatrix(modelViewMatrix);

		//set uniforms
		glUniformMatrix4fv(glGetUniformLocation(objectShader->getProgram(), "matProjection"),
			1,
			GL_FALSE,
			glm::value_ptr(renderableComponent->getProjectionMatrix()));

		glUniformMatrix4fv(glGetUniformLocation(objectShader->getProgram(), "matModelView"),
			1,
			GL_FALSE,
			glm::value_ptr(renderableComponent->getModelviewMatrix()));

		//Bind VBOs
		renderableComponent->bindVBOs();

		//Bind textures
		renderableComponent->bindTextures();

		//If we need to use our index buffer
		if (renderableComponent->usesIndexedVBO()) {
			//TODO: Consider using unsigned short rather than unsigned short for memory conservation
			glDrawElements(
				GL_TRIANGLES,
				renderableComponent->getIndexDataSize(),
				GL_UNSIGNED_INT,
				nullptr);
		}
		else{
			//draw arrays
			glDrawArrays(GL_TRIANGLES, 0, renderableComponent->getNumVerticesRender());
		}

		//Release vertex, textures and shaders
		renderableComponent->releaseTextures();
		renderableComponent->releaseVBOs();
		renderableComponent->releaseShader();

	}
}