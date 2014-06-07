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
#include "openal_output.h"
#include <iostream>

OpenALOutput::OpenALOutput(OpenALDevice & device, AudioFormat format, size_t hz)
	: m_Device(device),
	  m_Frequency(ALsizei(hz)),
	  m_Format(AL_NONE),
	  m_Source(0)
{
	m_Device.bindContext();

	switch (format)
	{
	case AudioMono8: m_Format = AL_FORMAT_MONO8; break;
	case AudioMono16: m_Format = AL_FORMAT_MONO16; break;
	case AudioStereo8: m_Format = AL_FORMAT_STEREO8; break;
	case AudioStereo16: m_Format = AL_FORMAT_STEREO16; break;
	}

	alGenSources(1, &m_Source);
	OpenALDevice::checkError(__FILE__, __LINE__ - 1, "alGenSources");

	alSourcei(m_Source, AL_LOOPING, AL_FALSE);
	OpenALDevice::checkError(__FILE__, __LINE__ - 1, "alSourcei(AL_LOOPING, AL_FALSE)");
}

OpenALOutput::~OpenALOutput()
{
	m_Device.bindContext();

	alDeleteSources(1, &m_Source);
	OpenALDevice::checkError(__FILE__, __LINE__ - 1, "alDeleteSources");

	alDeleteBuffers(ALsizei(m_Buffers.size()), m_Buffers.data());
	OpenALDevice::checkError(__FILE__, __LINE__ - 1, "alDeleteBuffers");
}

bool OpenALOutput::needMoreBuffers() const
{
	m_Device.bindContext();

	ALint numBuffers = 0;
	alGetSourcei(m_Source, AL_BUFFERS_PROCESSED, &numBuffers);
	OpenALDevice::checkError(__FILE__, __LINE__ - 1, "alGetSourcei(AL_BUFFERS_PROCESSED)");

	return numBuffers != 0;
}

void OpenALOutput::enqueueBuffer(const AudioBufferPtr & buffer)
{
	m_Device.bindContext();

	ALint numBuffers = 0;
	alGetSourcei(m_Source, AL_BUFFERS_PROCESSED, &numBuffers);
	OpenALDevice::checkError(__FILE__, __LINE__ - 1, "alGetSourcei(AL_BUFFERS_PROCESSED)");

	ALuint bufferHandle = 0;
	if (numBuffers > 0)
	{
		alSourceUnqueueBuffers(m_Source, 1, &bufferHandle);
		OpenALDevice::checkError(__FILE__, __LINE__ - 1, "alSourceUnqueueBuffers");
	}

	if (bufferHandle == 0)
	{
		alGenBuffers(1, &bufferHandle);
		OpenALDevice::checkError(__FILE__, __LINE__ - 1, "alGenBuffers");
		if (bufferHandle != 0)
			m_Buffers.push_back(bufferHandle);
	}

	alBufferData(bufferHandle, m_Format, buffer->data(), ALsizei(buffer->size()), m_Frequency);
	OpenALDevice::checkError(__FILE__, __LINE__ - 1, "alBufferData");

	alSourceQueueBuffers(m_Source, 1, &bufferHandle);
	OpenALDevice::checkError(__FILE__, __LINE__ - 1, "alSourceQueueBuffers");

	ALint state = AL_INITIAL;
	alGetSourcei(m_Source, AL_SOURCE_STATE, &state);
	OpenALDevice::checkError(__FILE__, __LINE__ - 1, "alGetSourcei(AL_SOURCE_STATE)");

	if (state != AL_PLAYING)
	{
		alSourcePlay(m_Source);
		OpenALDevice::checkError(__FILE__, __LINE__ - 1, "alSourcePlay");
	}
}

AudioOutput::Offset OpenALOutput::currentSample() const
{
	m_Device.bindContext();

	ALint offset = 0;
	alGetSourcei(m_Source, AL_SAMPLE_OFFSET, &offset);
	OpenALDevice::checkError(__FILE__, __LINE__ - 1, "alGetSourcei(AL_SAMPLE_OFFSET)");

	return offset;
}
