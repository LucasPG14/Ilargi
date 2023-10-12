#pragma once

namespace Ilargi
{
	class Shader
	{
	public:

		static std::shared_ptr<Shader> Create(std::string_view vert);
	};
}