#pragma once

#include "seri/renderer/AuxiliaryStructs.h"

#include <glad/gl.h>

#include <glm/glm.hpp>

#include <vector>
#include <memory>

class Graphic;

class MeshG
{
public:

	MeshG() = default;

	~MeshG() = default;

	std::vector<glm::vec3> vertices;
	std::vector<unsigned int> indices;
	std::vector<glm::vec2> uv0s;
	std::vector<glm::vec2> uv1s;
	std::vector<glm::vec2> uv2s;
	std::vector<glm::vec2> uv3s;
	std::vector<glm::vec2> uv4s;
	std::vector<glm::vec2> uv5s;
	std::vector<glm::vec2> uv6s;
	std::vector<glm::vec2> uv7s;
	std::vector<glm::vec3> colors;
	std::vector<glm::vec3> normals;
	std::vector<glm::vec3> tangents;

	aux::DrawMode drawMode = aux::DrawMode::triangles;

	unsigned int count = 6;

	static std::shared_ptr<MeshG> quad_2d()
	{
		std::shared_ptr<MeshG> mesh = std::make_shared<MeshG>();

		mesh->vertices = {
			{-1.0f, -1.0f, 0.0f},
			{+1.0f, -1.0f, 0.0f},
			{+1.0f, +1.0f, 0.0f},
			{-1.0f, +1.0f, 0.0f},
		};

		mesh->uv0s = {
			{0.0f, 0.0f},
			{1.0f, 0.0f},
			{1.0f, 1.0f},
			{0.0f, 1.0f},
		};

		mesh->indices = {
			0, 1, 3,
			1, 2, 3,
		};

		mesh->build();

		return mesh;
	}

	static std::shared_ptr<MeshG> quad_3d()
	{
		std::shared_ptr<MeshG> mesh = std::make_shared<MeshG>();

		mesh->vertices = {
			{0.0f, 0.0f, 0.0f},
			{1.0f, 0.0f, 0.0f},
			{1.0f, 1.0f, 0.0f},
			{0.0f, 1.0f, 0.0f},
		};

		mesh->uv0s = {
			{0.0f, 0.0f},
			{1.0f, 0.0f},
			{1.0f, 1.0f},
			{0.0f, 1.0f},
		};

		mesh->indices = {
			0, 1, 3,
			1, 2, 3,
		};

		mesh->build();

		return mesh;
	}

	static std::shared_ptr<MeshG> triangle()
	{
		std::shared_ptr<MeshG> mesh = std::make_shared<MeshG>();

		mesh->vertices = {
			{0.0f, 0.0f, 0.0f},
			{1.0f, 1.0f, 0.0f},
			{1.0f, 0.0f, 0.0f},
		};

		mesh->uv0s = {
			{0.0f, 0.0f},
			{1.0f, 1.0f},
			{1.0f, 0.0f},
			{0.0f, 1.0f},
		};

		mesh->indices = {
			0, 1, 2,
		};

		mesh->build();

		return mesh;
	}

	static std::shared_ptr<MeshG> cube_3d()
	{
		std::shared_ptr<MeshG> mesh = std::make_shared<MeshG>();

		mesh->vertices = {
			  {-1.0f, +1.0f, +1.0f},
			  {-1.0f, -1.0f, +1.0f},
			  {+1.0f, +1.0f, +1.0f},
			  {+1.0f, -1.0f, +1.0f},
			  {-1.0f, +1.0f, -1.0f},
			  {-1.0f, -1.0f, -1.0f},
			  {+1.0f, +1.0f, -1.0f},
			  {+1.0f, -1.0f, -1.0f},
		};

		mesh->uv0s = {
			{0.0f, 0.0f},
			{1.0f, 1.0f},
			{1.0f, 0.0f},
			{0.0f, 1.0f},
			{0.0f, 0.0f},
			{1.0f, 1.0f},
			{1.0f, 0.0f},
			{0.0f, 1.0f},
		};

		mesh->indices = {
			0, 2, 3, 0, 3, 1,
			2, 6, 7, 2, 7, 3,
			6, 4, 5, 6, 5, 7,
			4, 0, 1, 4, 1, 5,
			0, 4, 6, 0, 6, 2,
			1, 5, 7, 1, 7, 3,
		};

		mesh->build();

		return mesh;
	}

	static std::shared_ptr<MeshG> line_2d(glm::vec2 beg, glm::vec2 end)
	{
		std::shared_ptr<MeshG> mesh = std::make_shared<MeshG>();

		mesh->drawMode = aux::DrawMode::lines;

		glm::mat4 proj = Graphic::GetCameraOrtho()->getProjection();
		glm::vec4 beg_ = proj * glm::vec4(beg.x, beg.y, 0.0f, 1.0f);
		glm::vec4 end_ = proj * glm::vec4(end.x, end.y, 0.0f, 1.0f);

		mesh->vertices = {
			{beg_.x, beg_.y, 0.0f},
			{end_.x, end_.y, 0.0f},
		};

		mesh->build();

		return mesh;
	}

	void build()
	{
		if (_VAO != 0)
		{
			LOGGER(warning, "vao already generated");
			return;
		}

		generate();

		bind_vao();

		if (!vertices.empty())
		{
			bind_vbo_vertex();

			unsigned int index = aux::toGLenum(aux::Index::position);

			aux::DataBuffer dataBuffer;
			dataBuffer.target = aux::toGLenum(aux::Target::vbo);
			dataBuffer.size = aux::size(vertices);
			dataBuffer.data = aux::data(vertices);
			dataBuffer.usage = aux::toGLenum(aux::Usage::static_draw);

			aux::Attribute attr;
			attr.index = index;
			attr.size = aux::component(vertices);
			attr.type = aux::toGLenum(aux::Type::float_type);
			attr.normalized = false;
			attr.stride = 0;
			attr.pointer = nullptr;

			glEnableVertexAttribArray(index);
			glBufferData(dataBuffer.target, dataBuffer.size, dataBuffer.data, dataBuffer.usage);
			glVertexAttribPointer(attr.index, attr.size, attr.type, attr.normalized, attr.stride, attr.pointer);
		}

		if (!uv0s.empty())
		{
			bind_vbo_uv0();

			unsigned int index = aux::toGLenum(aux::Index::texture);

			aux::DataBuffer dataBuffer;
			dataBuffer.target = aux::toGLenum(aux::Target::vbo);
			dataBuffer.size = aux::size(uv0s);
			dataBuffer.data = aux::data(uv0s);
			dataBuffer.usage = aux::toGLenum(aux::Usage::static_draw);

			aux::Attribute attr;
			attr.index = index;
			attr.size = aux::component(uv0s);
			attr.type = aux::toGLenum(aux::Type::float_type);
			attr.normalized = false;
			attr.stride = 0;
			attr.pointer = nullptr;

			glEnableVertexAttribArray(index);
			glBufferData(dataBuffer.target, dataBuffer.size, dataBuffer.data, dataBuffer.usage);
			glVertexAttribPointer(attr.index, attr.size, attr.type, attr.normalized, attr.stride, attr.pointer);
		}

		if (!colors.empty())
		{
			bind_vbo_color();

			unsigned int index = aux::toGLenum(aux::Index::color);

			aux::DataBuffer dataBuffer;
			dataBuffer.target = aux::toGLenum(aux::Target::vbo);
			dataBuffer.size = aux::size(colors);
			dataBuffer.data = aux::data(colors);
			dataBuffer.usage = aux::toGLenum(aux::Usage::static_draw);

			aux::Attribute attr;
			attr.index = index;
			attr.size = aux::component(colors);
			attr.type = aux::toGLenum(aux::Type::float_type);
			attr.normalized = false;
			attr.stride = 0;
			attr.pointer = nullptr;

			glEnableVertexAttribArray(index);
			glBufferData(dataBuffer.target, dataBuffer.size, dataBuffer.data, dataBuffer.usage);
			glVertexAttribPointer(attr.index, attr.size, attr.type, attr.normalized, attr.stride, attr.pointer);
		}

		if (!indices.empty())
		{
			bind_index();

			aux::DataBuffer dataBuffer;
			dataBuffer.target = aux::toGLenum(aux::Target::ebo);
			dataBuffer.size = aux::size(indices);
			dataBuffer.data = aux::data(indices);
			dataBuffer.usage = aux::toGLenum(aux::Usage::static_draw);

			glBufferData(dataBuffer.target, dataBuffer.size, dataBuffer.data, dataBuffer.usage);
		}

		count = !indices.empty() ? static_cast<unsigned int>(indices.size()) : static_cast<unsigned int>(vertices.size());

		unbind_all();
	}

	bool hasIndex()
	{
		return !indices.empty();
	}

	void bind()
	{
		bind_vao();
	}

	void unbind()
	{
		unbind_vao();
	}

private:

	void generate()
	{
		glGenVertexArrays(1, &_VAO);
		glGenBuffers(1, &_VBO_VER);
		glGenBuffers(1, &_VBO_UV0);
		glGenBuffers(1, &_VBO_UV1);
		glGenBuffers(1, &_VBO_COL);
		glGenBuffers(1, &_EBO);
	}

	void bind_vao()
	{
		glBindVertexArray(_VAO);
	}

	void bind_vbo_vertex()
	{
		glBindBuffer(GL_ARRAY_BUFFER, _VBO_VER);
	}

	void bind_vbo_uv0()
	{
		glBindBuffer(GL_ARRAY_BUFFER, _VBO_UV0);
	}

	void bind_vbo_uv1()
	{
		glBindBuffer(GL_ARRAY_BUFFER, _VBO_UV1);
	}

	void bind_vbo_color()
	{
		glBindBuffer(GL_ARRAY_BUFFER, _VBO_COL);
	}

	void bind_index()
	{
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _EBO);
	}

	void unbind_all()
	{
		unbind_vao();
		unbind_vbo();
		unbind_index();
	}

	void unbind_vao()
	{
		glBindVertexArray(0);
	}

	void unbind_vbo()
	{
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}

	void unbind_index()
	{
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	}

	void deleteVAO()
	{
		glDeleteVertexArrays(1, &_VAO);
		glDeleteBuffers(1, &_VBO_VER);
		glDeleteBuffers(1, &_VBO_UV0);
		glDeleteBuffers(1, &_VBO_UV1);
		glDeleteBuffers(1, &_EBO);
	}

	unsigned int _VAO{ 0 };
	unsigned int _VBO_VER{ 0 };
	unsigned int _VBO_UV0{ 0 };
	unsigned int _VBO_UV1{ 0 };
	unsigned int _VBO_COL{ 0 };
	unsigned int _EBO{ 0 };

};