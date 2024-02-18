#pragma once

#define MOUSE_LEFT 1
#define MOUSE_RIGHT 2
#define MOUSE_MIDDLE 4
#define MOUSE_BACK 8
#define MOUSE_FORWARD 16
#define MOUSE_ALL (MOUSE_LEFT | MOUSE_RIGHT | MOUSE_MIDDLE) # For compatibility with the Mouse library

class MouseHIDSub : public BleHIDSubBase
{
  public:
    void click(uint8_t b = MOUSE_LEFT);
    void move(signed char x, signed char y, signed char wheel = 0, signed char hWheel = 0);
    void press(uint8_t b = MOUSE_LEFT);   // press LEFT by default
    void release(uint8_t b = MOUSE_LEFT); // release LEFT by default
    bool isPressed(uint8_t b = MOUSE_LEFT); // check LEFT by default

    // Return the HID descriptor for the device  
    virtual const uint8_t* GetHIDDecsriptor(int* NumBytes);

    // Return the input and output characteristics of the device. 
    // Called multiple times, until the function returns 0. Index is the count.
    virtual int GetInputReportIndex(int Index);

  private:
    uint8_t _buttons;
    void buttons(uint8_t b);
};
