#include <ros/ros.h>
#include <vector>
#include <string>
#include <iostream>
#include <fstream>
#include <boost/foreach.hpp>

#include "rosplan_action_interface/RPActionInterface.h"

#include "actionlib/client/simple_action_client.h"
#include "move_base_msgs/MoveBaseAction.h"
#include "mongodb_store/message_store.h"
#include "geometry_msgs/PoseStamped.h"
#include "std_srvs/Empty.h"

#ifndef KCL_movebase
#define KCL_movebase

/**
 * Executes actions planned by ROSPlan to manipulate an articulated object.
 */

namespace KCL_rosplan {

    class PacoDecreaseAngle: public RPActionInterface
    {

    private:

        ros::ServiceClient clear_costmaps_client;
        actionlib::SimpleActionClient<move_base_msgs::MoveBaseAction> action_client;

    public:

        /* constructor */
        PacoDecreaseAngle(ros::NodeHandle &nh, std::string &actionserver); //, std::string &actionserver

        /* listen to and process action_dispatch topic */
        bool concreteCallback(const rosplan_dispatch_msgs::ActionDispatch::ConstPtr& msg);
    };
}
#endif