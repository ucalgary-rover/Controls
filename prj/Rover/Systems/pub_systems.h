#include "phidget22.h"
#include "pub_general.h"
#include <iostream>

#pragma once

// Declare the event handler
static void CCONV onAttachHandler(PhidgetHandle ph, void* ctx) {
    // You can access the Phidget that fired the event by using the first
    // parameter of the event handler
    int deviceSerialNumber;
    Phidget_getDeviceSerialNumber(ph, &deviceSerialNumber);
    Logging::logI("pub_systems", "Device: %d has attached", deviceSerialNumber);
};

template <typename T>
void setAddressProperties(T* handler, int serialNum, int channel, int hubPort) {
    PhidgetReturnCode res;
    res = Phidget_setDeviceSerialNumber((PhidgetHandle)*handler, serialNum);
    res = Phidget_setHubPort((PhidgetHandle)*handler, hubPort);
    res = Phidget_setChannel((PhidgetHandle)*handler, channel);
    res = Phidget_setOnAttachHandler((PhidgetHandle)*handler, onAttachHandler,
                                     NULL);
    res = Phidget_openWaitForAttachment((PhidgetHandle)*handler, 10000);
};