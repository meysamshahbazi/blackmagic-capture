#include "capture_delegate.h"

DeckLinkCaptureDelegate::DeckLinkCaptureDelegate() : 
	m_refCount(1),
	m_pixelFormat(bmdFormat8BitYUV)
{
}

ULONG DeckLinkCaptureDelegate::AddRef(void)
{
	return __sync_add_and_fetch(&m_refCount, 1);
}

ULONG DeckLinkCaptureDelegate::Release(void)
{
	int32_t newRefValue = __sync_sub_and_fetch(&m_refCount, 1);
	if (newRefValue == 0)
	{
		delete this;
		return 0;
	}
	return newRefValue;
}



HRESULT DeckLinkCaptureDelegate::VideoInputFrameArrived(IDeckLinkVideoInputFrame* videoFrame , IDeckLinkAudioInputPacket* audioFrame )
{
	IDeckLinkVideoFrame3DExtensions*	threeDExtensions = NULL;
	void*								frameBytes;
	// Handle Video Frame
	if (videoFrame) {
		if (videoFrame->GetFlags() & bmdFrameHasNoInputSource) {
			printf("Frame received (#%lu) - No input signal detected\n", g_frameCount);
		}
		else {
			const char *timecodeString = NULL;
			if (g_config.m_timecodeFormat != 0) {
				IDeckLinkTimecode *timecode;
				if (videoFrame->GetTimecode(g_config.m_timecodeFormat, &timecode) == S_OK) {
					timecode->GetString(&timecodeString);
				}
			}

			printf("Frame received (#%lu) [%s] - %s - Size: %li bytes\n", g_frameCount,
				timecodeString != NULL ? timecodeString : "No timecode", "Valid Frame",
				videoFrame->GetRowBytes() * videoFrame->GetHeight());

			if (timecodeString)
				free((void*)timecodeString);

			// if (g_videoOutputFile != -1) {
			// 	videoFrame->GetBytes(&frameBytes);
			// 	write(g_videoOutputFile, frameBytes, videoFrame->GetRowBytes() * videoFrame->GetHeight());
			// }
		}

		g_frameCount++;
	}

	if (g_config.m_maxFrames > 0 && videoFrame && g_frameCount >= g_config.m_maxFrames) {
		g_do_exit = true;
		pthread_cond_signal(&g_sleepCond);
	}

	return S_OK;
}


HRESULT DeckLinkCaptureDelegate::VideoInputFormatChanged(BMDVideoInputFormatChangedEvents events, IDeckLinkDisplayMode *mode, BMDDetectedVideoInputFormatFlags formatFlags)
{
	// This only gets called if bmdVideoInputEnableFormatDetection was set
	// when enabling video input
	HRESULT	result;
	char*	displayModeName = NULL;
	BMDPixelFormat	pixelFormat = m_pixelFormat;
	
	if (events & bmdVideoInputColorspaceChanged)
	{
		// Detected a change in colorspace, change pixel format to match detected format
		if (formatFlags & bmdDetectedVideoInputRGB444)
			pixelFormat = bmdFormat10BitRGB;
		else if (formatFlags & bmdDetectedVideoInputYCbCr422)
			pixelFormat = (g_config.m_pixelFormat == bmdFormat8BitYUV) ? bmdFormat8BitYUV : bmdFormat10BitYUV;
		else
			goto bail;
	}

	// Restart streams if either display mode or pixel format have changed
	if ((events & bmdVideoInputDisplayModeChanged) || (m_pixelFormat != pixelFormat))
	{
		mode->GetName((const char**)&displayModeName);
		printf("Video format changed to %s %s\n", displayModeName, formatFlags & bmdDetectedVideoInputRGB444 ? "RGB" : "YUV");

		if (displayModeName)
			free(displayModeName);

		if (g_deckLinkInput)
		{
			g_deckLinkInput->StopStreams();

			result = g_deckLinkInput->EnableVideoInput(mode->GetDisplayMode(), pixelFormat, g_config.m_inputFlags);
			if (result != S_OK)
			{
				fprintf(stderr, "Failed to switch video mode\n");
				goto bail;
			}

			g_deckLinkInput->StartStreams();
		}

		m_pixelFormat = pixelFormat;
	}

bail:
	return S_OK;
}