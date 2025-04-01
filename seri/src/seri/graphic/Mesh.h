#pragma once

#include "seri/texture/Texture.h"
#include "seri/renderer/AuxiliaryStructs.h"

#include <glad/gl.h>

#include <glm/glm.hpp>

#include <vector>
#include <memory>

class Graphic;

class Mesh
{
public:
	Mesh() = default;

	~Mesh() = default;

	std::vector<Texture> textures;
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

	glm::mat4 transformation{ 1.0f };

	aux::DrawMode drawMode = aux::DrawMode::triangles;

	unsigned int count = 6;

	void AddVertices(std::vector<glm::vec3> ver)
	{
		vertices.insert(vertices.end(), ver.begin(), ver.end());
	}

	void AddColors(std::vector<glm::vec4> col)
	{
		colors.insert(colors.end(), col.begin(), col.end());
	}

	void AddUV(std::vector<glm::vec2> uv)
	{
		uv0s.insert(uv0s.end(), uv.begin(), uv.end());
	}

	void AddNormals(std::vector<glm::vec3> nor)
	{
		normals.insert(normals.end(), nor.begin(), nor.end());
	}

	void AddTextures(std::vector<Texture> texs)
	{
		for (auto& tex : texs)
		{
			textures.emplace_back(std::move(tex));
		}
	}

	void AddIndices(std::vector<unsigned int> ind)
	{
		indices.insert(indices.end(), ind.begin(), ind.end());
	}

	void Build()
	{
		if (_vao != 0)
		{
			LOGGER(warning, "vao already generated");
			return;
		}

		Generate();

		Bind_vao();

		if (!vertices.empty())
		{
			Bind_vbo_vertex();

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
			Bind_vbo_uv0();

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
			Bind_vbo_color();

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
			Bind_index();

			aux::DataBuffer dataBuffer;
			dataBuffer.target = aux::toGLenum(aux::Target::ebo);
			dataBuffer.size = aux::size(indices);
			dataBuffer.data = aux::data(indices);
			dataBuffer.usage = aux::toGLenum(aux::Usage::static_draw);

			glBufferData(dataBuffer.target, dataBuffer.size, dataBuffer.data, dataBuffer.usage);
		}

		count = !indices.empty() ? static_cast<unsigned int>(indices.size()) : static_cast<unsigned int>(vertices.size());

		Unbind_all();
	}

	bool HasIndex()
	{
		return !indices.empty();
	}

	void Bind()
	{
		Bind_vao();
	}

	void Unbind()
	{
		Unbind_vao();
	}

	static std::shared_ptr<Mesh> tri_2d()
	{
		std::shared_ptr<Mesh> mesh = std::make_shared<Mesh>();

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

		mesh->Build();

		return mesh;
	}

	static std::shared_ptr<Mesh> quad_2d()
	{
		std::shared_ptr<Mesh> mesh = std::make_shared<Mesh>();

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

		mesh->Build();

		return mesh;
	}

	static std::shared_ptr<Mesh> quad_3d()
	{
		std::shared_ptr<Mesh> mesh = std::make_shared<Mesh>();

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

		mesh->Build();

		return mesh;
	}

	static std::shared_ptr<Mesh> cube_3d()
	{
		std::shared_ptr<Mesh> mesh = std::make_shared<Mesh>();

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

		mesh->Build();

		return mesh;
	}

	static std::shared_ptr<Mesh> line_2d(glm::vec2 beg, glm::vec2 end)
	{
		std::shared_ptr<Mesh> mesh = std::make_shared<Mesh>();

		mesh->drawMode = aux::DrawMode::lines;

		glm::mat4 proj = Graphic::GetCameraOrtho()->getProjection();
		glm::vec4 beg_ = proj * glm::vec4(beg.x, beg.y, 0.0f, 1.0f);
		glm::vec4 end_ = proj * glm::vec4(end.x, end.y, 0.0f, 1.0f);

		mesh->vertices = {
			{beg_.x, beg_.y, 0.0f},
			{end_.x, end_.y, 0.0f},
		};

		mesh->Build();

		return mesh;
	}

private:

	void Generate()
	{
		glGenVertexArrays(1, &_vao);
		glGenBuffers(1, &_vbo_ver);
		glGenBuffers(1, &_vbo_uv0);
		glGenBuffers(1, &_vbo_uv1);
		glGenBuffers(1, &_vbo_col);
		glGenBuffers(1, &_ebo);
	}

	void Bind_vao()
	{
		glBindVertexArray(_vao);
	}

	void Bind_vbo_vertex()
	{
		glBindBuffer(GL_ARRAY_BUFFER, _vbo_ver);
	}

	void Bind_vbo_uv0()
	{
		glBindBuffer(GL_ARRAY_BUFFER, _vbo_uv0);
	}

	void Bind_vbo_uv1()
	{
		glBindBuffer(GL_ARRAY_BUFFER, _vbo_uv1);
	}

	void Bind_vbo_color()
	{
		glBindBuffer(GL_ARRAY_BUFFER, _vbo_col);
	}

	void Bind_index()
	{
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _ebo);
	}

	void Unbind_all()
	{
		Unbind_vao();
		Unbind_vbo();
		Unbind_index();
	}

	void Unbind_vao()
	{
		glBindVertexArray(0);
	}

	void Unbind_vbo()
	{
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}

	void Unbind_index()
	{
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	}

	void Delete_vao()
	{
		glDeleteVertexArrays(1, &_vao);
		glDeleteBuffers(1, &_vbo_ver);
		glDeleteBuffers(1, &_vbo_uv0);
		glDeleteBuffers(1, &_vbo_uv1);
		glDeleteBuffers(1, &_ebo);
	}

	unsigned int _vao{ 0 };
	unsigned int _vbo_ver{ 0 };
	unsigned int _vbo_uv0{ 0 };
	unsigned int _vbo_uv1{ 0 };
	unsigned int _vbo_col{ 0 };
	unsigned int _ebo{ 0 };

};
