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



#pragma once
#include <iostream>
#include <vector>
#include <string>
#include <string.h>

namespace Skuld
{
	class String
	{
	protected:
		std::u32string mChars;
		static std::u32string String_WChar_To_char32_t(const wchar_t* str);
		static std::wstring String_char32_t_To_WChar(const char32_t* str);
		static std::u32string String_char16_t_To_char32_t(const char16_t* str);
		static std::u16string String_char32_t_To_char16_t(const char32_t* str);
		static std::u32string String_CurrentCP_To_char32_t(const char* str);
		static std::string String_char32_t_To_CurrentCP(const char32_t* str);
		static std::u32string String_UTF8_To_char32_t(const char* str);
		static std::string String_char32_t_To_UTF8(const char32_t* str);

		friend class StringBuilder;
	public:
		static bool IsLetter(char32_t mChar) {
			return (mChar <= U'Z' && mChar >= U'A')
				|| (mChar <= U'z' && mChar >= U'a');
		}
		char32_t operator[](size_t mIndex) const {
			return this->mChars[mIndex];
		}
		char32_t& operator[](size_t mIndex) {
			return this->mChars[mIndex];
		}
		const char32_t* GetChars() const { return this->mChars.c_str(); }
		std::string GetStr() const {
			return String_char32_t_To_CurrentCP(this->mChars.c_str());
		}
		std::wstring GetWStr() const {
			return String_char32_t_To_WChar(this->mChars.c_str());
		}
		std::string GetUTF8Str() const {
			return String_char32_t_To_UTF8(this->mChars.c_str());
		}
		std::u16string GetUTF16Str() const {
			return String_char32_t_To_char16_t(this->mChars.c_str());
		}
		std::u32string GetUTF32Str() const {
			return mChars;
		}
		std::vector<String> Spilt(const char32_t * const chars, size_t char_count, bool trim = false) const {
			const char32_t * c = this->mChars.c_str();
			uint32_t ptr = 0;
			uint32_t size = 0;
			std::vector<String> ret;
			while (*c)
			{
				bool find = false;
				for (uint32_t i = 0;i < char_count;i++)
				{
					if (chars[i] == *c)
					{
						if (size || !trim) ret.push_back(this->Substr(ptr, size));
						ptr += (size + 1);
						size = 0;
						c++;
						find = true;
						break;
					}
				}
				if (!find)
				{
					c++;
					size++;
				}
			}
			if (size || !trim) ret.push_back(this->Substr(ptr, size));
			return ret;
		}
		std::vector<String> Spilt(const String& chars, bool trim = false) const {
			return this->Spilt(chars.mChars.c_str(), chars.CharCount(), trim);
		}
		std::vector<String> Spilt(char32_t charater, bool trim = false) const {
			return this->Spilt(&charater, 1, trim);
		}

		String Replace(char32_t origin, char32_t replace_char) const {
			size_t size = this->mChars.size();
			String ret;
			ret.mChars.resize(size);
			for (uint32_t i = 0;i < size;i++)
			{
				if (this->mChars[i] == origin) ret.mChars[i] = replace_char;
				else ret.mChars[i] = this->mChars[i];
			}
			return ret;
		}

		String ToUpper() const {
			size_t size = this->mChars.size();
			String ret;
			ret.mChars.resize(size);
			for (size_t i = 0;i < size;i++)
			{
				if (this->mChars[i] >= U'a' && this->mChars[i] <= U'z')
					ret.mChars[i] = this->mChars[i] + U'A' - U'a';
				else ret.mChars[i] = this->mChars[i];
			}
			return ret;
		}
		String ToLower() const {
			size_t size = this->mChars.size();
			String ret;
			ret.mChars.resize(size);
			for (size_t i = 0;i < size;i++)
			{
				if (this->mChars[i] >= U'A' && this->mChars[i] <= U'Z')
					ret.mChars[i] = this->mChars[i] + U'a' - U'A';
				else ret.mChars[i] = this->mChars[i];
			}
			return ret;
		}

		size_t CharCount() const {
			return this->mChars.size();
		}
		bool Has(const String& str) const {
			return mChars.find(str.mChars) != -1;
		}
		bool StartWith(const String& str) const {
			return (str.mChars.size() <= this->mChars.size()) ?
				memcmp(str.mChars.c_str(), this->mChars.c_str(), sizeof(char32_t) * str.mChars.size()) == 0 : false;
		}
		bool EndWith(const String& str) const {
			return (str.mChars.size() <= this->mChars.size()) ?
				memcmp(str.mChars.c_str(), &this->mChars[mChars.size() - str.mChars.size()],
					sizeof(char32_t) * mChars.size()) == 0 : false;
		}
		String(const wchar_t * str) {
			this->mChars = String_WChar_To_char32_t(str);
		}
		String(const char32_t * str) {
			this->mChars = str;
		}
		String(const char16_t * str) {
			this->mChars = String_char16_t_To_char32_t(str);
		}
		String(const char* str) {
			this->mChars = String_CurrentCP_To_char32_t(str);
		}
		String() : mChars(U"") {}
		String(const String& str) {
			this->mChars = str.mChars;
		}
		String(String&& str) {
			this->mChars = std::move(str.mChars);
		}
		String& operator=(const String& str) {
			if (this != &str)
				this->mChars = str.mChars;
			return *this;
		}
		String& operator+=(const String& str_a) {
			this->mChars += str_a.mChars;
			return *this;
		}
		String Substr(size_t start, size_t count = -1) const {
			String ret;
			ret.mChars = this->mChars.substr(start, count);
			return ret;
		}
		friend String operator+(const String& str_a, const String& str_b) {
			String ret;
			ret.mChars = str_a.mChars + str_b.mChars;
			return ret;
		}
		friend bool operator==(const String& str_a, const String& str_b) {
			return str_a.mChars == str_b.mChars;
		}
		friend bool operator!=(const String& str_a, const String& str_b) {
			return str_a.mChars != str_b.mChars;
		}
		friend bool operator<(const String& str_a, const String& str_b) {
			return str_a.mChars < str_b.mChars;
		}
		friend bool operator>(const String& str_a, const String& str_b) {
			return str_a.mChars > str_b.mChars;
		}
		friend bool operator<=(const String& str_a, const String& str_b) {
			return str_a.mChars <= str_b.mChars;
		}
		friend bool operator>=(const String& str_a, const String& str_b) {
			return str_a.mChars >= str_b.mChars;
		}
	};

	class UTF8String : public String
	{
	public:
		UTF8String(const char* str) { this->mChars = String_UTF8_To_char32_t(str); }
	};

	static inline std::wostream& operator<<(std::wostream& stream, const String& str)
	{
		stream << str.GetWStr();
		return stream;
	}

	static inline std::wistream& operator >> (std::wistream& stream, String& str)
	{
		std::wstring _str;
		stream >> _str;
		str = _str.c_str();
		return stream;
	}

	static inline std::ostream& operator<<(std::ostream& stream, const String& str)
	{
		stream << str.GetStr();
		return stream;
	}

	static inline std::istream& operator >> (std::istream& stream, String& str)
	{
		std::string _str;
		stream >> _str;
		str = _str.c_str();
		return stream;
	}
	
	template<typename T> T ToStdStr(const String& str) { static_assert(!(true || std::is_class<T>::value), "T is not a std string"); }

	template<> inline std::string ToStdStr<std::string>(const String& str) { return str.GetStr(); }
	template<> inline std::wstring ToStdStr<std::wstring>(const String& str) { return str.GetWStr(); }
	template<> inline std::u16string ToStdStr<std::u16string>(const String& str) { return str.GetUTF16Str(); }
	template<> inline std::u32string ToStdStr<std::u32string>(const String& str) { return str.GetUTF32Str(); }
}