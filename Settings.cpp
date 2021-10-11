#include "Settings.h"

//Settings::Settings(const Settings &other)
//{

//}

bool Settings::operator==(const Settings &other) const
{
    return m_micStateChangeOnStartup    == other.m_micStateChangeOnStartup
        && m_operatingMode              == other.m_operatingMode
        && m_trayIconStyle              == other.m_trayIconStyle
        && m_pushDelay                  == other.m_pushDelay
        && m_releaseDelay				== other.m_releaseDelay
        && m_notificationsFlags         == other.m_notificationsFlags
        && m_overrideVolume             == other.m_overrideVolume;
}
