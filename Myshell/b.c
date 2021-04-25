/*************************************************************************
    > File Name: b.c
    > Author: crushbb
    > Mail: 862127279@qq.com 
 ************************************************************************/

#include<stdio.h>
#include<readline/readline.h>
#include<readline/history.h>

int main()
{
	while(1)
	{
		char *p = readline("myshell:");
		add_history(p);
		p = readline("myshell:");
		add_history(p);
	}

	return 0;
}
