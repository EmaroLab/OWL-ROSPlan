OWL-ROSPlan Framework
=====================

This is a fork of [KCL-Planning ROSPlan](https://github.com/KCL-Planning/ROSPlan) modified to use OWL ontologies.

The original ROSPlan framework provides a generic method for task planning in a ROS system. ROSPlan encapsulates both planning and dispatch. It possesses a simple interface, and already includes interfaces to common ROS libraries.

On top of that, **OWL-ROSPlan** now use an OWL ontology to store knowledge and object instances, allowing logic reasoning.

The original ROSPlan documentation is available at the project
[homepage](http://kcl-planning.github.io/ROSPlan/).

## Differences with ROSPlan

Most modifications have been operated in accordance with the documentation linked above. Topics have not been modified, and in general this version should not break compatibility with any project based on ROSPlan.

+ OWL-ROSPlan currently does not support Esterel Planning
+ OWL-ROSPlan default planner is [Probe](http://www.ai.upf.edu/software/probe-classical-planner)
+ OWL-ROSPlan allows to define into the ontology a resoning mechanism for goal inference, this required the addition of a new topic for the GUI. More on this in the next section.
+ Light modifications to the rqt GUI to improve readability and reflect other changes.

Codewise, most functional modifications are in the ```rosplan_knowledge_base``` package. ```rosplan_planning_system``` has beed modified only to support Probe call and plan parsing, but it is otherwise unchanged. 

## Using ontologies

The system requires you to provide an ontology with a specific structure, which should be general enough for most planning scenarios. You can find an example in ```rosplan_config``` called ```paco.owl```.

 The general idea is that knowledge and instances fed to ROSPlan gets encoded in the ontology as individuals. The same goes for goals, that are rather considered as *norms*. When a norm is not satisfied, an SWRL rule detects the issue and add that goal to a *final state* individual, from which it can be later retrived to generate a problem file. The services to retrieve goals are:

+ ```/kcl_rosplan/get_current_goals```, gives only active goals (i.e., not satisfied norms)
+ ```/kcl_rosplan/get_satisfied_goals```, gives all goals currently satisfied, only used by the gui

Due to the nature of ontologies, you may want to take this work as an example and **reimplement** your own ontological system for reasoning and instance checking. In case, you may find useful modifying the ```ArmorManager``` class (e.g., you can modify the getCurrentGoals method if you have a different goal inference mechanism).

An ontology should be provided to the knowledge base through the following parameters:

+ ```/rosplan/owl_path```, the path to the owl file
+ ```/rosplan/iri```, the ontology IRI

The ontology should have the same name as the PDDL domain or the system will fail.

The underlying ARMOR framework provides an experimental gui to visualize the current state of the ontology. To enable it, add the following line to the launch file:

```<param name="/armor/settings/show_gui" value="true" />```

You can also communicate directly with the ARMOR framework, this allows you to save the current state of the ontology or add other types of knowledge beyond the strictly planning related one. In case, you can refer to ARMOR [homepage](https://github.com/EmaroLab/armor) and [commands](https://github.com/EmaroLab/armor/blob/master/commands.md) documentation.

## Installation

To run OWL-ROSPlan, you need to install first the following ROS modules:

+ Install [rosjava](http://wiki.ros.org/rosjava/Tutorials/indigo/Installation)

+ Clone and build [ros_multi_ontology_reference](https://github.com/EmaroLab/ros_multi_ontology_references) 

**or**, if you are not familiar with repository submodules, manually clone:

+ [AMOR](https://github.com/EmaroLab/multi_ontology_reference), a ROS-ready java library for ontology management
+ [ARMOR](https://github.com/EmaroLab/armor), a framework for OWL ontologies manipulation and querying in ROS
+ [armor_msgs](https://github.com/EmaroLab/armor_msgs)

Then, install the following dependencies.

#### ROS Indigo instructions

```sh
sudo apt-get install flex ros-indigo-mongodb-store ros-indigo-tf2-bullet freeglut3-dev
```

#### ROS Kinetic instructions

Install **MongoDB**:

```sh
git clone -b 26compat https://github.com/mongodb/mongo-cxx-driver.git
sudo apt-get install scons
cd mongo-cxx-driver
sudo  scons  --prefix=/usr/local/ --full --use-system-boost --disable-warnings-as-errors
```

Clone **mongodb_store** in your worskace:

```sh
sudo apt-get install python-pymongo
cd /home/$USER/catkin_ws/src
git clone https://github.com/strands-project/mongodb_store.git
cd ..
catkin_make
```

Install other dependencies:

```sh
sudo apt-get install flex ros-kinetic-move-base-msgs ros-kinetic-nav-msgs ros-kinetic-tf2-bullet freeglut3-dev
``` 

#### Get the code

Clone the following repositories in your workspace:

```sh
cd /home/$USER/catkin_ws/src
git clone https://github.com/clearpathrobotics/occupancy_grid_utils
git clone https://github.com/EmaroLab/OWL-ROSPlan
```

Compile everything:

```sh
cd ..
catkin_make
```

Remember to stop MongoDB before launching OWL-ROSPlan for the first time in a new session (just like in ROSPlan):

```sh
sudo service mongodb stop
```

## Running an example

An example is provided which simulates a robot acting on an articulated object to reach a given configuration. 

You can launch the example by typing:

 ```roslaunch rosplan_planning_system launch_onto_rosplan.launch```

## Warning

Currently KCL-ROSPlan has not implemented yet `forall` and `cond` action effects, which we use (also in the example).
For this reason, we have disabled effects from `RPActionInterface`.
This is not an issue with our current setup since we can get the updated states from perception and actually we encourage this behavior for semantic feedback.
An option to re-enable effects will be added as soon as `forall` and `cond` are implemented.

**Until then, you have to manually update ROSPlan knowledge base after every action.**

Not doing so will most probably cause several `violated preconditions` errors.

## Contacts:

[alessio.capitanelli@dibris.unige.it](mailto:alessio.capitanelli@dibris.unige.it)

