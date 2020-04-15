#include "BLEDevice.h"
static BLEUUID ServiceUUID("e093f3b5-00a3-a9e5-9eca-40016e0edc24");
static BLEUUID WriteCharacteristicUUID("e093f3b7-00a3-a9e5-9eca-40036e0edc24");
static String ThisMAC = "60:c0:bf:28:99:7d";
static BLERemoteCharacteristic* WriteCharacteristic;
static BLEAdvertisedDevice* ThisDevice;
static boolean Connected = false;

class AdvertisedDeviceCB: public BLEAdvertisedDeviceCallbacks {
    void onResult(BLEAdvertisedDevice advertisedDevice) {
      if ((String)advertisedDevice.getAddress().toString().c_str() == ThisMAC) {
        BLEDevice::getScan()->stop();
        ThisDevice = new BLEAdvertisedDevice(advertisedDevice);
      }
    }
};

class ClientCB : public BLEClientCallbacks {
    void onConnect(BLEClient* ThisClient) {
      Connected = true;
      digitalWrite(LED_BUILTIN, HIGH);
    }
    void onDisconnect(BLEClient* ThisClient) {
      Connected = false;
      digitalWrite(LED_BUILTIN, LOW);
    }
};

void setup() {
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(22, INPUT_PULLUP);
  pinMode(23, INPUT_PULLUP);
  BLEDevice::init("");
  BLEScan* ThisBLEScan = BLEDevice::getScan();
  ThisBLEScan->setAdvertisedDeviceCallbacks(new AdvertisedDeviceCB());
  ThisBLEScan->setInterval(1349);
  ThisBLEScan->setWindow(449);
  ThisBLEScan->setActiveScan(true);
  ThisBLEScan->start(5, false);
}

void loop() {
  if (!Connected) {
    BLEClient* ThisClient = BLEDevice::createClient();
    do {
      delay(1000);
      ThisClient->setClientCallbacks(new ClientCB());
      ThisClient->connect(ThisDevice);
    } while (!Connected);
    BLERemoteService* ThisRemoteService = ThisClient->getService(ServiceUUID);
    WriteCharacteristic = ThisRemoteService->getCharacteristic(WriteCharacteristicUUID);
  }
  else {
    if (digitalRead(22) == LOW) {
      WriteCharacteristic->writeValue("+50", 3);
      delay(200);
    }
    if (digitalRead(23) == LOW) {
      WriteCharacteristic->writeValue("-50", 3);
      delay(200);
    }
  }
}
