#pragma once

#include "seri/rendering/RenderingManagerBase.h"

#include <glad/gl.h>

#include <utility>
#include <stdexcept>

namespace seri
{
	class RenderingManagerOpenGL : public RenderingManagerBase
	{
	public:
		RenderingManagerOpenGL() = default;

		~RenderingManagerOpenGL() override
		{
			LOGGER(info, "[rendering] opengl rendering manager destroyed");
		}

		void Init(std::unique_ptr<WindowManagerBase>& windowManager) override
		{
			if (_initialized)
			{
				throw std::runtime_error("[rendering] opengl rendering manager is already initialized");
			}

			windowManager->SetOpenGLHints();
			windowManager->SetOpenGLContext();
			LoadGlad(windowManager);
			SetOpenGLOptions();
			LogOpenGLInfo();
			EnableOpenGLDebugOutput();

			mainRT = FramebufferBase::Create();

			seri::FramebufferDesc framebufferDesc{};
			framebufferDesc.width = 800;
			framebufferDesc.height = 600;
			seri::TextureDesc textureDesc{};
			textureDesc.format = seri::TextureFormat::rgba__rgba8ubyte;
			textureDesc.wrapS = seri::TextureWrap::clamp_to_edge;
			textureDesc.wrapR = seri::TextureWrap::clamp_to_edge;
			textureDesc.wrapT = seri::TextureWrap::clamp_to_edge;
			textureDesc.magFilter = seri::TextureMagFilter::linear;
			textureDesc.minFilter = seri::TextureMinFilter::linear;
			framebufferDesc.AddAttachments(
				{ textureDesc }
			);
			editorRT = seri::FramebufferBase::Create(framebufferDesc);

			_initialized = true;

			LOGGER(info, "[rendering] opengl rendering manager successfully");
		}

		void SetViewport(int x, int y, int width, int height) override
		{
			glViewport(x, y, width, height);
		}

		void Clear() override
		{
			glClearStencil(0);
			glClearDepth(1.0f);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
		}

		void ClearColor(float red = 0.2f, float green = 0.2f, float blue = 0.2f, float alpha = 1.0f) override
		{
			glClearColor(red, green, blue, alpha);
		}

		void SetPointSize(float size) override
		{
			glPointSize(size);
		}

		void SetLineWidth(float width) override
		{
			glLineWidth(width);
		}

		void SetBlend(bool enabled, BlendFactor srcFactor, BlendFactor dstFactor) override
		{
			if (enabled)
			{
				glEnable(GL_BLEND);
				glBlendFunc(GetBlendFactor(srcFactor), GetBlendFactor(dstFactor));
			}
			else
			{
				glDisable(GL_BLEND);
			}
		}

		void SetDepthFunc(bool enabled, DepthFunc depthFunc) override
		{
			if (enabled)
			{
				glEnable(GL_DEPTH_TEST);
				glDepthFunc(GetDepthFunc(depthFunc));
			}
			else
			{
				glDisable(GL_DEPTH_TEST);
			}
		}

		void SetDepthWrite(bool enabled) override
		{
			if (enabled)
			{
				glDepthMask(GL_TRUE);
			}
			else
			{
				glDepthMask(GL_FALSE);
			}
		}

		void SetStencilFunc(bool enabled, StencilFunc stencilFunc, int32_t ref, uint32_t mask) override
		{
			if (enabled)
			{
				glEnable(GL_STENCIL_TEST);
				glStencilFunc(GetStencilFunc(stencilFunc), ref, mask);
			}
			else
			{
				glDisable(GL_STENCIL_TEST);
			}
		}

		void SetStencilOp(StencilOp sfail, StencilOp dpfail, StencilOp dppass) override
		{
			glStencilOp(GetStencilOp(sfail), GetStencilOp(dpfail), GetStencilOp(dppass));
		}

		void SetStencilMask(uint32_t mask) override
		{
			glStencilMask(mask);
		}

		void SetCullFace(bool enabled, CullFace cullFace) override
		{
			if (enabled)
			{
				glEnable(GL_CULL_FACE);
				glCullFace(GetCullFace(cullFace));
			}
			else
			{
				glDisable(GL_CULL_FACE);
			}
		}

		void SetFrontFace(FrontFace frontFace) override
		{
			glFrontFace(GetFrontFace(frontFace));
		}

	protected:
		void LoadGlad(std::unique_ptr<WindowManagerBase>& windowManager)
		{
			int version = gladLoadGL(static_cast<GLADloadfunc>(windowManager->GetOpenGLProcAddress()));
			if (version == 0)
			{
				throw std::runtime_error("[rendering] glad load error");
			}

			LOGGER(info, "[rendering] loaded opengl " << GLAD_VERSION_MAJOR(version) << "." << GLAD_VERSION_MINOR(version));
		}

		void SetOpenGLOptions()
		{
			glEnable(GL_DEPTH_TEST);
			glDepthMask(GL_TRUE);

			glEnable(GL_BLEND);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

			glEnable(GL_STENCIL_TEST);

			//glEnable(GL_FRAMEBUFFER_SRGB);
		}

		void LogOpenGLInfo()
		{
			LOGGER(info, "[rendering] vendor: " << glGetString(GL_VENDOR));
			LOGGER(info, "[rendering] version: " << glGetString(GL_VERSION));
			LOGGER(info, "[rendering] renderer: " << glGetString(GL_RENDERER));
			LOGGER(info, "[rendering] shading language version: " << glGetString(GL_SHADING_LANGUAGE_VERSION));
		}

		void CheckOpenGLError()
		{
			GLenum err;
			while ((err = glGetError()) != GL_NO_ERROR)
			{
				LOGGER(error, "[rendering] gl error occurred: " << err);
			}
		}

		void EnableOpenGLDebugOutput()
		{
			static const auto getDebugSourceString = [](GLenum source)
				{
					if (source == GL_DEBUG_SOURCE_API)
					{
						return "Calls to the OpenGL API";
					}
					if (source == GL_DEBUG_SOURCE_WINDOW_SYSTEM)
					{
						return "Calls to a window-system API";
					}
					if (source == GL_DEBUG_SOURCE_SHADER_COMPILER)
					{
						return "A compiler for a shading language";
					}
					if (source == GL_DEBUG_SOURCE_THIRD_PARTY)
					{
						return "An application associated with OpenGL";
					}
					if (source == GL_DEBUG_SOURCE_APPLICATION)
					{
						return "Generated by the user of this application";
					}
					if (source == GL_DEBUG_SOURCE_OTHER)
					{
						return "Some source that isn't one of these";
					}

					return "Unknown source";
				};

			static const auto getDebugTypeString = [](GLenum type)
				{
					if (type == GL_DEBUG_TYPE_ERROR)
					{
						return "An error, typically from the API";
					}
					if (type == GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR)
					{
						return "Some behavior marked deprecated has been used";
					}
					if (type == GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR)
					{
						return "Something has invoked undefined behavior";
					}
					if (type == GL_DEBUG_TYPE_PORTABILITY)
					{
						return "Some functionality the user relies upon is not portable";
					}
					if (type == GL_DEBUG_TYPE_PERFORMANCE)
					{
						return "Code has triggered possible performance issues";
					}
					if (type == GL_DEBUG_TYPE_MARKER)
					{
						return "Command stream annotation";
					}
					if (type == GL_DEBUG_TYPE_PUSH_GROUP)
					{
						return "Group pushing";
					}
					if (type == GL_DEBUG_TYPE_POP_GROUP)
					{
						return "Group popping";
					}
					if (type == GL_DEBUG_TYPE_OTHER)
					{
						return "Some type that isn't one of these";
					}

					return "Unknown type";
				};

			static const auto getDebugSeverityString = [](GLenum severity)
				{
					if (severity == GL_DEBUG_SEVERITY_HIGH)
					{
						return "All OpenGL Errors, shader compilation / linking errors, or highly - dangerous undefined behavior";
					}
					else if (severity == GL_DEBUG_SEVERITY_MEDIUM)
					{
						return "Major performance warnings, shader compilation / linking warnings, or the use of deprecated functionality";
					}
					else if (severity == GL_DEBUG_SEVERITY_LOW)
					{
						return "Redundant state change performance warning, or unimportant undefined behavior";
					}
					else if (severity == GL_DEBUG_SEVERITY_NOTIFICATION)
					{
						return "Anything that isn't an error or performance issue";
					}

					return "Unknown source";
				};

			glEnable(GL_DEBUG_OUTPUT);
			glDebugMessageCallback(
				[](GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* userParam)
				{
					if (severity == GL_DEBUG_SEVERITY_NOTIFICATION)
					{
						return;
					}

					LOGGER(debug, "[rendering] gl debug message:" << "\n"
						<< "severity: " << getDebugSeverityString(severity) << "\n"
						<< "type: " << getDebugTypeString(type) << "\n"
						<< "source: " << getDebugSourceString(source) << "\n"
						<< "id: " << id << "\n"
						<< "message: " << message << "\n"
					);
				},
				nullptr
			);
		}

		void DisableOpenGLDebugOutput()
		{
			glDisable(GL_DEBUG_OUTPUT);
		}

		GLenum GetCullFace(CullFace cullFace)
		{
			switch (cullFace)
			{
				case CullFace::front: return GL_FRONT;
				case CullFace::back: return GL_BACK;
				case CullFace::front_and_back: return GL_FRONT_AND_BACK;
			}
			return GL_BACK;
		}

		GLenum GetFrontFace(FrontFace frontFace)
		{
			switch (frontFace)
			{
				case FrontFace::cw: return GL_CW;
				case FrontFace::ccw: return GL_CCW;
			}
			return GL_CCW;
		}

		GLenum GetDepthFunc(DepthFunc depthFunc)
		{
			switch (depthFunc)
			{
				case DepthFunc::never: return GL_NEVER;
				case DepthFunc::always: return GL_ALWAYS;
				case DepthFunc::less: return GL_LESS;
				case DepthFunc::equal: return GL_EQUAL;
				case DepthFunc::l_equal: return GL_LEQUAL;
				case DepthFunc::g_equal: return GL_GEQUAL;
				case DepthFunc::greater: return GL_GREATER;
				case DepthFunc::not_equal: return GL_NOTEQUAL;
			}
			return GL_LESS;
		}

		GLenum GetStencilFunc(StencilFunc stencilFunc)
		{
			switch (stencilFunc)
			{
				case StencilFunc::never: return GL_NEVER;
				case StencilFunc::always: return GL_ALWAYS;
				case StencilFunc::less: return GL_LESS;
				case StencilFunc::equal: return GL_EQUAL;
				case StencilFunc::l_equal: return GL_LEQUAL;
				case StencilFunc::g_equal: return GL_GEQUAL;
				case StencilFunc::greater: return GL_GREATER;
				case StencilFunc::not_equal: return GL_NOTEQUAL;
			}
			return GL_ALWAYS;
		}

		GLenum GetStencilOp(StencilOp stencilOp)
		{
			switch (stencilOp)
			{
				case StencilOp::keep: return GL_KEEP;
				case StencilOp::zero: return GL_ZERO;
				case StencilOp::replace: return GL_REPLACE;
				case StencilOp::incr: return GL_INCR;
				case StencilOp::incr_wrap: return GL_INCR_WRAP;
				case StencilOp::decr: return GL_DECR;
				case StencilOp::decr_wrap: return GL_DECR_WRAP;
				case StencilOp::invert: return GL_INVERT;
			}
			return GL_KEEP;
		}

		GLenum GetBlendFactor(BlendFactor factor)
		{
			switch (factor)
			{
				case BlendFactor::zero: return GL_ZERO;
				case BlendFactor::one: return GL_ONE;
				case BlendFactor::src_color: return GL_SRC_COLOR;
				case BlendFactor::one_minus_src_color: return GL_ONE_MINUS_SRC_COLOR;
				case BlendFactor::dst_color: return GL_DST_COLOR;
				case BlendFactor::one_minus_dst_color: return GL_ONE_MINUS_DST_COLOR;
				case BlendFactor::src_alpha: return GL_SRC_ALPHA;
				case BlendFactor::one_minus_src_alpha: return GL_ONE_MINUS_SRC_ALPHA;
				case BlendFactor::dst_alpha: return GL_DST_ALPHA;
				case BlendFactor::one_minus_dst_alpha: return GL_ONE_MINUS_DST_ALPHA;
				case BlendFactor::constant_color: return GL_CONSTANT_COLOR;
				case BlendFactor::one_minus_constant_color: return GL_ONE_MINUS_CONSTANT_COLOR;
				case BlendFactor::constant_alpha: return GL_CONSTANT_ALPHA;
				case BlendFactor::one_minus_constant_alpha: return GL_ONE_MINUS_CONSTANT_ALPHA;
				case BlendFactor::src_alpha_saturate: return GL_SRC_ALPHA_SATURATE;
			}

			return GL_ONE;
		}

	};
}
