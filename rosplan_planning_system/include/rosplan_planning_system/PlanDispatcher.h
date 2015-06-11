/**
 * This file dispatches a plan. TODO document
 */
#include "ros/ros.h"
#include "rosplan_dispatch_msgs/ActionDispatch.h"
#include "rosplan_dispatch_msgs/ActionFeedback.h"

#ifndef KCL_dispatcher
#define KCL_dispatcher

namespace KCL_rosplan
{

	class PlanDispatcher
	{
	private:

		/* action dispatch list (current plan) */
		size_t current_action;

		/* dispatch state */
		std::map<int,bool> action_received;
		std::map<int,bool> action_completed;
		bool dispatch_paused;

	public:

		/* dispatch modes */
		bool dispatch_on_completion;
		bool dispatch_concurrent;

		int getCurrentAction();
		void reset();

		/* action dispatch methods */
		bool dispatchPlan(const std::vector<rosplan_dispatch_msgs::ActionDispatch> &actionList, double missionStart, double planStart);
		bool replan_requested;

		/* action feedback methods */
		void feedbackCallback(const rosplan_dispatch_msgs::ActionFeedback::ConstPtr& msg);
		void actionFeedback(const rosplan_dispatch_msgs::ActionFeedback::ConstPtr& msg);

		/* ROS interface */
		ros::Publisher action_publisher;
	};
}

#endif
