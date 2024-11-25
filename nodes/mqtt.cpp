#include "mqtt.h"

#include <abmt/os.h>
#include <abmt/mutex.h>


using namespace std;
using namespace mqtt;

static abmt::mutex mqtt_lock;
static bool mqtt_init = false;

static void mqtt_handle_events(struct mosquitto *mosq){
    auto lock = mqtt_lock.get_scope_lock();
    mosquitto_loop_read(mosq,1);
    mosquitto_loop_write(mosq, 1);
    mosquitto_loop_misc(mosq);
}

static void mqtt_sub_on_connect(struct mosquitto *mosq, void *obj, int reason_code){
    mqtt_sub* node = (mqtt_sub*)obj;
    node->connected = true;
    if(reason_code != 0){
		abmt::log("error connecting to mqtt server");
		mosquitto_disconnect(mosq);
		return;
	}else{
	    abmt::log("connected to mqtt server");
	}
	
    int rc = mosquitto_subscribe(mosq, NULL, node->param_topic.c_str(), 1);
    abmt::die_if(mosq == NULL, "Error subscribe to msg");
}

static void mqtt_sub_on_disconnect(struct mosquitto *mosq, void *obj, int reason_code){
    mqtt_sub* node = (mqtt_sub*)obj;
    node->connected = false;
    abmt::log("mqtt disconnect reason: " + to_string(reason_code));
}

static void mqtt_sub_on_message(struct mosquitto *mosq, void *obj, const struct mosquitto_message *msg){
    mqtt_sub* node = (mqtt_sub*)obj;
	node->rcv_msg = true;
	node->msg = std::string((char *)msg->payload, msg->payloadlen);
}


void mqtt_sub::init(){
    auto lock = mqtt_lock.get_scope_lock();
    if(mqtt_init == false){
        mosquitto_lib_init();
    }
    mosq = mosquitto_new(NULL, true, this);
    
    abmt::die_if(mosq == NULL, "Error creating mosquitto instance!");
    mosquitto_connect_callback_set(mosq, mqtt_sub_on_connect);
    mosquitto_disconnect_callback_set(mosq, mqtt_sub_on_disconnect);
    mosquitto_message_callback_set(mosq, mqtt_sub_on_message);
    
    int rc = mosquitto_connect(mosq, param_host.c_str(), param_port, 60);
    abmt::die_if(rc != MOSQ_ERR_SUCCESS, "Error connecting to mmqt server");
    fd.fd = mosquitto_socket(mosq);
    fd.events = POLLIN;
}

abmt::time mqtt_sub::poll(){
    ::poll(&fd, 1, 2000);
    mqtt_handle_events(mosq);
    if(rcv_msg){
        rcv_msg = false;
        if(param_parse_json){
            out_data = abmt::json::parse(msg);
        }else{
            out_data = msg;
        }
        return 0;
    }else{
        return 1;
    }
}

void mqtt_sub::final(){
    auto lock = mqtt_lock.get_scope_lock();
    mosquitto_destroy(mosq);
    if(mqtt_init){
        mosquitto_lib_cleanup();
        mqtt_init = false;
    }
}


static void mqtt_pub_on_connect(struct mosquitto *mosq, void *obj, int reason_code){
    mqtt_pub_topic* node = (mqtt_pub_topic*)obj;
    node->connected = true;
    if(reason_code != 0){
		abmt::log("error connecting to mqtt server");
		mosquitto_disconnect(mosq);
	}else{
	    abmt::log("connected to mqtt server");
	}
}

static void mqtt_pub_on_disconnect(struct mosquitto *mosq, void *obj, int reason_code){
    mqtt_pub_topic* node = (mqtt_pub_topic*)obj;
    node->connected = false;
}

void mqtt_pub_topic::init(){
    auto lock = mqtt_lock.get_scope_lock();
    if(mqtt_init == false){
        mosquitto_lib_init();
    }
    mosq = mosquitto_new(NULL, true, this);

    abmt::die_if(mosq == NULL, "Error creating mosquitto instance!");
    mosquitto_connect_callback_set(mosq, mqtt_pub_on_connect);
	mosquitto_disconnect_callback_set(mosq, mqtt_pub_on_disconnect);
	
	int rc = mosquitto_connect(mosq, param_host.c_str(), param_port, 60);
	abmt::die_if(rc != MOSQ_ERR_SUCCESS, "Error connecting to mmqt server");
	abmt::die_if(rc != MOSQ_ERR_SUCCESS, "Error starting loop");
	mosquitto_loop(mosq, 10000, 1);
}

void mqtt_pub_topic::tick(){
    mqtt_handle_events(mosq);
    if(connected){
        if(param_pulish_only_on_change && last_data == in_data && last_topic == in_topic){
            return;
        }
        last_data = in_data;
        last_topic = in_topic;
        auto payload = in_data.str();
        mqtt_lock.lock();
        int rc = mosquitto_publish(mosq, NULL, in_topic.c_str(), payload.size(), payload.c_str(), param_qos, param_retain);
	    mqtt_lock.unlock();
	    if(rc != MOSQ_ERR_SUCCESS){
	        abmt::log("error pubishing data");
	    }
	    mqtt_handle_events(mosq);
    }
}

void mqtt_pub_topic::final(){
    auto lock = mqtt_lock.get_scope_lock();
    mosquitto_destroy(mosq);
    if(mqtt_init){
        mosquitto_lib_cleanup();
        mqtt_init = false;
    }
}

void mqtt_pub::init(){
    publisher.param_host = param_host;
    publisher.param_port = param_port;
    publisher.param_retain = param_retain;
    publisher.param_qos = param_qos;
    publisher.param_pulish_only_on_change = param_pulish_only_on_change;
    
    publisher.in_topic = param_topic;
    publisher.in_data = in_data;
    publisher.init();
}

void mqtt_pub::tick(){
    publisher.in_topic = param_topic;
    publisher.in_data = in_data;
    publisher.tick();
    
}

void mqtt_pub::final(){
    publisher.final();
}

