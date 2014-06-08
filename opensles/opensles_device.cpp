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
#include "opensles_device.h"
#include "opensles_output.h"
#include <iostream>

OpenSLESDevice::OpenSLESDevice()
	: m_EngineObj(nullptr),
	  m_Engine(nullptr),
	  m_OutputMixObj(nullptr)
{
	const SLInterfaceID pIDs[1] = { SL_IID_ENGINE };
	const SLboolean pIDsRequired[1]  = { SL_BOOLEAN_TRUE };

	SLresult result = slCreateEngine(&m_EngineObj, 0, nullptr, 1, pIDs, pIDsRequired);
	if (UNLIKELY(result != SL_RESULT_SUCCESS))
	{
		std::clog << "Sound: unable to allocate OpenSL engine." << std::endl;
		return;
	}

	result = (*m_EngineObj)->Realize(m_EngineObj, SL_BOOLEAN_FALSE);
	if (UNLIKELY(result != SL_RESULT_SUCCESS))
	{
		std::clog << "Sound: unable to initialize OpenSL engine." << std::endl;
		return;
	}

	result = (*m_EngineObj)->GetInterface(m_EngineObj, SL_IID_ENGINE, &m_Engine);
	if (UNLIKELY(result != SL_RESULT_SUCCESS))
	{
		std::clog << "Sound: unable to get OpenSL engine interface." << std::endl;
		return;
	}

	const SLInterfaceID pOutputMixIDs[] = {};
	const SLboolean pOutputMixRequired[] = {};

	result = (*m_Engine)->CreateOutputMix(m_Engine, &m_OutputMixObj, 0, pOutputMixIDs, pOutputMixRequired);
	if (UNLIKELY(result != SL_RESULT_SUCCESS))
	{
		std::clog << "Sound: unable to allocate OpenSL output mixer." << std::endl;
		return;
	}

	result = (*m_OutputMixObj)->Realize(m_OutputMixObj, SL_BOOLEAN_FALSE);
	if (UNLIKELY(result != SL_RESULT_SUCCESS))
	{
		std::clog << "Sound: unable to initialize OpenSL output mixer." << std::endl;
		return;
	}
}

OpenSLESDevice::~OpenSLESDevice()
{
	if (m_OutputMixObj)
	{
		(*m_OutputMixObj)->Destroy(m_OutputMixObj);
		m_OutputMixObj = nullptr;
	}

	if (m_EngineObj)
	{
		(*m_EngineObj)->Destroy(m_EngineObj);
		m_EngineObj = nullptr;
	}
}

AudioOutputPtr OpenSLESDevice::newOutput(AudioFormat format, size_t hz)
{
	return std::make_shared<OpenSLESOutput>(*this, format, hz);
}

AudioDevice & AudioDevice::instance()
{
	static OpenSLESDevice device;
	return device;
}
