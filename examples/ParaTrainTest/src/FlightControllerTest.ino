/*
 * Flight controller test
 */

#include <Arduino.h>
#include <BleFlexHID.h>
#include <GamepadHIDSub.h>
#include <KeyboardHIDSub.h>
#include <MouseHIDSub.h>
#include <TouchHIDSub.h>

#define numOfButtons 16
#define numOfHatSwitches 0
#define enableX true
#define enableY true
#define enableZ false
#define enableRZ false
#define enableRX false
#define enableRY false
#define enableSlider1 false
#define enableSlider2 false
#define enableRudder true
#define enableThrottle true
#define enableAccelerator false
#define enableBrake false
#define enableSteering false

BleFlexHID BleMaster("BLE Flight Controller", "lemmingDev", 100);
GamepadHIDSub Gamepad;
KeyboardHIDSub Keybord;
MouseHIDSub Mouse;
TouchHIDSub Touch;

void setup()
{
    Serial.begin(115200);
    Serial.println("Starting BLE work!");

    // Setup controller with 32 buttons, accelerator, brake and steering
    GamepadConfiguration GamepadHIDSubConfig;
    GamepadHIDSubConfig.setAutoReport(false);
    GamepadHIDSubConfig.setControllerType(CONTROLLER_TYPE_MULTI_AXIS); // CONTROLLER_TYPE_JOYSTICK, CONTROLLER_TYPE_GAMEPAD (DEFAULT), CONTROLLER_TYPE_MULTI_AXIS
    GamepadHIDSubConfig.setButtonCount(numOfButtons);
    GamepadHIDSubConfig.setIncludeStart(true);
    GamepadHIDSubConfig.setIncludeSelect(true);
    GamepadHIDSubConfig.setWhichAxes(enableX, enableY, enableZ, enableRX, enableRY, enableRZ, enableSlider1, enableSlider2);      // Can also be done per-axis individually. All are true by default
    GamepadHIDSubConfig.setWhichSimulationControls(enableRudder, enableThrottle, enableAccelerator, enableBrake, enableSteering); // Can also be done per-control individually. All are false by default
    GamepadHIDSubConfig.setHatSwitchCount(numOfHatSwitches);                                                                      // 1 by default
    // Some non-Windows operating systems and web based gamepad testers don't like min axis set below 0, so 0 is set by default
    GamepadHIDSubConfig.setAxesMin(0x8001); // -32767 --> int16_t - 16 bit signed integer - Can be in decimal or hexadecimal
    GamepadHIDSubConfig.setAxesMax(0x7FFF); // 32767 --> int16_t - 16 bit signed integer - Can be in decimal or hexadecimal 
    Gamepad.Configure(&GamepadHIDSubConfig);

    if (!BleMaster.AddDevice(&Gamepad)) 
        Serial.printf("ERROR: Cannot add Gamepad HID to BLE Master\n");
    if (!BleMaster.AddDevice(&Keybord)) 
        Serial.printf("ERROR: Cannot add Keybord HID to BLE Master\n");
    if (!BleMaster.AddDevice(&Mouse)) 
        Serial.printf("ERROR: Cannot add Mouse HID to BLE Master\n");
    if (!BleMaster.AddDevice(&Touch)) 
        Serial.printf("ERROR: Cannot add Touch HID to BLE Master\n");

    BleMaster.begin();

    // Set throttle to min
    Gamepad.setThrottle(-32767);

    // Set x and y axes and rudder to center
    Gamepad.setX(0);
    Gamepad.setY(0);
    Gamepad.setRudder(0);
}

void loop()
{
    if (BleMaster.isConnected())
    {
		static unsigned long startTime=millis();

#if 1
        // Gamepad tests
        Serial.println("Press all buttons one by one");
        for (int i = 1; i <= numOfButtons; i += 1)
        {
            Gamepad.press(i);
            Gamepad.sendReport();
            delay(100);
            Gamepad.release(i);
            Gamepad.sendReport();
            delay(20);
        }

        Serial.println("Press start and select");
        Gamepad.pressStart();
        delay(100);
        Gamepad.pressSelect();
        delay(100);
        Gamepad.releaseStart();
        delay(100);
        Gamepad.releaseSelect();

        Serial.println("Move x axis from center to max");
        for (int i = 0; i > -32767; i -= 256)
        {
            Gamepad.setX(i);
            Gamepad.sendReport();
            delay(10);
        }

        Serial.println("Move x axis from min to max");
        for (int i = -32767; i < 32767; i += 256)
        {
            Gamepad.setX(i);
            Gamepad.sendReport();
            delay(10);
        }

        Serial.println("Move x axis from max to center");
        for (int i = 32767; i > 0; i -= 256)
        {
            Gamepad.setX(i);
            Gamepad.sendReport();
            delay(10);
        }
        Gamepad.setX(0);
        Gamepad.sendReport();

        Serial.println("Move y axis from center to max");
        for (int i = 0; i > -32767; i -= 256)
        {
            Gamepad.setY(i);
            Gamepad.sendReport();
            delay(10);
        }

        Serial.println("Move y axis from min to max");
        for (int i = -32767; i < 32767; i += 256)
        {
            Gamepad.setY(i);
            Gamepad.sendReport();
            delay(10);
        }

        Serial.println("Move y axis from max to center");
        for (int i = 32767; i > 0; i -= 256)
        {
            Gamepad.setY(i);
            Gamepad.sendReport();
            delay(10);
        }
        Gamepad.setY(0);
        Gamepad.sendReport();

        Serial.println("Move rudder from min to max");
        // for(int i = 32767 ; i > -32767 ; i -= 256)    //Use this for loop setup instead if rudder is reversed
        for (int i = -32767; i < 32767; i += 256)
        {
            Gamepad.setRudder(i);
            Gamepad.sendReport();
            delay(10);
        }
        Gamepad.setRudder(0);
        Gamepad.sendReport();

        Serial.println("Move throttle from min to max");
        for (int i = -32767; i < 32767; i += 256)
        {
            Gamepad.setThrottle(i);
            Gamepad.sendReport();
            delay(10);
        }
        Gamepad.setThrottle(-32767);
        Gamepad.sendReport();
#endif

#if 0   
		// Mouse tests
		Serial.println("Scroll up");
		startTime = millis();
		while(millis()<startTime+2000) 
		{
			bleMouse.move(0,0,1);
			delay(100);
		}
		delay(500);

		Serial.println("Scroll down");
		startTime = millis();
		while(millis()<startTime+2000) 
		{
			bleMouse.move(0,0,-1);
			delay(100);
		}
		delay(500);

		Serial.println("Scroll left");
		startTime = millis();
		while(millis()<startTime+2000) 
		{
			bleMouse.move(0,0,0,-1);
			delay(100);
		}
		delay(500);

		Serial.println("Scroll right");
		startTime = millis();
		while(millis()<startTime+2000) 
		{
			bleMouse.move(0,0,0,1);
			delay(100);
		}
		delay(500);

		Serial.println("Move mouse pointer up");
		startTime = millis();
		while(millis()<startTime+2000) 
		{
			bleMouse.move(0,-1);
			delay(100);
		}
		delay(500);

		Serial.println("Move mouse pointer down");
		startTime = millis();
		while(millis()<startTime+2000) 
		{
			bleMouse.move(0,1);
			delay(100);
		}
		delay(500);

		Serial.println("Move mouse pointer left");
		startTime = millis();
		while(millis()<startTime+2000) 
		{
			bleMouse.move(-1,0);
			delay(100);
		}
		delay(500);

		Serial.println("Move mouse pointer right");
		startTime = millis();
		while(millis()<startTime+2000) 
		{
			bleMouse.move(1,0);
			delay(100);
		}
		delay(500);
#endif

#if 0
		// Test touch routines
		static int Action = 0;

		// Serial.printf("Touch tests\n");
		Touch.loop();
		if (millis()>=startTime)
		{
			switch(Action)
			{
				case 0:
					Serial.println("Move to center");
					Touch.click(5000,5000);
					Mouse.move(0,1);
					startTime = millis()+2000;
					Action++;
					break;
				case 1:
					Serial.println("drag left");
					Touch.click(3000,8000);
					startTime = millis()+2000;
					Action++;
					break;
				case 2:
					Touch.release();
					startTime = millis();
					Action++;
					break;

				case 3:
					Serial.println("Click center");
					Touch.click(5000, 5000);
					startTime = millis()+3000;
					Action++;
					break;

				case 4:
					Serial.println("Drag right");
					Touch.click(7000, 5000);
					startTime = millis()+3000;
					Action++;
					break;
				
				default:
					Touch.release();
					startTime = millis();
					Action=1;
					break;
			}
		}
		delay(100);
#endif

#if 0
        // Be careful with this test, it will write text to whatever window is active!
		Serial.println("Sending 'Hello world'...");
		Keybord.print("Hello world\n");
#endif
    }
}