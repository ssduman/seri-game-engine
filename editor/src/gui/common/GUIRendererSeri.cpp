#include "Editorpch.h"

#include "gui/common/GUIRendererSeri.h"

namespace seri::editor
{
	void GUIRendererSeri::Init()
	{
		auto& io = ImGui::GetIO();
		io.BackendRendererUserData = this;
		io.BackendRendererName = "imgui_impl_seri";
		io.BackendFlags |= ImGuiBackendFlags_RendererHasTextures;
		io.BackendFlags |= ImGuiBackendFlags_RendererHasViewports;

		ImGui::GetPlatformIO().Renderer_RenderWindow = RenderWindow;

		std::filesystem::path shaderPath = seri::project::ProjectManager::GetEngineSourceDirectory() / "shaders" / "gui.sshader";

		_shader = seri::ShaderBase::Create();
		_shader->Init(shaderPath.string());

		_vertexBuffer = seri::VertexBufferBase::Create(nullptr, 0, seri::BufferUsage::stream_draw);
		_vertexBuffer->AddElements({
			{ seri::LayoutLocation::loc_0, seri::ShaderDataType::float2_type },
			{ seri::LayoutLocation::loc_1, seri::ShaderDataType::float2_type },
			{ seri::LayoutLocation::loc_2, seri::ShaderDataType::ubyte4_type },
		});

		_indexBuffer = seri::IndexBufferBase::Create(nullptr, 0);

		LIB_LOGGER(info, gui_renderer) << "imgui renderer backend " << io.BackendRendererName << " inited";
	}

	void GUIRendererSeri::Shutdown()
	{
		for (ImTextureData* tex : ImGui::GetPlatformIO().Textures)
		{
			if (tex->RefCount == 1)
			{
				DestroyTexture(tex);
			}
		}

		_textures.clear();
		_shader = nullptr;
		_vertexBuffer = nullptr;
		_indexBuffer = nullptr;

		ImGui::GetPlatformIO().Renderer_RenderWindow = nullptr;

		auto& io = ImGui::GetIO();
		io.BackendRendererUserData = nullptr;
		io.BackendRendererName = nullptr;
		io.BackendFlags &= ~(ImGuiBackendFlags_RendererHasTextures | ImGuiBackendFlags_RendererHasViewports);
	}

	void GUIRendererSeri::NewFrame()
	{
	}

	void GUIRendererSeri::RenderDrawData(ImDrawData* drawData)
	{
		SERI_PROFILER_ZONE_SCOPED;

		int fbWidth = static_cast<int>(drawData->DisplaySize.x * drawData->FramebufferScale.x);
		int fbHeight = static_cast<int>(drawData->DisplaySize.y * drawData->FramebufferScale.y);
		if (fbWidth <= 0 || fbHeight <= 0)
		{
			return;
		}

		if (drawData->Textures != nullptr)
		{
			for (ImTextureData* tex : *drawData->Textures)
			{
				if (tex->Status != ImTextureStatus_OK)
				{
					UpdateTexture(tex);
				}
			}
		}

		SetupRenderState(drawData, fbWidth, fbHeight);

		// vao is created per call because vaos are not shared between viewport gl contexts
		auto vao = seri::VertexArrayBase::Create();
		vao->AddVertexBuffer(_vertexBuffer);
		vao->SetIndexBuffer(_indexBuffer);

		seri::DrawParams draw{};
		draw.dataType = sizeof(ImDrawIdx) == 2 ? seri::DataType::ushort_type : seri::DataType::uint_type;

		ImVec2 clipOff = drawData->DisplayPos;
		ImVec2 clipScale = drawData->FramebufferScale;

		for (const ImDrawList* drawList : drawData->CmdLists)
		{
			_vertexBuffer->SetData(drawList->VtxBuffer.Data, drawList->VtxBuffer.Size * sizeof(ImDrawVert));
			_indexBuffer->SetData(drawList->IdxBuffer.Data, drawList->IdxBuffer.Size, drawList->IdxBuffer.Size * sizeof(ImDrawIdx));

			for (const ImDrawCmd& cmd : drawList->CmdBuffer)
			{
				if (cmd.UserCallback != nullptr)
				{
					if (cmd.UserCallback == ImDrawCallback_ResetRenderState)
					{
						SetupRenderState(drawData, fbWidth, fbHeight);
					}
					else
					{
						cmd.UserCallback(drawList, &cmd);
					}
					continue;
				}

				float minX = (cmd.ClipRect.x - clipOff.x) * clipScale.x;
				float minY = (cmd.ClipRect.y - clipOff.y) * clipScale.y;
				float maxX = (cmd.ClipRect.z - clipOff.x) * clipScale.x;
				float maxY = (cmd.ClipRect.w - clipOff.y) * clipScale.y;
				if (maxX <= minX || maxY <= minY)
				{
					continue;
				}

				seri::RenderingManager::SetScissor(
					true,
					static_cast<int>(minX),
					static_cast<int>(fbHeight - maxY),
					static_cast<int>(maxX - minX),
					static_cast<int>(maxY - minY)
				);

				seri::TextureBase::BindTex2D(0, static_cast<uint32_t>(cmd.GetTexID()));

				draw.count = cmd.ElemCount;
				draw.indices = reinterpret_cast<const void*>(static_cast<intptr_t>(cmd.IdxOffset * sizeof(ImDrawIdx)));

				seri::RenderingManager::Draw(draw, vao);
			}
		}

		// leave shader and texture unbound so the engine bind caches stay valid in every viewport gl context
		seri::RenderingManager::SetScissor(false);
		seri::TextureBase::UnbindTex2D(0);
		vao->Unbind();
		_shader->Unbind();
	}

	void GUIRendererSeri::SetupRenderState(ImDrawData* drawData, int fbWidth, int fbHeight)
	{
		seri::RenderState state{};
		state.depthTestEnabled = false;

		seri::RenderingManager::SetState(state, true);
		seri::RenderingManager::SetPolygonMode(seri::PolygonMode::fill);
		seri::RenderingManager::SetViewport(0, 0, fbWidth, fbHeight);

		float left = drawData->DisplayPos.x;
		float right = drawData->DisplayPos.x + drawData->DisplaySize.x;
		float top = drawData->DisplayPos.y;
		float bottom = drawData->DisplayPos.y + drawData->DisplaySize.y;

		_shader->Bind();
		_shader->SetMat4("u_projection", glm::ortho(left, right, bottom, top));
		_shader->SetInt("u_texture", 0);
	}

	void GUIRendererSeri::UpdateTexture(ImTextureData* tex)
	{
		if (tex->Status == ImTextureStatus_WantCreate)
		{
			IM_ASSERT(tex->Format == ImTextureFormat_RGBA32);

			seri::TextureDesc desc{};
			desc.format = seri::TextureFormat::rgba__rgba8ubyte;

			auto texture = seri::TextureBase::Create();
			texture->Init(desc, tex->Width, tex->Height);
			texture->UpdateData(tex->GetPixels(), 0, 0, tex->Width, tex->Height);

			_textures[tex] = texture;

			tex->SetTexID(static_cast<ImTextureID>(texture->GetHandle()));
			tex->SetStatus(ImTextureStatus_OK);
		}
		else if (tex->Status == ImTextureStatus_WantUpdates)
		{
			auto& texture = _textures[tex];
			for (const ImTextureRect& rect : tex->Updates)
			{
				texture->UpdateData(tex->GetPixelsAt(rect.x, rect.y), rect.x, rect.y, rect.w, rect.h, tex->Width);
			}

			tex->SetStatus(ImTextureStatus_OK);
		}
		else if (tex->Status == ImTextureStatus_WantDestroy && tex->UnusedFrames > 0)
		{
			DestroyTexture(tex);
		}
	}

	void GUIRendererSeri::DestroyTexture(ImTextureData* tex)
	{
		_textures.erase(tex);

		tex->SetTexID(ImTextureID_Invalid);
		tex->SetStatus(ImTextureStatus_Destroyed);
	}

	void GUIRendererSeri::RenderWindow(ImGuiViewport* viewport, void*)
	{
		if (!(viewport->Flags & ImGuiViewportFlags_NoRendererClear))
		{
			seri::RenderingManager::ClearColor(0.0f, 0.0f, 0.0f, 1.0f);
			seri::RenderingManager::Clear();
		}

		static_cast<GUIRendererSeri*>(ImGui::GetIO().BackendRendererUserData)->RenderDrawData(viewport->DrawData);
	}
}
