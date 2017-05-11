#!/usr/bin/python

"""
Main node.
"""

# imports here

import rospy
import rosplan_knowledge_msgs.srv
import rosplan_knowledge_msgs.msg
import diagnostic_msgs.msg

__author__ = "Alessio Capitanelli"
# __credits__ = []
__license__ = "GNU"
__version__ = "1.0.0"
__maintainer__ = "Alessio Capitanelli"
__email__ = "alessio.capitanelli@dibris.unige.it"
__status__ = "Development"


def add_instance_call(service, name, type):
    msg = rosplan_knowledge_msgs.srv.KnowledgeUpdateServiceRequest()
    knowledge_item = rosplan_knowledge_msgs.msg.KnowledgeItem()
    msg.update_type = msg.ADD_KNOWLEDGE
    knowledge_item.knowledge_type = knowledge_item.INSTANCE
    knowledge_item.instance_type = type
    knowledge_item.instance_name = name
    msg.knowledge = knowledge_item
    service.call(msg)
    return


def add_fact_call(service, attribute, args):
    msg = rosplan_knowledge_msgs.srv.KnowledgeUpdateServiceRequest()
    knowledge_item = rosplan_knowledge_msgs.msg.KnowledgeItem()
    msg.update_type = msg.ADD_KNOWLEDGE
    knowledge_item.knowledge_type = knowledge_item.FACT
    knowledge_item.attribute_name = attribute

    for i, arg in enumerate(args):
        key_value = diagnostic_msgs.msg.KeyValue()
        key_value.key = "has_arg_" + str(i + 1)
        key_value.value = arg
        knowledge_item.values.append(key_value)

    msg.knowledge = knowledge_item
    service.call(msg)
    return


def add_goal_call(service, attribute, args):
    msg = rosplan_knowledge_msgs.srv.KnowledgeUpdateServiceRequest()
    knowledge_item = rosplan_knowledge_msgs.msg.KnowledgeItem()
    msg.update_type = msg.ADD_GOAL
    knowledge_item.knowledge_type = knowledge_item.FACT
    knowledge_item.attribute_name = attribute

    for i, arg in enumerate(args):
        key_value = diagnostic_msgs.msg.KeyValue()
        key_value.key = "has_arg_" + str(i + 1)
        key_value.value = arg
        knowledge_item.values.append(key_value)

    msg.knowledge = knowledge_item
    service.call(msg)
    return


def initialize_knowledge_client():
    rospy.wait_for_service('/kcl_rosplan/update_knowledge_base')
    try:
        service = rospy.ServiceProxy('/kcl_rosplan/update_knowledge_base',
                                     rosplan_knowledge_msgs.srv.KnowledgeUpdateService)
        return service
    except rospy.ServiceException, e:
        print "Service call failed: %s" % e


def populate(service):
    # add instances
    add_instance_call(service, "link1", "link")
    add_instance_call(service, "link2", "link")
    add_instance_call(service, "link3", "link")
    add_instance_call(service, "angle0", "angle")
    add_instance_call(service, "angle90", "angle")
    add_instance_call(service, "angle180", "angle")
    add_instance_call(service, "angle270", "angle")

    # add attributes
    add_fact_call(service, "angle-ord", ["angle0", "angle90"])
    add_fact_call(service, "angle-ord", ["angle90", "angle180"])
    add_fact_call(service, "angle-ord", ["angle180", "angle270"])
    add_fact_call(service, "angle-ord", ["angle270", "angle0"])

    add_fact_call(service, "affected", ["link2", "link1"])
    add_fact_call(service, "affected", ["link3", "link1"])
    add_fact_call(service, "affected", ["link3", "link2"])

    add_fact_call(service, "is-child-of", ["link2", "link1"])
    add_fact_call(service, "is-child-of", ["link3", "link2"])

    add_fact_call(service, "has-angle", ["link1", "angle0"])
    add_fact_call(service, "has-angle", ["link2", "angle0"])
    add_fact_call(service, "has-angle", ["link3", "angle0"])

    # add goals
    add_goal_call(service, "has-angle", ["link1", "angle0"])
    add_goal_call(service, "has-angle", ["link2", "angle90"])
    add_goal_call(service, "has-angle", ["link3", "angle180"])


if __name__ == "__main__":
    rospy.logout("Adding knowledge to the database...")
    add_knowledge_service = initialize_knowledge_client()
    rospy.sleep(2)
    populate(add_knowledge_service)
    rospy.logout("Knowledge added. The system is ready to plan.")
