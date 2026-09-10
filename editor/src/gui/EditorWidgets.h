#pragma once

#include "gui/EditorCommon.h"

namespace seri::editor
{
	void ShowEditorImage(std::shared_ptr<seri::TextureBase>& texture, float size, bool flip = true);

	bool ShowEditorImageButton(std::shared_ptr<seri::TextureBase>& texture, float size);

	bool ShowEditorScriptPickerPopup(std::string& selection);

	bool ShowEditorAssetPickerPopup(seri::asset::AssetType type, bool& selected, uint64_t& selection);

	bool DrawBool(const char* label, bool& value);

	bool DrawInt(const char* label, int& value, float speed = 1.0f, int min = 0, int max = 0);

	bool DrawFloat(const char* label, float& value, float speed = 0.1f, float min = 0.0f, float max = 0.0f, const char* format = "%.3f");

	bool DrawVec3(const char* label, glm::vec3& v, float speed);

	bool DrawColorVec3(const char* label, glm::vec3& color, float speed);

	bool DrawColorVec4(const char* label, glm::vec4& color, float speed);

	bool DrawLabel(const char* label, const char* value, bool isDisabled);

	bool DrawTextInput(const char* label, std::string& value, size_t bufferSize = 512);

	bool DrawTextArea(const char* label, std::string& value, float height);

	bool DrawCombo(const char* label, int& value, const char* const* names, int count);

	bool DrawAssetPicker(const char* label, uint64_t assetId, seri::asset::AssetType assetType, uint64_t& selection);
}
