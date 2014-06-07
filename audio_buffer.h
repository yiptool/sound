/* vim: set ai noet ts=4 sw=4 tw=115: */
//
// Copyright (c) 2014 Nikolay Zapolnov (zapolnov@gmail.com).
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.
//
#ifndef __21256e4f6d1e9633c5cc9b375c66bbcb__
#define __21256e4f6d1e9633c5cc9b375c66bbcb__

#include <yip-imports/cxx-util/macros.h>
#include <vector>
#include <memory>

class AudioBuffer
{
public:
	inline AudioBuffer() noexcept {}
	inline AudioBuffer(size_t size) noexcept : m_Data(size) {}

	inline void resize(size_t size) noexcept { m_Data.resize(size); }

	inline void * data() noexcept { return m_Data.data(); }
	inline const void * data() const noexcept { return m_Data.data(); }

private:
	std::vector<char> m_Data;
};

typedef std::shared_ptr<AudioBuffer> AudioBufferPtr;

#endif
