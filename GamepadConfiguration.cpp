#include "GamepadConfiguration.h"

GamepadConfiguration::GamepadConfiguration() : _controllerType(CONTROLLER_TYPE_GAMEPAD),
                                                     _autoReport(true),
                                                     _hidReportId(3),
                                                     _buttonCount(16),
                                                     _hatSwitchCount(1),
                                                     _whichSpecialButtons{false, false, false, false, false, false, false, false},
                                                     _whichAxes{true, true, true, true, true, true, true, true},
                                                     _whichSimulationControls{false, false, false, false, false},
                                                     _axesMin(0x0000),
                                                     _axesMax(0x7FFF),
                                                     _simulationMin(0x0000),
                                                     _simulationMax(0x7FFF)
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
void GamepadConfiguration::setAxesMin(int16_t value) { _axesMin = value; }
void GamepadConfiguration::setAxesMax(int16_t value) { _axesMax = value; }
void GamepadConfiguration::setSimulationMin(int16_t value) { _simulationMin = value; }
void GamepadConfiguration::setSimulationMax(int16_t value) { _simulationMax = value; }

//***************************************************************************
// Special configuration for Steam

#define numOfButtons 24
#define numOfHatSwitches 0
#define enableX true
#define enableY true
#define enableZ true
#define enableRX true
#define enableRY true
#define enableRZ true
#define enableSlider1 false
#define enableSlider2 false
#define enableRudder false
#define enableThrottle false
#define enableAccelerator false
#define enableBrake false
#define enableSteering false


SteamGamepadConfiguration::SteamGamepadConfiguration()
{
    // Setup controller with 32 buttons, accelerator, brake and steering
    setAutoReport(false);
    setControllerType(CONTROLLER_TYPE_GAMEPAD); // CONTROLLER_TYPE_JOYSTICK, CONTROLLER_TYPE_GAMEPAD (DEFAULT), CONTROLLER_TYPE_MULTI_AXIS
    setButtonCount(numOfButtons);
    setIncludeStart(true);
    setIncludeSelect(true);
    setWhichAxes(enableX, enableY, enableZ, enableRX, enableRY, enableRZ, enableSlider1, enableSlider2);      // Can also be done per-axis individually. All are true by default
    setWhichSimulationControls(enableRudder, enableThrottle, enableAccelerator, enableBrake, enableSteering); // Can also be done per-control individually. All are false by default
    setSimulationMin(0);
    setSimulationMax(255);
    setHatSwitchCount(numOfHatSwitches);                                                                      // 1 by default
    // Some non-Windows operating systems and web based gamepad testers don't like min axis set below 0, so 0 is set by default
    setAxesMin(0x8001); // -32767 --> int16_t - 16 bit signed integer - Can be in decimal or hexadecimal
    setAxesMax(0x7FFF); // 32767 --> int16_t - 16 bit signed integer - Can be in decimal or hexadecimal 
}


// Source: https://gist.github.com/fendent/5709856
// XBox360 trigger:
/*
    0x05, 0x09,                    //     USAGE_PAGE (Button)
    0x19, 0x01,                    //     USAGE_MINIMUM (Button 1)
    0x29, 0x04,                    //     USAGE_MAXIMUM (Button 4)
    0x81, 0x02,                    //     INPUT (Data,Var,Abs)
    0x75, 0x08,                    //     REPORT_SIZE (8)
    0x15, 0x00,                    //     LOGICAL_MINIMUM (0)
    0x26, 0xff, 0x00,              //     LOGICAL_MAXIMUM (255)
    0x35, 0x00,                    //     PHYSICAL_MINIMUM (0)
    0x46, 0xff, 0x00,              //     PHYSICAL_MAXIMUM (255)
    0x95, 0x02,                    //     REPORT_COUNT (2)
*/

// XBox360 descriptor:

char XBox360_ReportDescriptor[196] = {
    0x05, 0x01,                    // USAGE_PAGE (Generic Desktop)
    0x09, 0x05,                    // USAGE (Game Pad)
    0xa1, 0x01,                    // COLLECTION (Application)
    0x05, 0x01,                    //   USAGE_PAGE (Generic Desktop)
    0x09, 0x3a,                    //   USAGE (Counted Buffer)
    0xa1, 0x02,                    //   COLLECTION (Logical)
    0x75, 0x08,                    //     REPORT_SIZE (8)
    0x95, 0x02,                    //     REPORT_COUNT (2)

    0x05, 0x01,                    //     USAGE_PAGE (Generic Desktop)
    0x09, 0x3f,                    //     USAGE (Reserved)
    0x09, 0x3b,                    //     USAGE (Byte Count)
    0x81, 0x01,                    //     INPUT (Cnst,Ary,Abs)
    0x75, 0x01,                    //     REPORT_SIZE (1)
    0x15, 0x00,                    //     LOGICAL_MINIMUM (0)
    0x25, 0x01,                    //     LOGICAL_MAXIMUM (1)
    0x35, 0x00,                    //     PHYSICAL_MINIMUM (0)
    0x45, 0x01,                    //     PHYSICAL_MAXIMUM (1)
    0x95, 0x04,                    //     REPORT_COUNT (4)

    // Four buttons (12-15)
    0x05, 0x09,                    //     USAGE_PAGE (Button)
    0x19, 0x0c,                    //     USAGE_MINIMUM (Button 12)
    0x29, 0x0f,                    //     USAGE_MAXIMUM (Button 15)
    0x81, 0x02,                    //     INPUT (Data,Var,Abs)
    0x75, 0x01,                    //     REPORT_SIZE (1)
    0x15, 0x00,                    //     LOGICAL_MINIMUM (0)
    0x25, 0x01,                    //     LOGICAL_MAXIMUM (1)
    0x35, 0x00,                    //     PHYSICAL_MINIMUM (0)
    0x45, 0x01,                    //     PHYSICAL_MAXIMUM (1)
    0x95, 0x04,                    //     REPORT_COUNT (4)

    // Three buttons (9, 10, 7, 8 one more than report_count!)
    0x05, 0x09,                    //     USAGE_PAGE (Button)
    0x09, 0x09,                    //     USAGE (Button 9)
    0x09, 0x0a,                    //     USAGE (Button 10)
    0x09, 0x07,                    //     USAGE (Button 7)
    0x09, 0x08,                    //     USAGE (Button 8)
    0x81, 0x02,                    //     INPUT (Data,Var,Abs)
    0x75, 0x01,                    //     REPORT_SIZE (1)
    0x15, 0x00,                    //     LOGICAL_MINIMUM (0)
    0x25, 0x01,                    //     LOGICAL_MAXIMUM (1)
    0x35, 0x00,                    //     PHYSICAL_MINIMUM (0)
    0x45, 0x01,                    //     PHYSICAL_MAXIMUM (1)
    0x95, 0x03,                    //     REPORT_COUNT (3)

    // Buttons 5, 6, 11
    0x05, 0x09,                    //     USAGE_PAGE (Button)
    0x09, 0x05,                    //     USAGE (Button 5)
    0x09, 0x06,                    //     USAGE (Button 6)
    0x09, 0x0b,                    //     USAGE (Button 11)
    0x81, 0x02,                    //     INPUT (Data,Var,Abs)
    0x75, 0x01,                    //     REPORT_SIZE (1)
    0x95, 0x01,                    //     REPORT_COUNT (1)

    // 4 Buttons as array
    0x81, 0x01,                    //     INPUT (Cnst,Ary,Abs)
    0x75, 0x01,                    //     REPORT_SIZE (1)
    0x15, 0x00,                    //     LOGICAL_MINIMUM (0)
    0x25, 0x01,                    //     LOGICAL_MAXIMUM (1)
    0x35, 0x00,                    //     PHYSICAL_MINIMUM (0)
    0x45, 0x01,                    //     PHYSICAL_MAXIMUM (1)
    0x95, 0x04,                    //     REPORT_COUNT (4)

        //2 Trigger buttons
        0x05, 0x09,                    //     USAGE_PAGE (Button)
        0x19, 0x01,                    //     USAGE_MINIMUM (Button 1)
        0x29, 0x04,                    //     USAGE_MAXIMUM (Button 4)
        0x81, 0x02,                    //     INPUT (Data,Var,Abs)
        0x75, 0x08,                    //     REPORT_SIZE (8)
        0x15, 0x00,                    //     LOGICAL_MINIMUM (0)
        0x26, 0xff, 0x00,              //     LOGICAL_MAXIMUM (255)
        0x35, 0x00,                    //     PHYSICAL_MINIMUM (0)
        0x46, 0xff, 0x00,              //     PHYSICAL_MAXIMUM (255)
        0x95, 0x02,                    //     REPORT_COUNT (2)

    // Axes Z, Rz
    0x05, 0x01,                    //     USAGE_PAGE (Generic Desktop)
    0x09, 0x32,                    //     USAGE (Z)
    0x09, 0x35,                    //     USAGE (Rz)
    0x81, 0x02,                    //     INPUT (Data,Var,Abs)
    0x75, 0x10,                    //     REPORT_SIZE (16)
    0x16, 0x00, 0x80,              //     LOGICAL_MINIMUM (-32768)
    0x26, 0xff, 0x7f,              //     LOGICAL_MAXIMUM (32767)
    0x36, 0x00, 0x80,              //     PHYSICAL_MINIMUM (-32768)
    0x46, 0xff, 0x7f,              //     PHYSICAL_MAXIMUM (32767)

    // Axes X, Y, Rx, Ry
    0x05, 0x01,                    //     USAGE_PAGE (Generic Desktop)
    0x09, 0x01,                    //     USAGE (Pointer)
    0xa1, 0x00,                    //     COLLECTION (Physical)
    0x95, 0x02,                    //       REPORT_COUNT (2)
    0x05, 0x01,                    //       USAGE_PAGE (Generic Desktop)
    0x09, 0x30,                    //       USAGE (X)
    0x09, 0x31,                    //       USAGE (Y)
    0x81, 0x02,                    //       INPUT (Data,Var,Abs)
    0xc0,                          //     END_COLLECTION
    0x05, 0x01,                    //     USAGE_PAGE (Generic Desktop)
    0x09, 0x01,                    //     USAGE (Pointer)
    0xa1, 0x00,                    //     COLLECTION (Physical)
    0x95, 0x02,                    //       REPORT_COUNT (2)
    0x05, 0x01,                    //       USAGE_PAGE (Generic Desktop)
    0x09, 0x33,                    //       USAGE (Rx)
    0x09, 0x34,                    //       USAGE (Ry)
    0x81, 0x02,                    //       INPUT (Data,Var,Abs)
    0xc0,                          //     END_COLLECTION        // Pointer
    0xc0,                          //   END_COLLECTION      // Counted buffer
    0xc0                           // END_COLLECTION    // Application
};
