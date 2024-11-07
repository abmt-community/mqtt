#ifndef MQTT_PGSQL_TOPIC_H
#define MQTT_PGSQL_TOPIC_H MQTT_PGSQL_TOPIC_H

#include <memory>
#include <string>
#include <pqxx/pqxx>
#include <abmt/json.h>

namespace mqtt{

//@link: pqxx

//@node: auto
//@raster: auto
struct pgsql_topic{
    
    abmt::json out;
    
    std::string param_sql_uri = "postgresql://mqtt_user:mqtt_passwd@localhost/mqtt_db";
    std::string param_topic = "test_topic";
    double param_offset_sec = 0.0;
    abmt::json param_default = {};
    
    std::shared_ptr<pqxx::connection> cx;
	std::shared_ptr<pqxx::nontransaction> tx;
    
    void init();
    void tick();
    void final();
};




} // namespace mqtt

#endif // MQTT_PGSQL_TOPIC_H
