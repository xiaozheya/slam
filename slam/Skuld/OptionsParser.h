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

#include "String.h"
#include <map>

namespace Skuld
{
	class OptionsVariableMap
	{
	private:
		std::map<String, std::vector<String>> mVarMap;
		friend class OptionsParser;
	public:
		const String& operator()(const String& mKey, size_t mIndex = 0) const;
		bool Has(const String& mKey) const;
	};

	class OptionsParser
	{
	private:
		struct Option
		{
			String mKey;
			String mHelp;
			int mParamCount;
			char32_t mShortening;
		};

		String mAppName;
		std::vector<Option> mOpts;
	public:
		OptionsParser(const String& mAppName) : mAppName(mAppName) {}

		static const int UnknowParamCount = -1;

		friend std::ostream& operator<<(std::ostream& stream, const OptionsParser& mParser);
		friend std::wostream& operator<<(std::wostream & stream, const OptionsParser & mParser);

		OptionsParser& operator()(const String& mKey, const String& mHelp, int mParamCount = 0);
		OptionsParser& operator()(const String& mKey, char mShortening, const String& mHelp, int mParamCount = 0);

		void Parse(const String* mArgv, int mArgc, OptionsVariableMap& mMap);
	};
}