### Implementation of Real time data collection system

There are five sensors in a system, each of them giving five seperate parameters of the processing
plant. 

Each time when the sensor has data it writes into a particular file, next time when the data arrives it
over writes the file, so if we want to collect the data we need to read it before the next one arrives.
There is a specific frequeny at which sensor reads data and writes into the file.

Objecive is to implement a C program which will read the five files and send it to remote computer using
TCP/IP. So we have two programs one is server which runs on the computer inside the plant and it has
access to the file into which the sensor writes data periodically.
Then we have client program which runs on a laptop/Desktop which send periodic request to server to
get data. The client program as soon as it get write into a .csv file, which can processed later to generate
graphs and charts from the data stored.

### To run:

$ make run

First run the simulator to generate the 5 sensor files with values
$ python simulator.py 

$./server

$./client 0.0.0.0 

Results are stored in a results.txt (comma-seperated-values)
![Screenshot from 2019-03-21 14-28-00](https://user-images.githubusercontent.com/24211929/54769786-f935d100-4c27-11e9-9ec5-f96884fd9434.png)

