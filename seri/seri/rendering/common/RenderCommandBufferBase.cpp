#include "Seripch.h"

#include "seri/rendering/common/RenderCommandBufferBase.h"
#include "seri/rendering/render/RenderingManager.h"

namespace seri
{
	void RenderCommandBufferBase::Init()
	{
	}

	void RenderCommandBufferBase::Begin()
	{
		auto perspectiveCamera = Graphic::GetCameraPerspective();
		auto orthoCamera = Graphic::GetCameraOrtho();

		auto editorRT = seri::RenderingManager::GetEditorRT();
		auto shadowRT = seri::RenderingManager::GetShadowRT();

		RenderPass passShadow;
		passShadow.desc.type = PassType::shadow;
		passShadow.desc.rt = shadowRT;
		passShadow.desc.camera = nullptr;

		RenderPass passSkybox;
		passSkybox.desc.type = PassType::skybox;
		passSkybox.desc.rt = editorRT;
		passSkybox.desc.camera = perspectiveCamera;

		RenderPass passOpaque;
		passOpaque.desc.type = PassType::opaque;
		passOpaque.desc.rt = editorRT;
		passOpaque.desc.camera = perspectiveCamera;

		RenderPass passTransparent;
		passTransparent.desc.type = PassType::transparent;
		passTransparent.desc.rt = editorRT;
		passTransparent.desc.camera = perspectiveCamera;

		RenderPass passDebug;
		passDebug.desc.type = PassType::debug;
		passDebug.desc.rt = editorRT;
		passDebug.desc.camera = perspectiveCamera;

		RenderPass passUI;
		passUI.desc.type = PassType::ui;
		passUI.desc.rt = editorRT;
		passUI.desc.camera = orthoCamera;

		_frameGraph.Clear();
		_frameGraph.AddPass(passShadow);
		_frameGraph.AddPass(passSkybox);
		_frameGraph.AddPass(passOpaque);
		_frameGraph.AddPass(passTransparent);
		_frameGraph.AddPass(passDebug);
		_frameGraph.AddPass(passUI);
	}

	void RenderCommandBufferBase::End()
	{
	}

	void RenderCommandBufferBase::Submit(RenderItem renderItem)
	{
		_frameGraph.AddItem(renderItem);
	}

	void RenderCommandBufferBase::Submit(RenderCommand renderCommand)
	{
		_commands.emplace_back(renderCommand);
	}

	void RenderCommandBufferBase::SetState(RenderState state)
	{
		seri::RenderingManager::SetBlend(state.blendEnabled, state.blendFactorSrc, state.blendFactorDst);
		seri::RenderingManager::SetFrontFace(state.frontFace);
		seri::RenderingManager::SetCullFace(state.cullFaceEnabled, state.cullFace);
		seri::RenderingManager::SetDepthFunc(state.depthTestEnabled, state.depthFunc);
		seri::RenderingManager::SetDepthWrite(state.depthWriteEnabled);
		seri::RenderingManager::SetStencilFunc(state.stencilTestEnabled, state.stencilFunc, state.stencilRef, state.stencilMaskAND);
		seri::RenderingManager::SetStencilOp(state.stencilSfail, state.stencilDPfail, state.stencilDPpass);
		seri::RenderingManager::SetStencilMask(state.stencilMask);
		seri::RenderingManager::SetLineWidth(state.lineWidth);
		seri::RenderingManager::SetPointSize(state.pointSize);

		_statePrev = state;
	}

	void RenderCommandBufferBase::OnPassChanged(RenderPass renderPass)
	{
		auto& cam = renderPass.desc.camera;

		glm::vec4 camPos = cam->GetPosition();
		glm::mat4 view = cam->GetView();
		glm::mat4 projection = cam->GetProjection();

		UniformBufferCamera cameraUBO{};
		cameraUBO.view = view;
		cameraUBO.proj = projection;
		cameraUBO.viewProj = projection * view;
		cameraUBO.cameraPos = camPos;
		cameraUBO.time = glm::vec4{ TimeWrapper::GetTime(), TimeWrapper::GetDeltaTime(), 0.0f, 0.0f };

		seri::RenderingManager::GetCameraUBO()->SetData(&cameraUBO, sizeof(UniformBufferCamera));
	}

	void RenderCommandBufferBase::Execute()
	{
		for (const RenderCommand& cmd : _commands)
		{
			auto& rt = cmd.rt;
			auto& cam = cmd.camera;

			rt->Bind();

			SetState(cmd.state);

			if (cmd.noop)
			{
				rt->Unbind();
				continue;
			}

			glm::vec4 camPos = cam->GetPosition();
			glm::mat4 view = cam->GetView();
			glm::mat4 projection = cam->GetProjection();

			RenderPass pass{};
			pass.desc.camera = cam;
			OnPassChanged(pass);

			cmd.material->SetMat4(literals::kUniformModel, cmd.model);
			cmd.material->SetMat4(literals::kUniformView, view);
			cmd.material->SetMat4(literals::kUniformProjection, projection);
			cmd.material->SetFloat4(literals::kUniformCameraPos, camPos);
			cmd.material->Apply();

			Draw(cmd.draw, cmd.vao);

			rt->Unbind();
		}

		for (const RenderPass& pass : _frameGraph.passes)
		{
			auto& rt = pass.desc.rt;
			auto& cam = pass.desc.camera;
			if (!rt || pass.items.empty())
			{
				continue;
			}

			if (pass.desc.type == PassType::shadow)
			{
				// directional light shadow
				{
					rt->Bind();
					seri::RenderingManager::SetViewport(0, 0, rt->GetWidth(), rt->GetHeight());
					seri::RenderingManager::Clear();

					glm::mat4 lightViewProj = seri::RenderingManager::GetDirShadowLightViewProj();

					for (const RenderItem& item : pass.items)
					{
						SetState(item.state);
						item.material->SetMat4(literals::kUniformModel, item.model);
						item.material->SetMat4(literals::kUniformLightViewProjection, lightViewProj);
						item.material->Apply();
						Draw(item.draw, item.vao);
					}

					rt->Unbind();
				}

				// spot light shadows
				if (false)
				{
					int spotShadowCount = seri::RenderingManager::GetSpotShadowCount();
					for (int i = 0; i < spotShadowCount; i++)
					{
						auto spotShadowRT = seri::RenderingManager::GetSpotShadowRT(i);
						spotShadowRT->Bind();
						seri::RenderingManager::SetViewport(0, 0, spotShadowRT->GetWidth(), spotShadowRT->GetHeight());
						seri::RenderingManager::Clear();

						glm::mat4 spotLightViewProj = seri::RenderingManager::GetSpotShadowLightViewProj(i);

						for (const RenderItem& item : pass.items)
						{
							SetState(item.state);
							item.material->SetMat4(literals::kUniformModel, item.model);
							item.material->SetMat4(literals::kUniformLightViewProjection, spotLightViewProj);
							item.material->Apply();
							Draw(item.draw, item.vao);
						}

						spotShadowRT->Unbind();
					}
				}

				continue;
			}

			if (!cam)
			{
				continue;
			}

			rt->Bind();
			seri::RenderingManager::SetViewport(0, 0, rt->GetWidth(), rt->GetHeight());

			glm::vec4 camPos = cam->GetPosition();
			glm::mat4 view = cam->GetView();
			glm::mat4 projection = cam->GetProjection();

			OnPassChanged(pass);

			if (pass.desc.type == PassType::opaque)
			{
				const auto& shadowDepthTex = seri::RenderingManager::GetShadowRT()->GetDepthTexture();
				if (shadowDepthTex)
				{
					shadowDepthTex->Bind(static_cast<int>(seri::TextureSlotName::dir_shadow));
				}

				int spotShadowCount = seri::RenderingManager::GetSpotShadowCount();
				for (int i = 0; i < spotShadowCount; i++)
				{
					const auto& spotShadowDepthTex = seri::RenderingManager::GetSpotShadowRT(i)->GetDepthTexture();
					if (spotShadowDepthTex)
					{
						spotShadowDepthTex->Bind(static_cast<int>(seri::TextureSlotName::spot_shadow_0) + i);
					}
				}
			}

			for (const RenderItem& cmd : pass.items)
			{
				SetState(cmd.state);

				cmd.material->SetMat4(literals::kUniformModel, cmd.model);
				cmd.material->SetMat4(literals::kUniformView, view);
				cmd.material->SetMat4(literals::kUniformProjection, projection);
				cmd.material->SetFloat4(literals::kUniformCameraPos, camPos);
				cmd.material->SetInt(literals::kUniformDirLightShadowMap, static_cast<int>(seri::TextureSlotName::dir_shadow));
				cmd.material->SetInt(literals::kUniformSpotLightShadowMap0, static_cast<int>(seri::TextureSlotName::spot_shadow_0));
				cmd.material->SetInt(literals::kUniformSpotLightShadowMap1, static_cast<int>(seri::TextureSlotName::spot_shadow_1));
				cmd.material->SetInt(literals::kUniformSpotLightShadowMap2, static_cast<int>(seri::TextureSlotName::spot_shadow_2));
				cmd.material->SetInt(literals::kUniformSpotLightShadowMap3, static_cast<int>(seri::TextureSlotName::spot_shadow_3));
				cmd.material->Apply();

				Draw(cmd.draw, cmd.vao);
			}

			rt->Unbind();
		}

		_commands.clear();
		_statsPrev = _stats;
		_stats.Reset();
	}

}
