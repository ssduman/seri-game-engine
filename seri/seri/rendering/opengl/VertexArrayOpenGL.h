#pragma once

#include "seri/util/Util.h"
#include "seri/rendering/common/VertexArrayBase.h"

#include <glad/gl.h>

namespace seri
{
	GLenum GetShaderDataTypeOpenGL(ShaderDataType type);;

	class VertexArrayOpenGL : public VertexArrayBase
	{
	public:
		VertexArrayOpenGL();
		~VertexArrayOpenGL() override;

		void Bind() override;
		void Unbind() override;

		const std::shared_ptr<IndexBufferBase>& GetIndexBuffer() override;
		void SetIndexBuffer(const std::shared_ptr<IndexBufferBase>& indexBuffer) override;
		void AddVertexBuffer(const std::shared_ptr<VertexBufferBase>& vertexBuffer) override;

	private:
		unsigned int _handle;

	};
}
