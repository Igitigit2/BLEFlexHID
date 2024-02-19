#include <NimBLEDevice.h>
#include <NimBLEUtils.h>
#include <NimBLEServer.h>
#include "NimBLEHIDDevice.h"
#include "HIDTypes.h"
#include "HIDKeyboardTypes.h"
#include <driver/adc.h>
#include "sdkconfig.h"

#include "BleConnectionStatus.h"
#include "GamepadHIDSub.h"
#include "GamepadConfiguration.h"

#if defined(CONFIG_ARDUHAL_ESP_LOG)
#include "esp32-hal-log.h"
#define LOG_TAG "GamepadHIDSub"
#else
#include "esp_log.h"
static const char *LOG_TAG = "GamepadHIDSub";
#endif

#define SERVICE_UUID_DEVICE_INFORMATION        "180A"      // Service - Device information

#define CHARACTERISTIC_UUID_MODEL_NUMBER       "2A24"      // Characteristic - Model Number String - 0x2A24
#define CHARACTERISTIC_UUID_SOFTWARE_REVISION  "2A28"      // Characteristic - Software Revision String - 0x2A28
#define CHARACTERISTIC_UUID_SERIAL_NUMBER      "2A25"      // Characteristic - Serial Number String - 0x2A25
#define CHARACTERISTIC_UUID_FIRMWARE_REVISION  "2A26"      // Characteristic - Firmware Revision String - 0x2A26
#define CHARACTERISTIC_UUID_HARDWARE_REVISION  "2A27"      // Characteristic - Hardware Revision String - 0x2A27



// uint8_t tempHidReportDescriptor[150];
uint8_t tempHidReportDescriptor[300];
int hidReportDescriptorSize = 0;
uint8_t reportSize = 0;
uint8_t numOfButtonBytes = 0;
uint16_t vid;
uint16_t pid;
uint16_t guidVersion;
uint16_t axesMin;
uint16_t axesMax;
uint16_t simulationMin;
uint16_t simulationMax;
std::string modelNumber;
std::string softwareRevision;
std::string serialNumber;
std::string firmwareRevision;
std::string hardwareRevision;

GamepadHIDSub::GamepadHIDSub() : _buttons(),
                                                                                                       _specialButtons(0),
                                                                                                       _x(0),
                                                                                                       _y(0),
                                                                                                       _z(0),
                                                                                                       _rZ(0),
                                                                                                       _rX(0),
                                                                                                       _rY(0),
                                                                                                       _slider1(0),
                                                                                                       _slider2(0),
                                                                                                       _rudder(0),
                                                                                                       _throttle(0),
                                                                                                       _accelerator(0),
                                                                                                       _brake(0),
                                                                                                       _steering(0),
                                                                                                       _hat1(0),
                                                                                                       _hat2(0),
                                                                                                       _hat3(0),
                                                                                                       _hat4(0)
{
    this->resetButtons();
}

void GamepadHIDSub::resetButtons()
{
    memset(&_buttons, 0, sizeof(_buttons));
}

void GamepadHIDSub::Configure(GamepadConfiguration *config)
{
    configuration = *config; // we make a copy, so the user can't change actual values midway through operation, without calling the begin function again

    uint8_t buttonPaddingBits = 8 - (configuration.getButtonCount() % 8);
    if (buttonPaddingBits == 8)
    {
        buttonPaddingBits = 0;
    }
    uint8_t specialButtonPaddingBits = 8 - (configuration.getTotalSpecialButtonCount() % 8);
    if (specialButtonPaddingBits == 8)
    {
        specialButtonPaddingBits = 0;
    }
    uint8_t numOfAxisBytes = configuration.getAxisCount() * 2;
    uint8_t numOfSimulationBytes = configuration.getSimulationCount() * 2;

    numOfButtonBytes = configuration.getButtonCount() / 8;
    if (buttonPaddingBits > 0)
    {
        numOfButtonBytes++;
    }

    uint8_t numOfSpecialButtonBytes = configuration.getTotalSpecialButtonCount() / 8;
    if (specialButtonPaddingBits > 0)
    {
        numOfSpecialButtonBytes++;
    }

    reportSize = numOfButtonBytes + numOfSpecialButtonBytes + numOfAxisBytes + numOfSimulationBytes + configuration.getHatSwitchCount();

    // USAGE_PAGE (Generic Desktop)
    tempHidReportDescriptor[hidReportDescriptorSize++] = 0x05;
    tempHidReportDescriptor[hidReportDescriptorSize++] = 0x01;

    // USAGE (Joystick - 0x04; Gamepad - 0x05; Multi-axis Controller - 0x08)
    tempHidReportDescriptor[hidReportDescriptorSize++] = 0x09;
    tempHidReportDescriptor[hidReportDescriptorSize++] = configuration.getControllerType();

    // COLLECTION (Application)
    tempHidReportDescriptor[hidReportDescriptorSize++] = 0xa1;
    tempHidReportDescriptor[hidReportDescriptorSize++] = 0x01;

    // REPORT_ID (Default: 5)
    tempHidReportDescriptor[hidReportDescriptorSize++] = 0x85;
    tempHidReportDescriptor[hidReportDescriptorSize++] = GAMEPAD_REPORT_ID; //configuration.getHidReportId();        // resolves default to 3

    if (configuration.getButtonCount() > 0)
    {

        // USAGE_PAGE (Button)
        tempHidReportDescriptor[hidReportDescriptorSize++] = 0x05;
        tempHidReportDescriptor[hidReportDescriptorSize++] = 0x09;

        // LOGICAL_MINIMUM (0)
        tempHidReportDescriptor[hidReportDescriptorSize++] = 0x15;
        tempHidReportDescriptor[hidReportDescriptorSize++] = 0x00;

        // LOGICAL_MAXIMUM (1)
        tempHidReportDescriptor[hidReportDescriptorSize++] = 0x25;
        tempHidReportDescriptor[hidReportDescriptorSize++] = 0x01;

        // REPORT_SIZE (1)
        tempHidReportDescriptor[hidReportDescriptorSize++] = 0x75;
        tempHidReportDescriptor[hidReportDescriptorSize++] = 0x01;

        // USAGE_MINIMUM (Button 1)
        tempHidReportDescriptor[hidReportDescriptorSize++] = 0x19;
        tempHidReportDescriptor[hidReportDescriptorSize++] = 0x01;

        // USAGE_MAXIMUM (Up to 128 buttons possible)
        tempHidReportDescriptor[hidReportDescriptorSize++] = 0x29;
        tempHidReportDescriptor[hidReportDescriptorSize++] = configuration.getButtonCount();

        // REPORT_COUNT (# of buttons)
        tempHidReportDescriptor[hidReportDescriptorSize++] = 0x95;
        tempHidReportDescriptor[hidReportDescriptorSize++] = configuration.getButtonCount();

        // INPUT (Data,Var,Abs)
        tempHidReportDescriptor[hidReportDescriptorSize++] = 0x81;
        tempHidReportDescriptor[hidReportDescriptorSize++] = 0x02;

        if (buttonPaddingBits > 0)
        {

            // REPORT_SIZE (1)
            tempHidReportDescriptor[hidReportDescriptorSize++] = 0x75;
            tempHidReportDescriptor[hidReportDescriptorSize++] = 0x01;

            // REPORT_COUNT (# of padding bits)
            tempHidReportDescriptor[hidReportDescriptorSize++] = 0x95;
            tempHidReportDescriptor[hidReportDescriptorSize++] = buttonPaddingBits;

            // INPUT (Const,Var,Abs)
            tempHidReportDescriptor[hidReportDescriptorSize++] = 0x81;
            tempHidReportDescriptor[hidReportDescriptorSize++] = 0x03;

        } // Padding Bits Needed

    } // Buttons

    if (configuration.getTotalSpecialButtonCount() > 0)
    {
        // LOGICAL_MINIMUM (0)
        tempHidReportDescriptor[hidReportDescriptorSize++] = 0x15;
        tempHidReportDescriptor[hidReportDescriptorSize++] = 0x00;

        // LOGICAL_MAXIMUM (1)
        tempHidReportDescriptor[hidReportDescriptorSize++] = 0x25;
        tempHidReportDescriptor[hidReportDescriptorSize++] = 0x01;

        // REPORT_SIZE (1)
        tempHidReportDescriptor[hidReportDescriptorSize++] = 0x75;
        tempHidReportDescriptor[hidReportDescriptorSize++] = 0x01;

        if (configuration.getDesktopSpecialButtonCount() > 0)
        {

            // USAGE_PAGE (Generic Desktop)
            tempHidReportDescriptor[hidReportDescriptorSize++] = 0x05;
            tempHidReportDescriptor[hidReportDescriptorSize++] = 0x01;

            // REPORT_COUNT
            tempHidReportDescriptor[hidReportDescriptorSize++] = 0x95;
            tempHidReportDescriptor[hidReportDescriptorSize++] = configuration.getDesktopSpecialButtonCount();
            if (configuration.getIncludeStart())
            {
                // USAGE (Start)
                tempHidReportDescriptor[hidReportDescriptorSize++] = 0x09;
                tempHidReportDescriptor[hidReportDescriptorSize++] = 0x3D;
            }

            if (configuration.getIncludeSelect())
            {
                // USAGE (Select)
                tempHidReportDescriptor[hidReportDescriptorSize++] = 0x09;
                tempHidReportDescriptor[hidReportDescriptorSize++] = 0x3E;
            }

            if (configuration.getIncludeMenu())
            {
                // USAGE (App Menu)
                tempHidReportDescriptor[hidReportDescriptorSize++] = 0x09;
                tempHidReportDescriptor[hidReportDescriptorSize++] = 0x86;
            }

            // INPUT (Data,Var,Abs)
            tempHidReportDescriptor[hidReportDescriptorSize++] = 0x81;
            tempHidReportDescriptor[hidReportDescriptorSize++] = 0x02;
        }

        if (configuration.getConsumerSpecialButtonCount() > 0)
        {

            // USAGE_PAGE (Consumer Page)
            tempHidReportDescriptor[hidReportDescriptorSize++] = 0x05;
            tempHidReportDescriptor[hidReportDescriptorSize++] = 0x0C;

            // REPORT_COUNT
            tempHidReportDescriptor[hidReportDescriptorSize++] = 0x95;
            tempHidReportDescriptor[hidReportDescriptorSize++] = configuration.getConsumerSpecialButtonCount();

            if (configuration.getIncludeHome())
            {
                // USAGE (Home)
                tempHidReportDescriptor[hidReportDescriptorSize++] = 0x0A;
                tempHidReportDescriptor[hidReportDescriptorSize++] = 0x23;
		            tempHidReportDescriptor[hidReportDescriptorSize++] = 0x02;
            }

            if (configuration.getIncludeBack())
            {
                // USAGE (Back)
                tempHidReportDescriptor[hidReportDescriptorSize++] = 0x0A;
                tempHidReportDescriptor[hidReportDescriptorSize++] = 0x24;
	        	    tempHidReportDescriptor[hidReportDescriptorSize++] = 0x02;
            }

            if (configuration.getIncludeVolumeInc())
            {
                // USAGE (Volume Increment)
                tempHidReportDescriptor[hidReportDescriptorSize++] = 0x09;
                tempHidReportDescriptor[hidReportDescriptorSize++] = 0xE9;
            }

            if (configuration.getIncludeVolumeDec())
            {
                // USAGE (Volume Decrement)
                tempHidReportDescriptor[hidReportDescriptorSize++] = 0x09;
                tempHidReportDescriptor[hidReportDescriptorSize++] = 0xEA;
            }

            if (configuration.getIncludeVolumeMute())
            {
                // USAGE (Mute)
                tempHidReportDescriptor[hidReportDescriptorSize++] = 0x09;
                tempHidReportDescriptor[hidReportDescriptorSize++] = 0xE2;
            }

            // INPUT (Data,Var,Abs)
            tempHidReportDescriptor[hidReportDescriptorSize++] = 0x81;
            tempHidReportDescriptor[hidReportDescriptorSize++] = 0x02;
        }

        if (specialButtonPaddingBits > 0)
        {

            // REPORT_SIZE (1)
            tempHidReportDescriptor[hidReportDescriptorSize++] = 0x75;
            tempHidReportDescriptor[hidReportDescriptorSize++] = 0x01;

            // REPORT_COUNT (# of padding bits)
            tempHidReportDescriptor[hidReportDescriptorSize++] = 0x95;
            tempHidReportDescriptor[hidReportDescriptorSize++] = specialButtonPaddingBits;

            // INPUT (Const,Var,Abs)
            tempHidReportDescriptor[hidReportDescriptorSize++] = 0x81;
            tempHidReportDescriptor[hidReportDescriptorSize++] = 0x03;

        } // Padding Bits Needed

    } // Special Buttons

    if (configuration.getAxisCount() > 0)
    {
        // USAGE_PAGE (Generic Desktop)
        tempHidReportDescriptor[hidReportDescriptorSize++] = 0x05;
        tempHidReportDescriptor[hidReportDescriptorSize++] = 0x01;

        // USAGE (Pointer)
        tempHidReportDescriptor[hidReportDescriptorSize++] = 0x09;
        tempHidReportDescriptor[hidReportDescriptorSize++] = 0x01;

        // LOGICAL_MINIMUM (-32767)
        tempHidReportDescriptor[hidReportDescriptorSize++] = 0x16;
        tempHidReportDescriptor[hidReportDescriptorSize++] = lowByte(configuration.getAxesMin());
        tempHidReportDescriptor[hidReportDescriptorSize++] = highByte(configuration.getAxesMin());
        //tempHidReportDescriptor[hidReportDescriptorSize++] = 0x00;		// Use these two lines for 0 min
        //tempHidReportDescriptor[hidReportDescriptorSize++] = 0x00;
		    //tempHidReportDescriptor[hidReportDescriptorSize++] = 0x01;	// Use these two lines for -32767 min
        //tempHidReportDescriptor[hidReportDescriptorSize++] = 0x80;

        // LOGICAL_MAXIMUM (+32767)
        tempHidReportDescriptor[hidReportDescriptorSize++] = 0x26;
        tempHidReportDescriptor[hidReportDescriptorSize++] = lowByte(configuration.getAxesMax());
        tempHidReportDescriptor[hidReportDescriptorSize++] = highByte(configuration.getAxesMax());
        //tempHidReportDescriptor[hidReportDescriptorSize++] = 0xFF;	// Use these two lines for 255 max
        //tempHidReportDescriptor[hidReportDescriptorSize++] = 0x00;
		    //tempHidReportDescriptor[hidReportDescriptorSize++] = 0xFF;	// Use these two lines for +32767 max
        //tempHidReportDescriptor[hidReportDescriptorSize++] = 0x7F;

        // REPORT_SIZE (16)
        tempHidReportDescriptor[hidReportDescriptorSize++] = 0x75;
        tempHidReportDescriptor[hidReportDescriptorSize++] = 0x10;

        // REPORT_COUNT (configuration.getAxisCount())
        tempHidReportDescriptor[hidReportDescriptorSize++] = 0x95;
        tempHidReportDescriptor[hidReportDescriptorSize++] = configuration.getAxisCount();

            // COLLECTION (Physical)
            tempHidReportDescriptor[hidReportDescriptorSize++] = 0xA1;
            tempHidReportDescriptor[hidReportDescriptorSize++] = 0x00;

            if (configuration.getIncludeXAxis())
            {
                // USAGE (X)
                tempHidReportDescriptor[hidReportDescriptorSize++] = 0x09;
                tempHidReportDescriptor[hidReportDescriptorSize++] = 0x30;
            }

            if (configuration.getIncludeYAxis())
            {
                // USAGE (Y)
                tempHidReportDescriptor[hidReportDescriptorSize++] = 0x09;
                tempHidReportDescriptor[hidReportDescriptorSize++] = 0x31;
            }

            if (configuration.getIncludeZAxis())
            {
                // USAGE (Z)
                tempHidReportDescriptor[hidReportDescriptorSize++] = 0x09;
                tempHidReportDescriptor[hidReportDescriptorSize++] = 0x32;
            }

            if (configuration.getIncludeRzAxis())
            {
                // USAGE (Rz)
                tempHidReportDescriptor[hidReportDescriptorSize++] = 0x09;
                tempHidReportDescriptor[hidReportDescriptorSize++] = 0x35;
            }

            if (configuration.getIncludeRxAxis())
            {
                // USAGE (Rx)
                tempHidReportDescriptor[hidReportDescriptorSize++] = 0x09;
                tempHidReportDescriptor[hidReportDescriptorSize++] = 0x33;
            }

            if (configuration.getIncludeRyAxis())
            {
                // USAGE (Ry)
                tempHidReportDescriptor[hidReportDescriptorSize++] = 0x09;
                tempHidReportDescriptor[hidReportDescriptorSize++] = 0x34;
            }

            if (configuration.getIncludeSlider1())
            {
                // USAGE (Slider)
                tempHidReportDescriptor[hidReportDescriptorSize++] = 0x09;
                tempHidReportDescriptor[hidReportDescriptorSize++] = 0x36;
            }

            if (configuration.getIncludeSlider2())
            {
                // USAGE (Slider)
                tempHidReportDescriptor[hidReportDescriptorSize++] = 0x09;
                tempHidReportDescriptor[hidReportDescriptorSize++] = 0x36;
            }

            // INPUT (Data,Var,Abs)
            tempHidReportDescriptor[hidReportDescriptorSize++] = 0x81;
            tempHidReportDescriptor[hidReportDescriptorSize++] = 0x02;

            // END_COLLECTION (Physical)
            tempHidReportDescriptor[hidReportDescriptorSize++] = 0xc0;

    } // X, Y, Z, Rx, Ry, and Rz Axis

    if (configuration.getSimulationCount() > 0)
    {

        // USAGE_PAGE (Simulation Controls)
        tempHidReportDescriptor[hidReportDescriptorSize++] = 0x05;
        tempHidReportDescriptor[hidReportDescriptorSize++] = 0x02;

        // LOGICAL_MINIMUM (-32767)
        tempHidReportDescriptor[hidReportDescriptorSize++] = 0x16;
        tempHidReportDescriptor[hidReportDescriptorSize++] = lowByte(configuration.getSimulationMin());
        tempHidReportDescriptor[hidReportDescriptorSize++] = highByte(configuration.getSimulationMin());
        //tempHidReportDescriptor[hidReportDescriptorSize++] = 0x00;		// Use these two lines for 0 min
        //tempHidReportDescriptor[hidReportDescriptorSize++] = 0x00;
		//tempHidReportDescriptor[hidReportDescriptorSize++] = 0x01;	    // Use these two lines for -32767 min
        //tempHidReportDescriptor[hidReportDescriptorSize++] = 0x80;

        // LOGICAL_MAXIMUM (+32767)
        tempHidReportDescriptor[hidReportDescriptorSize++] = 0x26;
        tempHidReportDescriptor[hidReportDescriptorSize++] = lowByte(configuration.getSimulationMax());
        tempHidReportDescriptor[hidReportDescriptorSize++] = highByte(configuration.getSimulationMax());
        //tempHidReportDescriptor[hidReportDescriptorSize++] = 0xFF;	    // Use these two lines for 255 max
        //tempHidReportDescriptor[hidReportDescriptorSize++] = 0x00;
		//tempHidReportDescriptor[hidReportDescriptorSize++] = 0xFF;		// Use these two lines for +32767 max
        //tempHidReportDescriptor[hidReportDescriptorSize++] = 0x7F;

        // REPORT_SIZE (16)
        tempHidReportDescriptor[hidReportDescriptorSize++] = 0x75;
        tempHidReportDescriptor[hidReportDescriptorSize++] = 0x10;

        // REPORT_COUNT (configuration.getSimulationCount())
        tempHidReportDescriptor[hidReportDescriptorSize++] = 0x95;
        tempHidReportDescriptor[hidReportDescriptorSize++] = configuration.getSimulationCount();

            // COLLECTION (Physical)
            tempHidReportDescriptor[hidReportDescriptorSize++] = 0xA1;
            tempHidReportDescriptor[hidReportDescriptorSize++] = 0x00;

            if (configuration.getIncludeRudder())
            {
                // USAGE (Rudder)
                tempHidReportDescriptor[hidReportDescriptorSize++] = 0x09;
                tempHidReportDescriptor[hidReportDescriptorSize++] = 0xBA;
            }

            if (configuration.getIncludeThrottle())
            {
                // USAGE (Throttle)
                tempHidReportDescriptor[hidReportDescriptorSize++] = 0x09;
                tempHidReportDescriptor[hidReportDescriptorSize++] = 0xBB;
            }

            if (configuration.getIncludeAccelerator())
            {
                // USAGE (Accelerator)
                tempHidReportDescriptor[hidReportDescriptorSize++] = 0x09;
                tempHidReportDescriptor[hidReportDescriptorSize++] = 0xC4;
            }

            if (configuration.getIncludeBrake())
            {
                // USAGE (Brake)
                tempHidReportDescriptor[hidReportDescriptorSize++] = 0x09;
                tempHidReportDescriptor[hidReportDescriptorSize++] = 0xC5;
            }

            if (configuration.getIncludeSteering())
            {
                // USAGE (Steering)
                tempHidReportDescriptor[hidReportDescriptorSize++] = 0x09;
                tempHidReportDescriptor[hidReportDescriptorSize++] = 0xC8;
            }

            // INPUT (Data,Var,Abs)
            tempHidReportDescriptor[hidReportDescriptorSize++] = 0x81;
            tempHidReportDescriptor[hidReportDescriptorSize++] = 0x02;

            // END_COLLECTION (Physical)
            tempHidReportDescriptor[hidReportDescriptorSize++] = 0xc0;

    } // Simulation Controls

    if (configuration.getHatSwitchCount() > 0)
    {

            // COLLECTION (Physical)
            tempHidReportDescriptor[hidReportDescriptorSize++] = 0xA1;
            tempHidReportDescriptor[hidReportDescriptorSize++] = 0x00;

            // USAGE_PAGE (Generic Desktop)
            tempHidReportDescriptor[hidReportDescriptorSize++] = USAGE_PAGE(1);
            tempHidReportDescriptor[hidReportDescriptorSize++] = 0x01;

            // USAGE (Hat Switch)
            for (int currentHatIndex = 0; currentHatIndex < configuration.getHatSwitchCount(); currentHatIndex++)
            {
                tempHidReportDescriptor[hidReportDescriptorSize++] = USAGE(1);
                tempHidReportDescriptor[hidReportDescriptorSize++] = 0x39;
            }

            // Logical Min (1)
            tempHidReportDescriptor[hidReportDescriptorSize++] = 0x15;
            tempHidReportDescriptor[hidReportDescriptorSize++] = 0x01;

            // Logical Max (8)
            tempHidReportDescriptor[hidReportDescriptorSize++] = 0x25;
            tempHidReportDescriptor[hidReportDescriptorSize++] = 0x08;

            // Physical Min (0)
            tempHidReportDescriptor[hidReportDescriptorSize++] = 0x35;
            tempHidReportDescriptor[hidReportDescriptorSize++] = 0x00;

            // Physical Max (315)
            tempHidReportDescriptor[hidReportDescriptorSize++] = 0x46;
            tempHidReportDescriptor[hidReportDescriptorSize++] = 0x3B;
            tempHidReportDescriptor[hidReportDescriptorSize++] = 0x01;

            // Unit (SI Rot : Ang Pos)
            tempHidReportDescriptor[hidReportDescriptorSize++] = 0x65;
            tempHidReportDescriptor[hidReportDescriptorSize++] = 0x12;

            // Report Size (8)
            tempHidReportDescriptor[hidReportDescriptorSize++] = 0x75;
            tempHidReportDescriptor[hidReportDescriptorSize++] = 0x08;

            // Report Count (4)
            tempHidReportDescriptor[hidReportDescriptorSize++] = 0x95;
            tempHidReportDescriptor[hidReportDescriptorSize++] = configuration.getHatSwitchCount();

            // Input (Data, Variable, Absolute)
            tempHidReportDescriptor[hidReportDescriptorSize++] = 0x81;
            tempHidReportDescriptor[hidReportDescriptorSize++] = 0x42;

            // END_COLLECTION (Physical)
            tempHidReportDescriptor[hidReportDescriptorSize++] = 0xc0;
    }

    // END_COLLECTION (Application)
    tempHidReportDescriptor[hidReportDescriptorSize++] = 0xc0;

    Serial.printf("Gamepad HID descriptor size: %d\n", hidReportDescriptorSize);
}

void GamepadHIDSub::setAxes(int16_t x, int16_t y, int16_t z, int16_t rZ, int16_t rX, int16_t rY, int16_t slider1, int16_t slider2)
{
    if (x == -32768)
    {
        x = -32767;
    }
    if (y == -32768)
    {
        y = -32767;
    }
    if (z == -32768)
    {
        z = -32767;
    }
    if (rZ == -32768)
    {
        rZ = -32767;
    }
    if (rX == -32768)
    {
        rX = -32767;
    }
    if (rY == -32768)
    {
        rY = -32767;
    }
    if (slider1 == -32768)
    {
        slider1 = -32767;
    }
    if (slider2 == -32768)
    {
        slider2 = -32767;
    }

    _x = x;
    _y = y;
    _z = z;
    _rZ = rZ;
    _rX = rX;
    _rY = rY;
    _slider1 = slider1;
    _slider2 = slider2;

    if (configuration.getAutoReport())
    {
        sendReport();
    }
}

void GamepadHIDSub::setSimulationControls(int16_t rudder, int16_t throttle, int16_t accelerator, int16_t brake, int16_t steering)
{
    if (rudder == -32768)
    {
        rudder = -32767;
    }
    if (throttle == -32768)
    {
        throttle = -32767;
    }
    if (accelerator == -32768)
    {
        accelerator = -32767;
    }
    if (brake == -32768)
    {
        brake = -32767;
    }
    if (steering == -32768)
    {
        steering = -32767;
    }

    _rudder = rudder;
    _throttle = throttle;
    _accelerator = accelerator;
    _brake = brake;
    _steering = steering;

    if (configuration.getAutoReport())
    {
        sendReport();
    }
}

void GamepadHIDSub::setHats(signed char hat1, signed char hat2, signed char hat3, signed char hat4)
{
    _hat1 = hat1;
    _hat2 = hat2;
    _hat3 = hat3;
    _hat4 = hat4;

    if (configuration.getAutoReport())
    {
        sendReport();
    }
}

void GamepadHIDSub::setSliders(int16_t slider1, int16_t slider2)
{
    if (slider1 == -32768)
    {
        slider1 = -32767;
    }
    if (slider2 == -32768)
    {
        slider2 = -32767;
    }

    _slider1 = slider1;
    _slider2 = slider2;

    if (configuration.getAutoReport())
    {
        sendReport();
    }
}

void GamepadHIDSub::sendReport(void)
{
    if (IsConnected())
    {
        uint8_t currentReportIndex = 0;

        uint8_t m[reportSize];

        memset(&m, 0, sizeof(m));
        memcpy(&m, &_buttons, sizeof(_buttons));

        currentReportIndex += numOfButtonBytes;

        if (configuration.getTotalSpecialButtonCount() > 0)
        {
            m[currentReportIndex++] = _specialButtons;
        }

        if (configuration.getIncludeXAxis())
        {
            m[currentReportIndex++] = _x;
            m[currentReportIndex++] = (_x >> 8);
        }
        if (configuration.getIncludeYAxis())
        {
            m[currentReportIndex++] = _y;
            m[currentReportIndex++] = (_y >> 8);
        }
        if (configuration.getIncludeZAxis())
        {
            m[currentReportIndex++] = _z;
            m[currentReportIndex++] = (_z >> 8);
        }
        if (configuration.getIncludeRzAxis())
        {
            m[currentReportIndex++] = _rZ;
            m[currentReportIndex++] = (_rZ >> 8);
        }
        if (configuration.getIncludeRxAxis())
        {
            m[currentReportIndex++] = _rX;
            m[currentReportIndex++] = (_rX >> 8);
        }
        if (configuration.getIncludeRyAxis())
        {
            m[currentReportIndex++] = _rY;
            m[currentReportIndex++] = (_rY >> 8);
        }

        if (configuration.getIncludeSlider1())
        {
            m[currentReportIndex++] = _slider1;
            m[currentReportIndex++] = (_slider1 >> 8);
        }
        if (configuration.getIncludeSlider2())
        {
            m[currentReportIndex++] = _slider2;
            m[currentReportIndex++] = (_slider2 >> 8);
        }

        if (configuration.getIncludeRudder())
        {
            m[currentReportIndex++] = _rudder;
            m[currentReportIndex++] = (_rudder >> 8);
        }
        if (configuration.getIncludeThrottle())
        {
            m[currentReportIndex++] = _throttle;
            m[currentReportIndex++] = (_throttle >> 8);
        }
        if (configuration.getIncludeAccelerator())
        {
            m[currentReportIndex++] = _accelerator;
            m[currentReportIndex++] = (_accelerator >> 8);
        }
        if (configuration.getIncludeBrake())
        {
            m[currentReportIndex++] = _brake;
            m[currentReportIndex++] = (_brake >> 8);
        }
        if (configuration.getIncludeSteering())
        {
            m[currentReportIndex++] = _steering;
            m[currentReportIndex++] = (_steering >> 8);
        }

        if (configuration.getHatSwitchCount() > 0)
        {
            signed char hats[4];

            hats[0] = _hat1;
            hats[1] = _hat2;
            hats[2] = _hat3;
            hats[3] = _hat4;

            for (int currentHatIndex = configuration.getHatSwitchCount() - 1; currentHatIndex >= 0; currentHatIndex--)
            {
                m[currentReportIndex++] = hats[currentHatIndex];
            }
        }

        SendReport(m, sizeof(m), 0);
    }
}

void GamepadHIDSub::press(uint8_t b)
{
    uint8_t index = (b - 1) / 8;
    uint8_t bit = (b - 1) % 8;
    uint8_t bitmask = (1 << bit);

    uint8_t result = _buttons[index] | bitmask;

    if (result != _buttons[index])
    {
        _buttons[index] = result;
    }

    if (configuration.getAutoReport())
    {
        sendReport();
    }
}

void GamepadHIDSub::release(uint8_t b)
{
    uint8_t index = (b - 1) / 8;
    uint8_t bit = (b - 1) % 8;
    uint8_t bitmask = (1 << bit);

    uint64_t result = _buttons[index] & ~bitmask;

    if (result != _buttons[index])
    {
        _buttons[index] = result;
    }

    if (configuration.getAutoReport())
    {
        sendReport();
    }
}

uint8_t GamepadHIDSub::specialButtonBitPosition(uint8_t b)
{
    if (b >= POSSIBLESPECIALBUTTONS)
        throw std::invalid_argument("Index out of range");
    uint8_t bit = 0;
    for (int i = 0; i < b; i++)
    {
        if (configuration.getWhichSpecialButtons()[i])
            bit++;
    }
    return bit;
}

void GamepadHIDSub::pressSpecialButton(uint8_t b)
{
    uint8_t button = specialButtonBitPosition(b);
    uint8_t bit = button % 8;
    uint8_t bitmask = (1 << bit);

    uint64_t result = _specialButtons | bitmask;

    if (result != _specialButtons)
    {
        _specialButtons = result;
    }

    if (configuration.getAutoReport())
    {
        sendReport();
    }
}

void GamepadHIDSub::releaseSpecialButton(uint8_t b)
{
    uint8_t button = specialButtonBitPosition(b);
    uint8_t bit = button % 8;
    uint8_t bitmask = (1 << bit);

    uint64_t result = _specialButtons & ~bitmask;

    if (result != _specialButtons)
    {
        _specialButtons = result;
    }

    if (configuration.getAutoReport())
    {
        sendReport();
    }
}

void GamepadHIDSub::pressStart()
{
    pressSpecialButton(START_BUTTON);
}

void GamepadHIDSub::releaseStart()
{
    releaseSpecialButton(START_BUTTON);
}

void GamepadHIDSub::pressSelect()
{
    pressSpecialButton(SELECT_BUTTON);
}

void GamepadHIDSub::releaseSelect()
{
    releaseSpecialButton(SELECT_BUTTON);
}

void GamepadHIDSub::pressMenu()
{
    pressSpecialButton(MENU_BUTTON);
}

void GamepadHIDSub::releaseMenu()
{
    releaseSpecialButton(MENU_BUTTON);
}

void GamepadHIDSub::pressHome()
{
    pressSpecialButton(HOME_BUTTON);
}

void GamepadHIDSub::releaseHome()
{
    releaseSpecialButton(HOME_BUTTON);
}

void GamepadHIDSub::pressBack()
{
    pressSpecialButton(BACK_BUTTON);
}

void GamepadHIDSub::releaseBack()
{
    releaseSpecialButton(BACK_BUTTON);
}

void GamepadHIDSub::pressVolumeInc()
{
    pressSpecialButton(VOLUME_INC_BUTTON);
}

void GamepadHIDSub::releaseVolumeInc()
{
    releaseSpecialButton(VOLUME_INC_BUTTON);
}

void GamepadHIDSub::pressVolumeDec()
{
    pressSpecialButton(VOLUME_DEC_BUTTON);
}

void GamepadHIDSub::releaseVolumeDec()
{
    releaseSpecialButton(VOLUME_DEC_BUTTON);
}

void GamepadHIDSub::pressVolumeMute()
{
    pressSpecialButton(VOLUME_MUTE_BUTTON);
}

void GamepadHIDSub::releaseVolumeMute()
{
    releaseSpecialButton(VOLUME_MUTE_BUTTON);
}

void GamepadHIDSub::setLeftThumb(int16_t x, int16_t y)
{
    if (x == -32768)
    {
        x = -32767;
    }
    if (y == -32768)
    {
        y = -32767;
    }

    _x = x;
    _y = y;

    if (configuration.getAutoReport())
    {
        sendReport();
    }
}

void GamepadHIDSub::setRightThumb(int16_t z, int16_t rZ)
{
    if (z == -32768)
    {
        z = -32767;
    }
    if (rZ == -32768)
    {
        rZ = -32767;
    }

    _z = z;
    _rZ = rZ;

    if (configuration.getAutoReport())
    {
        sendReport();
    }
}

void GamepadHIDSub::setLeftTrigger(int16_t rX)
{
    if (rX == -32768)
    {
        rX = -32767;
    }

    _rX = rX;

    if (configuration.getAutoReport())
    {
        sendReport();
    }
}

void GamepadHIDSub::setRightTrigger(int16_t rY)
{
    if (rY == -32768)
    {
        rY = -32767;
    }

    _rY = rY;

    if (configuration.getAutoReport())
    {
        sendReport();
    }
}

void GamepadHIDSub::setTriggers(int16_t rX, int16_t rY)
{
    if (rX == -32768)
    {
        rX = -32767;
    }
    if (rY == -32768)
    {
        rY = -32767;
    }

    _rX = rX;
    _rY = rY;

    if (configuration.getAutoReport())
    {
        sendReport();
    }
}

void GamepadHIDSub::setHat(signed char hat)
{
    _hat1 = hat;

    if (configuration.getAutoReport())
    {
        sendReport();
    }
}

void GamepadHIDSub::setHat1(signed char hat1)
{
    _hat1 = hat1;

    if (configuration.getAutoReport())
    {
        sendReport();
    }
}

void GamepadHIDSub::setHat2(signed char hat2)
{
    _hat2 = hat2;

    if (configuration.getAutoReport())
    {
        sendReport();
    }
}

void GamepadHIDSub::setHat3(signed char hat3)
{
    _hat3 = hat3;

    if (configuration.getAutoReport())
    {
        sendReport();
    }
}

void GamepadHIDSub::setHat4(signed char hat4)
{
    _hat4 = hat4;

    if (configuration.getAutoReport())
    {
        sendReport();
    }
}

void GamepadHIDSub::setX(int16_t x)
{
    if (x == -32768)
    {
        x = -32767;
    }

    _x = x;

    if (configuration.getAutoReport())
    {
        sendReport();
    }
}

void GamepadHIDSub::setY(int16_t y)
{
    if (y == -32768)
    {
        y = -32767;
    }

    _y = y;

    if (configuration.getAutoReport())
    {
        sendReport();
    }
}

void GamepadHIDSub::setZ(int16_t z)
{
    if (z == -32768)
    {
        z = -32767;
    }

    _z = z;

    if (configuration.getAutoReport())
    {
        sendReport();
    }
}

void GamepadHIDSub::setRZ(int16_t rZ)
{
    if (rZ == -32768)
    {
        rZ = -32767;
    }

    _rZ = rZ;

    if (configuration.getAutoReport())
    {
        sendReport();
    }
}

void GamepadHIDSub::setRX(int16_t rX)
{
    if (rX == -32768)
    {
        rX = -32767;
    }

    _rX = rX;

    if (configuration.getAutoReport())
    {
        sendReport();
    }
}

void GamepadHIDSub::setRY(int16_t rY)
{
    if (rY == -32768)
    {
        rY = -32767;
    }

    _rY = rY;

    if (configuration.getAutoReport())
    {
        sendReport();
    }
}

void GamepadHIDSub::setSlider(int16_t slider)
{
    if (slider == -32768)
    {
        slider = -32767;
    }

    _slider1 = slider;

    if (configuration.getAutoReport())
    {
        sendReport();
    }
}

void GamepadHIDSub::setSlider1(int16_t slider1)
{
    if (slider1 == -32768)
    {
        slider1 = -32767;
    }

    _slider1 = slider1;

    if (configuration.getAutoReport())
    {
        sendReport();
    }
}

void GamepadHIDSub::setSlider2(int16_t slider2)
{
    if (slider2 == -32768)
    {
        slider2 = -32767;
    }

    _slider2 = slider2;

    if (configuration.getAutoReport())
    {
        sendReport();
    }
}

void GamepadHIDSub::setRudder(int16_t rudder)
{
    if (rudder == -32768)
    {
        rudder = -32767;
    }

    _rudder = rudder;

    if (configuration.getAutoReport())
    {
        sendReport();
    }
}

void GamepadHIDSub::setThrottle(int16_t throttle)
{
    if (throttle == -32768)
    {
        throttle = -32767;
    }

    _throttle = throttle;

    if (configuration.getAutoReport())
    {
        sendReport();
    }
}

void GamepadHIDSub::setAccelerator(int16_t accelerator)
{
    if (accelerator == -32768)
    {
        accelerator = -32767;
    }

    _accelerator = accelerator;

    if (configuration.getAutoReport())
    {
        sendReport();
    }
}

void GamepadHIDSub::setBrake(int16_t brake)
{
    if (brake == -32768)
    {
        brake = -32767;
    }

    _brake = brake;

    if (configuration.getAutoReport())
    {
        sendReport();
    }
}

void GamepadHIDSub::setSteering(int16_t steering)
{
    if (steering == -32768)
    {
        steering = -32767;
    }

    _steering = steering;

    if (configuration.getAutoReport())
    {
        sendReport();
    }
}

bool GamepadHIDSub::isPressed(uint8_t b)
{
    uint8_t index = (b - 1) / 8;
    uint8_t bit = (b - 1) % 8;
    uint8_t bitmask = (1 << bit);

    if ((bitmask & _buttons[index]) > 0)
        return true;
    return false;
}


//************************************************************
// Some higher level convenience functions


// WARNING: Buttons in Gamepad start with 1!
void GamepadHIDSub::PressButton(int i, int DurationMs, int DelayAfterMs)
{
    press(i);
    sendReport();
    delay(DurationMs);
    release(i);
    sendReport();
    delay(DelayAfterMs);
}


// Wait for a key press on Serial.
void GPWaitForSerial()
{
    while(Serial.available()){Serial.read();}
    while (!Serial.available()) { }
    Serial.println(Serial.read());
}

// Press all buttons etc. in the sequence as required by Steam to configure a 
// generic gamepad. The GamepadHIDSub object needs to be configured with a
// SteamGamepadConfiguration object.
void RunSteamGamepadTest(GamepadHIDSub* Gamepad)
{
	#define TEST_STEP                   1024
	#define STEAM_DELAY_BETWEEN_REPORTS 200
    #define BTN_PRESS_DURATION          100
    #define BTN_DELAY_AFTER_RELEASE     200

	Gamepad->setX(0);
	Gamepad->setY(0);
	Gamepad->setRX(0);
	Gamepad->setRY(0);
	Gamepad->setRudder(0);
	Gamepad->sendReport();

	Serial.println("Presss any key to start!");
	GPWaitForSerial();
	// Steam Einrichtung
	Serial.println("Pressing 'A'"); Gamepad->PressButton(1, BTN_PRESS_DURATION, BTN_DELAY_AFTER_RELEASE);     
	Serial.println("Pressing 'B'"); Gamepad->PressButton(2, BTN_PRESS_DURATION, BTN_DELAY_AFTER_RELEASE);       
	Serial.println("Pressing 'X'"); Gamepad->PressButton(3, BTN_PRESS_DURATION, BTN_DELAY_AFTER_RELEASE);  
	Serial.println("Pressing 'Y'"); Gamepad->PressButton(4, BTN_PRESS_DURATION, BTN_DELAY_AFTER_RELEASE); 

	Serial.println("Pressing 'DPl'"); Gamepad->PressButton(15, BTN_PRESS_DURATION, BTN_DELAY_AFTER_RELEASE);   
	Serial.println("Pressing 'DPr'"); Gamepad->PressButton(16, BTN_PRESS_DURATION, BTN_DELAY_AFTER_RELEASE);   
	Serial.println("Pressing 'DPt'"); Gamepad->PressButton(13, BTN_PRESS_DURATION, BTN_DELAY_AFTER_RELEASE);     
	Serial.println("Pressing 'DPd'"); Gamepad->PressButton(14, BTN_PRESS_DURATION, BTN_DELAY_AFTER_RELEASE);    

	//---------------------------------------------------------------------
	Serial.println("X to min (left) and back");
	for (int i=0; i>= -32767; i-=TEST_STEP) {Gamepad->setX(constrain(i, -32767, 32767)); Gamepad->sendReport(); delay(20);}
	delay(STEAM_DELAY_BETWEEN_REPORTS);  
	Gamepad->setX(0); Gamepad->sendReport(); 
	delay(STEAM_DELAY_BETWEEN_REPORTS);  

	Serial.println("X to max (right) and back");
	for (int i=0; i<=32767; i+=TEST_STEP) {Gamepad->setX(constrain(i, -32767, 32767)); Gamepad->sendReport(); delay(20);}
	delay(STEAM_DELAY_BETWEEN_REPORTS);  
	Gamepad->setX(0); Gamepad->sendReport(); 
	delay(STEAM_DELAY_BETWEEN_REPORTS);  

	Serial.println("Y to max (top) and back");
	for (int i=0; i<=32767; i+=TEST_STEP) {Gamepad->setY(constrain(i, -32767, 32767)); Gamepad->sendReport(); delay(20);}
	delay(STEAM_DELAY_BETWEEN_REPORTS);  
	Gamepad->setY(0); Gamepad->sendReport(); 
	delay(STEAM_DELAY_BETWEEN_REPORTS);  

	Serial.println("Y to min (bottom) and back");
	for (int i=0; i>= -32767; i-=TEST_STEP) {Gamepad->setY(constrain(i, -32767, 32767)); Gamepad->sendReport(); delay(20);}
	delay(STEAM_DELAY_BETWEEN_REPORTS);  
	Gamepad->setY(0); Gamepad->sendReport(); 
	delay(STEAM_DELAY_BETWEEN_REPORTS);  

	Serial.println("Pressing 'LSB/L3 (left analog)'"); Gamepad->PressButton(11, BTN_PRESS_DURATION, BTN_DELAY_AFTER_RELEASE);    

	//---------------------------------------------------------------------
	#define SET_X setRX     // setRudder
	#define SET_Y setRY     // setThrottle
	Serial.println("RX to min (left) and back");
	for (int i=0; i>= -32767; i-=TEST_STEP) {Gamepad->SET_X(constrain(i, -32767, 32767)); Gamepad->sendReport(); delay(20);}
	delay(STEAM_DELAY_BETWEEN_REPORTS);  
	Gamepad->SET_X(0); Gamepad->sendReport(); 
	delay(STEAM_DELAY_BETWEEN_REPORTS);  

	Serial.println("RX to max (right) and back");
	for (int i=0; i<=32767; i+=TEST_STEP) {Gamepad->SET_X(constrain(i, -32767, 32767)); Gamepad->sendReport(); delay(20);}
	delay(STEAM_DELAY_BETWEEN_REPORTS);  
	Gamepad->SET_X(0); Gamepad->sendReport(); 
	delay(STEAM_DELAY_BETWEEN_REPORTS);  

	Serial.println("RY to max (top) and back");
	for (int i=0; i<=32767; i+=TEST_STEP) {Gamepad->SET_Y(constrain(i, -32767, 32767)); Gamepad->sendReport(); delay(20);}
	delay(STEAM_DELAY_BETWEEN_REPORTS);  
	Gamepad->SET_Y(0); Gamepad->sendReport(); 
	delay(STEAM_DELAY_BETWEEN_REPORTS);  

	Serial.println("RY to min (bottom) and back");
	for (int i=0; i>= -32767; i-=TEST_STEP) {Gamepad->SET_Y(constrain(i, -32767, 32767)); Gamepad->sendReport(); delay(20);}
	delay(STEAM_DELAY_BETWEEN_REPORTS);  
	Gamepad->SET_Y(0); Gamepad->sendReport(); 
	delay(STEAM_DELAY_BETWEEN_REPORTS);  

	Serial.println("Pressing 'RSB/R3 (right analog)'"); Gamepad->PressButton(12, BTN_PRESS_DURATION, BTN_DELAY_AFTER_RELEASE);  

	//---------------------------------------------------------------------
	Serial.println("Pressing 'Left shoulder (front left top)'");     Gamepad->PressButton(5, BTN_PRESS_DURATION, BTN_DELAY_AFTER_RELEASE);   
	Serial.println("Pressing 'Left trigger  (front left bottom)'");  Gamepad->PressButton(7, BTN_PRESS_DURATION, BTN_DELAY_AFTER_RELEASE);   
	Serial.println("Pressing 'Right shoulder (front right top)'");   Gamepad->PressButton(6, BTN_PRESS_DURATION, BTN_DELAY_AFTER_RELEASE);       
	Serial.println("Pressing 'Right trigger (front right bottom)'"); Gamepad->PressButton(8, BTN_PRESS_DURATION, BTN_DELAY_AFTER_RELEASE);   

	//---------------------------------------------------------------------
	Serial.println("Pressing 'Back/Display/Share (middle left)'");  Gamepad->PressButton(9, BTN_PRESS_DURATION, BTN_DELAY_AFTER_RELEASE);      
	Serial.println("Pressing 'Start/Menu/Options (middle right)'"); Gamepad->PressButton(10, BTN_PRESS_DURATION, BTN_DELAY_AFTER_RELEASE);  

	Serial.println("Pressing 'Home/Guide (middle center'"); Gamepad->PressButton(17, BTN_PRESS_DURATION, BTN_DELAY_AFTER_RELEASE);     
	Serial.println("Pressing 'Share/Record (middle lower'"); Gamepad->PressButton(18, BTN_PRESS_DURATION, BTN_DELAY_AFTER_RELEASE);     

	#if 0
	for (int i=0; i<4; i++)
	{
		GPWaitForSerial();
		Serial.println("Pressing 'Share?'"); PressButton(1);     
	}
	#endif
}


//************************************************************
// Functions required by the BLE Master

// Return the HID descriptor for the device  
const uint8_t* GamepadHIDSub::GetHIDDecsriptor(int* NumBytes)
{
    *NumBytes = hidReportDescriptorSize;
    return tempHidReportDescriptor;
};

// Return the input and output characteristics of the device. 
// Called multiple times, until the function returns 0. Index is the count.
int GamepadHIDSub::GetInputReportIndex(int Index)
{
    if (Index>0)
        return -1;
    return GAMEPAD_REPORT_ID; // configuration.getHidReportId();
}
