#ifndef SETTINGS_H
#define SETTINGS_H

#include <QObject>

class QSettings;

class Settings
{
    Q_GADGET

public:
	enum MicStateChange
	{
		NoChange,
		SetEnable,
		SetDisable
	};
    Q_ENUM(MicStateChange)

	enum OperatingMode
	{
		Switch,
		PushToTalk,
		PushToMute
	};
    Q_ENUM(OperatingMode)

	enum IconStyle
	{
		Light,
		Dark
	};
    Q_ENUM(IconStyle)

	enum NotificationsFlag
	{
		Enabled = 1,
		ShowOnSelfSwitches = 1 << 1,
		SoundOnSelfSwitches = 1 << 2,
		ShowOnOtherSwitches = 1 << 3,
		SoundOnOtherSwitches = 1 << 4,
    };
    Q_DECLARE_FLAGS(NotificationsFlags, NotificationsFlag);

    Settings(QSettings *settings = nullptr);
    ~Settings();
    Settings(const Settings &other);

    void loadFromDisk();
    void saveToDisk() const;

    inline bool isMicEnabled() const;
    inline bool isNeedToOverrideVolume(const float micVolume) const;
    inline float micVolume() const;
    inline bool overrideVolume() const;

    inline int pushDelay() const;
    inline int releaseDelay() const;

    inline MicStateChange micStateChangeOnStartup() const;
    inline OperatingMode operatingMode() const;

    inline bool showNotifications() const;
	inline bool showNotificationsOnSelfSwitches() const;
	inline bool playSoundOnSelfSwitches() const;
	inline bool showNotificationsOnOtherSwitches() const;
	inline bool playSoundOnOtherSwitches() const;

    inline IconStyle trayIconStyle() const;

    inline void setMicEnabled(const bool enabled = true);
    inline bool setMicVolume(const float volume);
    inline void setOverrideVolume(const bool override = true);

    inline void setPushDelay(const int delayInMsec);
    inline void setReleaseDelay(const int delayInMsec);

    inline void setMicStateChangeOnStartup(MicStateChange stateChange);
    inline void setOperatingMode(OperatingMode mode);

    inline void setShowNotifications(const bool enabled = true);
	inline void setShowNotificationsOnSelfSwitches(const bool enabled = true);
	inline void setPlaySoundOnSelfSwitches(const bool enabled = true);
	inline void setShowNotificationsOnOtherSwitches(const bool enabled = true);
	inline void setPlaySoundOnOtherSwitches(const bool enabled = true);

    inline void setTrayIconStyle(IconStyle style);

    Settings &operator=(const Settings &other);
	bool operator==(const Settings &other) const;
	bool operator!=(const Settings &other) const; //inline

private:
    bool readValue(QVariant &value, const QString &key) const;
    friend class SettingsDialog;

    QSettings *m_settings = nullptr;

    QString *m_lang = nullptr;

    bool m_isMicEnabled                         = true;
    float m_micVolume                           = 1.0;
    bool m_overrideVolume                       = false;
    int m_pushDelay                             = 0;
    int m_releaseDelay                          = 0;
    MicStateChange m_micStateChangeOnStartup    = MicStateChange::NoChange;
    OperatingMode m_operatingMode				= OperatingMode::Switch;
    NotificationsFlags m_notificationsFlags;
    IconStyle m_trayIconStyle                   = IconStyle::Light;
};

Q_DECLARE_OPERATORS_FOR_FLAGS(Settings::NotificationsFlags);



inline bool Settings::isMicEnabled() const
{
    return m_isMicEnabled;
}

inline bool Settings::isNeedToOverrideVolume(const float micVolume) const
{
    return (m_overrideVolume && m_micVolume != micVolume);
}

inline float Settings::micVolume() const
{
    return m_micVolume;
}

inline bool Settings::overrideVolume() const
{
    return m_overrideVolume;
}

inline int Settings::pushDelay() const
{
    return m_pushDelay;
}

inline int Settings::releaseDelay() const
{
    return m_releaseDelay;
}

inline Settings::MicStateChange Settings::micStateChangeOnStartup() const
{
    return m_micStateChangeOnStartup;
}

inline Settings::OperatingMode Settings::operatingMode() const
{
    return m_operatingMode;
}

inline bool Settings::showNotifications() const
{
    return m_notificationsFlags.testFlag(NotificationsFlag::Enabled);
}

inline bool Settings::showNotificationsOnSelfSwitches() const
{
    return m_notificationsFlags.testFlag(NotificationsFlag::ShowOnSelfSwitches);
}

inline bool Settings::playSoundOnSelfSwitches() const
{
    return m_notificationsFlags.testFlag(NotificationsFlag::SoundOnSelfSwitches);
}

inline bool Settings::showNotificationsOnOtherSwitches() const
{
    return m_notificationsFlags.testFlag(NotificationsFlag::ShowOnOtherSwitches);
}

inline bool Settings::playSoundOnOtherSwitches() const
{
    return m_notificationsFlags.testFlag(NotificationsFlag::SoundOnOtherSwitches);
}

inline Settings::IconStyle Settings::trayIconStyle() const
{
    return m_trayIconStyle;
}

inline void Settings::setMicEnabled(const bool enabled)
{
    m_isMicEnabled = enabled;
}

inline bool Settings::setMicVolume(const float volume)
{
    if (volume < 0 || volume > 1.0) {
        return false;
    }

    m_micVolume = volume;
    return true;
}

inline void Settings::setOverrideVolume(const bool override)
{
    m_overrideVolume = override;
}

inline void Settings::setPushDelay(const int delayInMsec)
{
    if (delayInMsec >= 0) {
        m_pushDelay = delayInMsec;
    }
}

inline void Settings::setReleaseDelay(const int delayInMsec)
{
    if (delayInMsec >= 0) {
        m_releaseDelay = delayInMsec;
    }
}

inline void Settings::setMicStateChangeOnStartup(MicStateChange stateChange)
{
    m_micStateChangeOnStartup = stateChange;
}

inline void Settings::setOperatingMode(OperatingMode mode)
{
    m_operatingMode = mode;
}

inline void Settings::setShowNotifications(const bool enabled)
{
    m_notificationsFlags.setFlag(NotificationsFlag::Enabled, enabled);
}

inline void Settings::setShowNotificationsOnSelfSwitches(const bool enabled)
{
    m_notificationsFlags.setFlag(NotificationsFlag::ShowOnSelfSwitches, enabled);
}

inline void Settings::setPlaySoundOnSelfSwitches(const bool enabled)
{
    m_notificationsFlags.setFlag(NotificationsFlag::SoundOnSelfSwitches, enabled);
}

inline void Settings::setShowNotificationsOnOtherSwitches(const bool enabled)
{
    m_notificationsFlags.setFlag(NotificationsFlag::ShowOnOtherSwitches, enabled);
}

inline void Settings::setPlaySoundOnOtherSwitches(const bool enabled)
{
    m_notificationsFlags.setFlag(NotificationsFlag::SoundOnOtherSwitches, enabled);
}

inline void Settings::setTrayIconStyle(IconStyle style)
{
    m_trayIconStyle = style;
}

inline bool Settings::operator!=(const Settings &other) const
{
	return (!Settings::operator==(other));
}

#endif // SETTINGS_H
