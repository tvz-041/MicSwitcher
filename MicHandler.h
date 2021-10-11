#ifndef MICHANDLER_H
#define MICHANDLER_H

#include <QObject>

struct IMMDevice;
struct IMMDeviceEnumerator;
struct IAudioEndpointVolume;

class MicVolumeCallbackHandler;

class MicHandler : public QObject
{
	Q_OBJECT

public:
	static MicHandler &instance();
	~MicHandler();
    float micVolume() const;
    bool isMicEnabled() const;

public slots:
	void setMicEnabled(const bool enabled = true);
	void setMicDisabled(const bool disabled = true);
    void setMicVolume(const float volume = 1.0);

signals:
	void micSwitched(const bool isMicEnabled);
    void micVolumeChanged(const float micVolume);

private:
	MicHandler();
	MicHandler(const MicHandler &other) = delete;
	MicHandler &operator=(const MicHandler &other) = delete;

	IMMDeviceEnumerator *m_deviceEnumerator;
	IMMDevice *m_micDevice;
    IAudioEndpointVolume *m_micEndpointVolume;

	MicVolumeCallbackHandler *m_micVolumeHandler;
};



inline void MicHandler::setMicDisabled(const bool disabled)
{
	setMicEnabled(!disabled);
}

#endif //MICHANDLER_H
