#include "GamepadConfiguration.h"

GamepadConfiguration::GamepadConfiguration() : _controllerType(CONTROLLER_TYPE_GAMEPAD),
                                                     _autoReport(true),
                                                     _hidReportId(3),
                                                     _buttonCount(16),
                                                     _hatSwitchCount(1),
                                                     _whichSpecialButtons{false, false, false, false, false, false, false, false},
                                                     _whichAxes{true, true, true, true, true, true, true, true},
                                                     _whichSimulationControls{false, false, false, false, false},
                                                     _vid(0xe502),
                                                     _pid(0xbbab),
													 _guidVersion(0x0110),
                                                     _axesMin(0x0000),
                                                     _axesMax(0x7FFF),
                                                     _simulationMin(0x0000),
                                                     _simulationMax(0x7FFF),
                                                     _modelNumber("1.0.0"),
                                                     _softwareRevision("1.0.0"),
                                                     _serialNumber("0123456789"),
                                                     _firmwareRevision("0.5.2"),
                                                     _hardwareRevision("1.0.0")
{
}

uint8_t GamepadConfiguration::getTotalSpecialButtonCount()
{
    int count = 0;
    for (int i = 0; i < POSSIBLESPECIALBUTTONS; i++)
    {
        count += (int)_whichSpecialButtons[i];
    }

    return count;
}

uint8_t GamepadConfiguration::getDesktopSpecialButtonCount()
{
    int count = 0;
    for (int i = 0; i < 3; i++)
    {
        count += (int)_whichSpecialButtons[i];
    }

    return count;
}

uint8_t GamepadConfiguration::getConsumerSpecialButtonCount()
{
    int count = 0;
    for (int i = 3; i < 8; i++)
    {
        count += (int)_whichSpecialButtons[i];
    }

    return count;
}

uint8_t GamepadConfiguration::getAxisCount()
{
    int count = 0;
    for (int i = 0; i < POSSIBLEAXES; i++)
    {
        count += (int)_whichAxes[i];
    }

    return count;
}

uint8_t GamepadConfiguration::getSimulationCount()
{
    int count = 0;
    for (int i = 0; i < POSSIBLESIMULATIONCONTROLS; i++)
    {
        count += (int)_whichSimulationControls[i];
    }

    return count;
}

uint16_t GamepadConfiguration::getVid(){ return _vid; }
uint16_t GamepadConfiguration::getPid(){ return _pid; }
uint16_t GamepadConfiguration::getGuidVersion(){ return _guidVersion; }
int16_t GamepadConfiguration::getAxesMin(){ return _axesMin; }
int16_t GamepadConfiguration::getAxesMax(){ return _axesMax; }
int16_t GamepadConfiguration::getSimulationMin(){ return _simulationMin; }
int16_t GamepadConfiguration::getSimulationMax(){ return _simulationMax; }
uint8_t GamepadConfiguration::getControllerType() { return _controllerType; }
uint8_t GamepadConfiguration::getHidReportId() { return _hidReportId; }
uint16_t GamepadConfiguration::getButtonCount() { return _buttonCount; }
uint8_t GamepadConfiguration::getHatSwitchCount() { return _hatSwitchCount; }
bool GamepadConfiguration::getAutoReport() { return _autoReport; }
bool GamepadConfiguration::getIncludeStart() { return _whichSpecialButtons[START_BUTTON]; }
bool GamepadConfiguration::getIncludeSelect() { return _whichSpecialButtons[SELECT_BUTTON]; }
bool GamepadConfiguration::getIncludeMenu() { return _whichSpecialButtons[MENU_BUTTON]; }
bool GamepadConfiguration::getIncludeHome() { return _whichSpecialButtons[HOME_BUTTON]; }
bool GamepadConfiguration::getIncludeBack() { return _whichSpecialButtons[BACK_BUTTON]; }
bool GamepadConfiguration::getIncludeVolumeInc() { return _whichSpecialButtons[VOLUME_INC_BUTTON]; }
bool GamepadConfiguration::getIncludeVolumeDec() { return _whichSpecialButtons[VOLUME_DEC_BUTTON]; }
bool GamepadConfiguration::getIncludeVolumeMute() { return _whichSpecialButtons[VOLUME_MUTE_BUTTON]; }
const bool *GamepadConfiguration::getWhichSpecialButtons() const { return _whichSpecialButtons; }
bool GamepadConfiguration::getIncludeXAxis() { return _whichAxes[X_AXIS]; }
bool GamepadConfiguration::getIncludeYAxis() { return _whichAxes[Y_AXIS]; }
bool GamepadConfiguration::getIncludeZAxis() { return _whichAxes[Z_AXIS]; }
bool GamepadConfiguration::getIncludeRxAxis() { return _whichAxes[RX_AXIS]; }
bool GamepadConfiguration::getIncludeRyAxis() { return _whichAxes[RY_AXIS]; }
bool GamepadConfiguration::getIncludeRzAxis() { return _whichAxes[RZ_AXIS]; }
bool GamepadConfiguration::getIncludeSlider1() { return _whichAxes[SLIDER1]; }
bool GamepadConfiguration::getIncludeSlider2() { return _whichAxes[SLIDER2]; }
const bool *GamepadConfiguration::getWhichAxes() const { return _whichAxes; }
bool GamepadConfiguration::getIncludeRudder() { return _whichSimulationControls[RUDDER]; }
bool GamepadConfiguration::getIncludeThrottle() { return _whichSimulationControls[THROTTLE]; }
bool GamepadConfiguration::getIncludeAccelerator() { return _whichSimulationControls[ACCELERATOR]; }
bool GamepadConfiguration::getIncludeBrake() { return _whichSimulationControls[BRAKE]; }
bool GamepadConfiguration::getIncludeSteering() { return _whichSimulationControls[STEERING]; }
const bool *GamepadConfiguration::getWhichSimulationControls() const { return _whichSimulationControls; }
char *GamepadConfiguration::getModelNumber(){ return _modelNumber; }
char *GamepadConfiguration::getSoftwareRevision(){ return _softwareRevision; }
char *GamepadConfiguration::getSerialNumber(){ return _serialNumber; }
char *GamepadConfiguration::getFirmwareRevision(){ return _firmwareRevision; }
char *GamepadConfiguration::getHardwareRevision(){ return _hardwareRevision; }

void GamepadConfiguration::setWhichSpecialButtons(bool start, bool select, bool menu, bool home, bool back, bool volumeInc, bool volumeDec, bool volumeMute)
{
    _whichSpecialButtons[START_BUTTON] = start;
    _whichSpecialButtons[SELECT_BUTTON] = select;
    _whichSpecialButtons[MENU_BUTTON] = menu;
    _whichSpecialButtons[HOME_BUTTON] = home;
    _whichSpecialButtons[BACK_BUTTON] = back;
    _whichSpecialButtons[VOLUME_INC_BUTTON] = volumeInc;
    _whichSpecialButtons[VOLUME_DEC_BUTTON] = volumeDec;
    _whichSpecialButtons[VOLUME_MUTE_BUTTON] = volumeMute;
}

void GamepadConfiguration::setWhichAxes(bool xAxis, bool yAxis, bool zAxis, bool rxAxis, bool ryAxis, bool rzAxis, bool slider1, bool slider2)
{
    _whichAxes[X_AXIS] = xAxis;
    _whichAxes[Y_AXIS] = yAxis;
    _whichAxes[Z_AXIS] = zAxis;
    _whichAxes[RX_AXIS] = rxAxis;
    _whichAxes[RY_AXIS] = ryAxis;
    _whichAxes[RZ_AXIS] = rzAxis;
    _whichAxes[SLIDER1] = slider1;
    _whichAxes[SLIDER2] = slider2;
}

void GamepadConfiguration::setWhichSimulationControls(bool rudder, bool throttle, bool accelerator, bool brake, bool steering)
{
    _whichSimulationControls[RUDDER] = rudder;
    _whichSimulationControls[THROTTLE] = throttle;
    _whichSimulationControls[ACCELERATOR] = accelerator;
    _whichSimulationControls[BRAKE] = brake;
    _whichSimulationControls[STEERING] = steering;
}

void GamepadConfiguration::setControllerType(uint8_t value) { _controllerType = value; }
void GamepadConfiguration::setHidReportId(uint8_t value) { _hidReportId = value; }
void GamepadConfiguration::setButtonCount(uint16_t value) { _buttonCount = value; }
void GamepadConfiguration::setHatSwitchCount(uint8_t value) { _hatSwitchCount = value; }
void GamepadConfiguration::setAutoReport(bool value) { _autoReport = value; }
void GamepadConfiguration::setIncludeStart(bool value) { _whichSpecialButtons[START_BUTTON] = value; }
void GamepadConfiguration::setIncludeSelect(bool value) { _whichSpecialButtons[SELECT_BUTTON] = value; }
void GamepadConfiguration::setIncludeMenu(bool value) { _whichSpecialButtons[MENU_BUTTON] = value; }
void GamepadConfiguration::setIncludeHome(bool value) { _whichSpecialButtons[HOME_BUTTON] = value; }
void GamepadConfiguration::setIncludeBack(bool value) { _whichSpecialButtons[BACK_BUTTON] = value; }
void GamepadConfiguration::setIncludeVolumeInc(bool value) { _whichSpecialButtons[VOLUME_INC_BUTTON] = value; }
void GamepadConfiguration::setIncludeVolumeDec(bool value) { _whichSpecialButtons[VOLUME_DEC_BUTTON] = value; }
void GamepadConfiguration::setIncludeVolumeMute(bool value) { _whichSpecialButtons[VOLUME_MUTE_BUTTON] = value; }
void GamepadConfiguration::setIncludeXAxis(bool value) { _whichAxes[X_AXIS] = value; }
void GamepadConfiguration::setIncludeYAxis(bool value) { _whichAxes[Y_AXIS] = value; }
void GamepadConfiguration::setIncludeZAxis(bool value) { _whichAxes[Z_AXIS] = value; }
void GamepadConfiguration::setIncludeRxAxis(bool value) { _whichAxes[RX_AXIS] = value; }
void GamepadConfiguration::setIncludeRyAxis(bool value) { _whichAxes[RY_AXIS] = value; }
void GamepadConfiguration::setIncludeRzAxis(bool value) { _whichAxes[RZ_AXIS] = value; }
void GamepadConfiguration::setIncludeSlider1(bool value) { _whichAxes[SLIDER1] = value; }
void GamepadConfiguration::setIncludeSlider2(bool value) { _whichAxes[SLIDER2] = value; }
void GamepadConfiguration::setIncludeRudder(bool value) { _whichSimulationControls[RUDDER] = value; }
void GamepadConfiguration::setIncludeThrottle(bool value) { _whichSimulationControls[THROTTLE] = value; }
void GamepadConfiguration::setIncludeAccelerator(bool value) { _whichSimulationControls[ACCELERATOR] = value; }
void GamepadConfiguration::setIncludeBrake(bool value) { _whichSimulationControls[BRAKE] = value; }
void GamepadConfiguration::setIncludeSteering(bool value) { _whichSimulationControls[STEERING] = value; }
void GamepadConfiguration::setVid(uint16_t value) { _vid = value; }
void GamepadConfiguration::setPid(uint16_t value) { _pid = value; }
void GamepadConfiguration::setGuidVersion(uint16_t value) { _guidVersion = value; }
void GamepadConfiguration::setAxesMin(int16_t value) { _axesMin = value; }
void GamepadConfiguration::setAxesMax(int16_t value) { _axesMax = value; }
void GamepadConfiguration::setSimulationMin(int16_t value) { _simulationMin = value; }
void GamepadConfiguration::setSimulationMax(int16_t value) { _simulationMax = value; }
void GamepadConfiguration::setModelNumber(char *value) { _modelNumber = value; }
void GamepadConfiguration::setSoftwareRevision(char *value) { _softwareRevision = value; }
void GamepadConfiguration::setSerialNumber(char *value) { _serialNumber = value; }
void GamepadConfiguration::setFirmwareRevision(char *value) { _firmwareRevision = value; }
void GamepadConfiguration::setHardwareRevision(char *value) { _hardwareRevision = value; }

//***************************************************************************
// Special configuration for Steam

#define numOfButtons 24
#define numOfHatSwitches 0
#define enableX true
#define enableY true
#define enableZ false
#define enableRZ false
#define enableRX true
#define enableRY true
#define enableSlider1 false
#define enableSlider2 false
#define enableRudder true
#define enableThrottle true
#define enableAccelerator false
#define enableBrake false
#define enableSteering false


SteamGamepadConfiguration::SteamGamepadConfiguration()
{
    // Setup controller with 32 buttons, accelerator, brake and steering
    setAutoReport(false);
    setControllerType(CONTROLLER_TYPE_MULTI_AXIS); // CONTROLLER_TYPE_JOYSTICK, CONTROLLER_TYPE_GAMEPAD (DEFAULT), CONTROLLER_TYPE_MULTI_AXIS
    setButtonCount(numOfButtons);
    setIncludeStart(true);
    setIncludeSelect(true);
    setWhichAxes(enableX, enableY, enableZ, enableRX, enableRY, enableRZ, enableSlider1, enableSlider2);      // Can also be done per-axis individually. All are true by default
    setWhichSimulationControls(enableRudder, enableThrottle, enableAccelerator, enableBrake, enableSteering); // Can also be done per-control individually. All are false by default
    setHatSwitchCount(numOfHatSwitches);                                                                      // 1 by default
    // Some non-Windows operating systems and web based gamepad testers don't like min axis set below 0, so 0 is set by default
    setAxesMin(0x8001); // -32767 --> int16_t - 16 bit signed integer - Can be in decimal or hexadecimal
    setAxesMax(0x7FFF); // 32767 --> int16_t - 16 bit signed integer - Can be in decimal or hexadecimal 
}


