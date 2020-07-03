#pragma once

#include <iostream>  
#include "rdkafka.h"
#include "rdkafkacpp.h"
#include "HAtomicOperator.h"
#include "CString.h"
#include "CKFKConfig.h"

namespace app {
namespace net {


class CKFKConsummer {
public:
    CKFKConsummer();
    virtual ~CKFKConsummer();

    bool init(const s8* topic);

    //消费者无需周期性pull, read已经代替pull
    //s32 pull();

    /**
    * @brief 同步读,
    * @return Message, 用完必须删除.
    */
    RdKafka::Message* read(s32 timeout);

    CKFKConfig& getConfig() {
        return mConfig;
    }

    CKFKConfig& getTopicConfig() {
        return mConfigTopic;
    }

private:
    void destroy();

    CKFKEventCallback mEventer;
    CKFKOffsetCallback mOffset;
    CKFKRebalanceCallback mRebalance;
    CKFKConfig mConfig;     //Global
    CKFKConfig mConfigTopic;
    RdKafka::Topic* mTopic;
    RdKafka::KafkaConsumer* mConsummer;
};

}//namespace net 
}//namespace app 
