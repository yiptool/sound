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
#ifndef __a16676bd4b0d710b6e53803dfcde9183__
#define __a16676bd4b0d710b6e53803dfcde9183__

#include "audio_buffer.h"
#include <memory>

class AudioOutput
{
public:
	typedef unsigned long long Offset;

	virtual bool needMoreBuffers() const = 0;
	virtual void enqueueBuffer(const AudioBufferPtr & buffer) = 0;

protected:
	inline AudioOutput() {}
	virtual inline ~AudioOutput() {}

	AudioOutput(const AudioOutput &) = delete;
	AudioOutput & operator=(const AudioOutput &) = delete;
};

typedef std::shared_ptr<AudioOutput> AudioOutputPtr;

#endif
