//
//  VirtHID_Device.hpp
//  VirtHID
//
//  Created by Viktor Varenik on 17.10.2024.
//

#ifndef virthid_device_h
#define virthid_device_h

#include "IOKit/hid/IOHIDDevice.h"
#include "VirtHID_UserClient.hpp"

class it_kotleni_virthid_device : public IOHIDDevice {
    OSDeclareDefaultStructors(it_kotleni_virthid_device)
    
public:
    virtual bool init(OSDictionary *dictionary = 0) override;
    
    virtual void free(void) override;
    virtual bool start(IOService *provider) override;
    virtual void stop(IOService *provider) override;
    
    /**
     *  Set the device name.
     *  The reference count is automatically increased.
     *
     *  @param name A unique device name.
     */
    virtual void setName(OSString *name);
    
    /**
     *  Set the serial number string.
     *  The reference count is automatically increased.
     *
     *  @param serialNumberString: A serial number string.
     */
    virtual void setSerialNumberString(OSString *serialNumberString);
    
    /**
     *  Set the vendor ID.
     *
     *  @param vendorID The vendor ID.
     */
    virtual void setVendorID(uint32_t vendorID);
    
    /**
     *  Set the product ID.
     *
     *  @param productID The product ID.
     */
    virtual void setProductID(uint32_t productID);
    
    /**
     *  Return the device name, WITHOUT increasing the reference count.
     *
     *  @return The device name.
     */
    virtual OSString *name();

    /**
     *  Store a callback to be called whenever setReport is called on device.
     *
     *  @param subscriber Reference to callback.
     */
    virtual void subscribe(IOService *userClient);
    
    virtual OSString *newProductString() const override;
    virtual OSString *newSerialNumberString() const override;
    virtual OSNumber *newVendorIDNumber() const override;
    virtual OSNumber *newProductIDNumber() const override;
    
    virtual IOReturn newReportDescriptor(IOMemoryDescriptor **descriptor) const override;

    virtual IOReturn setReport(IOMemoryDescriptor *report, IOHIDReportType reportType,
                               IOOptionBits options = 0) override;

    unsigned char *reportDescriptor = nullptr;
    UInt16 reportDescriptor_len;
    
    bool isMouse = false;
    bool isKeyboard = false;
private:
    OSString *m_name = nullptr;
    OSString *m_serial_number_string = nullptr;
    OSNumber *m_vendor_id = nullptr;
    OSNumber *m_product_id = nullptr;
    it_kotleni_virthid_userclient *m_user_client = nullptr;
};

#endif
