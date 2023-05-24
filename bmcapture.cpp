#include "bmcapture.h"

BMCapture::BMCapture()
{
    deckLink = m_config.GetSelectedDeckLink();
	if (deckLink == NULL) {
		fprintf(stderr, "Unable to get DeckLink device %u\n", m_config.m_deckLinkIndex);
		// goto bail;
	}

	result = deckLink->QueryInterface(IID_IDeckLinkProfileAttributes, (void**)&deckLinkAttributes);
	if (result != S_OK){
		fprintf(stderr, "Unable to get DeckLink attributes interface\n");
		// goto bail;
	}

	// Check the DeckLink device is active
	result = deckLinkAttributes->GetInt(BMDDeckLinkDuplex, &duplexMode);
	if ((result != S_OK) || (duplexMode == bmdDuplexInactive))
	{
		fprintf(stderr, "The selected DeckLink device is inactive\n");
		// goto bail;
	}

	// Get the input (capture) interface of the DeckLink device
	result = deckLink->QueryInterface(IID_IDeckLinkInput, (void**)&m_deckLinkInput);
	if (result != S_OK) {
		fprintf(stderr, "The selected device does not have an input interface\n");
		// goto bail;
	}

	// Get the display mode
	if (m_config.m_displayModeIndex == -1)
	{
		// Check the card supports format detection
		result = deckLinkAttributes->GetFlag(BMDDeckLinkSupportsInputFormatDetection, &formatDetectionSupported);
		if (result != S_OK || !formatDetectionSupported)
		{
			fprintf(stderr, "Format detection is not supported on this device\n");
			// goto bail;
		}

		m_config.m_inputFlags |= bmdVideoInputEnableFormatDetection;
	}

	displayMode = m_config.GetSelectedDeckLinkDisplayMode(deckLink);

	if (displayMode == NULL) {
		fprintf(stderr, "Unable to get display mode %d\n", m_config.m_displayModeIndex);
		// goto bail;
	}

	// Get display mode name
	result = displayMode->GetName((const char**)&displayModeName);
	if (result != S_OK)
	{
		displayModeName = (char *)malloc(32);
		snprintf(displayModeName, 32, "[index %d]", m_config.m_displayModeIndex);
	}

	// Check display mode is supported with given options
	result = m_deckLinkInput->DoesSupportVideoMode(bmdVideoConnectionUnspecified, displayMode->GetDisplayMode(), m_config.m_pixelFormat, bmdNoVideoInputConversion, bmdSupportedVideoModeDefault, NULL, &supported);
	if (result != S_OK)
		// goto bail;

	if (! supported)
	{
		fprintf(stderr, "The display mode %s is not supported with the selected pixel format\n", displayModeName);
		// goto bail;
	}

	// Print the selected configuration
	m_config.DisplayConfiguration();

	// Configure the capture callback
	delegate = new DeckLinkCaptureDelegate(&m_config,m_deckLinkInput);
	m_deckLinkInput->SetCallback(delegate);

}

BMCapture::~BMCapture()
{

    m_deckLinkInput->StopStreams();
	m_deckLinkInput->DisableAudioInput();
    m_deckLinkInput->DisableVideoInput();


    if (displayModeName != NULL)
		free(displayModeName);

	if (displayMode != NULL)
		displayMode->Release();

	if (delegate != NULL)
		delegate->Release();

	if (m_deckLinkInput != NULL)
	{
		m_deckLinkInput->Release();
		m_deckLinkInput = NULL;
	}

	if (deckLinkAttributes != NULL)
		deckLinkAttributes->Release();

	if (deckLink != NULL)
		deckLink->Release();

	if (deckLinkIterator != NULL)
		deckLinkIterator->Release();
}

void BMCapture::run()
{
    result = m_deckLinkInput->EnableVideoInput(displayMode->GetDisplayMode(), m_config.m_pixelFormat, m_config.m_inputFlags);
    if (result != S_OK) {
        fprintf(stderr, "Failed to enable video input. Is another application using the card?\n");
        // goto bail;
    }

    result = m_deckLinkInput->StartStreams();
    // if (result != S_OK)
        // goto bail;

    // All Okay.
    exitStatus = 0;
}