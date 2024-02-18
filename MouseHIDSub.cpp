
#include "BleFlexHID.h"
#include "MouseHIDSub.h"


const uint8_t MouseHIDReportDescriptor[] = 
{
#if 1
	// ------------------------------------------------- Mouse
	USAGE_PAGE(1),       0x01, // USAGE_PAGE (Generic Desktop)
	USAGE(1),            0x02, // USAGE (Mouse)
	COLLECTION(1),       0x01, // COLLECTION (Application)
		USAGE(1),            0x01, //   USAGE (Pointer)
		COLLECTION(1),       0x00, //   COLLECTION (Physical)
			REPORT_ID(1),        MOUSE_REPORT_ID, //     REPORT_ID (1)
			// ------------------------------------------------- Buttons (Left, Right, Middle, Back, Forward)
			USAGE_PAGE(1),       0x09, //     USAGE_PAGE (Button)
			USAGE_MINIMUM(1),    0x01, //     USAGE_MINIMUM (Button 1)
			USAGE_MAXIMUM(1),    0x05, //     USAGE_MAXIMUM (Button 5)
			LOGICAL_MINIMUM(1),  0x00, //     LOGICAL_MINIMUM (0)
			LOGICAL_MAXIMUM(1),  0x01, //     LOGICAL_MAXIMUM (1)
			REPORT_SIZE(1),      0x01, //     REPORT_SIZE (1)
			REPORT_COUNT(1),     0x05, //     REPORT_COUNT (5)
			HIDINPUT(1),         0x02, //     INPUT (Data, Variable, Absolute) ;5 button bits
			// ------------------------------------------------- Padding
			REPORT_SIZE(1),      0x03, //     REPORT_SIZE (3)
			REPORT_COUNT(1),     0x01, //     REPORT_COUNT (1)
			HIDINPUT(1),         0x03, //     INPUT (Constant, Variable, Absolute) ;3 bit padding
			// ------------------------------------------------- X/Y position, Wheel
			USAGE_PAGE(1),       0x01, //     USAGE_PAGE (Generic Desktop)
			USAGE(1),            0x30, //     USAGE (X)
			USAGE(1),            0x31, //     USAGE (Y)
			USAGE(1),            0x38, //     USAGE (Wheel)
			LOGICAL_MINIMUM(1),  0x81, //     LOGICAL_MINIMUM (-127)
			LOGICAL_MAXIMUM(1),  0x7f, //     LOGICAL_MAXIMUM (127)
			REPORT_SIZE(1),      0x08, //     REPORT_SIZE (8)
			REPORT_COUNT(1),     0x03, //     REPORT_COUNT (3)
			HIDINPUT(1),         0x06, //     INPUT (Data, Variable, Relative) ;3 bytes (X,Y,Wheel)
			// ------------------------------------------------- Horizontal wheel
			USAGE_PAGE(1),       0x0c, //     USAGE PAGE (Consumer Devices)
			USAGE(2),      0x38, 0x02, //     USAGE (AC Pan)
			LOGICAL_MINIMUM(1),  0x81, //     LOGICAL_MINIMUM (-127)
			LOGICAL_MAXIMUM(1),  0x7f, //     LOGICAL_MAXIMUM (127)
			REPORT_SIZE(1),      0x08, //     REPORT_SIZE (8)
			REPORT_COUNT(1),     0x01, //     REPORT_COUNT (1)
			HIDINPUT(1),         0x06, //     INPUT (Data, Var, Rel)
		END_COLLECTION(0),         //   END_COLLECTION
	END_COLLECTION(0),          // END_COLLECTION

#endif
#if 0
	// ------------------------------------------------- Absolute Mouse (implemented as a digitizer!)
	0x05, 0x0d,                    /* USAGE_PAGE (Digitizer, Precision Touch pad) */
	0x09, 0x04,                    /* USAGE (Touch Screen) */
	0xa1, 0x01,                    /* COLLECTION (Application) */
	0x85, TOUCH_REPORTID,          /*    REPORT_ID */

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
#endif
};


void MouseHIDSub::click(uint8_t b)
{
  _buttons = b;
  move(0,0,0,0);
  _buttons = 0;
  move(0,0,0,0);
}

void MouseHIDSub::move(signed char x, signed char y, signed char wheel, signed char hWheel)
{
  if (IsConnected())
  {
    uint8_t m[5];
    m[0] = _buttons;
    m[1] = x;
    m[2] = y;
    m[3] = wheel;
    m[4] = hWheel;
    SendReport(m, 5, 0);
  }
}

void MouseHIDSub::buttons(uint8_t b)
{
  if (b != _buttons)
  {
    _buttons = b;
    move(0,0,0,0);
  }
}

void MouseHIDSub::press(uint8_t b)
{
  buttons(_buttons | b);
}

void MouseHIDSub::release(uint8_t b)
{
  buttons(_buttons & ~b);
}

bool MouseHIDSub::isPressed(uint8_t b)
{
  if ((b & _buttons) > 0)
    return true;
  return false;
}



//************************************************************
// Functions required by the BLE Master

// Return the HID descriptor for the device  
const uint8_t* MouseHIDSub::GetHIDDecsriptor(int* NumBytes)
{
    *NumBytes = sizeof(MouseHIDReportDescriptor);
    return MouseHIDReportDescriptor;
};

// Return the input and output characteristics of the device. 
// Called multiple times, until the function returns 0. Index is the count.
int MouseHIDSub::GetInputReportIndex(int Index)
{
    if (Index>0)
        return -1;
    return MOUSE_REPORT_ID; // configuration.getHidReportId();
}
