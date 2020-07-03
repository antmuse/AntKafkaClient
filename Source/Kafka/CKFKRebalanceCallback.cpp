#include "CKFKRebalanceCallback.h"
#include "CLogger.h"

namespace app {
namespace net {

void CKFKRebalanceCallback::rebalance_cb(RdKafka::KafkaConsumer* consumer,
    RdKafka::ErrorCode err, std::vector<RdKafka::TopicPartition*>& partitions) {
    if (err == RdKafka::ERR__ASSIGN_PARTITIONS) {
        //assign方法中的partitions中设置offset，即为从指定offset开始消费
        //如果不设置，则从服务器中存储的最后消费offset位置开始消费
        RdKafka::ErrorCode resp = consumer->assign(partitions);
        if (resp != RdKafka::ERR_NO_ERROR) {
            CLogger::logError("CKFKRebalanceCallback::rebalance_cb",
                "assign=%s,%s", consumer->name().c_str(), RdKafka::err2str(resp).c_str());
        }
    } else {
        RdKafka::ErrorCode ret = consumer->unassign();
        CLogger::logInfo("CKFKRebalanceCallback::rebalance_cb", "unassign=%s,%s, ret=%s",
            consumer->name().c_str(), RdKafka::err2str(err).c_str(),
            RdKafka::err2str(ret).c_str());
    }
}

}//namespace net 
}//namespace app

