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

#include <Skuld/OptionsParser.h>
#include <Skuld/Exception.h>
#include <algorithm>

namespace Skuld
{
	std::ostream & operator<<(std::ostream & stream, const OptionsParser & mParser)
	{
		stream << mParser.mAppName << ":" << std::endl;
		for (auto& it : mParser.mOpts)
		{
			size_t mTotalSize = 4;
			stream << "  ";
			if (it.mShortening != '\0')
			{
				stream << "-" << static_cast<char>(it.mShortening) << ",";
				mTotalSize += 3;
			}
			stream << "--" << it.mKey;
			for (size_t i = 0; i < it.mKey.CharCount(); i++)
			{
				if (it.mKey[i] <= 256) mTotalSize += 1;
				else mTotalSize += 2;
			}

			if (it.mParamCount < 0)
			{
				mTotalSize += 4;
				stream << " ...";
			}
			else if (it.mParamCount > 0)
			{
				for (int i = 0; i < it.mParamCount; i++)
				{
					mTotalSize += 4;
					stream << " arg";
				}
			}

			if (mTotalSize >= 32)
			{
				stream << std::endl;
				stream << std::string(32, ' ') << it.mHelp;
			}
			else
				stream << std::string(32 - mTotalSize, ' ') << it.mHelp;

			stream << std::endl;
		}

		return stream;
	}

	std::wostream & operator<<(std::wostream & stream, const OptionsParser & mParser)
	{
		stream << mParser.mAppName << L":" << std::endl;
		for (auto& it : mParser.mOpts)
		{
			size_t mTotalSize = 4;
			stream << L"  ";
			if (it.mShortening != '\0')
			{
				stream << L"-" << static_cast<char>(it.mShortening) << L",";
				mTotalSize += 3;
			}
			stream << L"--" << it.mKey;
			for (size_t i = 0; i < it.mKey.CharCount(); i++)
			{
				if (it.mKey[i] <= 256) mTotalSize += 1;
				else mTotalSize += 2;
			}

			if (it.mParamCount < 0)
			{
				mTotalSize += 4;
				stream << L" ...";
			}
			else if (it.mParamCount > 0)
			{
				for (int i = 0; i < it.mParamCount; i++)
				{
					mTotalSize += 4;
					stream << L" arg";
				}
			}

			if (mTotalSize >= 32)
			{
				stream << std::endl;
				stream << std::wstring(32, ' ') << it.mHelp;
			}
			else
				stream << std::wstring(32 - mTotalSize, ' ') << it.mHelp;

			stream << std::endl;
		}

		return stream;
	}

	OptionsParser & OptionsParser::operator()(const String & mKey, const String & mHelp, int mParamCount)
	{
		mOpts.push_back(OptionsParser::Option());
		OptionsParser::Option& i = mOpts[mOpts.size() - 1];
		i.mHelp = mHelp;
		i.mKey = mKey;
		i.mShortening = '\0';
		i.mParamCount = mParamCount;
		return *this;
	}

	OptionsParser & OptionsParser::operator()(const String & mKey, char mShortening, const String & mHelp, int mParamCount)
	{
		mOpts.push_back(OptionsParser::Option());
		OptionsParser::Option& i = mOpts[mOpts.size() - 1];
		i.mHelp = mHelp;
		i.mKey = mKey;
		if ((mShortening >= 'a' && mShortening <= 'z') ||
			(mShortening >= 'A' && mShortening <= 'Z') ||
			(mShortening >= '0' && mShortening <= '9'))
			i.mShortening = mShortening;
		else i.mShortening = '\0';
		i.mParamCount = mParamCount;
		return *this;
	}

	void OptionsParser::Parse(const String * mArgv, int mArgc, OptionsVariableMap & mMap)
	{
		auto& mVarMap = mMap.mVarMap;
		std::vector<String>* mCurrentOpt = nullptr;
		for (int i = 1; i < mArgc; i++)
		{
			if (mArgv[i][0] == U'-')
			{
				Option * mOpt = nullptr;
				if (mArgv[i][1] == U'-')
				{
					String mToken = mArgv[i].Substr(2);
					for (auto& it : mOpts)
					{
						if (it.mKey == mToken)
						{
							mOpt = &it;
							break;
						}
					}
				}
				else
				{
					String mToken = mArgv[i].Substr(1);
					if (mToken.CharCount() != 1) throw Exception(String(U"unrecognised option " + mArgv[i]));

					for (auto& it : mOpts)
					{
						if (it.mShortening == mToken[0])
						{
							mOpt = &it;
							break;
						}
					}
				}

				if (!mOpt) throw Exception(String(U"unrecognised option ") + mArgv[i]);

				if (mOpt->mParamCount < 0)
					mCurrentOpt = &mMap.mVarMap[mOpt->mKey];
				else
				{
					if (mMap.mVarMap.find(mOpt->mKey) == mMap.mVarMap.end())
					{
						mCurrentOpt = nullptr;
						auto& mDst = mVarMap[mOpt->mKey];
						for (int j = 0; j < std::min(mArgc - i - 1, mOpt->mParamCount); j++)
							mDst.push_back(mArgv[i + j + 1]);
						i += mOpt->mParamCount;
					}
					else throw Exception(String(U"reduplicate option ") + mArgv[i]);
				}
			}
			else
			{
				if (mCurrentOpt)
					mCurrentOpt->push_back(mArgv[i]);
				else throw Exception(String(U"unrecognised option ") + mArgv[i]);
			}
		}
	}

	const String & OptionsVariableMap::operator()(const String & mKey, size_t mIndex) const
	{
		std::map<String, std::vector<String>>::const_iterator it = mVarMap.find(mKey);
		if (it != mVarMap.end()) return it->second[mIndex];
		throw Exception(U"Out of range");
	}

	bool OptionsVariableMap::Has(const String & mKey) const
	{
		return mVarMap.find(mKey) != mVarMap.end();
	}
}