#include "CKFKConsummer.h"
#include "CLogger.h"


namespace app {
namespace net {


CKFKConsummer::CKFKConsummer() :
    mConfig(CKFKConfig::EKCT_GLOBAL),
    mConfigTopic(CKFKConfig::EKCT_TOPIC),
    mTopic(nullptr),
    mConsummer(nullptr) {
}


CKFKConsummer::~CKFKConsummer() {
    destroy();
}


void CKFKConsummer::destroy() {
    if (mTopic) {
        delete mTopic;
        mTopic = nullptr;
    }
    if (mConsummer) {
        RdKafka::ErrorCode ecd = mConsummer->unassign();
        if (RdKafka::ERR_NO_ERROR != ecd) {
            CLogger::logError("CKFKConsummer::destroy", "unassign=%d", ecd);
        }
        ecd = mConsummer->unsubscribe();
        if (RdKafka::ERR_NO_ERROR != ecd) {
            CLogger::logError("CKFKConsummer::destroy", "unsubscribe=%d", ecd);
        }
        ecd = mConsummer->close();
        if (RdKafka::ERR_NO_ERROR != ecd) {
            CLogger::logError("CKFKConsummer::destroy", "close=%d", ecd);
        }
        delete mConsummer;
        mConsummer = nullptr;
    }
}


bool CKFKConsummer::init(const s8* topic) {
    if (!topic) {
        return false;
    }
    s32 ret = mConfig.setOffsetCallback(mOffset);
    if (0 != ret) {
        CLogger::logError("CKFKConsummer::init", "setOffsetCallback=%d", ret);
        return false;
    }
    ret = mConfig.setRebalanceCallback(mRebalance);
    if (0 != ret) {
        CLogger::logError("CKFKConsummer::init", "setRebalanceCallback=%d", ret);
        return false;
    }
    ret = mConfig.setDefaultTopic(mConfigTopic);
    if (0 != ret) {
        CLogger::logError("CKFKConsummer::init", "setDefaultTopic=%d", ret);
        return false;
    }
    ret = mConfig.setEventCallback(mEventer);
    if (0 != ret) {
        CLogger::logError("CKFKConsummer::init", "setEventCallback=%d", ret);
        return false;
    }
    //mStartOffset = RdKafka::Topic::OFFSET_BEGINNING;
    std::string errorstr;
    mConsummer = RdKafka::KafkaConsumer::create(mConfig.getPointer(), errorstr);
    if (!mConsummer) {
        CLogger::logError("CKFKConsummer::init", "KafkaConsumer=null");
        return false;
    }
    std::string topicname(topic);
    mTopic = RdKafka::Topic::create(mConsummer, topicname, mConfigTopic.getPointer(), errorstr);
    if (!mTopic) {
        CLogger::logError("CKFKConsummer::init", "Topic=null");
        destroy();
        return false;
    }
    std::vector<std::string> topics;
    topics.push_back(topicname);
    RdKafka::ErrorCode resp = mConsummer->subscribe(topics);
    if (resp != RdKafka::ERR_NO_ERROR) {
        CLogger::logError("CKFKConsummer::init", "subscribe=%s", RdKafka::err2str(resp).c_str());
        destroy();
        return false;
    }
    return true;
}


RdKafka::Message* CKFKConsummer::read(s32 timeout) {
    return mConsummer ? mConsummer->consume(timeout) : nullptr;
}



}//namespace net 
}//namespace app
