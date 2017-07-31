//
// Created by alessio on 28/06/17.
//

#include "dummy_rosplan_kb_client.h"

rosplan_kb_client::rosplan_kb_client(){
    node_handle = ros::NodeHandle("~");
    ros::service::waitForService("/kcl_rosplan/update_knowledge_base");
    client = node_handle.serviceClient<rosplan_knowledge_msgs::KnowledgeUpdateService>("/kcl_rosplan/update_knowledge_base");
    req = rosplan_knowledge_msgs::KnowledgeUpdateServiceRequest();
    res = rosplan_knowledge_msgs::KnowledgeUpdateServiceResponse();
    item = diagnostic_msgs::KeyValue();
}

rosplan_kb_client::rosplan_kb_client(std::string client_name){
    node_handle = ros::NodeHandle("~");
    ros::service::waitForService(client_name);
    client = node_handle.serviceClient<rosplan_knowledge_msgs::KnowledgeUpdateService>(client_name);
    req = rosplan_knowledge_msgs::KnowledgeUpdateServiceRequest();
    res = rosplan_knowledge_msgs::KnowledgeUpdateServiceResponse();
    item = diagnostic_msgs::KeyValue();
}

void rosplan_kb_client::clear_cache() {
    req = rosplan_knowledge_msgs::KnowledgeUpdateServiceRequest();
    res = rosplan_knowledge_msgs::KnowledgeUpdateServiceResponse();
    item = diagnostic_msgs::KeyValue();
    return;
}

bool rosplan_kb_client::add_instance(std::string name, std::string type) {
    // TODO
    return false;
}

/**
     * Adds a facts belonging to specific predicate type
     * @param predicate predicate type
     * @param args ordered set of arguments
*/
bool rosplan_kb_client::add_fact(std::string predicate, std::vector<std::string> args) {
    this->clear_cache();
    req.update_type = rosplan_knowledge_msgs::KnowledgeUpdateServiceRequest::ADD_KNOWLEDGE;
    req.knowledge.knowledge_type = req.knowledge.FACT;
    req.knowledge.attribute_name = predicate;

    for (int arg = 0; arg < args.size(); arg++) {
        item.key = "has_arg_" + std::to_string(arg + 1);
        item.value = args[arg];
        req.knowledge.values.push_back(item);
    }

    client.call(req, res);
    return res.success;
}

bool rosplan_kb_client::add_goal(std::string predicate, std::vector<std::string> args) {
    // TODO
    return false;
}

bool rosplan_kb_client::remove_instance(std::string name) {
    // TODO
    return false;
}

/**
     * Removes a fact
     * @param predicate predicate type
     * @param args ordered set of arguments, "" acts as "all matches"
*/
bool rosplan_kb_client::remove_fact(std::string predicate, std::vector<std::string> args) {
    this->clear_cache();

    req.update_type = rosplan_knowledge_msgs::KnowledgeUpdateServiceRequest::REMOVE_KNOWLEDGE;
    req.knowledge.knowledge_type = req.knowledge.FACT;
    req.knowledge.attribute_name = predicate;

    for (int arg = 0; arg < args.size(); arg++) {
        item.key = "has_arg_" + std::to_string(arg + 1);
        item.value = args[arg];
        req.knowledge.values.push_back(item);
    }

    client.call(req, res);
    return res.success;
}

bool rosplan_kb_client::remove_goal(std::string predicate, std::vector<std::string> args) {
    // TODO
    return false;
}

bool rosplan_kb_client::update_instance_fact(std::string name, std::string predicate, std::vector<std::string> args) {
    // TODO
    return false;
}

bool rosplan_kb_client::update_instance_fact(std::string name, std::string predicate, std::vector<std::string> args,
                                             std::vector<std::string> old_args) {
    return false;
}
