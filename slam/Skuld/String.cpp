//
//	Copyright(c) 2017. zzzzRuby(zzzzRuby@hotmail.com)
//
//
//
//	Permission is hereby granted, free of charge, to any person obtaining a copy
//	of this software and associated documentation files(the "Software"), to deal
//	in the Software without restriction, including without limitation the rights
//	to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
//	copies of the Software, and to permit persons to whom the Software is
//	furnished to do so, subject to the following conditions :
//
//	The above copyright notice and this permission notice shall be included in all
//	copies or substantial portions of the Software.
//
//	THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
//	IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
//	FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE
//	AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
//	LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
//	OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
//	SOFTWARE.


#include <Skuld/String.h>
#include <Skuld/Exception.h>
#include <Skuld/Preprocess.h>
#include <memory>

namespace Skuld
{
	static const char32_t replace_char = 0x0000fffd;
	static const char32_t char32_t_max = 0x0010ffff;
	static const wchar_t utf16_sub_height_start = 0xd800;
	static const wchar_t utf16_sub_height_end = 0xdbff;
	static const wchar_t utf16_sub_low_start = 0xdc00;
	//static const wchar_t utf16_sub_low_end = 0xdfff;
	static const char32_t half_base = 0x0010000;
	static const char32_t half_mask = 0x3ff;
	static const int half_shift = 10;

	static inline size_t u32len(const char32_t* str)
	{
		const char32_t *end = str;
		while (*end++);
		return(end - str - 1);
	}

	static inline size_t u16len(const char16_t* str)
	{
		const char16_t *end = str;
		while (*end++);
		return(end - str - 1);
	}

	std::u32string String::String_UTF8_To_char32_t(const char* str)
	{
		std::u32string mRet;
		char32_t mCache[0x104];
		size_t mCurrent = 0;
		size_t mStrLen = strlen(str);

		for (size_t i = 0; i < mStrLen; i++)
		{
			char32_t& ch = mCache[mCurrent++];

			uint8_t b = static_cast<uint8_t>(str[i]);

			if (b < 0x80)
			{
				ch = b;
			}
			else if (b < 0xC0 || b > 0xFD)
			{
				ch = replace_char;
			}
			else
			{
				size_t len = 0;

				if (b < 0xE0)
				{
					ch = b & 0x1F;
					len = 2;
				}
				else if (b < 0xF0)
				{
					ch = b & 0x0F;
					len = 3;
				}
				else if (b < 0xF8)
				{
					ch = b & 0x07;
					len = 4;
				}
				else if (b < 0xFC)
				{
					ch = b & 0x03;
					len = 5;
				}
				else
				{
					ch = b & 0x01;
					len = 6;
				}

				for (size_t l = 1; l < len; l++)
				{
					b = str[i + l];
					if (b < 0x80 || b > 0xBF)
						throw Exception(U"编码转换失败");
					ch = (ch << 6) + (b & 0x3F);
				}

				i += len - 1;
			}

			if (mCurrent >= 0x100)
			{
				mCache[mCurrent] = 0;
				mRet.append(mCache);
				mCurrent = 0;
			}
		}

		mCache[mCurrent] = 0;
		mRet.append(mCache);
		return mRet;
	}

	//static const uint8_t utf8_mask[7] = { 0x00, 0x00, 0xC0, 0xE0, 0xF0, 0xF8, 0xFC };

	std::string String::String_char32_t_To_UTF8(const char32_t* str)
	{
		std::string mRet;
		char mCache[0x120];
		size_t mCurrent = 0;
		size_t mStrLen = u32len(str);

		for (size_t i = 0; i < mStrLen; i++)
		{
			if (str[i] <= 0x0000007F) mCache[mCurrent++] = (str[i] & 0x7F);
			else if (str[i] >= 0x00000080 && str[i] <= 0x000007FF)
			{
				mCache[mCurrent++] = ((str[i] >> 6) & 0x1F) | 0xC0;
				mCache[mCurrent++] = (str[i] & 0x3F) | 0x80;
			}
			else if (str[i] >= 0x00000800 && str[i] <= 0x0000FFFF)
			{
				mCache[mCurrent++] = ((str[i] >> 12) & 0x0F) | 0xE0;
				mCache[mCurrent++] = ((str[i] >> 6) & 0x3F) | 0x80;
				mCache[mCurrent++] = (str[i] & 0x3F) | 0x80;
			}
			else if (str[i] >= 0x00010000 && str[i] <= 0x001FFFFF)
			{
				mCache[mCurrent++] = ((str[i] >> 18) & 0x07) | 0xF0;
				mCache[mCurrent++] = ((str[i] >> 12) & 0x3F) | 0x80;
				mCache[mCurrent++] = ((str[i] >> 6) & 0x3F) | 0x80;
				mCache[mCurrent++] = (str[i] & 0x3F) | 0x80;
			}
			else if (str[i] >= 0x00200000 && str[i] <= 0x03FFFFFF)
			{
				mCache[mCurrent++] = ((str[i] >> 24) & 0x03) | 0xF8;
				mCache[mCurrent++] = ((str[i] >> 18) & 0x3F) | 0x80;
				mCache[mCurrent++] = ((str[i] >> 12) & 0x3F) | 0x80;
				mCache[mCurrent++] = ((str[i] >> 6) & 0x3F) | 0x80;
				mCache[mCurrent++] = (str[i] & 0x3F) | 0x80;
			}
			else if (str[i] >= 0x04000000 && str[i] <= 0x7FFFFFFF)
			{
				mCache[mCurrent++] = ((str[i] >> 30) & 0x01) | 0xFC;
				mCache[mCurrent++] = ((str[i] >> 24) & 0x3F) | 0x80;
				mCache[mCurrent++] = ((str[i] >> 18) & 0x3F) | 0x80;
				mCache[mCurrent++] = ((str[i] >> 12) & 0x3F) | 0x80;
				mCache[mCurrent++] = ((str[i] >> 6) & 0x3F) | 0x80;
				mCache[mCurrent++] = (str[i] & 0x3F) | 0x80;
			}

			if (mCurrent >= 0x100)
			{
				mCache[mCurrent] = 0;
				mRet.append(mCache);
				mCurrent = 0;
			}
		}

		mCache[mCurrent] = 0;
		mRet.append(mCache);

		return mRet;
	}

	std::u32string String::String_char16_t_To_char32_t(const char16_t* str)
	{
		const size_t mStrLen = u16len(str);
		std::u32string mRet;
		char32_t mCache[0x104] = { 0 };
		size_t mCurrent = 0;
		char32_t ch, ch2;
		for (size_t i = 0; i < mStrLen; i++)
		{
			ch = str[i];
			if (ch >= utf16_sub_height_start && ch <= utf16_sub_height_end)
			{
				if (i + 1 < mStrLen)
				{
					ch2 = str[i + 1];
					if (ch2 >= utf16_sub_low_start && ch2 <= utf16_sub_low_start)
					{
						ch = ((ch - utf16_sub_height_start) << half_shift) + (ch2 - utf16_sub_low_start) + half_base;
						i++;
					}
				}
			}
			mCache[mCurrent++] = (char32_t)ch;
			if (mCurrent >= 0x100)
			{
				mCache[mCurrent] = 0;
				mRet.append(mCache);
				mCurrent = 0;
			}
		}
		mCache[mCurrent] = 0;
		mRet.append(mCache);
		return mRet;
	}

	std::u16string String::String_char32_t_To_char16_t(const char32_t* str)
	{
		std::u16string mRet;
		const size_t mStrLen = u32len(str);
		char16_t mCache[0x104] = { 0 };
		size_t mCurrent = 0;
		for (size_t i = 0; i < mStrLen; i++)
		{
			char32_t ch = str[i];
			if (ch <= 0xffff)
				mCache[mCurrent++] = (ch >= utf16_sub_height_start && ch <= utf16_sub_height_end) ? replace_char : static_cast<char16_t>(ch);
			else if (ch > char32_t_max) mCache[mCurrent++] = replace_char;
			else
			{
				if (i + 1 >= mStrLen)
					throw Exception(U"编码转换失败");
				ch -= half_base;
				mCache[mCurrent++] = static_cast<char16_t>((ch >> half_shift) + utf16_sub_height_start);
				mCache[mCurrent++] = static_cast<char16_t>((ch & half_mask) + utf16_sub_low_start);
			}

			if (mCurrent >= 0x100)
			{
				mCache[mCurrent] = 0;
				mRet.append(mCache);
				mCurrent = 0;
			}
		}
		mCache[mCurrent] = 0;
		mRet.append(mCache);
		return mRet;
	}
}
#if defined(SKULD_PLATFORM_IS_WINDOWS)

#include <Windows.h>

namespace Skuld
{
	static std::wstring String_CurrentCP_To_WChar(const char* str)
	{
		int numChar = MultiByteToWideChar(0, 0, str, -1, NULL, 0);
		std::wstring str2;
		str2.resize(numChar - 1);
		MultiByteToWideChar(0, 0, str, -1, &str2[0], numChar);
		return str2;
	}

	static std::string String_WChar_To_CurrentCP(const wchar_t* str)
	{
		int numChar = ::WideCharToMultiByte(0, 0, str, -1, NULL, 0, NULL, NULL);
		std::string str2;
		str2.resize(numChar - 1);
		WideCharToMultiByte(0, 0, str, -1, &str2[0], numChar, NULL, NULL);
		return str2;
	}

	std::u32string String::String_CurrentCP_To_char32_t(const char* str)
	{
		return String::String_WChar_To_char32_t(String_CurrentCP_To_WChar(str).c_str());
	}

	std::string String::String_char32_t_To_CurrentCP(const char32_t* str)
	{
		return String_WChar_To_CurrentCP(String::String_char32_t_To_WChar(str).c_str());
	}

	std::u32string String::String_WChar_To_char32_t(const wchar_t* str)
	{
		return String::String_char16_t_To_char32_t((const char16_t*)str);
	}

	std::wstring String::String_char32_t_To_WChar(const char32_t* str)
	{
		return (const wchar_t*)(String::String_char32_t_To_char16_t(str).c_str());
	}
}
#else
namespace Skuld
{
	std::u32string String::String_WChar_To_char32_t(const wchar_t* str)
	{
		return (const char32_t*)str;
	}

	std::wstring String::String_char32_t_To_WChar(const char32_t* str)
	{
		return (const wchar_t*)str;
	}

	std::u32string String::String_CurrentCP_To_char32_t(const char* str)
	{
		return String::String_UTF8_To_char32_t(str);
	}

	std::string String::String_char32_t_To_CurrentCP(const char32_t* str)
	{
		return String::String_char32_t_To_UTF8(str);
	}
}
#endif
