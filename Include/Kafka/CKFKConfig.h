#pragma once
#include <string>
#include <iostream>  
#include "rdkafka.h"    
#include "rdkafkacpp.h"
#include "CKFKEventCallback.h"
#include "CKFKDeliverCallback.h"
#include "CKFKPartitionCallback.h"
#include "CKFKOffsetCallback.h"
#include "CKFKRebalanceCallback.h"
#include "CKFKConsumeCallback.h"

namespace app {
namespace net {

enum EKafkaStatusCode {
    EKFK_SUCCESS = 0,
    EKFK_FAILED = -1
};


//typedef void (*AppKafkaLogCallback)(const rd_kafka_t* rk, int level, const char *fac, const char *buf);


/**
* @brief call this when all kfk handles closed and your app wanna quit.
* @param iMillisecond wait time in millisecond.
* @reutrn 0 if success, else quit with abnormalities.
*/
s32 AppKfkWaitExit(s32 iMillisecond);


class CKFKConfig {
public:
    enum EKafkaConfitType {
        EKCT_GLOBAL,
        EKCT_TOPIC
    };
    CKFKConfig(EKafkaConfitType type);
    ~CKFKConfig();

    s32 set(const s8* key, const s8* value);

    /**
    * @brief 消费者组
    */
    s32 setGroup(const s8* val) {
        return set("group.id", val);
    }
    
    s32 setDefaultTopic(CKFKConfig& val);

    /**
    * @brief 仅限旧版消费者类使用
    */
    s32 setConsumeCallback(CKFKConsumeCallback& value);

    s32 setOffsetCallback(CKFKOffsetCallback& value);

    s32 setRebalanceCallback(CKFKRebalanceCallback& value);

    s32 setEventCallback(CKFKEventCallback& value);

    s32 setDeliverCallback(CKFKDeliverCallback& value);

    s32 setPartitionCallback(CKFKPartitionCallback& value);

    //s32 setLoggerCallback(AppKafkaLogCallback loger);

    RdKafka::Conf* getPointer()const {
        return mConfig;
    }

private:
    RdKafka::Conf* mConfig;
};


}//namespace net 
}//namespace app 
