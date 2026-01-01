#pragma once

#include <xhash>

namespace Ilargi
{
	class UUID
	{
	public:
		UUID();
		UUID(uint64_t aUUID);
		UUID(const UUID&) = default;

		operator uint64_t() const { return mUUID; }
	private:
		uint64_t mUUID; // Identifier.
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