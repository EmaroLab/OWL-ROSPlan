#include "rosplan_knowledge_base/ArmorManager.h"

namespace KCL_rosplan{

    ArmorManager::ArmorManager(std::string refName, ros::NodeHandlePtr nh) {
        this->refName = refName;
        this->nh = nh;

        ROS_INFO("Waiting for ARMOR...");
        ros::service::waitForService("armor_interface_srv");
        ROS_INFO("Connected!");
        this->armorClient = nh->serviceClient<armor_msgs::ArmorDirective>("armor_interface_srv", false);
        this->armorClientSerial = nh->serviceClient<armor_msgs::ArmorDirectiveList>("armor_interface_serialized_srv", false);

        this->msgTemplate.armor_request.client_name = "KCL_rosplan";
        this->msgTemplate.armor_request.reference_name = refName;
    }

    std::string ArmorManager::getRefName() {
        return this->refName;
    }

    void ArmorManager::setMsgDirective(armor_msgs::ArmorDirectiveRequest& msg, std::string command,
                                       std::string primarySpec, std::string secondarySpec) {
        msg.armor_request.command = command;
        msg.armor_request.primary_command_spec = primarySpec;
        msg.armor_request.secondary_command_spec = secondarySpec;
    }

    armor_msgs::ArmorDirectiveRequest ArmorManager::newMessage(std::string command, std::string primarySpec,
                                                               std::string secondarySpec) {
        armor_msgs::ArmorDirectiveRequest newMsg = this->msgTemplate;
        setMsgDirective(newMsg, command, primarySpec, secondarySpec);
        return newMsg;
    }

    armor_msgs::ArmorDirectiveRequest ArmorManager::newMessage(std::string command, std::string primarySpec,
                                                               std::string secondarySpec, std::vector<std::string> args) {
        armor_msgs::ArmorDirectiveRequest newMsg = this->msgTemplate;
        setMsgDirective(newMsg, command, primarySpec, secondarySpec);
        newMsg.armor_request.args = args;
        return newMsg;
    }

    bool ArmorManager::pollDomainOntology() {
        armor_msgs::ArmorDirectiveRequest req = newMessage("GET", "ALL", "REFS");
        armor_msgs::ArmorDirectiveResponse res;
        if(armorClient.call(req, res)) {
            if (res.armor_response.success) {
                return std::find(res.armor_response.queried_objects.begin(), res.armor_response.queried_objects.end(), refName)
                       != res.armor_response.queried_objects.end();
            }else return false;
        }else return false;
    }

    std::string ArmorManager::addPredicate(std::string predicateName, rosplan_knowledge_msgs::KnowledgeItem msg, bool normative){
        std::string id = std::to_string(maxId++);
        std::string predicateId = predicateName + "_" + id;

        std::vector<std::string> params;
        for (size_t i = 0; i < msg.values.size(); i++){
            params.push_back(msg.values[i].value);
        }

        armor_msgs::ArmorDirectiveRequest req = newMessage("ADD", "IND", "CLASS", {predicateId, predicateName});
        armor_msgs::ArmorDirectiveResponse res;

        // create predicate individual
        if(armorClient.call(req, res)){
            if (!normative) {
                req.armor_request.args = {predicateId, "Predicate_descriptive"};
                if (!armorClient.call(req, res)) return "";
            }else{
                req.armor_request.args = {predicateId, "Predicate_normative"};
                if (!armorClient.call(req, res)) return "";
            }
            // assign arguments
            for (size_t i = 0; i < params.size(); i++){
                req = newMessage("ADD", "OBJECTPROP", "IND", {getArgProperty((int)i + 1), predicateId, params[i]});
                if (!armorClient.call(req, res)) return "";
            }
            applyChanges();
            return predicateId;
        }
        return "";
    }

    bool ArmorManager::removeEntity(std::string entityName){
        armor_msgs::ArmorDirectiveResponse res;
        armor_msgs::ArmorDirectiveRequest req = newMessage("REMOVE", "IND", "", {entityName});
        return armorClient.call(req, res) && applyChanges();
    }

    bool ArmorManager::addInstance(std::string type, std::string name) {
        armor_msgs::ArmorDirectiveResponse res;
        armor_msgs::ArmorDirectiveRequest req = newMessage("ADD", "IND", "CLASS", {name, type});
        return armorClient.call(req, res) && applyChanges();
    }

    std::string ArmorManager::addFact(rosplan_knowledge_msgs::KnowledgeItem msg) {
        return addPredicate(msg.attribute_name, msg, false);
    }

    std::string ArmorManager::addNorm(rosplan_knowledge_msgs::KnowledgeItem msg) {
        return addPredicate(msg.attribute_name, msg, true);
    }

    std::vector<rosplan_knowledge_msgs::KnowledgeItem> ArmorManager::getGoals(std::string goal_type){
        //TODO add exceptions
        std::vector<rosplan_knowledge_msgs::KnowledgeItem> goals;
        armor_msgs::ArmorDirectiveRequest req =
                newMessage("QUERY", "OBJECTPROP", "IND", {"is_defined_by", goal_type});
        armor_msgs::ArmorDirectiveResponse res;
        if(armorClient.call(req, res) && res.armor_response.success && res.armor_response.queried_objects.size() > 0){
            // initialize goal
            rosplan_knowledge_msgs::KnowledgeItem goal;
            goal.knowledge_type = 1;
            goal.function_value = 0;
            goal.is_negative = false;
            // iterate on all current goals
            std::vector<std::string>::iterator goalsIt;
            for (goalsIt = res.armor_response.queried_objects.begin();
                 goalsIt != res.armor_response.queried_objects.end(); goalsIt++){
                armor_msgs::ArmorDirectiveResponse tmpRes1;
                req = newMessage("QUERY", "IND", "OBJECTPROP", {*goalsIt});
                if (armorClient.call(req, tmpRes1) && tmpRes1.armor_response.success){
                    // iterate on current predicate args
                    std::string tmpName = *goalsIt;
                    goal.attribute_name = tmpName.erase(goalsIt->rfind("_")).c_str();
                    goal.values.clear();
                    std::vector<std::string>::iterator argsIt;
                    // sort to ensure queried args are in the right order
                    std::sort(tmpRes1.armor_response.queried_objects.begin(),
                              tmpRes1.armor_response.queried_objects.end());
                    for (argsIt = tmpRes1.armor_response.queried_objects.begin();
                         argsIt!= tmpRes1.armor_response.queried_objects.end(); argsIt++){
                        //filter out irrelevant or inferred properties
                        if ((*argsIt).find(this->baseArgPropertyName) != std::string::npos) {
                            armor_msgs::ArmorDirectiveResponse tmpRes2;
                            req = newMessage("QUERY", "OBJECTPROP", "IND", {*argsIt, *goalsIt});
                            if (armorClient.call(req, tmpRes2) && tmpRes2.armor_response.success
                                && tmpRes2.armor_response.queried_objects.size() > 0) {
                                diagnostic_msgs::KeyValue kv;
                                kv.key = *argsIt;
                                kv.value = tmpRes2.armor_response.queried_objects[0];
                                goal.values.push_back(kv);
                            } else return goals;
                        }
                    }
                    goals.push_back(goal);
                }else return goals;
            }
        }else return goals;
        return goals;
    }

    std::vector<rosplan_knowledge_msgs::KnowledgeItem> ArmorManager::getCurrentGoals() {
        return getGoals("Final_state_instance");
    }

    std::vector<rosplan_knowledge_msgs::KnowledgeItem> ArmorManager::getSatisfiedGoals() {
        return getGoals("Satisfied_state_instance");
    }

    bool ArmorManager::clearClass(std::string className) {
        armor_msgs::ArmorDirectiveRequest req = newMessage("QUERY", "IND", "CLASS", {className});
        armor_msgs::ArmorDirectiveResponse res;
        if(!armorClient.call(req, res) || !res.armor_response.success) return false;
        if(res.armor_response.queried_objects.size() == 0) return true;

        armor_msgs::ArmorDirectiveListRequest serialRequest;
        armor_msgs::ArmorDirectiveListResponse serialResponse;
        for (uint i = 0; i < res.armor_response.queried_objects.size(); i++){
            armor_msgs::ArmorDirectiveReq tmpReq =
                        newMessage("REMOVE", "IND", "", {res.armor_response.queried_objects[i]}).armor_request;
            serialRequest.armor_requests.push_back(tmpReq);
        }

        return armorClientSerial.call(serialRequest, serialResponse) && serialResponse.success && applyChanges();
    }

    bool ArmorManager::applyChanges(){
        // explicitly states all predicates and individuals are disjoint, needed for SWRL rules to work
        armor_msgs::ArmorDirectiveRequest req = newMessage("DISJOINT", "IND", "CLASS", {"Item"});
        armor_msgs::ArmorDirectiveResponse res;
        if(!armorClient.call(req, res) || !res.armor_response.success) return false;

        req = newMessage("APPLY", "", "", {});
        if(!armorClient.call(req, res) || !res.armor_response.success) return false;

        req = newMessage("REASON", "", "");
        return !(!armorClient.call(req, res) || !res.armor_response.success);
    }

    bool ArmorManager::loadOntology(std::string path, std::string iri) {
        armor_msgs::ArmorDirectiveRequest req = newMessage("LOAD", "FILE", "", {path, iri, "true", "PELLET", "true"});
        armor_msgs::ArmorDirectiveResponse res;
        return armorClient.call(req, res) && res.armor_response.success;
    }

    bool ArmorManager::mountOnOntology(){
        armor_msgs::ArmorDirectiveRequest req = newMessage("MOUNT", "", "", {});
        armor_msgs::ArmorDirectiveResponse res;
        return armorClient.call(req, res) && res.armor_response.success;
    }

    bool ArmorManager::unmountFromOntology(){
        armor_msgs::ArmorDirectiveRequest req = newMessage("UNMOUNT", "", "", {});
        armor_msgs::ArmorDirectiveResponse res;
        return armorClient.call(req, res) && res.armor_response.success;
    }

    std::string ArmorManager::getArgProperty(int propNumber){
        return this->baseArgPropertyName + std::to_string(propNumber);
    }
}