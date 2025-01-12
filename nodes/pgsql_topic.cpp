#include "pgsql_topic.h"

#include <abmt/os.h>
#include <abmt/util/str_utils.h>

using namespace std;
using namespace mqtt;

void pgsql_topic::init(){
    cx = make_shared<pqxx::connection>(param_sql_uri);
	tx = make_shared<pqxx::nontransaction>(*cx);

    /*cx->prepare("query", 
	    "SELECT d.data FROM json_data d LEFT JOIN topic t ON t.id = d.topic "
	    "WHERE t.topic = '" + param_topic + "' AND d.date < $1 " 
	    "ORDER BY d.date DESC LIMIT 1;"
	);
	*/
	cx->prepare("query", 
	    "SELECT d.data FROM json_data d WHERE "
	    "d.topic = $1 "
	    "AND d.date < $2 " 
	    "ORDER BY d.date DESC LIMIT 1;"
	);
	auto res = tx->exec_params("SELECT t.id FROM topic t WHERE t.topic = $1", param_topic);
	abmt::die_if(res.empty(), "topic "+ param_topic + " not found");
    topic_id = res[0][0].as<int>();
}

void pgsql_topic::tick(){
    auto date = abmt::time::now();
    date -= abmt::time::ms(param_offset_sec*1000);
    auto res =  tx->exec_prepared("query", topic_id, abmt::util::str_replace_date("#Y-#M-#D #h:#m:#s.s", date));
    if(res.empty()){
        out = param_default;
    }else{
        out = abmt::json::parse(res[0][0].c_str());
    }
}

void pgsql_topic::final(){
    
}


