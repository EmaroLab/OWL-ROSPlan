#include "rosplan_planning_system/ProbePlanParser.h"

namespace KCL_rosplan {

	void ProbePlanParser::reset() {
		filter_objects.clear();
		filter_attributes.clear();
		knowledge_filter.clear();
		action_list.clear();
	}

	/* constructor */
    ProbePlanParser::ProbePlanParser(ros::NodeHandle &nh) : node_handle(&nh)
	{
		// knowledge interface
		update_knowledge_client = nh.serviceClient<rosplan_knowledge_msgs::KnowledgeUpdateService>("/kcl_rosplan/update_knowledge_base");
	}

	/*----------------------*/
	/* Post processing plan */
	/*----------------------*/

	/**
	 * parses the output of probe, generating a list of action messages.
	 */
	void ProbePlanParser::preparePlan(std::string &dataPath, PlanningEnvironment &environment, size_t freeActionID) {
		// trim the end of any existing plan
		while(action_list.size() > freeActionID)
			action_list.pop_back();

		// probe output
		std::ifstream planfile;
		planfile.open((dataPath + "plan.1").c_str());

		int curr, next;
		std::string line;
		std::vector<rosplan_dispatch_msgs::ActionDispatch> potentialPlan;
		double planDuration;
		double expectedPlanDuration = 0;

		size_t planFreeActionID = freeActionID;

		while(!planfile.eof()) {

			// TODO implement duration

			planDuration = 0;

			getline(planfile, line);
			std::transform(line.begin(), line.end(), line.begin(), ::tolower);

			if (line.length() < 2)
				break;

			rosplan_dispatch_msgs::ActionDispatch msg;

			// action ID
			msg.action_id = planFreeActionID++;

			// dispatchTime
			msg.dispatch_time = 0;

			// check for parameters
			curr = line.find("(") + 1;
			bool paramsExist = (line.find(" ", curr) < line.find(")", curr));

			if (paramsExist) {

				// name
				next = line.find(" ", curr);
				std::string name = line.substr(curr, next - curr).c_str();
				msg.name = name;

				// parameters
				std::vector<std::string> params;
				curr = next + 1;
				next = line.find(")", curr);
				int param_begin = curr;
				while (param_begin < next) {
					int param_end = line.find(" ", curr);
					int line_end = line.find(")", curr);
					curr = (param_end < line_end && param_end != -1) ? param_end : line_end;
					std::string plan_param = line.substr(param_begin, curr - param_begin);
					std::transform(plan_param.begin(), plan_param.end(), plan_param.begin(), ::tolower);
					std::string param = environment.name_map[plan_param];
					params.push_back(param);
					param_begin = ++curr;
				}
				processPDDLParameters(msg, params, environment);

			} else {

				// name
				next = line.find(")", curr);
				std::string name = line.substr(curr, next - curr).c_str();
				msg.name = name;

			}

			// duration
			msg.duration = 0;

			potentialPlan.push_back(msg);

			// update plan duration
			planDuration = msg.duration + 0;

		}

		if(planDuration - expectedPlanDuration < 0.01)  {

			// trim any previously read plan
			while(action_list.size() > freeActionID) {
				action_list.pop_back();
			}

			// save better optimised plan
			for(size_t i = 0; i < potentialPlan.size(); i++) {
				action_list.push_back(potentialPlan[i]);
				generateFilter(environment);
			}

			total_plan_duration = planDuration;

		} else {
			ROS_INFO("Duration: %f, expected %f; plan discarded", planDuration, expectedPlanDuration);
		}

		planfile.close();
	}

	/**
	 * processes the parameters of a single PDDL action into an ActionDispatch message
	 */
	void ProbePlanParser::processPDDLParameters(rosplan_dispatch_msgs::ActionDispatch &msg, std::vector<std::string> &params, PlanningEnvironment &environment) {

		// find the correct PDDL operator definition
		std::map<std::string,std::vector<std::string> >::iterator ait;
		ait = environment.domain_operators.find(msg.name);
		if(ait != environment.domain_operators.end()) {

			// add the PDDL parameters to the action dispatch
			for(size_t i=0; i<ait->second.size(); i++) {
				diagnostic_msgs::KeyValue pair;
				pair.key = ait->second[i];
				pair.value = params[i];
				msg.parameters.push_back(pair);

				// prepare object existence for the knowledge filter
				bool add = true;
				for(size_t j=0; j<filter_objects.size(); j++)
					if(0==filter_objects[j].compare(params[i])) add = false;
				if(add) filter_objects.push_back(params[i]);
			}

			// prepare object attributes for the knowledge filter
			for(size_t i=0; i<environment.domain_operator_precondition_map[msg.name].size(); i++) {
				std::vector<std::string> filterAttribute;
				std::vector<std::string> precondition = environment.domain_operator_precondition_map[msg.name][i];
				filterAttribute.push_back(precondition[0]);
				for(size_t j=1; j<precondition.size(); j++) {
					// label
					if(j>1) filterAttribute.push_back(precondition[j]);
					// instance name
					for(size_t k=0;k<ait->second.size();k++) {
						if(0==ait->second[k].compare(precondition[j]))
							filterAttribute.push_back(params[k]);
					}
				}
				filter_attributes.push_back(filterAttribute);
			}
		} // end of operator
	}

	/*-----------------*/
	/* Planning filter */
	/*-----------------*/

	/**
	 * populates the knowledge filter messages
	 */
	void ProbePlanParser::generateFilter(PlanningEnvironment &environment) {

		knowledge_filter.clear();

		// populate filter message with objects
		for(size_t i=0; i<filter_objects.size(); i++) {
			rosplan_knowledge_msgs::KnowledgeItem filterItem;
			filterItem.knowledge_type = rosplan_knowledge_msgs::KnowledgeItem::INSTANCE;
			filterItem.instance_type = environment.object_type_map[filter_objects[i]];
			filterItem.instance_name = filter_objects[i];
			knowledge_filter.push_back(filterItem);
		}

		// populate filter message with attributes
		// TODO only statics, not all preconditions.
		for(size_t i=0; i<filter_attributes.size(); i++) {
			rosplan_knowledge_msgs::KnowledgeItem filterItem;
			filterItem.knowledge_type = rosplan_knowledge_msgs::KnowledgeItem::FACT;
			filterItem.attribute_name = filter_attributes[i][0];
			if (filter_attributes[i].size() > 1)
			{
				filterItem.instance_type = environment.object_type_map[filter_attributes[i][1]];
				filterItem.instance_name = filter_attributes[i][1];
				for(size_t j=2; j<filter_attributes[i].size()-1; j+=2) {
					diagnostic_msgs::KeyValue pair;
					pair.key = filter_attributes[i][j];
					pair.value = filter_attributes[i][j+1];
					filterItem.values.push_back(pair);
				}
			} 
			knowledge_filter.push_back(filterItem);
		}
	}
} // close namespace
