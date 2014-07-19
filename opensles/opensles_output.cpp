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
#include "opensles_output.h"
#include <iostream>

OpenSLESOutput::OpenSLESOutput(OpenSLESDevice & device, AudioFormat format, size_t hz)
	: m_Device(device),
	  m_PlayerObj(nullptr),
	  m_Play(nullptr),
	  m_BufferQueue(nullptr),
	  m_Volume(nullptr),
	  m_NeedMoreBuffers(2)
{
	if (UNLIKELY(!m_Device.m_Engine))
		return;

	bool isMono = (format == AudioMono8 || format == AudioMono16);
	bool is16Bit = (format == AudioMono16 || format == AudioStereo16);

	SLDataFormat_PCM formatPCM;
	formatPCM.formatType = SL_DATAFORMAT_PCM;
	formatPCM.numChannels = (isMono ? 1 : 2);
	formatPCM.samplesPerSec = hz * 1000;
	formatPCM.bitsPerSample = (is16Bit ? SL_PCMSAMPLEFORMAT_FIXED_16 : SL_PCMSAMPLEFORMAT_FIXED_8);
	formatPCM.containerSize = formatPCM.bitsPerSample;
	formatPCM.channelMask = (isMono ? SL_SPEAKER_FRONT_CENTER : SL_SPEAKER_FRONT_LEFT | SL_SPEAKER_FRONT_RIGHT);
	formatPCM.endianness = SL_BYTEORDER_LITTLEENDIAN;

	SLDataLocator_BufferQueue locatorBufferQueue = { SL_DATALOCATOR_BUFFERQUEUE, 2 };
	SLDataSource audioSrc = { &locatorBufferQueue, &formatPCM };

	SLDataLocator_OutputMix locatorOutMix = { SL_DATALOCATOR_OUTPUTMIX, device.m_OutputMixObj };
	SLDataSink audioSnk = { &locatorOutMix, nullptr };

	const SLInterfaceID pPlayerIDs[3] = { SL_IID_BUFFERQUEUE, SL_IID_VOLUME, SL_IID_PLAY };
	const SLboolean pPlayerIDsRequired[3] = { SL_BOOLEAN_TRUE, SL_BOOLEAN_TRUE, SL_BOOLEAN_TRUE };

	SLresult result = (*device.m_Engine)->CreateAudioPlayer(device.m_Engine, &m_PlayerObj, &audioSrc, &audioSnk,
		3, pPlayerIDs, pPlayerIDsRequired);
	if (UNLIKELY(result != SL_RESULT_SUCCESS))
	{
		std::clog << "Sound: unable to allocate OpenSL audio player." << std::endl;
		return;
	}

	result = (*m_PlayerObj)->Realize(m_PlayerObj, SL_BOOLEAN_FALSE);
	if (UNLIKELY(result != SL_RESULT_SUCCESS))
	{
		std::clog << "Sound: unable to initialize OpenSL audio player." << std::endl;
		return;
	}

	result = (*m_PlayerObj)->GetInterface(m_PlayerObj, SL_IID_PLAY, &m_Play);
	if (UNLIKELY(result != SL_RESULT_SUCCESS))
	{
		std::clog << "Sound: unable to get OpenSL audio player interface." << std::endl;
		return;
	}

	result = (*m_PlayerObj)->GetInterface(m_PlayerObj, SL_IID_BUFFERQUEUE, &m_BufferQueue);
	if (UNLIKELY(result != SL_RESULT_SUCCESS))
	{
		std::clog << "Sound: unable to get OpenSL buffer queue interface." << std::endl;
		return;
	}

	result = (*m_PlayerObj)->GetInterface(m_PlayerObj, SL_IID_VOLUME, &m_Volume);
	if (UNLIKELY(result != SL_RESULT_SUCCESS))
	{
		std::clog << "Sound: unable to get OpenSL volume interface." << std::endl;
		return;
	}

	result = (*m_BufferQueue)->RegisterCallback(m_BufferQueue, callback, this);
	if (UNLIKELY(result != SL_RESULT_SUCCESS))
	{
		std::clog << "Sound: unable to register OpenSL buffer queue callback." << std::endl;
		return;
	}

	result = (*m_Play)->SetPlayState(m_Play, SL_PLAYSTATE_PLAYING);
	if (UNLIKELY(result != SL_RESULT_SUCCESS))
	{
		std::clog << "Sound: unable to start playback." << std::endl;
		return;
	}
}

OpenSLESOutput::~OpenSLESOutput()
{
	if (m_PlayerObj)
	{
		(*m_PlayerObj)->Destroy(m_PlayerObj);
		m_PlayerObj = nullptr;
	}
}

bool OpenSLESOutput::needMoreBuffers() const
{
	return m_NeedMoreBuffers.load() > 0;
}

void OpenSLESOutput::enqueueBuffer(const AudioBufferPtr & buffer)
{
	int value = m_NeedMoreBuffers.fetch_sub(1);
	if (value < 0)
		m_NeedMoreBuffers.fetch_add(1);

	if (m_BufferQueue)
		(*m_BufferQueue)->Enqueue(m_BufferQueue, buffer->data(), buffer->size());
}

void OpenSLESOutput::callback(SLBufferQueueItf itf, void * ud)
{
	OpenSLESOutput * self = reinterpret_cast<OpenSLESOutput *>(ud);
	self->m_NeedMoreBuffers.fetch_add(1);
}
