#include <conio.h>
#include <iostream>
#include <fstream>
#include "Json/json.h"
#include "CLogger.h"
#include "CTimer.h"
#include "CKFKProducter.h"
#include "CKFKConsummer.h"
#include "CTimeManager.h"

namespace app {

int AppWriteJson() {
    Json::Value root;
    Json::StreamWriterBuilder builder;
    const std::unique_ptr<Json::StreamWriter> writer(builder.newStreamWriter());

    root["Name"] = "robin";
    root["Age"] = 20;
    root["Param"].append("arr");	// 新建一个key为array，类型为数组，对第一个元素赋值为字符串“arr”
    root["Param"].append(123);		// 为数组 key_array 赋值，对第二个元素赋值为：1234
    root["Node1"]["Min"] = 1;
    root["Node1"]["Min"].setComment("///<min val", Json::commentAfterOnSameLine);
    root["Node1"]["Max"] = 20;
    root["Node1"]["Current"] = 12;
    root["Node1"]["Comment"] = "cnt";
    root["Node2"]["Min"] = 1;
    root["Node2"]["Max"] = 20;
    root["Node2"]["Current"] = 12;
    root["Node2"]["Comment"] = "cnter";
    root["Node2"]["metadata.broker.list"] = "10.1.63.126:9092,10.1.63.127:9092,10.1.63.128:9092";

    writer->write(root, &std::cout);

    std::ofstream ofs("writed.json");
    ofs << root;
    ofs.close();

    return EXIT_SUCCESS;
}


void AppStartKafkaSend() {
    s64 lastime = 0;
    s64 usedtime = 0;
    core::CTimeManager tmanager(5);
    Json::Value root;
    std::ifstream ifs;
    ifs.open("Config/config.json");
    Json::CharReaderBuilder builder;
    builder["collectComments"] = true;
    JSONCPP_STRING errs;
    if (!parseFromStream(builder, ifs, &root, &errs)) {
        std::cout << errs << std::endl;
        return;
    }
    //show all
    //std::cout << root << std::endl;

    net::CKFKProducter producter;
    net::CKFKConfig& cfg = producter.getConfig();
    net::CKFKConfig& cfgtopic = producter.getTopicConfig();

    Json::Value& kbase = root["kafka_base"];
    std::vector<Json::String> pat = kbase.getMemberNames();
    for (s32 i = pat.size() - 1; i >= 0; --i) {
        cfg.set(pat[i].c_str(), kbase[pat[i]].asCString());
        printf("%s---%s\n", pat[i].c_str(), kbase[pat[i]].asCString());
    }

    Json::Value& kprod = root["kafka_produce"];
    pat = kprod.getMemberNames();
    for (s32 i = pat.size() - 1; i >= 0; --i) {
        cfg.set(pat[i].c_str(), kprod[pat[i]].asCString());
        printf("%s---%s\n", pat[i].c_str(), kprod[pat[i]].asCString());
    }

    Json::Value& ktopc = root["kafka_topic"];
    pat = ktopc.getMemberNames();
    for (s32 i = pat.size() - 1; i >= 0; --i) {
        cfgtopic.set(pat[i].c_str(), ktopc[pat[i]].asCString());
        printf("%s---%s\n", pat[i].c_str(), ktopc[pat[i]].asCString());
    }


    s32 loglv = root["log_level"].asInt();
    CLogger::getInstance().setLogLevel((ELogLevel)loglv);

    tmanager.start();
    tmanager.getTimeWheel().add(producter.getTimeNode(), 1000, -1);

    std::string kfktopic(root["topic_send"].asCString());
    if (net::EKFK_SUCCESS != producter.init(kfktopic.c_str())) {
        printf("AppStartKafkaSend>> @producer init failed\n");
        return;
    }
    s32 allsent = 0;
    s32 tmp;

APP_GOTO_AUTO:
    printf("AppStartKafkaSend>> @start auto send style, how many msg should be sent out ?\n");
    scanf("%d", &tmp);
    s8 msgbuffer[1024];
    ::strcpy(msgbuffer, "auto style: counter's buffer.");

    lastime = CTimer::getTime();
    for (s32 i = 0; i < tmp; ) {
        if (net::EKFK_SUCCESS == producter.sendBuffer(msgbuffer, 512)) {
            ++i;
            ++allsent;
        } else {
            CThread::sleep(5);
            usedtime = (CTimer::getTime() - lastime) / 1000;
            usedtime = (usedtime > 0 ? usedtime : 1);
            printf("AppStartKafkaSend>> @current sending packet[512] = %u, %u, speed = %.1f/s\n",
                1 + i, allsent, (f64)allsent / usedtime);
        }
    }
    usedtime = (CTimer::getTime() - lastime) / 1000;
    usedtime = (usedtime > 0 ? usedtime : 1);
    printf("AppStartKafkaSend>> @auto send sytle finished = %u, speed = %.1f/s\n",
        allsent, (f64)allsent / usedtime);
    printf("AppStartKafkaSend>> @manual style: input [*] to quit\n");
    printf("AppStartKafkaSend>> @manual style: input [@] to auto send style\n");
    printf("AppStartKafkaSend>> @manual style: input your msg to send\n");
    printf("AppStartKafkaSend>> @manual style: input [enter] to flush and show result of auto send style\n");
    printf("AppStartKafkaSend>> @start manual send style, pls go on.\n\n");


    while (::fgets(msgbuffer, sizeof(msgbuffer), stdin)) {
        if ('*' == *msgbuffer) {//exit
            break;
        }
        if ('@' == *msgbuffer) {//change style
            goto APP_GOTO_AUTO;
        }
        size_t len = ::strlen(msgbuffer);
        if (1 == len) {
            printf("AppStartKafkaSend>> @auto send result = %u\n", allsent);
            continue;
        }
        if (msgbuffer[len - 1] == '\n') {
            msgbuffer[len - 1] = '\0';
        }
        if (net::EKFK_SUCCESS != producter.sendBuffer(msgbuffer, len)) {
            printf("AppStartKafkaSend>> @push data failed %s\n", msgbuffer);
        } else {
            allsent++;
        }
    }//while

    tmanager.getTimeWheel().remove(producter.getTimeNode());
    tmanager.stop();
    s32 ret = producter.flush(1000);
    CLogger::logInfo("AppStartKafkaSend", "flush = %s",
        net::EKFK_SUCCESS == ret ? "success" : "failed");
}


void AppStartKafkaReceive() {
    s64 lastime = 0;
    s64 usedtime = 0;
    core::CTimeManager tmanager(5);
    Json::Value root;
    std::ifstream ifs;
    ifs.open("Config/config.json");
    Json::CharReaderBuilder builder;
    builder["collectComments"] = true;
    JSONCPP_STRING errs;
    if (!parseFromStream(builder, ifs, &root, &errs)) {
        std::cout << errs << std::endl;
        return;
    }
    //show all
    //std::cout << root << std::endl;

    net::CKFKConsummer consummer;
    net::CKFKConfig& cfg = consummer.getConfig();
    net::CKFKConfig& cfgtopic = consummer.getTopicConfig();

    Json::Value& kbase = root["kafka_base"];
    std::vector<Json::String> pat = kbase.getMemberNames();
    for (s32 i = pat.size() - 1; i >= 0; --i) {
        cfg.set(pat[i].c_str(), kbase[pat[i]].asCString());
        printf("%s---%s\n", pat[i].c_str(), kbase[pat[i]].asCString());
    }

    Json::Value& kprod = root["kafka_consume"];
    pat = kprod.getMemberNames();
    for (s32 i = pat.size() - 1; i >= 0; --i) {
        cfg.set(pat[i].c_str(), kprod[pat[i]].asCString());
        printf("%s---%s\n", pat[i].c_str(), kprod[pat[i]].asCString());
    }

    Json::Value& ktopc = root["kafka_topic"];
    pat = ktopc.getMemberNames();
    for (s32 i = pat.size() - 1; i >= 0; --i) {
        cfgtopic.set(pat[i].c_str(), ktopc[pat[i]].asCString());
        printf("%s---%s\n", pat[i].c_str(), ktopc[pat[i]].asCString());
    }


    s32 loglv = root["log_level"].asInt();
    CLogger::getInstance().setLogLevel((ELogLevel)loglv);

    tmanager.start();
    //tmanager.getTimeWheel().add(STimeNode, 1000, -1);

    std::string kfktopic(root["topic_receive"].asCString());
    std::string kgroup(root["topic_group"].asCString());
    if (!consummer.init(kfktopic.c_str())) {
        printf("AppStartKafkaReceive>> @init failed\n");
        return;
    }

    lastime = CTimer::getTime();
    bool red = true;
    s32 cnt = 0;
    s32 timeout = 0;

    while ((!_kbhit() || _getch() != 27)) {
        RdKafka::Message* msg = consummer.read(timeout);
        red = msg && RdKafka::ERR_NO_ERROR == msg->err();
        if (red) {
            printf("read[%d]=%.*s\n", ++cnt, (s32)msg->len(), (s8*)msg->payload());
            timeout = 0;
        } else {
            timeout = 1000;
            //CThread::sleep(1000);
            printf("none msg readed.\n");
        }
        delete msg;
    }//while

    usedtime = (CTimer::getTime() - lastime) / 1000;
    usedtime = (usedtime != 0 ? usedtime : 1);
    printf("AppStartKafkaReceive>> time=%llds, exit...\n", usedtime);


    //tmanager.getTimeWheel().remove(STimeNode);
    tmanager.stop();

    CLogger::logInfo("AppStartKafkaReceive", "done");
}


}//namespace app



int main(int argc, char** argv) {
    //app::s8 a = 127;
    //app::s8 b = 127; 
    //app::s8 t = 126;
    ////a++;
    //a += 123;
    //b += 132;
    //app::s32 max = 10;
    //t = b - a;
    //printf("curr=%d, last=%d, b-a=%d\n", b, a, t);
    //if (t < max) {
    //    printf("sleep=%d\n", max - t);
    //}
    //return 0;


    app::CLogger::getInstance().addReceiver(app::CLogger::ELRT_FILE_HTML);
    //app::AppWriteJson();

    //app::AppStartKafkaSend();
    app::AppStartKafkaReceive();

    app::s32 ret = app::net::AppKfkWaitExit(1000);
    app::CLogger::logInfo("main", "AppKfkWaitExit=%d", ret);

    app::CLogger::clear();
    printf("main>> exited\n");
    return 0;
}