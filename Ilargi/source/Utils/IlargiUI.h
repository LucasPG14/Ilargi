#pragma once

namespace Ilargi::UI
{
	void Checkbox(std::string_view name, bool* value);

	void Slider(std::string_view name, float* value, float min = 0.0f, float max = 1000.0f);

	void IlargiStyle();
}