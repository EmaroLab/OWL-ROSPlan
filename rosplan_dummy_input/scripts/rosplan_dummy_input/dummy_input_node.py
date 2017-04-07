"""
Main node.
"""

# imports here

import rospy
import rosplan_knowledge_msgs.srv

__author__ = "Alessio Capitanelli"
# __credits__ = []
__license__ = "GNU"
__version__ = "1.0.0"
__maintainer__ = "Alessio Capitanelli"
__email__ = "alessio.capitanelli@dibris.unige.it"
__status__ = "Development"


def add_instance_call(client, name, type):
    return


def add_knowledge_call(client, predicate, args):
    return


def add_goal_call(client, predicate, args):
    return


def initialize_knowledge_client():
    rospy.wait_for_service('update_knowledge_base')
    try:
        service = rospy.ServiceProxy('update_knowledge_base',
                                     rosplan_knowledge_msgs.srv.KnowledgeUpdateService)
        return add_knowledge_service
    except rospy.ServiceException, e:
        print "Service call failed: %s" % e


if __name__ == "__main__":
    rospy.logout("Adding knowledge to the database...")
    add_knowledge_service = initialize_knowledge_client()
    rospy.logout("Knowledge added. The system is ready to plan.")
