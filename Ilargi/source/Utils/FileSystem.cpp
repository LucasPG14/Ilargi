#include "ilargipch.h"

#include "FileSystem.h"
#include "Base/Application.h"

#include <windows.h>
#include <GLFW/glfw3.h>
#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3native.h>

namespace Ilargi
{	
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

	void FileSystem::OpenApp()
	{
		STARTUPINFOW si;
		PROCESS_INFORMATION pi;

		ZeroMemory(&si, sizeof(si));
		si.cb = sizeof(si);
		ZeroMemory(&pi, sizeof(pi));

		wchar_t comando[] = L"C:/Users/lucas/Documents/GitHub/Ilargi/IlargiEditor/IlargiEditor.exe -project Hola";

		BOOL ok = CreateProcessW(
			NULL,
			comando,
			NULL,
			NULL,
			FALSE,
			0,
			NULL,
			NULL,
			&si,
			&pi
		);

		if (!ok)
		{
			MessageBoxW(NULL, L"No se pudo abrir la calculadora", L"Error", MB_OK);
			return;
		}

		CloseHandle(pi.hProcess);
		CloseHandle(pi.hThread);
	}
}