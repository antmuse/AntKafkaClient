#pragma once
#include <string>
#include <iostream>  
#include "rdkafka.h"    
#include "rdkafkacpp.h"
#include "HConfig.h"

namespace app {
namespace net {


class CKFKDeliverCallback : public RdKafka::DeliveryReportCb {
public:
    CKFKDeliverCallback() :
        mSuccessCount(0),
        mFailedCount(0) {
    }
    virtual ~CKFKDeliverCallback() {
    }

    virtual  void dr_cb(RdKafka::Message &message);

    s64 getSuccessCount()const {
        return mSuccessCount;
    }

    s64 getFailedCount()const {
        return mFailedCount;
    }

private:
    s64 mSuccessCount;
    s64 mFailedCount;
};


}//namespace net 
}//namespace app 
