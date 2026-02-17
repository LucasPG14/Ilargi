#pragma once

namespace Ilargi
{
	struct BinaryWriter
	{
		BinaryWriter(const std::string& aPath) : data(aPath, std::ios::binary) {}
		BinaryWriter(const std::filesystem::path& aPath) : data(aPath, std::ios::binary) {}

		void Write(const void* aData, size_t aBytes)
		{
			data.write(reinterpret_cast<const char*>(aData), aBytes);
			size += aBytes;
		}

		template<typename T>
		void Write(const T& aValue)
		{
			Write(&aValue, sizeof(T));
		}

		template<typename T>
		void Write(const std::vector<T>& aVector)
		{
			Write(aVector.data(), sizeof(T) * aVector.size());
		}

		void WriteString(const std::string& aString)
		{
			size_t length{ aString.length() };
			Write(length);
			Write(aString.data(), length);
		}

		template<typename T>
		void WriteVector(const std::vector<T>& aVector)
		{
			Write(aVector.size());
			for (const auto& v : aVector)
				Write(v);
		}

	private:
		std::ofstream data;
		size_t size = 0;
	};

	struct BinaryReader
	{
		BinaryReader(const std::string& aPath) : data(aPath, std::ios::binary)
		{
			ILG_ASSERT(data.is_open(), "The file doesn't exist or couldn't be open");
			data.seekg(0, std::ios::end);
			size = data.tellg();
			data.seekg(0, std::ios::beg);
		}

		BinaryReader(const std::filesystem::path& aPath) : data(aPath, std::ios::binary)
		{
			ILG_ASSERT(data.is_open(), "The file doesn't exist or couldn't be open");
			data.seekg(0, std::ios::end);
			size = data.tellg();
			data.seekg(0, std::ios::beg);
		}

		void Read(void* aData, size_t aBytes)
		{
			data.read(reinterpret_cast<char*>(aData), aBytes);
		}

		template<typename T>
		void Read(T& aData)
		{
			Read(&aData, sizeof(T));
		}

		template<typename T>
		void Read(std::vector<T>& aVector)
		{
			Read(aVector.data(), sizeof(T) * aVector.size());
		}

		void ReadString(std::string& aString)
		{
			size_t length;
			Read(length);
			aString.resize(length);
			Read(aString.data(), length);
		}

		template<typename T>
		void ReadVector(std::vector<T>& aVector)
		{
			size_t size;
			Read(size);
			aVector.resize(size);
			for (auto& v : aVector)
			{
				Read(v);
			}
		}

		const size_t GetSize() const { return size; }

	private:
		std::ifstream data;
		size_t size = 0;
	};

	class FileSystem
	{
	public:
		/*
		* @brief Opens a open file menu.
		* @param aFilter Filter of files to show.
		* @return The name of the file to open.
		*/
		static std::string OpenFile(const char* aFilter);
		
		/*
		* @brief Opens a save file menu.
		* @param aFilter Filter of files to show.
		* @return The name of the file to save.
		*/
		static std::string SaveFile(const char* aFilter);

		/*
		* @brief Opens a browser with the URL specified.
		* @param aURL The URL of the website to open.
		*/
		static void OpenWeb(const std::wstring& aURL);
	};
}