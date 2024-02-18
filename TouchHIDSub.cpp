
#include "BleFlexHID.h"
#include "TouchHIDSub.h"


const uint8_t TouchHIDReportDescriptor[] = 
{
	// ------------------------------------------------- Absolute Mouse (implemented as a digitizer!)
	0x05, 0x0d,                    /* USAGE_PAGE (Digitizer, Precision Touch pad) */
	0x09, 0x04,                    /* USAGE (Touch Screen) */
	0xa1, 0x01,                    /* COLLECTION (Application) */
	0x85, TOUCH_REPORT_ID,          /*    REPORT_ID */

	/* declare a finger collection */
	0x09, 0x20,                    /*   Usage (Stylus) */
	0xA1, 0x00,                    /*   Collection (Physical) */

	/* Declare a finger touch (finger up/down) */
	0x09, 0x42,                    /*     Usage (Tip Switch) */
	0x09, 0x32,                    /*     USAGE (In Range) */
	0x15, 0x00,                    /*     LOGICAL_MINIMUM (0) */
	0x25, 0x01,                    /*     LOGICAL_MAXIMUM (1) */
	0x75, 0x01,                    /*     REPORT_SIZE (1) */
	0x95, 0x02,                    /*     REPORT_COUNT (2) */
	0x81, 0x02,                    /*     INPUT (Data,Var,Abs) */

	/* Declare the remaining 6 bits of the first data byte as constant -> the driver will ignore them */
	0x75, 0x01,                    /*     REPORT_SIZE (1) */
	0x95, 0x06,                    /*     REPORT_COUNT (6) */
	0x81, 0x01,                    /*     INPUT (Cnst,Ary,Abs) */

	/* Define absolute X and Y coordinates of 16 bit each (percent values multiplied with 100) */
	/* http://www.usb.org/developers/hidpage/Hut1_12v2.pdf */
	/* Chapter 16.2 says: "In the Stylus collection a Pointer physical collection will contain the axes reported by the stylus." */
	0x05, 0x01,                    /*     Usage Page (Generic Desktop) */
	0x09, 0x01,                    /*     Usage (Pointer) */
	0xA1, 0x00,                    /*     Collection (Physical) */
	0x09, 0x30,                    /*        Usage (X) */
	0x09, 0x31,                    /*        Usage (Y) */
	0x16, 0x00, 0x00,              /*        Logical Minimum (0) */
	0x26, 0x10, 0x27,              /*        Logical Maximum (10000) */
	0x36, 0x00, 0x00,              /*        Physical Minimum (0) */
	0x46, 0x10, 0x27,              /*        Physical Maximum (10000) */
	0x66, 0x00, 0x00,              /*        UNIT (None) */
	0x75, 0x10,                    /*        Report Size (16), */
	0x95, 0x02,                    /*        Report Count (2), */
	0x81, 0x02,                    /*        Input (Data,Var,Abs) */
	0xc0,                          /*     END_COLLECTION */
	0xc0,                          /*   END_COLLECTION */
	0xc0                           /* END_COLLECTION */

	// With this declaration a data packet must be sent as:
	// byte 1   -> "touch" state          (bit 0 = pen up/down, bit 1 = In Range)
	// byte 2,3 -> absolute X coordinate  (0...10000)
	// byte 4,5 -> absolute Y coordinate  (0...10000)
};


#define LSB(v) ((v >> 8) & 0xff)
#define MSB(v) (v & 0xff)


void TouchHIDSub::send(int state, int16_t x, int16_t y)
{
    uint8_t m[5];
    m[0] = state;
    m[1] = MSB(x);
    m[2] = LSB(x);
    m[3] = MSB(y);
    m[4] = LSB(y);
	SendReport(m, 5, 0);
}

void TouchHIDSub::click(int16_t x, int16_t y)
{
	LastX = x; LastY = y;
	LastState = 3;
	send(LastState, x, y);
	isPressed = true;
}

void TouchHIDSub::release()
{
	LastState = 0;
	send(LastState, LastX, LastY);
	isPressed = false;
}


void TouchHIDSub::loop()
{
	if (isPressed && LastReportSent+100<millis())
	{
		LastReportSent = millis();
		send(LastState, LastX, LastY);
	}
}


//************************************************************
// Functions required by the BLE Master

// Return the HID descriptor for the device  
const uint8_t* TouchHIDSub::GetHIDDecsriptor(int* NumBytes)
{
    *NumBytes = sizeof(TouchHIDReportDescriptor);
    return TouchHIDReportDescriptor;
};

// Return the input and output characteristics of the device. 
// Called multiple times, until the function returns 0. Index is the count.
int TouchHIDSub::GetInputReportIndex(int Index)
{
    if (Index>0)
        return -1;
    return TOUCH_REPORT_ID; // configuration.getHidReportId();
}
