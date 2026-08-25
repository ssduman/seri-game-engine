#pragma once

namespace seri::literals
{
	constexpr const char* kVersion = "0.1";
	constexpr const char* kEngineName = "Seri Game Engine";

	constexpr const char* kUniformColor = "u_color";
	constexpr const char* kUniformCameraPos = "u_camera_pos";
	constexpr const char* kUniformLightDir = "u_light_dir";
	constexpr const char* kUniformLightColor = "u_light_color";
	constexpr const char* kUniformBones = "u_bones";
	constexpr const char* kUniformView = "u_view";
	constexpr const char* kUniformModel = "u_model";
	constexpr const char* kUniformProjection = "u_projection";
	constexpr const char* kUniformViewSkybox = "u_view_skybox";
	constexpr const char* kUniformViewProjection = "u_view_projection";
	constexpr const char* kUniformLightViewProjection = "u_light_view_projection";

	constexpr const char* kUniformDirLight = "u_dir_light";
	constexpr const char* kUniformDirLightExists = "u_dir_light_exists";
	constexpr const char* kUniformDirLightShadowMap = "u_dir_light_shadow_map";

	constexpr const char* kUniformSpotLights = "u_spot_lights";
	constexpr const char* kUniformSpotLightCount = "u_spot_light_count";
	constexpr const char* kUniformSpotLightShadowMap0 = "u_spot_light_shadow_map_0";
	constexpr const char* kUniformSpotLightShadowMap1 = "u_spot_light_shadow_map_1";
	constexpr const char* kUniformSpotLightShadowMap2 = "u_spot_light_shadow_map_2";
	constexpr const char* kUniformSpotLightShadowMap3 = "u_spot_light_shadow_map_3";

	constexpr const char* kUniformPointLights = "u_point_lights";
	constexpr const char* kUniformPointLightCount = "u_point_light_count";

	constexpr const char* kUniformDiffTexture = "u_diff_texture";
	constexpr const char* kUniformNormalTexture = "u_normal_texture";
	constexpr const char* kUniformArmTexture = "u_arm_texture";

	constexpr const int kMaxDirLightCount = 1;
	constexpr const int kMaxSpotLightCount = 8;
	constexpr const int kMaxPointLightCount = 8;
	constexpr const int kMaxSpotLightShadowCount = 4;
}
