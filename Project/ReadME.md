# RTOS

#### Project Partners:
**IMT2015508** Vishal Gattani\
**IMT2015517** Pranav Agarwal\
**IMT2015518** Pranav Kedia

### CC BY-NC-SA 
https://creativecommons.org/licenses/by-nc-sa/4.0/

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


![occup-1](https://user-images.githubusercontent.com/24211929/57179697-1a801300-6e9e-11e9-9fbf-2b21b5581d36.jpg)
[Circuit Diagram.pdf](https://github.com/vishalgattani/RTOS/files/3144412/Circuit.Diagram.pdf)


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
We have divided the pathway into two sections namely, left and right, to accomodate for errors as the sensors on the left shouldn't interfere with those on the right and vice-versa. 

![Document 1](https://user-images.githubusercontent.com/24211929/57180052-587f3600-6ea2-11e9-952c-153f81b20a02.png)


So the cases we have considered where a person can be detected to enter or exit are as follows:

- A person entering/exiting from left/right will be counted.
- Two people simultaneously walking in from either directions of entry/exit will be taken into consideration.
- A person walking halfway through and then exiting his motion only to revert back and not enter/exit will not be taken into consideration as he/she hasn't completed his motion through the pathway.
- Cases where three people are entering/exiting at a time hasn't been taken into consideration based on the assumption that the pathway can only accommodate two people at a time.



## Error logs
The following cases are error prone when detecting an entry/exit.
- A person walking exactly through the midway of the pathway, either both or neither both the left and right sections may detect the person. The former will result in a count of +2 and the latter will result in no addition to the count of persons that have entered/exited.
- A person walking diagonally will trigger the first(front) sensor of either left/right section and then trigger the second(behind) sensor resulting in the state diagram getting changes only in the intermediate stages and it will not be effecting the count.


## RTOS Applications

## IPC using Sockets 
Our multiple client nodes are connected to the server using sockets, which are used to relay data information to the server
where it is collated to obtained the occupancy data of the desired room.


## File Locking
We have used multiple processed for different tasks like sensors reading , client send operation and so on which operate on one common file, so to avoid race condition among these processes we have used file locking in C and python.

## Conclusion
We have simulated a scenario where we have one pathway deployed and another pathway is just simulator to help visualize multiple entry exit points. So each client (*ClientA* (code.py) and *ClientB* (simulator.py)) are two pathways where one is just inputting a count value to a file by either adding or subtracting 1 or by doing nothing. 

The two files (code.py and simulator.py) are writing into a file by using appropriate file locking mechanisms to ensure they both do not write into the file at the same instances. The two client codes read the value from the file and send it to the server which displays the count values as detected by both the clients and prints them by showing the **<count_from_path1> <count_from_path2>**.







