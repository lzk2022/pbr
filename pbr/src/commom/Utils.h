#pragma once
#include <string>
#include <vector>

class File
{
public:
	static std::string ReadText(const std::string& filename);
	static std::vector<char> ReadBinary(const std::string& filename);
};

class Utility
{
public:
	template<typename T> static constexpr bool IsPowerOfTwo(T value)
	{
		return value != 0 && (value & (value - 1)) == 0;
	}
	template<typename T> static constexpr T RoundToPowerOfTwo(T value, int POT)
	{
		return (value + POT - 1) & -POT;
	}
	template<typename T> static constexpr T NumMipmapLevels(T width, T height)
	{
		T levels = 1;
		while((width|height) >> levels) 
		{
			++levels;
		}
		return levels;
	}

#if _WIN32
	static std::string ConvertToUTF8(const std::wstring& wstr);
	static std::wstring ConvertToUTF16(const std::string& str);
#endif // _WIN32
};
