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

