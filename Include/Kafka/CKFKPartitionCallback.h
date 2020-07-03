#pragma once
#include <string>
#include <iostream>
#include "rdkafka.h"    
#include "rdkafkacpp.h"
#include "HConfig.h"

namespace app {
namespace net {


class CKFKPartitionCallback : public RdKafka::PartitionerCb {
public:
    virtual ~CKFKPartitionCallback() {
    }


    virtual s32 partitioner_cb(const RdKafka::Topic *topic,
        const std::string* key,
        s32 partition_cnt, 
        void* msg_opaque);

private:
    u32 djb_hash(const s8* str, size_t len);
};


}//namespace net 
}//namespace app 
