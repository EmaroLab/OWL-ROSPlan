#include <string>
#include <ros/ros.h>
#include "armor_msgs/ArmorDirective.h"
#include "armor_msgs/ArmorDirectiveRequest.h"

#ifndef ROSPLAN_KNOWLEDGE_BASE_ARMORMANAGER_H
#define ROSPLAN_KNOWLEDGE_BASE_ARMORMANAGER_H

namespace KCL_rosplan {

    class ArmorManager {

    private:
        std::string refName;
        ros::NodeHandlePtr nh;
        ros::ServiceClient armorClient;
        armor_msgs::ArmorDirectiveRequest msgTemplate;

        void setMsgDirective(armor_msgs::ArmorDirectiveRequest& msg, std::string command,
                             std::string primarySpec, std::string secondarySpec);

    public:
        ArmorManager(std::string refName, ros::NodeHandlePtr nh);
        bool pollDomainOntology();
        std::string getRefName();

    };

}

#endif //ROSPLAN_KNOWLEDGE_BASE_ARMORMANAGER_H
