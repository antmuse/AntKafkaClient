#include "CKFKEventCallback.h"
#include <string>
#include <iostream>
#include "CLogger.h"

namespace app {
namespace net {


void CKFKEventCallback::event_cb(RdKafka::Event& event) {
    switch (event.type()) {
    case RdKafka::Event::EVENT_ERROR:
        CLogger::log(ELOG_ERROR, "CKFKEventCallback::event_cb", "ERROR = %s,%s", RdKafka::err2str(event.err()).c_str(), event.str().c_str());
        if (event.err() == RdKafka::ERR__ALL_BROKERS_DOWN) {
            mRunning = false;
        }
        break;

    case RdKafka::Event::EVENT_STATS:
        CLogger::log(ELOG_INFO, "CKFKEventCallback::event_cb", "STATS = %s", event.str().c_str());
        break;

    case RdKafka::Event::EVENT_LOG:
        CLogger::log(ELOG_INFO, "CKFKEventCallback::event_cb", "LOG-%d-%s: %s", event.severity(), event.fac().c_str(), event.str().c_str());
        break;

    default:
        CLogger::log(ELOG_INFO, "CKFKEventCallback::event_cb", "ERROR = %d,%s,%s", event.type(), RdKafka::err2str(event.err()).c_str(), event.str().c_str());
        break;
    }
}


}//namespace net 
}//namespace app 
