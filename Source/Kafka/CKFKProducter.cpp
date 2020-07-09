#include "CKFKProducter.h"
#include <string>
#include <iostream>  
#include <assert.h>
#include <time.h>
#include "rdwin32.h"
#include "CLogger.h"

namespace app {
namespace net {

static void AppKfkProducTimeout(void* nd) {
    net::CKFKProducter& producter = *(net::CKFKProducter*)nd;
    producter.pull();
    //printf("AppKfkProducTimeout>> producter pull\n");
}

void AppKfkLogCallback(const rd_kafka_t* rk, s32 level, const s8* fac, const s8* buf) {
    CLogger::log(ELOG_INFO, "AppKfkLogCallback", "%i-%s: %s: %s",
        level, fac, rk ? rd_kafka_name(rk) : "", buf);
}


CKFKProducter::CKFKProducter() :
    mConfig(CKFKConfig::EKCT_GLOBAL),
    mConfigTopic(CKFKConfig::EKCT_TOPIC),
    mPollTimeout(10),
    mTopic(nullptr),
    mProducer(nullptr) {

    mTimeNode.mCallback = AppKfkProducTimeout;
    mTimeNode.mCallbackData = this;
}


CKFKProducter::~CKFKProducter() {
    flush(10);
    destroy();
}

void CKFKProducter::destroy() {
    if (mTopic) {
        delete mTopic;  //step 1
        mTopic = 0;
    }
    if (mProducer) {
        delete mProducer; //step 2
        mProducer = 0;
    }
}


s32 CKFKProducter::init(const s8* topic) {
    APP_ASSERT(topic);
    if (mProducer) {
        return 0;
    }
    s32 ret = mConfig.setEventCallback(mEventer);
    if (0 != ret) {
        CLogger::logError("CKFKProducter::init", "setEventCallback=%d", ret);
        return false;
    }
    ret = mConfigTopic.setPartitionCallback(mPartitioner);
    if (0 != ret) {
        CLogger::logError("CKFKProducter::init", "setPartitionCallback=%d", ret);
        return false;
    }
    ret = mConfig.setDeliverCallback(mDeliver);
    if (0 != ret) {
        CLogger::logError("CKFKProducter::init", "setDeliverCallback=%d", ret);
        return false;
    }
    std::string errstr;
    //rd_kafka_conf_set_log_cb(m_config, AppKfkLogCallback);
    //rd_kafka_set_log_level(m_kafka, LOG_DEBUG);
    mProducer = RdKafka::Producer::create(mConfig.getPointer(), errstr);
    if (!mProducer) {
        destroy();
        CLogger::logError("CKFKProducter::init", "Producer=null");
        return EKFK_FAILED;
    }
    mTopic = RdKafka::Topic::create(mProducer, topic, mConfigTopic.getPointer(), errstr);
    if (!mTopic) {
        destroy();
        CLogger::logError("CKFKProducter::init", "Topic=null");
        return EKFK_FAILED;
    }
    return EKFK_SUCCESS;
}


s32 CKFKProducter::pull() {
    return mProducer ? mProducer->poll(mPollTimeout) : EKFK_FAILED;
}

s32 CKFKProducter::sendBuffer(void* buffer, const u32 buf_len,
    s32 msgflag/*= RdKafka::Producer::RK_MSG_COPY*/,
    const void* key/* = 0*/, u32 keysize/* = 0*/,
    s32 partion/* = -1*/, void* user/* = 0*/) {

    if (0 == buffer || !mProducer) {
        return 0;
    }
    RdKafka::ErrorCode ret = mProducer->produce(mTopic,
        partion >= 0 ? partion : RdKafka::Topic::PARTITION_UA,
        msgflag, buffer, (size_t)buf_len, key, keysize, user);

    if (RdKafka::ERR_NO_ERROR != ret) {
        mProducer->poll(500);
        return EKFK_FAILED;
    }
    //mProducer->poll(0); //在定时线程中pull
    return EKFK_SUCCESS;
}


s32 CKFKProducter::flush(s32 timeout) {
    return mProducer ? mProducer->flush(timeout) : EKFK_FAILED;
}

}//namespace net 
}//namespace app 