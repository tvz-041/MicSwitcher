#include <QMetaEnum>
#include <QSettings>
#include <QVariant>

#include "applicationVersion.h"

#include "Settings.h"

template<class Enum>
Enum enumFromString(const QString &string, const Enum defaultValue)
{
    int key = QMetaEnum::fromType<Enum>().keyToValue(string.toStdString().c_str());
    if (key == -1) {
        key = defaultValue;
    }

    return Enum(key);
}

template<class Enum>
QString enumToString(const Enum &enumValue)
{
    return QVariant::fromValue(enumValue).toString();
}

Settings::Settings(QSettings *settings) :
    m_settings(settings),
    m_lang(new QString("en_US"))
{
    if (m_settings) {
        loadFromDisk();
    }
}

Settings::Settings(const Settings &other) :
    m_settings(nullptr),
    m_lang(new QString(*other.m_lang))
{
    m_micStateChangeOnStartup   = other.m_micStateChangeOnStartup;
    m_micStateChangeOnExit      = other.m_micStateChangeOnExit;
    m_switchMode                = other.m_switchMode;
    m_notificationsFlags        = other.m_notificationsFlags;
    m_trayIconStyle             = other.m_trayIconStyle;

    m_isMicEnabled   = other.m_isMicEnabled;
    m_micVolume      = other.m_micVolume;
    m_overrideVolume = other.m_overrideVolume;
    m_pushDelay      = other.m_pushDelay;
    m_releaseDelay   = other.m_releaseDelay;
}

Settings::~Settings()
{
    if (m_settings) {
        delete m_settings;
    }

    delete m_lang;
}

bool Settings::readValue(QVariant &value, const QString &key) const
{
    if (m_settings->contains(key)) {
        value = m_settings->value(key);
        if (value != QVariant()) {
            return true;
        }
    }

    return false;
}

void Settings::loadFromDisk()
{
    QVariant value;
    QVariant invalidValue;
//    int loadedApplicationVersion = applicationVersion(1, 0, 0);

//    if (readValue(value, "General/sAppVersion")) {
//        loadedApplicationVersion = applicationVersionFromString(value.toString());
//    }

    if (readValue(value, "General/sLang")) {
        *m_lang = value.toString();
    }

    if (readValue(value, "General/fMicVolume")) {
        setMicVolume(value.toFloat());
    }

    if (readValue(value, "General/bOverrideVolume")) {
        m_overrideVolume = value.toBool();
    }

    if (readValue(value, "General/iPushDelay")) {
        setPushDelay(value.toInt());
    }

    if (readValue(value, "General/iReleaseDelay")) {
        setReleaseDelay(value.toInt());
    }

    if (readValue(value, "General/sMicStateChangeOnStartup")) {
        m_micStateChangeOnStartup = enumFromString<MicStateChange>(value.toString(), m_micStateChangeOnStartup);
    }

    if (readValue(value, "General/sMicStateChangeOnExit")) {
        m_micStateChangeOnExit = enumFromString<MicStateChange>(value.toString(), m_micStateChangeOnExit);
    }

    if (readValue(value, "General/sSwitchMode")) {
        m_switchMode = enumFromString<SwitchMode>(value.toString(), m_switchMode);
    }

    if (readValue(value, "General/iNotificationsFlags")) {
        m_notificationsFlags = NotificationsFlags(value.toInt());
    }

    if (readValue(value, "General/sTrayIconStyle")) {
        m_trayIconStyle = enumFromString<IconStyle>(value.toString(), m_trayIconStyle);
    }
}

void Settings::saveToDisk() const
{
    m_settings->clear();

    m_settings->setValue("General/sAppVersion", applicationVersionToString());
    m_settings->setValue("General/sLang", *m_lang);
    m_settings->setValue("General/sSwitchMode", enumToString<SwitchMode>(m_switchMode));
    m_settings->setValue("General/fMicVolume", QString::number(m_micVolume, 'f', 2));
    m_settings->setValue("General/bOverrideVolume", m_overrideVolume);
    m_settings->setValue("General/iPushDelay", m_pushDelay);
    m_settings->setValue("General/iReleaseDelay", m_releaseDelay);
    m_settings->setValue("General/sMicStateChangeOnStartup", enumToString<MicStateChange>(m_micStateChangeOnStartup));
    m_settings->setValue("General/sMicStateChangeOnExit", enumToString<MicStateChange>(m_micStateChangeOnExit));
    m_settings->setValue("General/iNotificationsFlags", (int) m_notificationsFlags);
    m_settings->setValue("General/sTrayIconStyle", enumToString<IconStyle>(m_trayIconStyle));

    m_settings->sync();
}

Settings &Settings::operator=(const Settings &other)
{
    if (this == &other) {
        return *this;
    }

    m_micStateChangeOnStartup   = other.m_micStateChangeOnStartup;
    m_micStateChangeOnExit      = other.m_micStateChangeOnExit;
    m_switchMode                = other.m_switchMode;
    m_notificationsFlags        = other.m_notificationsFlags;
    m_trayIconStyle             = other.m_trayIconStyle;

    m_isMicEnabled   = other.m_isMicEnabled;
    m_micVolume      = other.m_micVolume;
    m_overrideVolume = other.m_overrideVolume;
    m_pushDelay      = other.m_pushDelay;
    m_releaseDelay   = other.m_releaseDelay;
    (*m_lang)        = (*other.m_lang);

    return *this;
}

bool Settings::operator==(const Settings &other) const
{
    return m_micStateChangeOnStartup == other.m_micStateChangeOnStartup
        && m_micStateChangeOnExit    == other.m_micStateChangeOnExit
        && m_switchMode              == other.m_switchMode
        && m_notificationsFlags      == other.m_notificationsFlags
        && m_trayIconStyle           == other.m_trayIconStyle

        && m_isMicEnabled   == other.m_isMicEnabled
        && m_micVolume      == other.m_micVolume
        && m_overrideVolume == other.m_overrideVolume
        && m_pushDelay      == other.m_pushDelay
        && m_releaseDelay   == other.m_releaseDelay
        && (*m_lang)        == (*other.m_lang);
}
