#pragma once

#include "HConfig.h"
#include "rdkafka.h"
#include "rdkafkacpp.h"  
#include "CKFKConfig.h"
#include "CTimerWheel.h"

namespace app {
namespace net {


class CKFKProducter {
public:
    CKFKProducter();
    virtual ~CKFKProducter();

    s32 init(const s8* topic);

    s32 flush(s32 timeout);

    s32 pull();

    void setPollTimeout(s32 val) {
        mPollTimeout = val;
    }

    s32 sendBuffer(void* buffer, const u32 buf_len,
        s32 msgflag = RdKafka::Producer::RK_MSG_COPY,
        const void* key = 0, u32 keysize = 0, s32 partion = -1, void* user = 0);



    CKFKConfig& getConfig() {
        return mConfig;
    }

    CKFKConfig& getTopicConfig() {
        return mConfigTopic;
    }

    CTimerWheel::STimeNode& getTimeNode() {
        return mTimeNode;
    }

private:
    void destroy();

    s32 mPollTimeout;
    CTimerWheel::STimeNode mTimeNode;
    RdKafka::Topic* mTopic;
    RdKafka::Producer* mProducer;
    CKFKConfig mConfig;
    CKFKConfig mConfigTopic;
    CKFKEventCallback mEventer;
    CKFKDeliverCallback mDeliver;
    CKFKPartitionCallback mPartitioner;
};

}//namespace net 
}//namespace app 
