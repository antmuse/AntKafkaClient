#include "CKFKPartitionCallback.h"
#include <string>
#include <iostream>

namespace app {
namespace net {

s32 CKFKPartitionCallback::partitioner_cb(const RdKafka::Topic *topic, const std::string *key,
    s32 partition_cnt, void *msg_opaque) {
    return djb_hash(key->c_str(), key->size()) % partition_cnt;
}

inline u32 CKFKPartitionCallback::djb_hash(const s8 *str, size_t len) {
    unsigned int hash = 5381;
    for (size_t i = 0; i < len; i++)
        hash = ((hash << 5) + hash) + str[i];
    return hash;
}

}//namespace net 
}//namespace app 
