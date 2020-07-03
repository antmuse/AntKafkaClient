#pragma once

#include "rdkafka.h"    
#include "rdkafkacpp.h"

namespace app {
namespace net {

/**
* @brief for old RdKafka::Consumer
*/
class CKFKConsumeCallback : public RdKafka::ConsumeCb {
public:
    CKFKConsumeCallback();
    virtual ~CKFKConsumeCallback() {}

    virtual void consume_cb(RdKafka::Message& message, void* opaque);

private:
};


}//namespace net 
}//namespace app 
