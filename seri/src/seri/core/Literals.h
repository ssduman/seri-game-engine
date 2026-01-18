#pragma once

namespace seri::literals
{
	constexpr const char* kVersion = "0.1";
	constexpr const char* kEngineName = "Seri Game Engine";

	constexpr const char* kUniformColor = "u_color";
	constexpr const char* kUniformViewPos = "u_view_pos";
	constexpr const char* kUniformCameraPos = "u_camera_pos";
	constexpr const char* kUniformLightDir = "u_light_dir";
	constexpr const char* kUniformLightColor = "u_light_color";
	constexpr const char* kUniformBones = "u_bones";
	constexpr const char* kUniformView = "u_view";
	constexpr const char* kUniformModel = "u_model";
	constexpr const char* kUniformProjection = "u_projection";
	constexpr const char* kUniformViewSkybox = "u_view_skybox";
	constexpr const char* kUniformViewProjection = "u_view_projection";

	constexpr const char* kUniformDirLight = "u_dir_light";
	constexpr const char* kUniformDirLightExists = "u_dir_light_exists";

	constexpr const char* kUniformDirSpotLights = "u_spot_lights";
	constexpr const char* kUniformSpotLightCount = "u_spot_light_count";

	constexpr const char* kUniformDirPointLights = "u_point_lights";
	constexpr const char* kUniformPointLightCount = "u_point_light_count";

	constexpr const int kMaxSpotLightCount = 8;
	constexpr const int kMaxPointLightCount = 8;
}
