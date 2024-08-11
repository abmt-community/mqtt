#ifndef MQTT_MQTT_H
#define MQTT_MQTT_H MQTT_MQTT_H

#include <poll.h>
#include <abmt/json.h>
#include <abmt/time.h>
#include <mosquitto.h>

//@link: mosquitto

namespace mqtt{

//@node: auto
//@raster: source
struct mqtt_sub{
    abmt::json out_data;
    
    std::string param_host = "mqtt.local";
    int param_port = 1883;
    std::string param_topic = "test/topic";
    bool param_parse_json = false;
    
    mosquitto* mosq;
    bool connected = false;
    std::string msg;
    bool rcv_msg = false;
    pollfd fd;
    
    void init();
    abmt::time poll();
    void final();
};


//@node: auto
//@raster: auto
struct mqtt_pub{
    
    abmt::json in_data;
    
    std::string param_host = "mqtt.local";
    int param_port = 1883;
    std::string param_topic = "test/topic";
    bool param_retain = true;
    int param_qos = 0;
    bool param_pulish_only_on_change = true;
    
    mosquitto* mosq;
    bool connected = false;
    abmt::json last_data;
    
    void init();
    void tick();
    void final();
};



} // namespace mqtt

#endif // MQTT_MQTT_H
