#include "ilargipch.h"

#include "UUID.h"

#include <random>

namespace Ilargi
{
	static std::random_device randomDevice;
	static std::mt19937_64 engine(randomDevice());
	static std::uniform_int_distribution<uint64_t> uniformDistribution;

	UUID::UUID() : uuid(uniformDistribution(engine))
	{
	}
}