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

namespace Skuld
{
	class Exception
	{
	protected:
		String _msg;
	public:
		explicit Exception(const String& msg) : _msg(msg) {}
		explicit Exception(const char* msg) : _msg(msg) {}
		explicit Exception(const wchar_t* msg) : _msg(msg) {}
		explicit Exception(const char32_t* msg) : _msg(msg) {}
		explicit Exception(const char16_t* msg) : _msg(msg) {}
		virtual String Message() {
			return this->_msg;
		}
		virtual ~Exception() {}
	};
}