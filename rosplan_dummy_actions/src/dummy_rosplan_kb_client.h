//
// Created by alessio on 28/06/17.
//

#ifndef PACO_ACTIONS_ROSPLAN_KB_CLIENT_H
#define PACO_ACTIONS_ROSPLAN_KB_CLIENT_H


#include <ros/ros.h>
#include <ros/service_client.h>
#include <rosplan_knowledge_msgs/KnowledgeUpdateService.h>

/**
* A class to manipulate axioms in ROSPlan Knowledge Base
*/
class rosplan_kb_client {

    ros::NodeHandle node_handle;
    ros::ServiceClient client;
    rosplan_knowledge_msgs::KnowledgeUpdateServiceRequest req;
    rosplan_knowledge_msgs::KnowledgeUpdateServiceResponse res;
    diagnostic_msgs::KeyValue item;

    void clear_cache();

public:
    rosplan_kb_client();
    rosplan_kb_client(std::string client_name);

    bool add_instance(std::string name, std::string type);
    bool add_fact(std::string predicate, std::vector<std::string> args);
    bool add_goal(std::string predicate, std::vector<std::string> args);

    bool remove_instance(std::string name);
    bool remove_fact(std::string predicate, std::vector<std::string> args);
    bool remove_goal(std::string predicate, std::vector<std::string> args);

    bool update_instance_fact(std::string name, std::string predicate, std::vector<std::string> args);
    bool update_instance_fact(std::string name, std::string predicate,
                              std::vector<std::string> args, std::vector<std::string> old_args);
};


#endif //PACO_ACTIONS_ROSPLAN_KB_CLIENT_H
