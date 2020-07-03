#include "CKFKDeliverCallback.h"
#include <string>
#include <iostream>
#include "CLogger.h"
#include "HAtomicOperator.h"

namespace app {
namespace net {


void CKFKDeliverCallback::dr_cb(RdKafka::Message& message) {
    const RdKafka::ErrorCode ecode = message.err();

    //CLogger::log(ELOG_ERROR, "CKFKDeliverCallback::dr_cb", "%lld,%s:%s",
    //    cnt, RdKafka::err2str(ecode).c_str(), message.errstr().c_str());

    if (RdKafka::ERR_NO_ERROR == ecode) {
        AppAtomicIncrementFetch(&mSuccessCount);
        /*s32 sentsz = (s32)message.len();
        printf("CKFKDeliverCallback::dr_cb>> success sent byte = %d, key = %s\n",
            sentsz, message.key() ? message.key()->c_str() : "None");*/
    } else {
        s64 cnt = AppAtomicIncrementFetch(&mFailedCount);
        CLogger::log(ELOG_ERROR, "CKFKDeliverCallback::dr_cb", "count=%lld,ecode=%d",
            cnt, ecode);
    }
}


}//namespace net 
}//namespace app 
