#include "CKFKConfig.h"
#include "CLogger.h"
#include "Json/json.h"

namespace app {
namespace net {

s32 AppKfkWaitExit(s32 iMillisecond) {
    return RdKafka::wait_destroyed(iMillisecond);
}


CKFKConfig::CKFKConfig(EKafkaConfitType type) :
    mConfig(0) {
    APP_ASSERT(RdKafka::Conf::CONF_GLOBAL == EKCT_GLOBAL);
    APP_ASSERT(RdKafka::Conf::CONF_TOPIC == EKCT_TOPIC);
    mConfig = RdKafka::Conf::create((RdKafka::Conf::ConfType)type);
    APP_ASSERT(mConfig);
}

CKFKConfig::~CKFKConfig() {
    if (mConfig) {
        delete mConfig;
        mConfig = 0;
    }
}

s32 CKFKConfig::set(const s8* key, const s8* value) {
    s32 ret = -1;
    if (key&& value && mConfig) {
        std::string error;
        ret = mConfig->set(key, value, error);
        if (RdKafka::Conf::CONF_OK != ret) {
            CLogger::log(ELOG_ERROR, "CKFKConfig::set", "%s", error.c_str());
        }
    }
    return ret;
}

s32 CKFKConfig::setRebalanceCallback(CKFKRebalanceCallback& value) {
    s32 ret = -1;
    if (mConfig) {
        std::string error;
        ret = mConfig->set("rebalance_cb", &value, error);
        if (RdKafka::Conf::CONF_OK != ret) {
            CLogger::log(ELOG_ERROR, "CKFKConfig::setRebalanceCallback", "%s", error.c_str());
        }
    }
    return ret;
}

s32 CKFKConfig::setConsumeCallback(CKFKConsumeCallback& value) {
    s32 ret = -1;
    if (mConfig) {
        std::string error;
        ret = mConfig->set("consume_cb", &value, error);
        if (RdKafka::Conf::CONF_OK != ret) {
            CLogger::log(ELOG_ERROR, "CKFKConfig::setConsumeCallback", "%s", error.c_str());
        }
    }
    return ret;
}


s32 CKFKConfig::setOffsetCallback(CKFKOffsetCallback& value) {
    s32 ret = -1;
    if (mConfig) {
        std::string error;
        ret = mConfig->set("offset_commit_cb", &value, error);
        if (RdKafka::Conf::CONF_OK != ret) {
            CLogger::log(ELOG_ERROR, "CKFKConfig::setOffsetCallback", "%s", error.c_str());
        }
    }
    return ret;
}


s32 CKFKConfig::setEventCallback(CKFKEventCallback& value) {
    s32 ret = -1;
    if (mConfig) {
        std::string error;
        ret = mConfig->set("event_cb", &value, error);
        if (RdKafka::Conf::CONF_OK != ret) {
            CLogger::log(ELOG_ERROR, "CKFKConfig::setEventCallback", "%s", error.c_str());
        }
    }
    return ret;
}

s32 CKFKConfig::setDeliverCallback(CKFKDeliverCallback& value) {
    s32 ret = -1;
    if (mConfig) {
        std::string error;
        ret = mConfig->set("dr_cb", &value, error);
        if (RdKafka::Conf::CONF_OK != ret) {
            CLogger::log(ELOG_ERROR, "CKFKConfig::setDeliverCallback", "%s", error.c_str());
        }
    }
    return ret;
}

s32 CKFKConfig::setPartitionCallback(CKFKPartitionCallback& value) {
    s32 ret = -1;
    if (mConfig) {
        std::string error;
        ret = mConfig->set("partitioner_cb", &value, error);
        if (RdKafka::Conf::CONF_OK != ret) {
            CLogger::log(ELOG_ERROR, "CKFKConfig::setPartitionCallback", "%s", error.c_str());
        }
    }
    return ret;
}

s32 CKFKConfig::setDefaultTopic(CKFKConfig& val) {
    s32 ret = -1;
    if (mConfig) {//EKCT_GLOBAL
        std::string error;
        ret = mConfig->set("default_topic_conf", val.getPointer(), error);
        if (RdKafka::Conf::CONF_OK != ret) {
            CLogger::log(ELOG_ERROR, "CKFKConfig::setDefaultTopic", "%s", error.c_str());
        }
    }
    return ret;
}

//s32 CKFKConfig::setLoggerCallback(AppKafkaLogCallback value) {
//    s32 ret = -1;
//    if (mConfig) {
//        std::string error;
//        //ret = mConfig->set("log_cb", value, error);
//        if (RdKafka::Conf::CONF_OK != ret) {
//            //log error msg
//        }
//    }
//    return ret;
//}


}//namespace net 
}//namespace app 
