#include "time.h"
#include <poll.h>
#include <iostream>
#include <vector>
#include <fstream>
#include <filesystem>
#include <abmt/os.h>
#include <abmt/time.h>
#include <abmt/json.h>
#include <abmt/util/bt_exception.h>
#include <abmt/util/str_utils.h>
#include <pqxx/pqxx>
#include <mosquitto.h>

namespace fs = std::filesystem;

using namespace std;

void abmt::log(std::string s){
	std::cout << s << std::endl;
}

void abmt::die(std::string s){
	throw abmt::util::bt_exception(s);
}

void abmt::die_if(bool condition, std::string msg){
	if(condition){
		abmt::die(msg);
	}
}

abmt::time abmt::time::now(){
        timespec t;
        clock_gettime(CLOCK_REALTIME, &t);
        int64_t res;
        res = t.tv_sec * 1000LL*1000*1000 + t.tv_nsec; // 1000LL very important for 32bit systems
        return time(res);
}

string read_file(std::string filename) {
	abmt::die_if(fs::exists(filename) == false, "Unable to open file: " + filename);
	ifstream f(filename.c_str());
	stringstream ss;
	ss << f.rdbuf();
	f.close();
	return ss.str();
}

static void mqtt_sub_on_connect(struct mosquitto *mosq, void *obj, int reason_code);
static void mqtt_sub_on_disconnect(struct mosquitto *mosq, void *obj, int reason_code);
static void mqtt_sub_on_message(struct mosquitto *mosq, void *obj, const struct mosquitto_message *msg);

struct mqtt2sql{
	abmt::json config = abmt::json::object();
	bool mqtt_connected;

	mosquitto* mosq;
	pollfd fd;

	shared_ptr<pqxx::connection> cx;
	shared_ptr<pqxx::nontransaction> tx;

	mqtt2sql(abmt::json c): config(c){
		cx = make_shared<pqxx::connection>(config["db"]["uri"].str());
		tx = make_shared<pqxx::nontransaction>(*cx);

		mosq = mosquitto_new(NULL, true, this);
		abmt::die_if(mosq == NULL, "Error creating mosquitto instance!");
		mosquitto_connect_callback_set(mosq, mqtt_sub_on_connect);
		mosquitto_disconnect_callback_set(mosq, mqtt_sub_on_disconnect);
		mosquitto_message_callback_set(mosq, mqtt_sub_on_message);
		
		int rc = mosquitto_connect(mosq, config["mqtt"]["host"].str().c_str(), config["mqtt"]["port"], 60);
		abmt::die_if(rc != MOSQ_ERR_SUCCESS, "Error connecting to mmqt server");
		fd.fd = mosquitto_socket(mosq);
		fd.events = POLLIN;

		cx->prepare( "insert", 
		    "INSERT INTO "+ config["db"]["table"].str() + " (topic, date, data) "
			"VALUES ($1::character varying, $3::timestamp without time zone, $2::"+config["db"]["data_type"].str()+");"
		);
	}

	void insert(string topic, string value){
		// (Normally you'd check for valid command-line arguments.)
		try{
			tx->exec_prepared("insert", topic, value, abmt::util::str_replace_date("#Y-#M-#D #h:#m:#s.s"));
		}catch(pqxx::data_exception e){
			cout << "error parsing data for topic " << topic << endl;
		}
	}

	void poll(){
		::poll(&fd, 1, 2000);
		mosquitto_loop_read(mosq,1);
		mosquitto_loop_write(mosq, 1);
		mosquitto_loop_misc(mosq);
	}

	~mqtt2sql(){
		//todo: destructor is called after main-return when the lib is already closed
		mosquitto_destroy(mosq);
	}
};

static void mqtt_sub_on_connect(struct mosquitto *mosq, void *obj, int reason_code){
    mqtt2sql* m2s = (mqtt2sql*)obj;
    m2s->mqtt_connected = true;
    if(reason_code != 0){
		abmt::log("error connecting to mqtt server");
		mosquitto_disconnect(mosq);
		return;
	}

	int rc;
	for(auto t: m2s->config["topics"]){
		rc = mosquitto_subscribe(mosq, NULL, t.value.str().c_str(), 1);
    	abmt::die_if(mosq == NULL, "Error subscribe to msg");
	}
}

static void mqtt_sub_on_disconnect(struct mosquitto *mosq, void *obj, int reason_code){
    mqtt2sql* m2s = (mqtt2sql*)obj;
    m2s->mqtt_connected = false;
    abmt::die("mqtt disconnect reason: " + to_string(reason_code));
}

static void mqtt_sub_on_message(struct mosquitto *mosq, void *obj, const struct mosquitto_message *msg){
    mqtt2sql* m2s = (mqtt2sql*)obj;
	m2s->insert(msg->topic, {(char *)msg->payload, (size_t) msg->payloadlen});
}

int main(int argc, char* argv[]){
	if(argc != 2){
		cout << "usage: mqtt2sql <config.json>" << endl;
		return 1;
	}
	mosquitto_lib_init();

	mqtt2sql m2s(abmt::json::parse(read_file(argv[1])));

	while(true){
		m2s.poll();
	}

	mosquitto_lib_cleanup();
	
    return 0;
}