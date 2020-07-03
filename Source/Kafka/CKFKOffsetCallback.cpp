#include "CKFKOffsetCallback.h"

namespace app {
namespace net {

void CKFKOffsetCallback::offset_commit_cb(RdKafka::ErrorCode err,
    std::vector<RdKafka::TopicPartition*>& offsets) {
    for (usz i = 0; i < offsets.size(); ++i) {
        printf("partition=%d,offset=%llu\n", offsets[i]->partition(), offsets[i]->offset());
    }
}


}//namespace net 
}//namespace app 
