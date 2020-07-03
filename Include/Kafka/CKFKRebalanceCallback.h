#pragma once

#include "rdkafkacpp.h"

namespace app {
namespace net {


class CKFKRebalanceCallback : public RdKafka::RebalanceCb {
public:
    virtual ~CKFKRebalanceCallback() {
    }

    virtual void rebalance_cb(RdKafka::KafkaConsumer* consumer,
        RdKafka::ErrorCode err,
        std::vector<RdKafka::TopicPartition*>&partitions)override;

private:
    //bool mRunning;
};


}//namespace net 
}//namespace app

