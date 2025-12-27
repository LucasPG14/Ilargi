#pragma once

namespace Ilargi
{
	struct Buffer
	{
		char* data = nullptr;
		size_t size = 0;
	};

	class FileSystem
	{
	public:
		static Buffer ReadBinaryFile(const std::filesystem::path& aFilepath);
		static void WriteBinaryFile(const std::filesystem::path& aFilepath, const Buffer& aBuffer);

		static std::string OpenFile(const char* aFilter);
		static std::string SaveFile(const char* aFilter);

		static void OpenApp();
	};
}