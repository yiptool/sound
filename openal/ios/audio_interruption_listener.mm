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
#import "audio_interruption_listener.h"
#import <yip-imports/cxx-util/macros.h>
#import <TargetConditionals.h>
#import <Foundation/Foundation.h>
#import <AVFoundation/AVFoundation.h>
#import <AudioToolbox/AudioToolbox.h>

@interface AudioInterruptionListener : NSObject
@end

@implementation AudioInterruptionListener

static AudioInterruptionListener * g_Instance;
static std::function<void(bool)> g_Callback;

-(void)audioSessionDidChangeInterruptionType:(NSNotification *)notification
{
	NSNumber * eventCode = [notification.userInfo objectForKey:AVAudioSessionInterruptionTypeKey];
	AVAudioSessionInterruptionType event = (AVAudioSessionInterruptionType)[eventCode unsignedIntegerValue];
	if (event == AVAudioSessionInterruptionTypeBegan)
	{
		NSLog(@"Sound: received AVAudioSessionInterruptionTypeBegan.");
		if (g_Callback)
			g_Callback(false);
	}
	else if (event == AVAudioSessionInterruptionTypeEnded)
	{
		NSLog(@"Sound: received AVAudioSessionInterruptionTypeEnded.");
		[AudioInterruptionListener setSessionActive];
		if (g_Callback)
			g_Callback(true);
	}
}

+(void)setSessionActive
{
	NSError * error = nil;
	if (UNLIKELY(![[AVAudioSession sharedInstance] setActive:YES error:&error]))
		NSLog(@"Sound: unable to activate audio session: %@", error);
}

+(void)setup
{
	g_Instance = [AudioInterruptionListener new];

	AVAudioSession * audioSession = [AVAudioSession sharedInstance];
	[[NSNotificationCenter defaultCenter] addObserver:g_Instance
		selector:@selector(audioSessionDidChangeInterruptionType:) name:AVAudioSessionInterruptionNotification
		object:audioSession];

	NSString * category = nil;
	if (!audioSession.otherAudioPlaying)
	{
		NSLog(@"Sound: using the AVAudioSessionCategorySoloAmbient audio session category.");
		category = AVAudioSessionCategorySoloAmbient;
	}
	else
	{
		NSLog(@"Sound: using the AVAudioSessionCategoryAmbient audio session category.");
		category = AVAudioSessionCategoryAmbient;
	}

	NSError * error = nil;
	if (UNLIKELY(![audioSession setCategory:category error:&error]))
		NSLog(@"Sound: unable to set audio session category: %@", error);

	[AudioInterruptionListener setSessionActive];
}

@end

void AudioInterruptionListener_setup()
{
	[AudioInterruptionListener setup];
}

void AudioInterruptionListener_setCallback(const std::function<void(bool)> & callback)
{
	g_Callback = callback;
}
