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
    Logging::logD("pub_systems", "%d", serialNum);
    Phidget_setDeviceSerialNumber((PhidgetHandle)*handler, serialNum);
    Logging::logD("pub_systems", "%d", hubPort);
    Phidget_setHubPort((PhidgetHandle)*handler, hubPort);
    Phidget_setChannel((PhidgetHandle)*handler, channel);
    Phidget_setOnAttachHandler((PhidgetHandle)*handler, onAttachHandler, NULL);
    Phidget_openWaitForAttachment((PhidgetHandle)*handler,
                                  PHIDGET_TIMEOUT_DEFAULT);
};