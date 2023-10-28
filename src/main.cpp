#include <Arduino.h>
#include <M5StickC.h>
#include <Wire.h>
#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>
#include <VL53L0X.h>



// TOF Sensor
VL53L0X sensor;
bool isObjectNear = false;

// Bluetooth
BLEServer* pServer;
BLEService* pService;
BLECharacteristic* pCharacteristic;
BLEAdvertising* pAdvertising;

void setup() {
  M5.begin();
  
  // Initialize TOF sensor
  if (!sensor.init()) {
    Serial.println("Failed to initialize TOF sensor.");
    while (1);
  }

  // Start TOF sensor
  sensor.setTimeout(500);
  sensor.startContinuous();

  // Initialize Bluetooth
  BLEDevice::init("M5StickC");
  pServer = BLEDevice::createServer();
  pService = pServer->createService(BLEUUID("5a12b742-9e6c-48fc-84c2-0c0aefa24f88"));
  pCharacteristic = pService->createCharacteristic(BLEUUID("5a12b742-9e6c-48fc-84c2-0c0aefa24f88"), BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_NOTIFY);
  pCharacteristic->addDescriptor(new BLE2902());
  pCharacteristic->setValue("Hello, BLE!");
  pAdvertising = pServer->getAdvertising();
  pService->start();
  pAdvertising->setMinPreferred(0x06);  // recommended
  pAdvertising->setMinPreferred(0x12);  // recommended
  pAdvertising->addServiceUUID(BLEUUID("5a12b742-9e6c-48fc-84c2-0c0aefa24f88"));
  pServer->startAdvertising();
}

void loop() {
  // Read TOF sensor data
  uint16_t distance = sensor.readRangeContinuousMillimeters();
  
  if (distance < 100) {  // Adjust this threshold as needed
    isObjectNear = true;
  } else {
    isObjectNear = false;
  }

  // Update the characteristic with object detection status
  pCharacteristic->setValue(isObjectNear ? "1" : "0");
  pCharacteristic->notify();
  
  delay(1000);  // Adjust the delay as needed
}
