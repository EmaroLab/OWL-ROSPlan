#include <string>
#include <ros/ros.h>
#include "rosplan_knowledge_base/ArmorManager.h"
#include "armor_msgs/ArmorDirective.h"

namespace KCL_rosplan{

    ArmorManager::ArmorManager(std::string refName, ros::NodeHandlePtr nh) {
        this->refName = refName;
        this->nh = nh;

        this->armorClient = nh->serviceClient<armor_msgs::ArmorDirective>("armor_interface_srv", false);

        this->msgTemplate.client_name = "KCL_rosplan";
        this->msgTemplate.reference_name = refName;

    }

    bool ArmorManager::pollDomainOntology() {
        setMsgDirective(msgTemplate, "GET", "ALL", "REFS");
        armor_msgs::ArmorDirectiveResponse res;
        if(armorClient.call(msgTemplate, res)){
            if (res.success) {
                return std::find(res.queried_objects.begin(), res.queried_objects.end(), refName)
                       != res.queried_objects.end();
            }
        }

    }

    void ArmorManager::setMsgDirective(armor_msgs::ArmorDirectiveRequest& msg, std::string command,
                                       std::string primarySpec, std::string secondarySpec) {
        msg.command = command;
        msg.primary_command_spec = primarySpec;
        msg.secondary_command_spec = secondarySpec;
    }

}