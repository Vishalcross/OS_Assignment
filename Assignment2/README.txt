###############################################################################################################################################################
About:
This is the readme for the second assignment of Operating system. Here we shall be covering the useage and the dependencies to run the assignments
Dependencies:
1. C compiler
2. Python3
3. Numpy: a package to calculate the statistical distributions
4. pthread: a library for multithreading
5. math: a library for math functions in C

Question 1:
Useage:
1. First set the properties for the config.json file
{
	"file_name": "data.txt",
	"process_count": 5,
	"beta": 10,
	"scheduling_mode": "MULTQ"
}
"file_name": Here you can set the file for the data,
"process_count": The number of processes,
"beta": The value for beta in the traget distrabution,
"scheduling_mode": The kind of scheduling required
						It is either FCFS for first come first serve
						or MULTQ for multilevel Queue

2. Run the generate_data.py file
3. Compile question1.c 

!!!!!WARNING!!!!!
If you make any changes in the configurations, please follow step 2 followed by step 3 to satisfy internal dependencies

Question 2:
Useage
1. Compile question2_alternate.c with the options -pthread and -lm
2. Input the number of times each should run and the probability of retreival as well as the initial values of the resources.
3. Observe the output of the program.

!!!WARNING!!!
The program makes use of an atomic deadlock check, in addition, a delay of a second has been added to make sure multiple resources will try to fight for the same resources.
Thus, the program does take a significant amount of time to complete. In addition, if the resources are not collected they are lost permanently. Thus, ther is also a 
scenario in which the data may be lost permanently. Thus, an explicit check is present to account for such a scenario.