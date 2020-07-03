#include "CKFKConsumeCallback.h"

namespace app {
namespace net {

CKFKConsumeCallback::CKFKConsumeCallback() {
}

void CKFKConsumeCallback::consume_cb(RdKafka::Message& message, void* opaque) {
    switch (message.err()) {
    case RdKafka::ERR__TIMED_OUT:
        break;

    case RdKafka::ERR_NO_ERROR:
        /* got message */
        break;

    case RdKafka::ERR__PARTITION_EOF:
        /* Last message */
        break;

    case RdKafka::ERR__UNKNOWN_TOPIC:
    case RdKafka::ERR__UNKNOWN_PARTITION:
    {
        break;
    }

    default:
    {
        break;
    }
    }//switch
}


}//namespace net 
}//namespace app 