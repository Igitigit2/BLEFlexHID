


#include "nimconfig.h"
#include <NimBLEDevice.h>
#include <NimBLEUtils.h>
#include <NimBLEServer.h>
#include "NimBLEHIDDevice.h"
#include "HIDTypes.h"
#include "BleConnectionStatus.h"
#include "BleFlexHID.h"


#if defined(CONFIG_ARDUHAL_ESP_LOG)
    #include "esp32-hal-log.h"
    #define LOG_TAG "BleCombo"
#else
    #include "esp_log.h"
    static const char *LOG_TAG = "BleCombo";
#endif

#define SERVICE_UUID_DEVICE_INFORMATION        "180A"      // Service - Device information

#define CHARACTERISTIC_UUID_MODEL_NUMBER       "2A24"      // Characteristic - Model Number String - 0x2A24
#define CHARACTERISTIC_UUID_SOFTWARE_REVISION  "2A28"      // Characteristic - Software Revision String - 0x2A28
#define CHARACTERISTIC_UUID_SERIAL_NUMBER      "2A25"      // Characteristic - Serial Number String - 0x2A25
#define CHARACTERISTIC_UUID_FIRMWARE_REVISION  "2A26"      // Characteristic - Firmware Revision String - 0x2A26
#define CHARACTERISTIC_UUID_HARDWARE_REVISION  "2A27"      // Characteristic - Hardware Revision String - 0x2A27


// HID Utilities
bool BleHIDSubBase::IsConnected(void) 
{
    if (!pHIDMaster)
    {
        Serial.println("ERROR: BleFlexHID not attached. Did you call BleFlexHID.AddDevice()?");
        return false;
    }
    return pHIDMaster->isConnected();
}



void BleHIDSubBase::SendReport(uint8_t* Message, int SizeMessage, int SubReportIndex)
{
	if (IsConnected())
	{
		NimBLECharacteristic* pC = pHIDMaster->GetInputCharacterstics(SubDeviceIndex, SubReportIndex);
		pC->setValue((uint8_t*)Message, SizeMessage);
		pC->notify();
	}
}


BleFlexHID::BleFlexHID(std::string _deviceName, std::string _deviceManufacturer, uint8_t _batteryLevel) : hid(0)
{
    deviceName = _deviceName;
    deviceManufacturer = _deviceManufacturer;
    batteryLevel = _batteryLevel;
    connectionStatus = new BleConnectionStatus();
    MasterDescriptorSize = 0;
    MasterDescriptor = 0;
    NumDevices=0;
    NumInputCharacteristics = 0;
    NumOutputCharacteristics = 0;
}



/**
 * @brief Sets the Plug n Play characteristic value.
 * @param [in] VendorSourceID The vendor ID source number.
 * @param [in] VendorID The vendor ID number.
 * @param [in] ProductID The product ID number.
 * @param [in] Version The produce version number.
 */
void BleFlexHID::SetProductID(int VendorSourceID, int _VendorID, int _ProductID, int _Version)
{
    SigID = VendorSourceID;
    VendorID = _VendorID;
    ProductID = _ProductID;
    Version = _Version;
}


bool BleFlexHID::AddDevice(BleHIDSubBase* pDevice)
{
    if (NumDevices>=MAX_HIDS)
    {
        Serial.printf("ERROR: BleFlexHID() too many devices!\n");
        return false;
    }

    Devices[NumDevices].SetDeviceInfo(pDevice);
    Devices[NumDevices].BaseDevice->pHIDMaster = this;
    Devices[NumDevices].BaseDevice->SubDeviceIndex = NumDevices;
    NumDevices++;

    return true;
}


NimBLECharacteristic* BleFlexHID::GetInputCharacterstics(int DeviceIdx, int CharacteristicIndex)
{
    // calculate the index of the characteristic
    int Index = 0;
    for (int d=0; d<DeviceIdx; d++)
    {
        Index += Devices[d].NumInputReports;
    }
    Index += CharacteristicIndex;
    return InputCharacterstics[Index];
}


NimBLECharacteristic* BleFlexHID::GetOutputCharacterstics(int DeviceIdx, int CharacteristicIndex)
{
    // calculate the index of the characteristic
    int Index = 0;
    for (int d=0; d<DeviceIdx; d++)
    {
        Index += Devices[d].NumOutputReports;
    }
    Index += CharacteristicIndex;
    return OutputCharacterstics[Index];
}


// Create the master report descriptor, which is just a concatenation of all the individual
// descriptors from all HID sub devices.
void BleFlexHID::BuildMasterDescriptor()
{
    Serial.printf("Building master report descriptor for %d sub devices\n", NumDevices);
    if (MasterDescriptorSize!=0 || MasterDescriptor!=0)
        Serial.println("   ERROR: MasterDescriptor not empty! BleFlexHID::BuildMasterDescriptor() was probably called already.");

    // Calculate the required size
    MasterDescriptorSize = 0;
    for (int d=0; d<NumDevices; d++)
        MasterDescriptorSize += Devices[d].ReportDescriptorSize;
    
    // Allocate and build the table
    Serial.printf("   Allocating master report descriptor of size %d bytes\n", MasterDescriptorSize); 
    MasterDescriptor = new uint8_t[MasterDescriptorSize];
    if (MasterDescriptor==0)
        Serial.printf("   ERROR: Failed to allocate memory for master decriptor table!\n");

    uint8_t* NextDescriptor = MasterDescriptor;
    for (int d=0; d<NumDevices; d++)
    {
        Serial.printf("   Adding %d bytes to master report descriptor\n", Devices[d].ReportDescriptorSize); 
        memcpy(NextDescriptor, Devices[d].ReportDescriptor, Devices[d].ReportDescriptorSize);
        NextDescriptor += Devices[d].ReportDescriptorSize;
    }
    Serial.printf("   Done with creation of master report descriptor\n"); 
}


void BleFlexHID::BuildCharacteristicsTable()
{
    // Build table with input/output characteristics
    Serial.printf("Building table with input/output characteristics of all sub devices\n");
    int InputIndex=0;
    int OutputIndex=0;
    for (int d=0; d<NumDevices; d++)
    {
        for (int i=0; i<Devices[d].NumInputReports; i++) 
        {
            Serial.printf("   Adding one input characteristic for report index %d of type %d for device %d\n", i,  Devices[d].InputReportIndices[i], d);
            InputCharacterstics[InputIndex++] = hid->inputReport(Devices[d].InputReportIndices[i]); // <-- input REPORTID from report map
        }
        for (int i=0; i<Devices[d].NumOutputReports; i++) 
        {
            Serial.printf("   Adding one output characteristic for report index %d of type %d for device %d\n", i,  Devices[d].OutputReportIndices[i], d);
            OutputCharacterstics[OutputIndex] = hid->outputReport(Devices[d].OutputReportIndices[i]); // <-- output REPORTID from report map
            OutputCallbacks[OutputIndex++] = Devices[d].OutputCallbacks[i];
        }
    }

    NumInputCharacteristics  = InputIndex,
    NumOutputCharacteristics = OutputIndex;

    Serial.printf("   Linking %d output callbacks\n", NumOutputCharacteristics);
    for (int i=0; i<NumOutputCharacteristics; i++)
        OutputCharacterstics[i]->setCallbacks(OutputCallbacks[i]);

    Serial.printf("   Done with table creation\n"); 
}


void BleFlexHID::begin()
{
     xTaskCreate(taskServer, "server", 20000, (void *)this, 5, NULL);
}

void BleFlexHID::end(void)
{
}

bool BleFlexHID::isConnected(void)
{
    return this->connectionStatus->connected;
}

void BleFlexHID::setBatteryLevel(uint8_t level)
{
    batteryLevel = level;
    if (hid != 0)
    {
        hid->setBatteryLevel(this->batteryLevel);

        if (isConnected())
        {
            hid->batteryLevel()->notify();
        }
		
//        if (configuration.getAutoReport())
        if (0)
        {
//            sendReport();
        }
    }
}



int BleFlexHID::SigID = 0x02;
int BleFlexHID::VendorID=0xe502;
int BleFlexHID::Version=0x0210;
int BleFlexHID::ProductID=0x1234;       // Dev

void BleFlexHID::taskServer(void *pvParameter)
{
    BleFlexHID* bleInstance = (BleFlexHID *) pvParameter; //static_cast<BleComboKeyboard *>(pvParameter);
    NimBLEDevice::init(bleInstance->deviceName);
    NimBLEServer *pServer = NimBLEDevice::createServer();
    pServer->setCallbacks(bleInstance->connectionStatus);

    bleInstance->hid = new NimBLEHIDDevice(pServer);

    bleInstance->BuildMasterDescriptor();
    bleInstance->BuildCharacteristicsTable();

    /**
     * @brief Sets the Plug n Play characteristic value.
     * @param [in] sig The vendor ID source number.
     * @param [in] vid The vendor ID number.
     * @param [in] pid The product ID number.
     * @param [in] version The produce version number.
     */

    bleInstance->hid->manufacturer()->setValue(bleInstance->deviceManufacturer);
    bleInstance->hid->pnp(SigID, VendorID, ProductID, Version);
    bleInstance->hid->hidInfo(0x00,0x01);

    BLESecurity *pSecurity = new BLESecurity();

    pSecurity->setAuthenticationMode(ESP_LE_AUTH_BOND);

    bleInstance->hid->reportMap(bleInstance->MasterDescriptor, bleInstance->MasterDescriptorSize);
    bleInstance->hid->startServices();

    BLEAdvertising *pAdvertising = pServer->getAdvertising();
    pAdvertising->setAppearance(HID_KEYBOARD);
    pAdvertising->addServiceUUID(bleInstance->hid->hidService()->getUUID());
    pAdvertising->start();
    bleInstance->hid->setBatteryLevel(bleInstance->batteryLevel);

    ESP_LOGD(LOG_TAG, "Advertising started!");
    vTaskDelay(portMAX_DELAY); //delay(portMAX_DELAY);
}
