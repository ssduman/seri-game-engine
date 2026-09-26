#include "Seripch.h"

#include "seri/rendering/common/RenderCommandBufferBase.h"
#include "seri/rendering/render/RenderingManager.h"
#include "seri/shader/ShaderLibrary.h"

#include <algorithm>
#include <vector>

namespace seri
{
	void RenderCommandBufferBase::Init()
	{
	}

	void RenderCommandBufferBase::Begin()
	{
		auto activeCamera = Graphic::GetActiveCamera();
		auto uiCamera = Graphic::GetCameraUI();
		auto runtimeCamera = Graphic::GetRuntimeCamera();

		auto editorRT = seri::RenderingManager::GetEditorRT();
		auto gameRT = seri::RenderingManager::GetGameRT();
		auto editorSceneRT = seri::RenderingManager::GetEditorSceneRT();
		auto gameSceneRT = seri::RenderingManager::GetGameSceneRT();
		auto shadowRT = seri::RenderingManager::GetShadowRT();

		editorSceneRT->Resize(editorRT->GetWidth(), editorRT->GetHeight());
		gameSceneRT->Resize(gameRT->GetWidth(), gameRT->GetHeight());

		RenderPass passShadow;
		passShadow.desc.type = PassType::shadow;
		passShadow.desc.rt = shadowRT;
		passShadow.desc.camera = nullptr;

		RenderPass passSkybox;
		passSkybox.desc.type = PassType::skybox;
		passSkybox.desc.rt = editorSceneRT;
		passSkybox.desc.camera = activeCamera;

		RenderPass passOpaque;
		passOpaque.desc.type = PassType::opaque;
		passOpaque.desc.rt = editorSceneRT;
		passOpaque.desc.camera = activeCamera;

		RenderPass passTransparent;
		passTransparent.desc.type = PassType::transparent;
		passTransparent.desc.rt = editorSceneRT;
		passTransparent.desc.camera = activeCamera;

		RenderPass passPost;
		passPost.desc.type = PassType::post;
		passPost.desc.rt = editorRT;
		passPost.desc.source = editorSceneRT;
		passPost.desc.camera = activeCamera;

		RenderPass passDebug;
		passDebug.desc.type = PassType::debug;
		passDebug.desc.rt = editorRT;
		passDebug.desc.camera = activeCamera;

		RenderPass passUI;
		passUI.desc.type = PassType::ui;
		passUI.desc.rt = editorRT;
		passUI.desc.camera = uiCamera;

		_frameGraph.Clear();
		_frameGraph.AddPass(passShadow);
		_frameGraph.AddPass(passSkybox);
		_frameGraph.AddPass(passOpaque);
		_frameGraph.AddPass(passTransparent);
		_frameGraph.AddPass(passPost);
		_frameGraph.AddPass(passDebug);
		_frameGraph.AddPass(passUI);

		if (runtimeCamera == nullptr || !seri::RenderingManager::GetGameViewVisible())
		{
			return;
		}

		RenderPass passGameSkybox;
		passGameSkybox.desc.type = PassType::skybox;
		passGameSkybox.desc.rt = gameSceneRT;
		passGameSkybox.desc.camera = runtimeCamera;

		RenderPass passGameOpaque;
		passGameOpaque.desc.type = PassType::opaque;
		passGameOpaque.desc.rt = gameSceneRT;
		passGameOpaque.desc.camera = runtimeCamera;

		RenderPass passGameTransparent;
		passGameTransparent.desc.type = PassType::transparent;
		passGameTransparent.desc.rt = gameSceneRT;
		passGameTransparent.desc.camera = runtimeCamera;

		RenderPass passGamePost;
		passGamePost.desc.type = PassType::post;
		passGamePost.desc.rt = gameRT;
		passGamePost.desc.source = gameSceneRT;
		passGamePost.desc.camera = runtimeCamera;

		RenderPass passGameUI;
		passGameUI.desc.type = PassType::ui;
		passGameUI.desc.rt = gameRT;
		passGameUI.desc.camera = uiCamera;

		_frameGraph.AddPass(passGameSkybox);
		_frameGraph.AddPass(passGameOpaque);
		_frameGraph.AddPass(passGameTransparent);
		_frameGraph.AddPass(passGamePost);
		_frameGraph.AddPass(passGameUI);
	}

	void RenderCommandBufferBase::End()
	{
	}

	void RenderCommandBufferBase::Submit(RenderItem renderItem)
	{
		_frameGraph.AddItem(std::move(renderItem));
	}

	void RenderCommandBufferBase::Submit(RenderCommand renderCommand)
	{
		_commands.emplace_back(renderCommand);
	}

	void RenderCommandBufferBase::SetState(const RenderState& state)
	{
		if (RenderState::IsBlendChanged(state, _statePrev))
		{
			seri::RenderingManager::SetBlend(state.blendEnabled, state.blendFactorSrc, state.blendFactorDst);
		}

		if (RenderState::IsFrontFaceChanged(state, _statePrev))
		{
			seri::RenderingManager::SetFrontFace(state.frontFace);
		}

		if (RenderState::IsCullFaceChanged(state, _statePrev))
		{
			seri::RenderingManager::SetCullFace(state.cullFaceEnabled, state.cullFace);
		}

		if (RenderState::IsDepthFuncChanged(state, _statePrev))
		{
			seri::RenderingManager::SetDepthFunc(state.depthTestEnabled, state.depthFunc);
		}

		if (RenderState::IsDepthWriteChanged(state, _statePrev))
		{
			seri::RenderingManager::SetDepthWrite(state.depthWriteEnabled);
		}

		if (RenderState::IsStencilFuncChanged(state, _statePrev))
		{
			seri::RenderingManager::SetStencilFunc(state.stencilTestEnabled, state.stencilFunc, state.stencilRef, state.stencilMaskAND);
		}

		if (RenderState::IsStencilOpChanged(state, _statePrev))
		{
			seri::RenderingManager::SetStencilOp(state.stencilSfail, state.stencilDPfail, state.stencilDPpass);
		}

		if (RenderState::IsStencilMaskChanged(state, _statePrev))
		{
			seri::RenderingManager::SetStencilMask(state.stencilMask);
		}

		if (RenderState::IsLineWidthChanged(state, _statePrev))
		{
			seri::RenderingManager::SetLineWidth(state.lineWidth);
		}

		if (RenderState::IsPointSizeChanged(state, _statePrev))
		{
			seri::RenderingManager::SetPointSize(state.pointSize);
		}

		_statePrev = state;
	}

	void RenderCommandBufferBase::OnPassChanged(const RenderPass& renderPass)
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
		SERI_PROFILER_ZONE_SCOPED;

		{
			SERI_PROFILER_ZONE_SCOPED_N("Commands");

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

				seri::RenderingManager::SetViewport(0, 0, rt->GetWidth(), rt->GetHeight());

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
		}

		for (const RenderPass& pass : _frameGraph.passes)
		{
			SERI_PROFILER_ZONE_TRANSIENT(GetPassName(pass.desc.type));

			auto& rt = pass.desc.rt;
			auto& cam = pass.desc.camera;

			if (pass.desc.type == PassType::post)
			{
				RenderPost(pass);
				continue;
			}

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

					DrawShadowItems(pass, seri::RenderingManager::GetDirShadowLightViewProj());

					rt->Unbind();
				}

				// spot light shadows
				{
					int spotShadowCount = seri::RenderingManager::GetSpotShadowCount();
					for (int i = 0; i < spotShadowCount; i++)
					{
						auto spotShadowRT = seri::RenderingManager::GetSpotShadowRT(i);
						spotShadowRT->Bind();
						seri::RenderingManager::SetViewport(0, 0, spotShadowRT->GetWidth(), spotShadowRT->GetHeight());
						seri::RenderingManager::Clear();

						DrawShadowItems(pass, seri::RenderingManager::GetSpotShadowLightViewProj(i));

						spotShadowRT->Unbind();
					}
				}

				continue;
			}

			if (!cam)
			{
				continue;
			}

			bool wireframe =
				seri::RenderingManager::GetEditorWireframe() &&
				rt == seri::RenderingManager::GetEditorSceneRT() &&
				(pass.desc.type == PassType::opaque || pass.desc.type == PassType::transparent);

			rt->Bind();
			seri::RenderingManager::SetViewport(0, 0, rt->GetWidth(), rt->GetHeight());

			if (wireframe)
			{
				seri::RenderingManager::SetPolygonMode(PolygonMode::line);
			}

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

			std::vector<const RenderItem*> items;
			items.reserve(pass.items.size());
			for (const RenderItem& item : pass.items)
			{
				items.push_back(&item);
			}

			if (pass.desc.type == PassType::ui)
			{
				std::stable_sort(items.begin(), items.end(),
					[](const RenderItem* a, const RenderItem* b)
					{
						return a->sortOrder < b->sortOrder;
					});
			}

			for (const RenderItem* item : items)
			{
				const RenderItem& cmd = *item;

				SetState(cmd.state);

				{
					SERI_PROFILER_ZONE_SCOPED_N("SetUniforms");

					if (pass.desc.type == PassType::skybox)
					{
						cmd.material->SetMat4(literals::kUniformViewSkybox, glm::mat4(glm::mat3(view)));
					}

					cmd.material->SetMat4(literals::kUniformModel, cmd.model);
					cmd.material->SetMat4(literals::kUniformView, view);
					cmd.material->SetMat4(literals::kUniformProjection, projection);
					cmd.material->SetFloat4(literals::kUniformCameraPos, camPos);
					cmd.material->SetInt(literals::kUniformDirLightShadowMap, static_cast<int>(seri::TextureSlotName::dir_shadow));
					cmd.material->SetInt(literals::kUniformSpotLightShadowMap0, static_cast<int>(seri::TextureSlotName::spot_shadow_0));
					cmd.material->SetInt(literals::kUniformSpotLightShadowMap1, static_cast<int>(seri::TextureSlotName::spot_shadow_1));
					cmd.material->SetInt(literals::kUniformSpotLightShadowMap2, static_cast<int>(seri::TextureSlotName::spot_shadow_2));
					cmd.material->SetInt(literals::kUniformSpotLightShadowMap3, static_cast<int>(seri::TextureSlotName::spot_shadow_3));
				}

				cmd.material->Apply();

				if (!cmd.bones.empty())
				{
					auto shader = cmd.material->GetShader();
					if (shader && shader->IsActiveForUsing())
					{
						shader->SetMat4Array(literals::kUniformBones, cmd.bones.data(), static_cast<uint32_t>(cmd.bones.size()));
					}
				}

				Draw(cmd.draw, cmd.vao);
			}

			if (wireframe)
			{
				seri::RenderingManager::SetPolygonMode(PolygonMode::fill);
			}

			rt->Unbind();
		}

		_commands.clear();
		_statsPrev = _stats;
		_stats.Reset();
	}

	void RenderCommandBufferBase::InitPost()
	{
		if (_postMaterial)
		{
			return;
		}

		_postMaterial = std::make_shared<Material>();
		_postMaterial->SetShader(ShaderLibrary::Find("fxaa"));

		_depthViewMaterial = std::make_shared<Material>();
		_depthViewMaterial->SetShader(ShaderLibrary::Find("depth_view"));

		std::vector<glm::vec3> positions{
			{ -1.0f, -1.0f, 0.0f },
			{ +3.0f, -1.0f, 0.0f },
			{ -1.0f, +3.0f, 0.0f },
		};

		auto vertexBuffer = VertexBufferBase::Create(positions);
		vertexBuffer->AddElement(
			{ seri::LayoutLocation::vertex, seri::ShaderDataType::float3_type, false }
		);

		_postVao = VertexArrayBase::Create();
		_postVao->AddVertexBuffer(vertexBuffer);
	}

	void RenderCommandBufferBase::RenderPost(const RenderPass& pass)
	{
		SERI_PROFILER_ZONE_SCOPED;

		auto& source = pass.desc.source;
		auto& rt = pass.desc.rt;

		if (!source || !rt)
		{
			return;
		}

		InitPost();

		bool depthView = seri::RenderingManager::GetEditorDepthView() && rt == seri::RenderingManager::GetEditorRT();

		std::shared_ptr<Material> material = depthView ? _depthViewMaterial : _postMaterial;

		if (depthView)
		{
			glm::vec2 cameraPlanes{ 0.1f, 1000.0f };
			if (pass.desc.camera)
			{
				const CameraProperties& cameraProperties = pass.desc.camera->GetCameraProperties();
				cameraPlanes = glm::vec2{ cameraProperties.nearPlane, cameraProperties.farPlane };
			}

			material->SetTexture("u_depth_texture", source->GetDepthTexture());
			material->SetFloat2("u_camera_planes", cameraPlanes);
		}
		else
		{
			material->SetTexture("u_source_texture", source->GetColorTexture(0));
			material->SetFloat2("u_texel_size", glm::vec2{ 1.0f / source->GetWidth(), 1.0f / source->GetHeight() });
			material->SetInt("u_fxaa_enabled", seri::RenderingManager::GetFxaaEnabled() ? 1 : 0);
		}

		RenderState state{};
		state.depthTestEnabled = false;
		state.depthWriteEnabled = false;
		state.blendEnabled = false;

		rt->Bind();
		seri::RenderingManager::SetViewport(0, 0, rt->GetWidth(), rt->GetHeight());
		SetState(state);
		material->Apply();

		DrawParams draw{};
		draw.mode = DrawMode::arrays;
		draw.count = 3;
		Draw(draw, _postVao);

		rt->Unbind();

		source->BlitDepthTo(rt);

		TextureBase::UnbindTex2D(0);

		SetState(RenderState{});
	}

	void RenderCommandBufferBase::DrawShadowItems(const RenderPass& pass, const glm::mat4& lightViewProj)
	{
		SERI_PROFILER_ZONE_SCOPED;

		for (const RenderItem& item : pass.items)
		{
			SetState(item.state);
			item.material->SetMat4(literals::kUniformModel, item.model);
			item.material->SetMat4(literals::kUniformLightViewProjection, lightViewProj);
			item.material->Apply();

			if (!item.bones.empty())
			{
				auto shader = item.material->GetShader();
				if (shader && shader->IsActiveForUsing())
				{
					shader->SetMat4Array(literals::kUniformBones, item.bones.data(), static_cast<uint32_t>(item.bones.size()));
				}
			}

			Draw(item.draw, item.vao);
		}
	}

	const char* RenderCommandBufferBase::GetPassName(PassType type)
	{
		switch (type)
		{
			case PassType::shadow: return "shadow";
			case PassType::skybox: return "skybox";
			case PassType::opaque: return "opaque";
			case PassType::transparent: return "transparent";
			case PassType::debug: return "debug";
			case PassType::ui: return "ui";
			case PassType::post: return "post";
		}
		return "unknown";
	}

}
