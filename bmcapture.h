#ifndef _BMCAPTURE_H_
#define _BMCAPTURE_H_

#include "DeckLinkAPI.h"
#include "capture_delegate.h"


class BMCapture{
public:
    BMCapture();
    ~BMCapture();
    void run();

private:
    BMDConfig		m_config;
    HRESULT							result;
	int								exitStatus{1};

	IDeckLinkIterator*				deckLinkIterator{NULL};
	IDeckLink*						deckLink{NULL};

	IDeckLinkProfileAttributes*		deckLinkAttributes{NULL};
	bool							formatDetectionSupported;
	int64_t							duplexMode;

	IDeckLinkDisplayMode*			displayMode{NULL};
	char*							displayModeName{NULL};
	bool							supported;

	DeckLinkCaptureDelegate*		delegate{NULL};
    IDeckLinkInput*	m_deckLinkInput{NULL};
};

#endif