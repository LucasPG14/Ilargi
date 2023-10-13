#include "ilargipch.h"

#include "FileSystem.h"

namespace Ilargi
{
	Buffer FileSystem::ReadBinaryFile(const std::filesystem::path& filepath)
	{
		Buffer buffer = {};

		std::ifstream file(filepath, std::ios::in | std::ios::binary);

		if (file.is_open())
		{
			file.seekg(0, std::ios::end);
			buffer.size = file.tellg();
			buffer.data = new char[buffer.size];
			
			file.seekg(0, std::ios::beg);
			file.read((char*)buffer.data, buffer.size);
			file.close();
		}

		return buffer;
	}
	
	void FileSystem::WriteBinaryFile(const std::filesystem::path& filepath, const Buffer& buffer)
	{
		std::ofstream file(filepath, std::ios::out | std::ios::binary);

		if (file.is_open())
		{
			file.write((char*)buffer.data, buffer.size);
			file.flush();
			file.close();
		}
	}
}