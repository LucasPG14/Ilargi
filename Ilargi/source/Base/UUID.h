#pragma once

#include <xhash>

namespace Ilargi
{
	class UUID
	{
	public:
		UUID();
		UUID(uint64_t id);
		UUID(const UUID&) = default;

		operator uint64_t() const { return uuid; }
	private:
		uint64_t uuid;
	};
}

namespace std
{
	template<>
	struct hash<Ilargi::UUID>
	{
		std::size_t operator()(const Ilargi::UUID& uuid) const
		{
			return hash<uint64_t>()((uint64_t)uuid);
		}
	};
}