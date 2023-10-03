#pragma once

typedef int ImGuiTreeNodeFlags;

namespace Ilargi::UI
{
	void Checkbox(std::string_view name, bool* value);
	void Slider(std::string_view name, float* value, float min = 0.0f, float max = 1000.0f);

	bool BeginTreeNode(void* id, std::string name, ImGuiTreeNodeFlags flags = 0);
	void EndTreeNode(void* id, bool open);
	
	bool BeginCollapsingHeader(void* id, std::string name);
	void EndCollapsingHeader(void* id);

	void IlargiStyle();
}