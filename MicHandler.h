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

public slots:
	void setMicEnabled(const bool enabled = true);
	void setMicDisabled(const bool disabled = true);

signals:
	void micSwitched(const bool isMicEnabled);

private:
	MicHandler();
	MicHandler(const MicHandler &other) = delete;
	MicHandler &operator=(const MicHandler &other) = delete;

	IMMDeviceEnumerator *m_deviceEnumerator;
	IMMDevice *m_micDevice;
	IAudioEndpointVolume *m_micVolume;

	MicVolumeCallbackHandler *m_micVolumeHandler;
};



inline void MicHandler::setMicDisabled(const bool disabled)
{
	setMicEnabled(!disabled);
}

#endif //MICHANDLER_H
