#pragma once

namespace Ilargi
{
	struct Buffer
	{
		void* data = nullptr;
		size_t size = 0;
	};

	class FileSystem
	{
	public:
		static Buffer ReadBinaryFile(const std::filesystem::path& file);
		static void WriteBinaryFile(const std::filesystem::path& file, const Buffer& buffer);
	};
}