#include <endpointvolume.h>
#include <mmdeviceapi.h>
#include <windows.h>

#include <QDebug>

#include "MicHandler.h"

class MicVolumeCallbackHandler : public IAudioEndpointVolumeCallback
{
public:
    MicVolumeCallbackHandler() = default;
	virtual ~MicVolumeCallbackHandler() = default;

    bool isEnabled() const;
    bool isMuted() const;
    bool volume() const;

    void setMuted(const bool muted = true);
    bool setVolume(const float volume = 1.0);
    void switchMic();

    ULONG STDMETHODCALLTYPE AddRef() override;
    ULONG STDMETHODCALLTYPE Release() override;
    HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, VOID **ppvInterface) override;
    HRESULT STDMETHODCALLTYPE OnNotify(PAUDIO_VOLUME_NOTIFICATION_DATA pNotify) override;

private:
    LONG m_cRef = 1;
    bool m_muted = false;
    float m_volume = 1.0;
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
    m_micDevice->Activate(__uuidof(IAudioEndpointVolume), CLSCTX_ALL, nullptr, (void**)&m_micEndpointVolume);
	m_micVolumeHandler = new MicVolumeCallbackHandler();
    m_micEndpointVolume->RegisterControlChangeNotify(m_micVolumeHandler);
}

MicHandler::~MicHandler()
{
    m_micEndpointVolume->UnregisterControlChangeNotify(m_micVolumeHandler);
	delete m_micVolumeHandler;
	m_deviceEnumerator->Release();
	m_micDevice->Release();
    m_micEndpointVolume->Release();
}

float MicHandler::micVolume() const
{
    return m_micVolumeHandler->volume();
}

bool MicHandler::isMicEnabled() const
{
    return !m_micVolumeHandler->isEnabled();
}

void MicHandler::setMicEnabled(const bool enabled)
{
    if (m_micVolumeHandler->isEnabled() == enabled) {
		return;
	}

    m_micVolumeHandler->switchMic();
    m_micEndpointVolume->SetMute(m_micVolumeHandler->isMuted(), nullptr);
}

void MicHandler::setMicVolume(const float volume)
{
    if (m_micVolumeHandler->setVolume(volume)) {
        m_micEndpointVolume->SetMasterVolumeLevelScalar(volume, nullptr);
    }
}



///class MicVolumeCallbackHandler:

inline bool MicVolumeCallbackHandler::isEnabled() const
{
    return !isMuted();
}

inline bool MicVolumeCallbackHandler::isMuted() const
{
    return m_muted;
}

inline bool MicVolumeCallbackHandler::volume() const
{
    return m_volume;
}

inline void MicVolumeCallbackHandler::setMuted(const bool muted)
{
    if (muted != m_muted) {
        m_muted = muted;
    }
}

inline void MicVolumeCallbackHandler::switchMic()
{
    m_muted = !m_muted;
}

inline bool MicVolumeCallbackHandler::setVolume(const float volume)
{
    if (volume < 0 || volume > 1.0 || volume == m_volume) {
        return false;
    }

    m_volume = volume;
    return true;
}

inline ULONG STDMETHODCALLTYPE MicVolumeCallbackHandler::AddRef()
{
    return InterlockedIncrement(&m_cRef);
}

inline ULONG STDMETHODCALLTYPE MicVolumeCallbackHandler::Release()
{
    ULONG ulRef = InterlockedDecrement(&m_cRef);
    if (0 == ulRef) {
        delete this;
    }

    return ulRef;
}

HRESULT STDMETHODCALLTYPE MicVolumeCallbackHandler::QueryInterface(REFIID riid, VOID **ppvInterface)
{
    if (IID_IUnknown == riid) {
        AddRef();
        *ppvInterface = (IUnknown*)this;
    } else if (__uuidof(IAudioEndpointVolumeCallback) == riid) {
        AddRef();
        *ppvInterface = (IAudioEndpointVolumeCallback*)this;
    } else {
        *ppvInterface = nullptr;
        return E_NOINTERFACE;
    }

    return S_OK;
}

HRESULT STDMETHODCALLTYPE MicVolumeCallbackHandler::OnNotify(PAUDIO_VOLUME_NOTIFICATION_DATA pNotify)
{
    if (pNotify == nullptr) {
        return E_INVALIDARG;
    }

    if (m_muted != pNotify->bMuted) {
        m_muted = pNotify->bMuted;
        emit MicHandler::instance().micSwitched(!m_muted);
    }

    if (m_volume != pNotify->fMasterVolume) {
        m_volume = pNotify->fMasterVolume;
        emit MicHandler::instance().micVolumeChanged(m_volume);
    }

    return S_OK;
}
