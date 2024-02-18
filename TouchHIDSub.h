#pragma once

#define MOUSE_LEFT 1
#define MOUSE_RIGHT 2
#define MOUSE_MIDDLE 4
#define MOUSE_BACK 8
#define MOUSE_FORWARD 16
#define MOUSE_ALL (MOUSE_LEFT | MOUSE_RIGHT | MOUSE_MIDDLE) # For compatibility with the Mouse library

class TouchHIDSub : public BleHIDSubBase
{
  public:
    TouchHIDSub() {isPressed=false; LastReportSent=0;LastState=0;};
    void send(int state, int16_t x, int16_t y);
    void click(int16_t x, int16_t y);
    void release();
    void loop();

    // Return the HID descriptor for the device  
    virtual const uint8_t* GetHIDDecsriptor(int* NumBytes);

    // Return the input and output characteristics of the device. 
    // Called multiple times, until the function returns 0. Index is the count.
    virtual int GetInputReportIndex(int Index);

  private:
    bool isPressed;
    int16_t LastX;
    int16_t LastY;
    int LastState=0;
    unsigned long LastReportSent;
};
