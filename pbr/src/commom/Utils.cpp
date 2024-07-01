#include <fstream>
#include <sstream>
#include <memory>
#include "Log.h"

#if _WIN32
#include <Windows.h>
#endif // _WIN32

#include "Utils.h"

std::string File::ReadText(const std::string& filename)
{
	std::ifstream file{filename};
	LOG_ASSERT(!file.is_open(), "Could not open file: " + filename);

	std::stringstream buffer;
	buffer << file.rdbuf();
	return buffer.str();
}
	
std::vector<char> File::ReadBinary(const std::string& filename)
{
	std::ifstream file{filename, std::ios::binary | std::ios::ate};
	LOG_ASSERT(!file.is_open(), "Could not open file: " + filename);

	std::streamsize size = file.tellg();
	file.seekg(0, std::ios::beg);

	std::vector<char> buffer(size);
	file.read(buffer.data(), size);
	return buffer;
}

#if _WIN32
std::string Utility::ConvertToUTF8(const std::wstring& wstr)
{
	const int bufferSize = WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), -1, nullptr, 0, nullptr, nullptr);
	const std::unique_ptr<char[]> buffer(new char[bufferSize]);
	WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), -1, buffer.get(), bufferSize, nullptr, nullptr);
	return std::string(buffer.get());
}

std::wstring Utility::ConvertToUTF16(const std::string& str)
{
	const int bufferSize = MultiByteToWideChar(CP_UTF8, 0, str.c_str(), -1, nullptr, 0);
	const std::unique_ptr<wchar_t[]> buffer(new wchar_t[bufferSize]);
	MultiByteToWideChar(CP_UTF8, 0, str.c_str(), -1, buffer.get(), bufferSize);
	return std::wstring(buffer.get());
}
#endif // _WIN32
