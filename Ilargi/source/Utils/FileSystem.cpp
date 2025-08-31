#include "ilargipch.h"

#include "FileSystem.h"
#include "Base/Application.h"

#include <Windows.h>
#include <GLFW/glfw3.h>
#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3native.h>

namespace Ilargi
{
	Buffer FileSystem::ReadBinaryFile(const std::filesystem::path& filepath)
	{
		Buffer buffer {};

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
	
	std::string FileSystem::OpenFile(const char* filter)
	{
		OPENFILENAMEA openFile;
		CHAR sizeFile[256] { 0 };
		ZeroMemory(&openFile, sizeof(OPENFILENAMEA));
		openFile.lStructSize = sizeof(OPENFILENAMEA);
		openFile.hwndOwner = glfwGetWin32Window(Application::Get()->GetWindow().GetWindow());
		openFile.lpstrFile = sizeFile;
		openFile.nMaxFile = sizeof(sizeFile);
		openFile.lpstrFilter = filter;
		openFile.nFilterIndex = 1;
		openFile.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_NOCHANGEDIR;
		if (GetOpenFileNameA(&openFile) == TRUE)
		{
			return openFile.lpstrFile;
		}

		return std::string();
	}
	
	std::string FileSystem::SaveFile(const char* filter)
	{
		OPENFILENAMEA openFile;
		CHAR sizeFile[256] { 0 };
		ZeroMemory(&openFile, sizeof(OPENFILENAMEA));
		openFile.lStructSize = sizeof(OPENFILENAMEA);
		openFile.hwndOwner = glfwGetWin32Window(Application::Get()->GetWindow().GetWindow());
		openFile.lpstrFile = sizeFile;
		openFile.nMaxFile = sizeof(sizeFile);
		openFile.lpstrDefExt = LPCSTR(".ilargi");
		openFile.lpstrFilter = filter;
		openFile.nFilterIndex = 1;
		openFile.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_NOCHANGEDIR;
		if (GetSaveFileNameA(&openFile) == TRUE)
		{
			return openFile.lpstrFile;
		}

		return std::string();
	}
}