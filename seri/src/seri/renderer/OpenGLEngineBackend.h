#pragma once

#include "seri/logging/Logger.h"
#include "seri/renderer/IEngineBackend.h"
#include "seri/renderer/AuxiliaryStructs.h"

#include <glad/gl.h>

#include <vector>

class OpenGLEngineBackend : public IEngineBackend
{
public:
	OpenGLEngineBackend()
	{
		generate();
		bind();
	}

	OpenGLEngineBackend(OpenGLEngineBackend& other) = default;

	OpenGLEngineBackend(OpenGLEngineBackend&& other) = delete;

	OpenGLEngineBackend& operator=(OpenGLEngineBackend& other) = delete;

	OpenGLEngineBackend& operator=(OpenGLEngineBackend&& other) = default;

	~OpenGLEngineBackend() override
	{
		//release();
	}

	void draw() override
	{
		bindVAO();
		if (aux::DrawType::arrays == _drawType)
		{
			drawArrays();
		}
		if (aux::DrawType::elements == _drawType)
		{
			drawElements();
		}
		unbindVAO();
	}

	void release() override
	{
		unbind();
		del();
	}

	void reserveDataBufferSize(GLsizeiptr size)
	{
		bind();
		dataBuffer({ size });
		unbind();
	}

	template<typename T>
	void setDataBuffer(aux::Index index, const typename std::vector<T>& vec, const void* pointer = nullptr)
	{
		indexCheck(index, vec);

		bind();
		dataBuffer({ aux::size(vec), aux::data(vec) });
		attribute({ index, aux::length(vec), pointer });
		enable(index);
		unbind();
	}

	void setDataBuffer(const aux::DataBuffer& dataB)
	{
		bind();
		dataBuffer(dataB);
		unbind();
	}

	void setSubDataBuffer(const aux::SubDataBuffer& subDataB)
	{
		bind();
		subdataBuffer(subDataB);
		unbind();
	}

	void setAttribute(const aux::Attribute& attributes)
	{
		bind();
		attribute(attributes);
		unbind();
	}

	void setEnable(const aux::Index& index)
	{
		bind();
		enable(index);
		unbind();
	}

	template<typename T>
	void setSubDataBuffer(aux::Index index, const typename std::vector<T>& vec, GLintptr offset, const void* pointer = nullptr)
	{
		indexCheck(index, vec);

		bind();
		subdataBuffer({ offset, aux::size(vec), aux::data(vec) });
		attribute({ index, aux::length(vec), (const void*)offset });
		enable(index);
		unbind();
	}

	void mapBuffer(const aux::MapBuffer& mapBuffer)
	{
		bind();
		void* ptr = glMapBuffer(mapBuffer.target, mapBuffer.access);
		memcpy(ptr, mapBuffer.data, mapBuffer.size);
		glUnmapBuffer(mapBuffer.target);
		unbind();
	}

	void setDrawMode(aux::DrawMode drawMode)
	{
		_drawMode = aux::toGLenum(drawMode);
	}

	void setDrawType(aux::DrawType drawType)
	{
		_drawType = drawType;
	}

	void setDrawCount(unsigned int drawCount)
	{
		if (_drawCount != 0)
		{
			//LOGGER(warning, "draw count already set to " << _drawCount << " and will not change to given value " << drawCount);
			return;
		}
		_drawCount = drawCount;
	}

	void bindVAO()
	{
		glBindVertexArray(_VAO);
	}

	void bindVBO()
	{
		glBindBuffer(GL_ARRAY_BUFFER, _VBO);
	}

	void bindEBO()
	{
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _EBO);
	}

private:
	void drawArrays()
	{
		glDrawArrays(_drawMode, 0, _drawCount);
	}

	void drawElements()
	{
		glDrawElements(_drawMode, _drawCount, GL_UNSIGNED_INT, nullptr);
	}

	void dataBuffer(const aux::DataBuffer& dataBuffer)
	{
		glBufferData(dataBuffer.target, dataBuffer.size, dataBuffer.data, dataBuffer.usage);
	}

	void subdataBuffer(const aux::SubDataBuffer& subDataBuffer)
	{
		glBufferSubData(subDataBuffer.target, subDataBuffer.offset, subDataBuffer.size, subDataBuffer.data);
	}

	void attribute(const aux::Attribute& attribute)
	{
		glVertexAttribPointer(attribute.index, attribute.size, attribute.type, attribute.normalized, attribute.stride, attribute.pointer);
	}

	void enable(aux::Index index)
	{
		glEnableVertexAttribArray(aux::toUInt(index));
	}

	template<typename T>
	void indexCheck(aux::Index index, const typename std::vector<T>& vec)
	{
		if (aux::Index::position == index)
		{
			setDrawCount(aux::count(vec));
		}
		if (aux::Index::color == index)
		{
		}
		if (aux::Index::texture == index)
		{
		}
	}

	void generate()
	{
		generateVAO();
		generateVBO();
		generateEBO();
	}

	void generateVAO()
	{
		glGenVertexArrays(1, &_VAO);
	}

	void generateVBO()
	{
		glGenBuffers(1, &_VBO);
	}

	void generateEBO()
	{
		glGenBuffers(1, &_EBO);
	}

	void bind()
	{
		bindVAO();
		bindVBO();
		bindEBO();
	}

	void unbind()
	{
		unbindVAO();
		unbindVBO();
		unbindEBO();
	}

	void unbindVAO()
	{
		glBindVertexArray(0);
	}

	void unbindVBO()
	{
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}

	void unbindEBO()
	{
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	}

	void del()
	{
		deleteVAO();
		deleteVBO();
		deleteEBO();
	}

	void deleteVAO()
	{
		glDeleteVertexArrays(1, &_VAO);
		_VAO = 0;
	}

	void deleteVBO()
	{
		glDeleteBuffers(1, &_VBO);
		_VBO = 0;
	}

	void deleteEBO()
	{
		glDeleteBuffers(1, &_EBO);
		_EBO = 0;
	}

	unsigned int _VAO{ 0 };
	unsigned int _VBO{ 0 };
	unsigned int _EBO{ 0 };
	unsigned int _drawCount{ 0 };
	unsigned int _drawMode{ aux::toGLenum(aux::DrawMode::triangles) };
	aux::DrawType _drawType{ aux::DrawType::arrays };

};
