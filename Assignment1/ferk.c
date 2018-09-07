#include<linux/kernel.h>
#include<linux/syscalls.h>

asmlinkage long sys_ferk(){
	printk("you got ferked boy");
	return sys_fork();
}
