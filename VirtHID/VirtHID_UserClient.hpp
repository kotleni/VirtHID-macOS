//
//  VirtHID_UserClient.hpp
//  VirtHID
//
//  Created by Viktor Varenik on 17.10.2024.
//

#ifndef virthid_userclient_h
#define virthid_userclient_h

#include <IOKit/IOService.h>
#include <IOKit/IOUserClient.h>

#include "VirtHID.hpp"

/**
 The goal of this User Client is to expose to user space the following selector.
*/
enum {
    it_kotleni_virthid_method_create,
    it_kotleni_virthid_method_destroy,
    it_kotleni_virthid_method_send,
    it_kotleni_virthid_method_list,
    it_kotleni_virthid_method_subscribe,

    it_kotleni_virthid_method_count  // Keep track of the length of this enum.
};

class it_kotleni_virthid_userclient : public IOUserClient {
    OSDeclareDefaultStructors(it_kotleni_virthid_userclient);
    
public:
    virtual bool initWithTask(task_t owningTask, void *securityToken,
                              UInt32 type, OSDictionary *properties) override;
    
    virtual bool start(IOService *provider) override;
    virtual void stop(IOService *provider) override;
    
    virtual IOReturn externalMethod(uint32_t selector,
                                    IOExternalMethodArguments *arguments,
                                    IOExternalMethodDispatch *dispatch,
                                    OSObject *target, void *reference) override;

    virtual IOReturn notifySubscriber(IOMemoryDescriptor *report);

protected:
    /**
     * The following methods unpack/handle the given arguments and
     * call the related driver method.
     */
    virtual IOReturn methodCreate(IOExternalMethodArguments *arguments);
    virtual IOReturn methodDestroy(IOExternalMethodArguments *arguments);
    virtual IOReturn methodSend(IOExternalMethodArguments *arguments);
    virtual IOReturn methodList(IOExternalMethodArguments *arguments);
    virtual IOReturn methodSubscribe(IOExternalMethodArguments *arguments);

    /**
     *  The following static methods redirect the call to the 'target' instance.
     */
    static IOReturn sMethodCreate(it_kotleni_virthid_userclient *target,
                                 void *reference,
                                 IOExternalMethodArguments *arguments);
    static IOReturn sMethodDestroy(it_kotleni_virthid_userclient *target,
                                  void *reference,
                                  IOExternalMethodArguments *arguments);
    static IOReturn sMethodSend(it_kotleni_virthid_userclient *target,
                               void *reference,
                               IOExternalMethodArguments *arguments);
    static IOReturn sMethodList(it_kotleni_virthid_userclient *target,
                               void *reference,
                               IOExternalMethodArguments *arguments);
    static IOReturn sMethodSubscribe(it_kotleni_virthid_userclient *target,
                                    void *reference,
                                    IOExternalMethodArguments *arguments);

private:
    /**
     *  Method dispatch table.
     */
    static const IOExternalMethodDispatch s_methods[it_kotleni_virthid_method_count];
    
    /**
     *  Driver provider.
     */
    it_kotleni_virthid *m_hid_provider;

    /**
     *  Userland subscriber.
     */
    OSAsyncReference64 *m_subscriber = nullptr;
    
    /**
     *  Task owner.
     */
    task_t m_owner;
};

#endif
