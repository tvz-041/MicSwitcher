#include <endpointvolume.h>
#include <mmdeviceapi.h>
#include <windows.h>

#include "MicHandler.h"

class MicVolumeCallbackHandler : public IAudioEndpointVolumeCallback
{
	LONG _cRef;

public:
	bool isMuted = false;

	MicVolumeCallbackHandler() :
		_cRef(1)
	{}

	virtual ~MicVolumeCallbackHandler() = default;

	ULONG STDMETHODCALLTYPE AddRef()
	{
		return InterlockedIncrement(&_cRef);
	}

	ULONG STDMETHODCALLTYPE Release()
	{
		ULONG ulRef = InterlockedDecrement(&_cRef);
		if (0 == ulRef) {
			delete this;
		}

		return ulRef;
	}

	HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, VOID **ppvInterface)
	{
		if (IID_IUnknown == riid) {
			AddRef();
			*ppvInterface = (IUnknown*)this;
		} else if (__uuidof(IAudioEndpointVolumeCallback) == riid) {
			AddRef();
			*ppvInterface = (IAudioEndpointVolumeCallback*)this;
		} else {
			*ppvInterface = NULL;
			return E_NOINTERFACE;
		}

		return S_OK;
	}

	HRESULT STDMETHODCALLTYPE OnNotify(PAUDIO_VOLUME_NOTIFICATION_DATA pNotify)
	{
		if (pNotify == NULL) {
			return E_INVALIDARG;
		} else if (isMuted != pNotify->bMuted) {
			isMuted = pNotify->bMuted;
			emit MicHandler::instance().micSwitched(!isMuted);
		}

		return S_OK;
	}
};

MicHandler &MicHandler::instance()
{
	static MicHandler instance;
	return instance;
}

MicHandler::MicHandler()
{
	CoCreateInstance(
		__uuidof(MMDeviceEnumerator), NULL,
		CLSCTX_ALL, __uuidof(IMMDeviceEnumerator),
		(void**)&m_deviceEnumerator
	);

	m_deviceEnumerator->GetDefaultAudioEndpoint(EDataFlow::eCapture, ERole::eCommunications, &m_micDevice);
	m_micDevice->Activate(__uuidof(IAudioEndpointVolume), CLSCTX_ALL, nullptr, (void**)&m_micVolume);
	m_micVolumeHandler = new MicVolumeCallbackHandler();
	m_micVolume->RegisterControlChangeNotify(m_micVolumeHandler);
}

MicHandler::~MicHandler()
{
	m_micVolume->UnregisterControlChangeNotify(m_micVolumeHandler);
	delete m_micVolumeHandler;
	m_deviceEnumerator->Release();
	m_micDevice->Release();
	m_micVolume->Release();
}

void MicHandler::setMicEnabled(const bool enabled)
{
	if (m_micVolumeHandler->isMuted ^ enabled) {
		return;
	}

	m_micVolumeHandler->isMuted = !m_micVolumeHandler->isMuted;
	m_micVolume->SetMute(m_micVolumeHandler->isMuted, nullptr);
}
