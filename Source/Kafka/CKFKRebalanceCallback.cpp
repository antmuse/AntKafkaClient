#include "CKFKRebalanceCallback.h"
#include "CLogger.h"

namespace app {
namespace net {

void CKFKRebalanceCallback::rebalance_cb(RdKafka::KafkaConsumer* consumer,
    RdKafka::ErrorCode err, std::vector<RdKafka::TopicPartition*>& partitions) {
    if (err == RdKafka::ERR__ASSIGN_PARTITIONS) {
        //assign�����е�partitions������offset����Ϊ��ָ��offset��ʼ����
        //��������ã���ӷ������д洢���������offsetλ�ÿ�ʼ����
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

