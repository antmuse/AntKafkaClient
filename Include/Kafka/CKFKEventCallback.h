#pragma once
#include <string>
#include <iostream>  
#include "rdkafka.h"    
#include "rdkafkacpp.h"

namespace app {
namespace net {


class CKFKEventCallback : public RdKafka::EventCb {
public:
    virtual ~CKFKEventCallback() {}

    virtual void event_cb(RdKafka::Event& event);

private:
    bool mRunning;
};


}//namespace net 
}//namespace app 
