#include "paco_actions/dummy_increase_angle_node.h"
#include "dummy_rosplan_kb_client.h"

/* The implementation of RPMoveBase.h */
namespace KCL_rosplan {
    bool i = true;
    rosplan_kb_client* kb_client_ptr = NULL;

    /* constructor */
    PacoIncreaseAngle::PacoIncreaseAngle(ros::NodeHandle &nh, std::string &actionserver)
            : action_client(actionserver, true) {
        params.name = "increase_angle"; //TODO get rid of hardcode, use params
        // costmap client
        clear_costmaps_client = nh.serviceClient<std_srvs::Empty>("/move_base/clear_costmaps");
    }

    /* action dispatch callback */
    bool PacoIncreaseAngle::concreteCallback(const rosplan_dispatch_msgs::ActionDispatch::ConstPtr& msg) {
        ROS_ERROR("Executing action %s, params: %s %s %s", msg->name.c_str(), msg->parameters[0].value.c_str(),
                  msg->parameters[1].value.c_str(), msg->parameters[2].value.c_str());

        ros::Duration d(2.5);
        d.sleep();

        //update knowledge base

        kb_client_ptr->remove_fact("has-angle", {msg->parameters[0].value, ""});
        kb_client_ptr->add_fact("has-angle", {msg->parameters[0].value, msg->parameters[2].value});

        // update following links status (simulate conditional)

        if (i) {
            kb_client_ptr->remove_fact("has-angle", {"link2", ""});
            kb_client_ptr->add_fact("has-angle", {"link2", "angle90"});
            kb_client_ptr->remove_fact("has-angle", {"link3", ""});
            kb_client_ptr->add_fact("has-angle", {"link3", "angle90"});
            i = false;
        }
        return true;
    }
} // close namespace

/*-------------*/
/* Main method */
/*-------------*/

int main(int argc, char **argv) {

    ros::init(argc, argv, "paco_increase_angle");
    ros::NodeHandle nh("~");

    std::string actionserver;
    nh.param("action_server", actionserver, std::string("/increase_angle"));

    //initialize rosplan_kb_client
    rosplan_kb_client kb_client = rosplan_kb_client();
    KCL_rosplan::kb_client_ptr = &kb_client;

    // create PDDL action subscriber
    KCL_rosplan::PacoIncreaseAngle increaseAngle(nh, actionserver);

    // listen for action dispatch
    ros::Subscriber ds = nh.subscribe("/kcl_rosplan/action_dispatch", 1000,
                                      &KCL_rosplan::RPActionInterface::dispatchCallback,
                                      dynamic_cast<KCL_rosplan::RPActionInterface*>(&increaseAngle));
    increaseAngle.runActionInterface();

    return 0;
}