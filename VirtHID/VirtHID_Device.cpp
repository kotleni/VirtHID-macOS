//
//  VirtHID_Device.cpp
//  VirtHID
//
//  Created by Viktor Varenik on 17.10.2024.
//

#include <IOKit/IOLib.h>
#include "VirtHID_Device.hpp"
#include "debug.h"

#define super IOHIDDevice
OSDefineMetaClassAndStructors(it_kotleni_virthid_device, IOHIDDevice)

bool it_kotleni_virthid_device::init(OSDictionary *dict) {
    LogD("Initializing a new virtual HID device.");
    
    if (!super::init(dict)) {
        return false;
    }
    
    if (isMouse) {
        setProperty("HIDDefaultBehavior", "Mouse");
    } else if (isKeyboard) {
        setProperty("HIDDefaultBehavior", "Keyboard");
    }
    
    return true;
}

bool it_kotleni_virthid_device::start(IOService *provider) {
    LogD("Executing 'it_kotleni_virthid_device::start()'.");
    return super::start(provider);
}

void it_kotleni_virthid_device::stop(IOService *provider) {
    LogD("Executing 'it_kotleni_virthid_device::stop()'.");
    
    super::stop(provider);
}

void it_kotleni_virthid_device::free() {
    LogD("Executing 'it_kotleni_virthid_device::free()'.");
    
    if (reportDescriptor) IOFree(reportDescriptor, reportDescriptor_len);
    if (m_name) m_name->release();
    if (m_serial_number_string) m_serial_number_string->release();
    
    super::free();
}

OSString *it_kotleni_virthid_device::name() {
    return m_name;
}

void it_kotleni_virthid_device::subscribe(IOService *userClient) {
    m_user_client = OSDynamicCast(it_kotleni_virthid_userclient, userClient);
}

void it_kotleni_virthid_device::setName(OSString *name) {
    if (name) name->retain();
    m_name = name;
}

void it_kotleni_virthid_device::setSerialNumberString(OSString *serialNumberString) {
    if (serialNumberString) {
        serialNumberString->retain();
    }
    
    m_serial_number_string = serialNumberString;
}

void it_kotleni_virthid_device::setVendorID(uint32_t vendorID) {
    m_vendor_id = OSNumber::withNumber(vendorID, 32);
}

void it_kotleni_virthid_device::setProductID(uint32_t productID) {
    m_product_id = OSNumber::withNumber(productID, 32);
}

IOReturn it_kotleni_virthid_device::newReportDescriptor(IOMemoryDescriptor **descriptor) const {
    LogD("Executing 'it_kotleni_virthid_device::newReportDescriptor()'.");
    IOBufferMemoryDescriptor *buffer =
        IOBufferMemoryDescriptor::inTaskWithOptions(kernel_task, 0, reportDescriptor_len);
    
    if (!buffer) {
        LogD("Error while allocating new IOBufferMemoryDescriptor.");
        return kIOReturnNoResources;
    }
    
    buffer->writeBytes(0, reportDescriptor, reportDescriptor_len);
    *descriptor = buffer;
    
    return kIOReturnSuccess;
}

IOReturn it_kotleni_virthid_device::setReport(IOMemoryDescriptor *report, IOHIDReportType reportType, IOOptionBits options) {
    // No one is listening yet.
    if (!m_user_client) return kIOReturnSuccess;

    return m_user_client->notifySubscriber(report);
}

OSString *it_kotleni_virthid_device::newProductString() const {
    m_name->retain();
    return m_name;
}

OSString *it_kotleni_virthid_device::newSerialNumberString() const {
    m_serial_number_string->retain();
    return m_serial_number_string;
}

OSNumber *it_kotleni_virthid_device::newVendorIDNumber() const {
    m_vendor_id->retain();
    return m_vendor_id;
}

OSNumber *it_kotleni_virthid_device::newProductIDNumber() const {
    m_product_id->retain();
    return m_product_id;
}
