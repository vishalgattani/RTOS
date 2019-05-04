# RTOS

#### Project Partners:
**IMT2015508** Vishal Gattani\
**IMT2015517** Pranav Agarwal\
**IMT2015518** Pranav Kedia

## Abstract
The project consists of multiple **HC-SR04** ultrasonic sensors interfaced on a **Raspberry PI 3** to determine the occupancy of a room that has a entry/exit point. Synchronization between the multiple sensors and nodes is to determine how many people have entered/exited simultaneously. This is achieved using Finite State Machines (FSMs). During the working of the project, various test cases have been considered and the results have been documented in the report.

## Components 

The components used in the project are as follows:

- **HC_SR04**
	> The HC-SR04 is a cheap Ultrasonic Distance Sensor available all over the price very cheap and fairly reliable, however its accuracy is +/- 3 mm. This accuracy means that anything less than 1 cm measurement is at best a guess.

#### Top view of sensor
  ![hc-sr04-board-t](https://user-images.githubusercontent.com/24211929/57176843-bd269a80-6e7a-11e9-8838-0608ce595d76.jpg)

#### Bottom view of sensor
  ![hc-sr04-board-b](https://user-images.githubusercontent.com/24211929/57176842-bc8e0400-6e7a-11e9-95a4-6f65a265d805.jpg)
  
- **Raspberry Pi 3**

![770A5842-1612x1080](https://user-images.githubusercontent.com/24211929/57176872-2c9c8a00-6e7b-11e9-8c9f-77e4c907feec.jpg)




## Circuit Diagram 



## Circuit Analysis

- There are **4** HC-SR04 sensors in total with 2 sensors deployed on either side to help determine the direction of entry/exit. 
- All the HC-SR04 sensors' are triggered simultaneously in order to ease the tracking of each sensor instead of triggering them at different instances and then receiving the echo.  
- The optimal distances from the sensors were calculated to receive the sensor data. The maximum range detected by the sensor is 4 meters and the minimum range is 2 centimeters with a measuring angle of 15 degrees.
- We configured the sensor to output a value of **1** that an object has passed by if and only if it receives an echo within a time interval of  **0.0023 seconds**. 
- This is done to reduce the interference between the sensors as their cone of measurement might align with each other. Also, faster data rates are achieved in order to detect objects passing by as the echo time is significantly reduced.

## FSM Logic Implementation
 - To give an idea of how we have incorporated the concept of a Finite State Machine to determine the entry/exit of a person, consider the case where a person is walking through the pathway. We shall take into consideration only the two sensors on one side for ease of explanation.
 - To know if the person has entered/exited, all the sensors in the beginning of the test output the value of 0 stating that they do not detect the presence of an obstacle.
 - Let the initial state of the two sensors be defined by the outputs of the sensors i.e., **00** in the beginning.
 - When the person walks through the pathway, the sensors give outputs in the order as shown in the diagram.
 
 The states are represented as follows 
 - S1S2 (Sensor 1 O/P: Sensor 2 O/P)
 
 The person is entering from the side where S1 is triggered first then S2.

![download (1)](https://user-images.githubusercontent.com/24211929/57177100-3a9fda00-6e7e-11e9-9006-508aa524cbba.png)

The above state diagram takes only into consideration the notion of single direction of a persons motion. Only if all the states are progressed from the beginning state i.e, **00** and ending after all the consecutive states are reached until finally it reaches the state of empty i.e., **00**, then the count is incremented for entry and decremented for exit.

To account for both entry/exit, the state diagram modifies as follows: 

![download (2)](https://user-images.githubusercontent.com/24211929/57177186-5657b000-6e7f-11e9-88f2-7276a813426c.png)


## Overview of the Setup 
![Document 1](https://user-images.githubusercontent.com/24211929/57177512-06c6b380-6e82-11e9-8fae-b9139cbdaa17.png)




## FSM 

The entire state diagram can be shown as follows:

![MainFSM](https://user-images.githubusercontent.com/24211929/57177636-90c34c00-6e83-11e9-8049-acbd0ba67aa5.png)



## Cases

## Error logs

## RTOS Applications

## Conclusion






