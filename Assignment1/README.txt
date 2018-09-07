README 
@author
Ch Vishal f2015B5A70605
Prateek Gupta f2015B5A70482
Tanmay Kulkarni f2015B5A70647

#############################Question 1#######################################################################################
The objective of this program is to create a shell which provides the following functionality
	1. Changing the directory
	2. Echo to stdout
	3. Show contents of the directory
	4. Word count of files
Note: Currently this DOES NOT SUPPORT space separated locations
Compilation Instructions
Use the standard procedure: gcc Question1.c
##############################################################################################################################

#############################Question 2#######################################################################################
The objective is to include a system call in the linux kernel that forks the current program.
Instructions for building the kernel:
	1. Run the following commands first:
		-> sudo apt-get update
		-> sudo apt-get upgrade
		-> sudo apt-get dist-upgrade (if required)
		-> sudo apt-get install gcc
		-> apt-get install python-pip-dev libffi-dev libssl-dev libxml2-dev libxslt1-dev libjpeg8-dev zlib1g-dev
		-> apt-get install libncursesw5-dev
		-> apt-get install bison
		-> apt-get install flex
		-> sudo tar -xvf linux-4.18.1.tar.gz -C /usr/src/
	2. Now create a new folder in /usr/src/linux_distr that contains the new system call.
	3. Add the C file for the system call with the following code:
			#include<linux/kernel.h>
			#include<linux/syscalls.h>

			asmlinkage long sys_ferk(){
				printk("you got ferked boy");
				return sys_fork();
			}
	4. Create a Makefile corrosponding to the above program	with the following instructions
			obj-y:filename.o 
			//Here filename is the name of the file of the previous step.
	5. Add the system call to linux_distr/arch/x86/entry/syscalls/ in the system_64.tbl file.
	6. Add the foldername to the Makefile present in the /usr/src/linux_distr folder.
	7. Add the function definition in the syscalls.h header file present in linux_distr/include/linux/
	8. Run the following set of commands:
		-> sudo apt install git fakeroot build-essential ncurses-dev xz-utils libssl-dev bc
		-> cp /boot/config-$(uname -r) .config
		-> make menuconfig
		-> sudo make -j 4
		-> sudo make modules_install install
	9. Reboot and choose the required distribution from grub advanced options
	10. Create a header file containing the following information:
			#ifndef FERK
			#define FERK
			int ferk(void);
			#endif
	11. Create a C file with the following data
			#include<unistd.h>

			int ferk(){
				return syscall(332);
			}
			
	12. Include these two in the folder where we wish to call ferk();
##############################################################################################################################

#############################Question 3#######################################################################################	
1. The objective of the program is to extract two numbers from a PREDEFINED set of numbers and
  then make the parent sleep for x/3 and the child for y%3 seconds. 
2. In addition, the numbers extracted should not be a repetition of the previous.
3. The execution should stop when all the numbers have been extracted.
Compilation Instructions
Use the standard procedure: gcc Question3.c
##############################################################################################################################

#############################Question 4#######################################################################################	
The objective of this program is to create an application that is similar to a process 
manager. It takes the number of times it show processes and how many processes at a time.
Furhtermore, it gives the option of killing processes bases on user demand.
Compilation Instructions
Use the following procedure: gcc question4_4.c -o timeDelay
To Execute ./timeDelay sleepTime numberOfProcesses NumberOfTimes
##############################################################################################################################

#############################Question 5#######################################################################################	
This program searches for the occurences of a particular regular expression and returns the 
line numbers where matches are found.
Use the following procedure: gcc question5Again.c -o newgrep
To Execute ./timeDelay regex fileName
##############################################################################################################################