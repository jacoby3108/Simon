#include <stdlib.h>

void main (void)
{
	
	
	int pid;
	pid=fork();
	if(pid==0)
	{
		//printf("I am the child\n");
		execlp("aplay green.wav", NULL);		//Execute file: file, arguments (1 or more strings followed by NULL)
		_exit(0);
	}
	else
	{
		//printf("I am the parent\n");
		wait();
	} 
	
	
	
	
}
