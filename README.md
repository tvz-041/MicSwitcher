<h1>MicSwitcher</h1>

A small program to system-wide switch of your microphone. At the current stage the hotkeys are hardcoded for the 4th and 5th mouse  buttons. Hotkey buttons will not work in the usual way (for example, you won't be able to navigate in the browser by pressing them).
<br/>All settings can be edited by config INI file which is located in the program folder:

>[%General]
<br/>sAppVersion=0.1.2
<br/>sLang=en_US
<br/>fMicVolume=1.00
<br/>bOverrideVolume=false
<br/>iPushDelay=0
<br/>iReleaseDelay=0
<br/>sMicStateChangeOnStartup=NoChange
<br/>sMicStateChangeOnExit=SetEnable
<br/>sSwitchMode=PushToSwitch
<br/>iNotificationsFlags=0
<br/>sTrayIconStyle=Dark

**fMicVolume**: microphone volume. Can be a float number from ***0.00*** to ***1.00***.
<br/>**bOverrideVolume**: flag that indicates to reset microphone volume if some other program has changed the volume.
<br/>**iPushDelay** and **iReleaseDelay**: delays before switching the microphone after pressing/releasing the hotkey.
<br/>**sMicStateChangeOnStartup**: flag to set the microphone to a specific state when the program starts. Possible values: "***NoChange***", "***SetEnable***", "***SetDisable***".
<br/>**sMicStateChangeOnExit**: flag to set the microphone to a specific state when the program close. Possible values: "***NoChange***", "***SetEnable***", "***SetDisable***".
<br/>**sSwitchMode**: flag to set the microphone switch mode. Possible values: "***PushToSwitch***", "***ReleaseToSwitch***", "***PushToTalk***", "***PushToMute***".
<br/>**iNotificationsFlags**: flag to show system notification on microphone switch. Possible values: ***0***, ***1***.
<br/>**sTrayIconStyle**: flag to setting the style of the program tray icon. Possible values: "***Light***", "***Dark***".
<br/>
<br/>Some of these settings can also be changed through the context menu on the taskbar tray.
