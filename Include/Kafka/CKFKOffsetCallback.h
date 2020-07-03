#pragma once

#include "rdkafkacpp.h"
#include "HConfig.h"

namespace app {
namespace net {


class CKFKOffsetCallback : public RdKafka::OffsetCommitCb {
public:
    virtual ~CKFKOffsetCallback() {
    }

    virtual void offset_commit_cb(RdKafka::ErrorCode err,
        std::vector<RdKafka::TopicPartition*>& offsets)override;

private:
    //bool mRunning;
};


}//namespace net 
}//namespace app

