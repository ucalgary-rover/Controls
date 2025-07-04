#include <iostream>

#pragma once

// Declare the event handler
static void CCONV onAttachHandler(PhidgetHandle ph, void* ctx) {
    // You can access the Phidget that fired the event by using the first
    // parameter of the event handler
    int deviceSerialNumber;
    Phidget_getDeviceSerialNumber(ph, &deviceSerialNumber);
    std::cout << "Device: " << deviceSerialNumber << "Has attached";
};

template <typename T>
void setAddressProperties(T* handle, int serialNum, int channel) {
    Phidget_setDeviceSerialNumber((PhidgetHandle)*handle, serialNum);
    Phidget_setHubPort((PhidgetHandle)*handle, channel);
    Phidget_setOnAttachHandler((PhidgetHandle)*handle, onAttachHandler, NULL);
    Phidget_openWaitForAttachment((PhidgetHandle)*handle, 5000);
};