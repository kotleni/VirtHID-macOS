//
//  VirtHID_UserClient.cpp
//  VirtHID
//
//  Created by Viktor Varenik on 17.10.2024.
//

#include <IOKit/IOLib.h>
#include <IOKit/IOKitKeys.h>

#include "VirtHID_UserClient.hpp"
#include "VirtHID_Types.hpp"
#include "debug.h"
#include <string.h>

#define super IOUserClient
OSDefineMetaClassAndStructors(it_kotleni_virthid_userclient, IOUserClient)

bool it_kotleni_virthid_userclient::initWithTask(task_t owningTask, void *securityToken,
                                              UInt32 type, OSDictionary *properties) {
    LogD("Executing 'it_kotleni_virthid_userclient::initWithTask()'.");
    
    if (!owningTask) {
        return false;
    }
    
    if (!super::initWithTask(owningTask, securityToken, type, properties)) {
        return false;
    }
    
    m_owner = owningTask;
    
    return true;
}

bool it_kotleni_virthid_userclient::start(IOService *provider) {
    LogD("Executing 'it_kotleni_virthid_userclient::start()'.");
    
    if (!super::start(provider)) {
        return false;
    }
    
    m_hid_provider = OSDynamicCast(it_kotleni_virthid, provider);
    if (!m_hid_provider) {
        return false;
    }
    
    return true;
}

void it_kotleni_virthid_userclient::stop(IOService *provider) {
    LogD("Executing 'it_kotleni_virthid_userclient::stop()'.");
    super::stop(provider);
}

/**
 * A dispatch table for this User Client interface, used by 'it_kotleni_virthid_userclient::externalMethod()'.
 * The fields of the IOExternalMethodDispatch type follows:
 *
 *  struct IOExternalMethodDispatch
 *  {
 *      IOExternalMethodAction function;
 *      uint32_t           checkScalarInputCount;
 *      uint32_t           checkStructureInputSize;
 *      uint32_t           checkScalarOutputCount;
 *      uint32_t           checkStructureOutputSize;
 *  };
 */
const IOExternalMethodDispatch it_kotleni_virthid_userclient::s_methods[it_kotleni_virthid_method_count] = {
    {(IOExternalMethodAction)&it_kotleni_virthid_userclient::sMethodCreate, 8, 0, 0, 0},
    {(IOExternalMethodAction)&it_kotleni_virthid_userclient::sMethodDestroy, 2, 0, 0, 0},
    {(IOExternalMethodAction)&it_kotleni_virthid_userclient::sMethodSend, 4, 0, 0, 0},
    {(IOExternalMethodAction)&it_kotleni_virthid_userclient::sMethodList, 2, 0, 2, 0},
    {(IOExternalMethodAction)&it_kotleni_virthid_userclient::sMethodSubscribe, 2, 0, 0, 0},
};

IOReturn it_kotleni_virthid_userclient::externalMethod(uint32_t selector, IOExternalMethodArguments *arguments,
                                                    IOExternalMethodDispatch *dispatch, OSObject *target, void *reference) {
    LogD("Executing 'it_kotleni_virthid_userclient::externalMethod()' with selector #%d.", selector);
    
    if (selector >= it_kotleni_virthid_method_count) {
        return kIOReturnUnsupported;
    }
    
    dispatch = (IOExternalMethodDispatch *)&s_methods[selector];
    target = this;
    reference = nullptr;
    
    return super::externalMethod(selector, arguments, dispatch, target, reference);
}

IOReturn it_kotleni_virthid_userclient::sMethodCreate(it_kotleni_virthid_userclient *target, void *reference,
                                                  IOExternalMethodArguments *arguments) {
    return target->methodCreate(arguments);
}

IOReturn it_kotleni_virthid_userclient::sMethodDestroy(it_kotleni_virthid_userclient *target, void *reference,
                                                   IOExternalMethodArguments *arguments) {
    return target->methodDestroy(arguments);
}

IOReturn it_kotleni_virthid_userclient::sMethodSend(it_kotleni_virthid_userclient *target, void *reference,
                                                IOExternalMethodArguments *arguments) {
    return target->methodSend(arguments);
}

IOReturn it_kotleni_virthid_userclient::sMethodList(it_kotleni_virthid_userclient *target, void *reference,
                                                IOExternalMethodArguments *arguments) {
    return target->methodList(arguments);
}

IOReturn it_kotleni_virthid_userclient::sMethodSubscribe(it_kotleni_virthid_userclient *target, void *reference,
                                                 IOExternalMethodArguments *arguments) {
    return target->methodSubscribe(arguments);
}

IOReturn it_kotleni_virthid_userclient::methodCreate(IOExternalMethodArguments *arguments) {
    IOMemoryDescriptor *user_buf = nullptr;
    IOMemoryDescriptor *descriptor_buf = nullptr;
    IOMemoryDescriptor *serial_number_buf = nullptr;
    
    bool user_buf_complete = false;
    bool descriptor_buf_complete = false;
    bool serial_number_buf_complete = false;
    
    IOMemoryMap *map = nullptr;
    IOMemoryMap *map2 = nullptr;
    IOMemoryMap *map3 = nullptr;
    
    char *ptr = nullptr;
    unsigned char *ptr2 = nullptr;
    char *ptr3 = nullptr;
    
    bool ret = false;
    
    UInt8 *name_ptr = (UInt8 *)arguments->scalarInput[0];
    UInt8 name_len = (UInt8)arguments->scalarInput[1];
    UInt8 *descriptor_ptr = (UInt8 *)arguments->scalarInput[2];
    UInt16 descriptor_len = (UInt16)arguments->scalarInput[3];
    
    UInt8 *serial_number_ptr = (UInt8 *)arguments->scalarInput[4];
    UInt8 serial_number_len = (UInt8)arguments->scalarInput[5];
    UInt32 vendorID = (UInt32)arguments->scalarInput[6];
    UInt32 productID = (UInt32)arguments->scalarInput[7];
    
    user_buf = IOMemoryDescriptor::withAddressRange((vm_address_t)name_ptr, name_len,
                                                    kIODirectionOut, m_owner);
    if (!user_buf) goto nomem;
    if (user_buf->prepare() != kIOReturnSuccess) goto nomem;
    user_buf_complete = true;
    
    descriptor_buf = IOMemoryDescriptor::withAddressRange((vm_address_t)descriptor_ptr, descriptor_len,
                                                          kIODirectionOut, m_owner);
    if (!descriptor_buf) goto nomem;
    if (descriptor_buf->prepare() != kIOReturnSuccess) goto nomem;
    descriptor_buf_complete = true;
    
    serial_number_buf = IOMemoryDescriptor::withAddressRange((vm_address_t)serial_number_ptr,
                                                             serial_number_len,
                                                             kIODirectionOut, m_owner);
    
    if (!serial_number_buf) goto nomem;
    if (serial_number_buf->prepare() != kIOReturnSuccess) goto nomem;
    serial_number_buf_complete = true;
    
    map = user_buf->map();
    if (!map) goto nomem;
    
    map2 = descriptor_buf->map();
    if (!map2) goto nomem;
    
    map3 = serial_number_buf->map();
    if (!map3) goto nomem;
    
    ptr = (char *)map->getAddress();
    if (!ptr) goto nomem;
    
    ptr2 = (unsigned char *)map2->getAddress();
    if (!ptr2) goto nomem;
    
    ptr3 = (char *)map3->getAddress();
    if (!ptr3) goto nomem;
    
    ret = m_hid_provider->methodCreate(ptr, name_len, ptr2, descriptor_len, ptr3,
                                       serial_number_len, vendorID, productID);
    
    user_buf->complete();
    descriptor_buf->complete();
    serial_number_buf->complete();
    
    map->release();
    map2->release();
    map3->release();
    
    user_buf->release();
    descriptor_buf->release();
    serial_number_buf->release();
    
    if (ret) {
        return kIOReturnSuccess;
    }
    
    return kIOReturnDeviceError;
    
nomem:
    if (map) map->release();
    if (map2) map2->release();
    if (map3) map2->release();
    if (user_buf_complete) user_buf->complete();
    if (user_buf) user_buf->release();
    if (descriptor_buf_complete) descriptor_buf->complete();
    if (descriptor_buf) descriptor_buf->release();
    if (serial_number_buf_complete) serial_number_buf->complete();
    if (serial_number_buf) serial_number_buf->release();
    return kIOReturnNoMemory;
}

IOReturn it_kotleni_virthid_userclient::methodDestroy(IOExternalMethodArguments *arguments) {
    IOMemoryDescriptor *user_buf = nullptr;
    bool user_buf_complete = false;
    
    IOMemoryMap *map = nullptr;
    
    char *ptr = nullptr;
    
    bool ret = false;
    
    UInt8 *name_ptr = (UInt8 *)arguments->scalarInput[0];
    UInt8 name_len = (UInt8)arguments->scalarInput[1];
    
    user_buf = IOMemoryDescriptor::withAddressRange((vm_address_t)name_ptr, name_len,
                                                    kIODirectionOut, m_owner);
    if (!user_buf) goto nomem;
    if (user_buf->prepare() != kIOReturnSuccess) goto nomem;
    user_buf_complete = true;
    
    map = user_buf->map();
    if (!map) goto nomem;
    
    ptr = (char *)map->getAddress();
    if (!ptr) goto nomem;
    ret = m_hid_provider->methodDestroy(ptr, name_len);
    
    user_buf->complete();
    map->release();
    user_buf->release();
    
    if (ret) {
        return kIOReturnSuccess;
    }
    
    return kIOReturnDeviceError;
    
nomem:
    if (map) map->release();
    if (user_buf_complete) user_buf->complete();
    if (user_buf) user_buf->release();
    return kIOReturnNoMemory;
}

IOReturn it_kotleni_virthid_userclient::methodSend(IOExternalMethodArguments *arguments) {
    IOMemoryDescriptor *user_buf = nullptr;
    IOMemoryDescriptor *descriptor_buf = nullptr;
    IOMemoryMap *map = nullptr;
    IOMemoryMap *map2 = nullptr;
    
    char *ptr = nullptr;
    unsigned char *ptr2 = nullptr;
    
    bool ret = false;
    
    UInt8 *name_ptr = (UInt8 *)arguments->scalarInput[0];
    UInt8 name_len = (UInt8)arguments->scalarInput[1];
    UInt8 *descriptor_ptr = (UInt8 *)arguments->scalarInput[2];
    UInt16 descriptor_len = (UInt16)arguments->scalarInput[3];
    
    user_buf = IOMemoryDescriptor::withAddressRange((vm_address_t)name_ptr, name_len,
                                                    kIODirectionOut, m_owner);
    if (!user_buf) goto nomem;
    if (user_buf->prepare() != kIOReturnSuccess) goto nomem;
    
    descriptor_buf = IOMemoryDescriptor::withAddressRange((vm_address_t)descriptor_ptr, descriptor_len,
                                                          kIODirectionOut, m_owner);
    
    if (!descriptor_buf) goto nomem;
    if (descriptor_buf->prepare() != kIOReturnSuccess) goto nomem;
    
    map = user_buf->map();
    if (!map) goto nomem;
    
    map2 = descriptor_buf->map();
    if (!map2) goto nomem;
    
    ptr = (char *)map->getAddress();
    if (!ptr) goto nomem;
    
    ptr2 = (unsigned char *)map2->getAddress();
    if (!ptr2) goto nomem;
    
    ret = m_hid_provider->methodSend(ptr, name_len, ptr2, descriptor_len);
    
    user_buf->complete();
    descriptor_buf->complete();
    
    map->release();
    map2->release();
    
    user_buf->release();
    descriptor_buf->release();
    
    if (ret) {
        return kIOReturnSuccess;
    }
    
    return kIOReturnDeviceError;
    
nomem:
    if (map) map->release();
    if (map2) map2->release();
    if (user_buf) user_buf->release();
    if (descriptor_buf) descriptor_buf->release();
    return kIOReturnNoMemory;
}

IOReturn it_kotleni_virthid_userclient::methodList(IOExternalMethodArguments *arguments) {
    IOMemoryDescriptor *user_buf = nullptr;
    
    IOMemoryMap *map = nullptr;
    
    UInt16 needed = 0, items = 0;
    
    char *ptr = nullptr;
    bool ret = false;
    
    UInt8 *name_ptr = (UInt8 *)arguments->scalarInput[0];
    UInt16 name_len = (UInt16)arguments->scalarInput[1];
    
    user_buf = IOMemoryDescriptor::withAddressRange((vm_address_t)name_ptr, name_len,
                                                    kIODirectionIn, m_owner);
    if (!user_buf) goto nomem;
    if (user_buf->prepare() != kIOReturnSuccess) goto nomem;
    
    map = user_buf->map();
    if (!map) goto nomem;
    
    ptr = (char *)map->getAddress();
    if (!ptr) goto nomem;
    
    ret = m_hid_provider->methodList(ptr, name_len, &needed, &items);
    
    user_buf->complete();
    
    map->release();
    
    user_buf->release();
    if (ret) {
        arguments->scalarOutput[0] = needed;
        arguments->scalarOutput[1] = items;
        return kIOReturnSuccess;
    }
    
    return kIOReturnDeviceError;
    
nomem:
    if (map) map->release();
    if (user_buf) user_buf->release();
    return kIOReturnNoMemory;
}

IOReturn it_kotleni_virthid_userclient::methodSubscribe(IOExternalMethodArguments *arguments) {
    IOMemoryDescriptor *user_buf = nullptr;
    bool user_buf_complete = false;

    IOMemoryMap *map = nullptr;

    char *ptr = nullptr;

    bool ret = false;

    UInt8 *name_ptr = (UInt8 *)arguments->scalarInput[0];
    UInt8 name_len = (UInt8)arguments->scalarInput[1];

    if (!arguments->asyncReference) return kIOReturnBadArgument;

    user_buf = IOMemoryDescriptor::withAddressRange((vm_address_t)name_ptr, name_len,
                                                    kIODirectionOut, m_owner);
    if (!user_buf) goto nomem;
    if (user_buf->prepare() != kIOReturnSuccess) goto nomem;
    user_buf_complete = true;

    map = user_buf->map();
    if (!map) goto nomem;

    ptr = (char *)map->getAddress();
    if (!ptr) goto nomem;

    if (m_subscriber) IOFree(m_subscriber, sizeof(OSAsyncReference64));
    m_subscriber = (OSAsyncReference64 *)IOMalloc(sizeof(OSAsyncReference64));
    if (!m_subscriber) goto nomem;
    memcpy(m_subscriber, arguments->asyncReference, sizeof(OSAsyncReference64));

    ret = m_hid_provider->methodSubscribe(ptr, name_len, this);

    user_buf->complete();
    map->release();
    user_buf->release();

    if (ret) {
        return kIOReturnSuccess;
    }

    return kIOReturnDeviceError;

nomem:
    if (map) map->release();
    if (user_buf_complete) user_buf->complete();
    if (user_buf) user_buf->release();
    return kIOReturnNoMemory;
}

IOReturn it_kotleni_virthid_userclient::notifySubscriber(IOMemoryDescriptor *report) {
    IOMemoryMap *reportMap;
    virthid_report userReport;
    io_user_reference_t *args = (io_user_reference_t *)&userReport;
    uint32_t numArgs = sizeof(virthid_report) / sizeof(io_user_reference_t);

    // Max HID report size is 64 bytes. This shouldn't happen.
    if (report->getLength() > virthid_max_report) return kIOReturnBadArgument;

    report->prepare();
    reportMap = report->map();

    userReport.size = reportMap->getSize();
    memcpy(userReport.data, (void *)reportMap->getAddress(), userReport.size);
    sendAsyncResult64(*m_subscriber, kIOReturnSuccess, args, numArgs);

    report->complete();
    reportMap->release();

    // Sleep for a bit to not freak out clients.
    IOSleep(1); // 1ms

    return kIOReturnSuccess;
}
