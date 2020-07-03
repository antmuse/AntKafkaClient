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

    //����������������pull, read�Ѿ�����pull
    //s32 pull();

    /**
    * @brief ͬ����,
    * @return Message, �������ɾ��.
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
