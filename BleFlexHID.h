#pragma once

#include "BleConnectionStatus.h"
#include "NimBLEHIDDevice.h"
#include "NimBLECharacteristic.h"


//--------------------------------------------------------
// Report IDs:      These MUST be used in the descriptors!
#define KEYBOARD_REPORT_ID         0x01
#define MEDIA_KEYS_REPORT_ID       0x02
#define MOUSE_REPORT_ID            0x03
#define TOUCH_REPORT_ID            0x04
#define GAMEPAD_REPORT_ID          0x05


#define MAX_HIDS                    5    // Max number of HID devices which can be simultaneously managed managed BleFlexHID
#define MAX_REPORTS_PER_HID         5    // Max number of Reports per HID. separate for in and out.
#define MAX_CHARACTERISTICS         MAX_HIDS*MAX_REPORTS_PER_HID


class BleFlexHID;

class BleHIDSubBase
{
    public:

        // HID Utilities
        bool IsConnected(void); 
        void SendReport(uint8_t* Message, int SizeMessage, int SubReportIndex);

        // Return the HID descriptor for the device  
        virtual const uint8_t* GetHIDDecsriptor(int* NumBytes)=0;

        // Return the input and output characteristics of the device. 
        // Called multiple times, until the function returns 0. Index is the count.
        virtual int GetInputReportIndex (int Index)=0;
        virtual int GetOutputReportIndex(int Index, NimBLECharacteristicCallbacks** OutputCallback) {return -1;};

        BleFlexHID* pHIDMaster;
        
        // Index of the device in the BLE Master.
        int SubDeviceIndex;
};


class HIDSubDevice
{
    public:
        HIDSubDevice()
        {
            for (int i=0; i<MAX_REPORTS_PER_HID; i++)
            {
                InputReportIndices [i] = -1;
                OutputReportIndices[i] = -1;
            }
            ReportDescriptor = 0;
            ReportDescriptorSize = 0;
            NumInputReports = 0;
            NumOutputReports = 0;
        };

        void SetDeviceInfo(BleHIDSubBase* Device)
        {
            Serial.printf("Querying details from new sub device:\n"); 
            BaseDevice = Device;
            ReportDescriptor = BaseDevice->GetHIDDecsriptor(&ReportDescriptorSize);
            Serial.printf("   Sub device report descriptor has size %d bytes,\n", ReportDescriptorSize); 

            // Mapping between sequential report indices and report IDs in the descriptor.
            NumInputReports=0;
            while(NumInputReports<MAX_REPORTS_PER_HID && 0<=(InputReportIndices[NumInputReports]=BaseDevice->GetInputReportIndex(NumInputReports))) 
                NumInputReports++;
            Serial.printf("   Sub device supports %d input reports,\n", NumInputReports); 

            NumOutputReports=0;
            while(NumOutputReports<MAX_REPORTS_PER_HID &&  0<=(OutputReportIndices[NumOutputReports]=BaseDevice->GetOutputReportIndex(NumOutputReports, &OutputCallbacks[NumOutputReports]))) 
                NumOutputReports++;
            Serial.printf("   Sub device supports %d output reports.\n", NumOutputReports); delay(500);
        };

        BleHIDSubBase* BaseDevice;
        int InputReportIndices [MAX_REPORTS_PER_HID];
        int OutputReportIndices[MAX_REPORTS_PER_HID];
        NimBLECharacteristicCallbacks* OutputCallbacks[MAX_REPORTS_PER_HID];
        const uint8_t* ReportDescriptor;
        int ReportDescriptorSize;
        int NumInputReports;
        int NumOutputReports;
};

class BleFlexHID
{
    public:
        BleFlexHID(std::string deviceName = "ESP32 BLE Gamepad", std::string deviceManufacturer = "Espressif", uint8_t batteryLevel = 100);

        /**
         * @brief Sets the Plug n Play characteristic value.
         * @param [in] VendorSourceID The vendor ID source number.
         * @param [in] VendorID The vendor ID number.
         * @param [in] ProductID The product ID number.
         * @param [in] Version The produce version number.
         */
        void SetProductID(int VendorSourceID, int VendorID, int ProductID, int Version);

        void begin();
        void end(void);
        bool isConnected(void);
        void setBatteryLevel(uint8_t level);

        // Add one more device, returns the index of the new device. returns -1 if error.
        // This should not be called after begin()!
        bool AddDevice(BleHIDSubBase* pDevice);
        void BuildCharacteristicsTable();
        void BuildMasterDescriptor();

        NimBLECharacteristic* GetInputCharacterstics(int DeviceIdx, int CharacteristicIndex);
        NimBLECharacteristic* GetOutputCharacterstics(int DeviceIdx, int CharacteristicIndex);

        uint8_t batteryLevel;
        std::string deviceManufacturer;
        std::string deviceName;

    protected:
        virtual void onStarted(NimBLEServer *pServer){};

    private:
        // void rawAction(uint8_t msg[], char msgSize);
        static void taskServer(void *pvParameter);
        static int SigID;
        static int VendorID;
        static int ProductID;
        static int Version;

        BleConnectionStatus *connectionStatus;
        NimBLEHIDDevice *hid;

        HIDSubDevice Devices[MAX_HIDS];

        NimBLECharacteristic* InputCharacterstics [MAX_CHARACTERISTICS];
        NimBLECharacteristic* OutputCharacterstics[MAX_CHARACTERISTICS];
        NimBLECharacteristicCallbacks* OutputCallbacks[MAX_CHARACTERISTICS];

        
        int NumDevices;                 // Number of devices in Devices[]
        int NumInputCharacteristics;
        int NumOutputCharacteristics;
        uint8_t* MasterDescriptor;
        int MasterDescriptorSize;
};
