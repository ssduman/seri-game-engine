#include "Renderer.h"

Renderer::Renderer(float w, float h, float z) {
	float vertices[120] = {
		-h, +w, +z,
		 0.0f, 0.0f,
		+h, +w, +z,
		 1.0f, 0.0f,
		+h, +w, -z,
		 1.0f, 1.0f,
		-h, +w, -z,
		 0.0f, 1.0f,

		-h, +w, -z,
		 0.0f, 0.0f,
		+h, +w, -z,
		 1.0f, 0.0f,
		+h, -w, -z,
		 1.0f, 1.0f,
		-h, -w, -z,
		 0.0f, 1.0f,

		+h, +w, -z,
		 0.0f, 0.0f,
		+h, +w, +z,
		 1.0f, 0.0f,
		+h, -w, +z,
		 1.0f, 1.0f,
		+h, -w, -z,
		 0.0f, 1.0f,

		-h, +w, +z,
		 0.0f, 0.0f,
		-h, +w, -z,
		 1.0f, 0.0f,
		-h, -w, -z,
		 1.0f, 1.0f,
		-h, -w, +z,
		 0.0f, 1.0f,

		+h, +w, +z,
		 0.0f, 0.0f,
		-h, +w, +z,
		 1.0f, 0.0f,
		-h, -w, +z,
		 1.0f, 1.0f,
		+h, -w, +z,
		 0.0f, 1.0f,

		+h, -w, -z,
		 0.0f, 0.0f,
		-h, -w, -z,
		 1.0f, 0.0f,
		-h, -w, +z,
		 1.0f, 1.0f,
		+h, -w, +z,
		 0.0f, 1.0f
	};

	unsigned int vb, eb;

	glGenVertexArrays(1, &va);
	glGenBuffers(1, &vb);
	glGenBuffers(1, &eb);

	glBindVertexArray(va);
	glBindBuffer(GL_ARRAY_BUFFER, vb);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, eb);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void Renderer::init(unsigned int c, std::vector<glm::vec3> &coordinates) {
	count = c;

	glBindVertexArray(va);
	unsigned int vb2;
	glGenBuffers(1, &vb2);
	glBindBuffer(GL_ARRAY_BUFFER, vb2);
	glBufferData(GL_ARRAY_BUFFER, count * sizeof(std::vector<glm::vec3>), &coordinates[0], GL_STATIC_DRAW);
	glEnableVertexAttribArray(2);

	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)(sizeof(float) * 0));
	glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)(sizeof(float) * 3));
	glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)(sizeof(float) * 6));
	glVertexAttribPointer(5, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)(sizeof(float) * 9));

	glEnableVertexAttribArray(2);
	glEnableVertexAttribArray(3);
	glEnableVertexAttribArray(4);
	glEnableVertexAttribArray(5);

	glVertexAttribDivisor(2, 1);
	glVertexAttribDivisor(3, 1);
	glVertexAttribDivisor(4, 1);
	glVertexAttribDivisor(5, 1);

	glBindVertexArray(0);
}

void Renderer::render() {
	glBindVertexArray(va);
	glDrawElementsInstanced(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0, count);
	glBindVertexArray(0);
}
