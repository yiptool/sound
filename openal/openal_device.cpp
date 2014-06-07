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
#include "openal_device.h"
#include "openal_output.h"
#include <iostream>

#ifdef __APPLE__
 #include <TargetConditionals.h>
 #if TARGET_OS_IPHONE
  #include "ios/audio_interruption_listener.h"
 #endif
#endif

OpenALDevice::OpenALDevice()
	: m_Device(nullptr),
	  m_Context(nullptr),
	  m_Suspended(false)
{
  #if defined(__APPLE__) && TARGET_OS_IPHONE
	AudioInterruptionListener_setup();
  #endif

	const char * defaultDevice = alcGetString(nullptr, ALC_DEFAULT_DEVICE_SPECIFIER);
	std::clog << "Sound: using default device: " << (defaultDevice ? defaultDevice : "(null)") << std::endl;

	m_Device = alcOpenDevice(defaultDevice);
	if (UNLIKELY(!m_Device))
	{
		std::clog << "Sound: unable to open audio device." << std::endl;
		return;
	}

	m_Context = alcCreateContext(m_Device, nullptr);
	if (UNLIKELY(!m_Context))
	{
		std::clog << "Sound: unable to create OpenAL context." << std::endl;
		return;
	}

  #if defined(__APPLE__) && TARGET_OS_IPHONE
	AudioInterruptionListener_setCallback([this](bool activate) { m_Suspended = !activate; });
  #endif

	bindContext();
	std::clog << "Sound: OpenAL vendor: " << alGetString(AL_VENDOR) << std::endl;
	std::clog << "Sound: OpenAL renderer: " << alGetString(AL_RENDERER) << std::endl;
	std::clog << "Sound: OpenAL version: " << alGetString(AL_VERSION) << std::endl;
}

OpenALDevice::~OpenALDevice()
{
  #if defined(__APPLE__) && TARGET_OS_IPHONE
	AudioInterruptionListener_setCallback(std::function<void(bool)>());
  #endif

	if (m_Context)
	{
		alcDestroyContext(m_Context);
		m_Context = nullptr;
	}

	if (m_Device)
	{
		alcCloseDevice(m_Device);
		m_Device = nullptr;
	}
}

AudioOutputPtr OpenALDevice::newOutput(AudioFormat format, size_t hz)
{
	return std::make_shared<OpenALOutput>(*this, format, hz);
}

void OpenALDevice::bindContext()
{
	if (UNLIKELY(!alcMakeContextCurrent(m_Context)))
		std::clog << "Sound: unable to activate OpenAL context." << std::endl;
	else
	{
		while (alGetError() != AL_NO_ERROR)
			;
	}
}

void OpenALDevice::checkError(const char * file, int line, const char * func)
{
	for (;;)
	{
		ALenum error = alGetError();
		if (LIKELY(error == AL_NO_ERROR))
			return;

		std::cerr << "Sound: " << func << ": " << alGetString(error) << " (in file " << file << " at line "
			<< line << ")." << std::endl;
	}
}

AudioDevice & AudioDevice::instance()
{
	static OpenALDevice device;
	return device;
}
