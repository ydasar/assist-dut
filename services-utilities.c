/** @file services-utilities.c
 *  @brief All DUT requests are handled
 *
 *  Request can be following
 * 
 *  Command execution : Exeucte a command, foreground or background
 *  Request for console logs : Send console logs to DUT
 *  Clear console logs : Truncate the console log file
 *  Reboot assist board : Rebbot the assist board
 *  Start process : Start a process
 *  Kill a process : Kill aprocess
 *  Check the process state : Provide process state to DUT
 *  Assist board health : Provide assist board and assist server status to DUT
 *  
 *  NOTE : We can restart the assist server. But we can create systemd ervice.
 *
 *  @author Yashwanth Dasar (yashwanth_dasar@mentor.com)
 *  @bug No know bugs.
 */

#include "./include/assist.h"



/** @file services-utilities.c
 *  @brief Read the console log file
 *
 *  Console logs are pushed to /tmp/cmd_console_logs during command execution. 
 *  Collect the console logs. 
 *
 *  @param sockfd (socket descriptor)
 *  @return retVal (success 0, failure -1).
 */

int request_console_logs(int sockfd)
{
	char* console_logs = NULL;
	int retVal = -1;

	/* Read the log file and colect the content */
	if((console_logs = collect_console_logs()) == NULL)
	{
		printf("\nAssist : collect_console_logs() fail\n");
		if(console_logs)
		{
			free(console_logs); 
			console_logs = NULL;
		}
        return retVal = -1;
	}

	/* If the console logs length is zero byte or less, it is error */
	else if(strlen(console_logs) < 1)
	{
		printf("\nAssist : collect_console_logs() null\n");
		if(console_logs)
		{
			free(console_logs); 
			console_logs = NULL;
		}
        return retVal = -1;
	}

	/* Send/write  console logs to socket */
	if((write_socket(console_logs, sockfd)) == -1)
	{
		printf("\nAssist : Write console logs failed\n");
		if(console_logs)
		{
			free(console_logs); 
			console_logs = NULL;
		}
        return retVal = -1;
	}		

	/* Free console_log memory */
	if(console_logs)
	{
		free(console_logs); 
		console_logs = NULL;	
	}

	return retVal = 0;
}



/** @file services-utilities.c
 *  @brief Read the console log file
 *
 *  Console logs are pushed to /tmp/cmd_console_logs during command execution. 
 *  Read the file and collect the console logs. 
 *
 *  @param none
 *  @return console_logs. Data on success, NULL on error
 */

char* collect_console_logs(void)
{
	char *console_logs = NULL; 
	FILE *fd_cmd_output;
	unsigned console_logs_len = 0; 

	/* Collect the console logs */
	/* Open console log file /tmp/cmd_output */
	if((fd_cmd_output = fopen(CONSOLE_LOG_FILE, "r")) == NULL)
	{
		printf("\nCannot open the file");
		/* Here console logs is NULL */
		return NULL;
	}
	/* Move to end of file */
    if(fseek(fd_cmd_output, 0 , SEEK_END) != 0)
    {
		printf("\nfile seek fail\n");
    }

    /* Find the size of console logs */
    if((console_logs_len = ftell(fd_cmd_output)) == 0)
    {
    	printf("\nfile size is zero\n");	
    	fclose(fopen(CONSOLE_LOG_FILE, "w"));
    	console_logs = NULL;
		return console_logs;	    	
    }

    /* Move to begining of file */
    if(fseek(fd_cmd_output, 0 , SEEK_SET) != 0)
    {
		printf("\nfile seek fail\n");
    }

    if(console_logs_len > 0)
    {
    	printf("\nconsole log len is %d\n", console_logs_len);
    	console_logs = malloc(console_logs_len + strlen("\n") + strlen("AssistDataEnds "));
    }

    if(console_logs == NULL)
    {
		printf("\nMemory allocation fail at collect_console_logs()\n");
		fclose(fopen(CONSOLE_LOG_FILE, "w"));
		console_logs = NULL;
		return console_logs;	    	
    }
	
	if((fread(console_logs, sizeof(char), console_logs_len, fd_cmd_output)) != console_logs_len)
	{
		printf("\nfile read fail\n");
	}
	console_logs[console_logs_len]='\0';
	strcat(console_logs, "\n");
	strcat(console_logs, "AssistDataEnds");
	
	#ifdef DEBUG
		printf("\nfile read data is \n%s\n", console_logs);
	#endif
	
	sleep(1);
	fclose(fd_cmd_output);

	return console_logs;
}


/** @file services-utilities.c
 *  @brief Clear the console logs
 *
 *  Clear the console logs
 *
 *  @param sockfd (Socket descriptor)
 *  @return 0 on success and -1 on error
 */

int clear_console_logs(int sockfd)
{
	FILE *fd_cmd_output;

	if((fd_cmd_output = fopen(CONSOLE_LOG_FILE, "w")) == NULL)
	{
		printf("\nAssist : Cannot open the file");
		return -1;
	}	

	fclose(fd_cmd_output);

	write_socket("Assist : clear_console_logs() pass. AssistDataEnds", sockfd);

	return 0;
}


/** @file services-utilities.c
 *  @brief Reboot the assist board
 *
 *  Reboot the assist board
 *
 *  @param sockfd (Socket descriptor)
 *  @return retVal (0 on success and -1 on error).
 */

int reboot_assist_board(int sockfd)
{
	char tmpBuf[256];
	int retVal = -1;

	sprintf(tmpBuf, "reboot -h now");
	retVal = system(tmpBuf);

	if(0 == retVal)
	{
		printf("\nAssist : Board rebooting\n");
		sprintf(tmpBuf, "Assist : Board rebooting. Return value is %d. AssistDataEnds", retVal);
		write_socket(tmpBuf, sockfd);
		retVal = 0;
	}
	else
	{
		printf("\nAssist : Board reboot fail\n");
		sprintf(tmpBuf, "Assist : Board reboot fail. Return value is %d. AssistDataEnds", retVal);
		write_socket(tmpBuf, sockfd);
		retVal = -1;
	}
	return retVal;
}


/** @file services-utilities.c
 *  @brief assist-server is able to respond
 *
 *  assist-server is able to respond
 *
 *  @param sockfd (socket descriptor)
 *  @return 0 on success. Nothing on error
 */

int health_check_assist_board(int sockfd)
{
	printf("\nAssist : Health check is OK\n");
	write_socket("Assist : Health check is OK. AssistDataEnds", sockfd);
	return 0;
}


/** @file services-utilities.c
 *  @brief Start a process/program
 *
 *  Start a process/program
 *
 *  @param request (commands) and sockfd (socket descriptor)
 *  @return retVal (0 on success, -1 on failure).
 */

int start_process(char* request, int sockfd)
{
	char tmpBuf[256];
	int retVal = -1;

	sprintf(tmpBuf, "%s", &request[13]);
	retVal = system(tmpBuf);

	if(0 == retVal)
	{
		printf("\nAssist : Process %s started successfully\n", &request[13]);
		sprintf(tmpBuf, "Assist : Process %s started successfully. Return value is %d. AssistDataEnds", &request[13], retVal);
		write_socket(tmpBuf, sockfd);
		retVal = 0;
	}
	else
	{
		printf("\nAssist : Process %s started successfully\n", &request[13]);
		sprintf(tmpBuf, "Assist : Process %s failed to start. Return value is %d. AssistDataEnds", &request[13], retVal);
		write_socket(tmpBuf, sockfd);
		retVal = -1;
	}
	return retVal;	
}


/** @file services-utilities.c
 *  @brief Check process/program is running
 *
 *  Check the process is runing, send the status
 *
 *  @param request (commands) and sockfd (socket descriptor)
 *  @return retVal (0 on success, -1 on failure).
 */

int check_process_running(char* request, int sockfd)
{
	char tmpBuf[256];
	int retVal = -1;

	/* strcpy(process, &request[20]); */
	/* printf("\nProcess name is %s\n", process); */
	sprintf(tmpBuf, "pidof -x %s", &request[20]);
	printf("\nCommand is %s\n", tmpBuf);
	/* if(0 == system(command)) */
	if(0 == system(tmpBuf))
	{
		printf("\nAssist : Process exist\n");
		sprintf(tmpBuf, "Assist : Process \"%s\" exist. AssistDataEnds", &request[20]);
		write_socket(tmpBuf, sockfd);
		retVal = 0;
	}
	else
	{
		printf("\nProcess do not exist\n");
		sprintf(tmpBuf, "Assist : Process \"%s\" do not exist. AssistDataEnds", &request[20]);
		write_socket(tmpBuf, sockfd);
		retVal = -1;
	}
	return retVal;
}


/** @file services-utilities.c
 *  @brief Kill the program
 *
 *  Check requested program is running. If so kill it by name
 *
 *  @param request (commands) and sockfd (socket descriptor)
 *  @return retVal (0 on success, -1 on failure).
 */

int kill_running_process(char* request, int sockfd)
{
	char tmpBuf[256];
	int retVal = -1;

	sprintf(tmpBuf, "pkill %s", &request[19]);
	printf("\nCommand is %s\n", tmpBuf);
	/* if(0 == system(command))*/
	retVal = system(tmpBuf);
	printf("\nsystem return value is %d\n", retVal);
	if(0 == retVal)
	{
		printf("\nAssist : Process killed\n");
		sprintf(tmpBuf, "Assist : Process \"%s\" killed. Return value is %d. AssistDataEnds", &request[19], retVal);
		write_socket(tmpBuf, sockfd);
		retVal = 0;
	}
	else
	{
		printf("\nProcess do not exist or not killed\n");
		sprintf(tmpBuf, "Assist : Process \"%s\" do not exist or not killed. Return value is %d. AssistDataEnds", &request[19], retVal);
		write_socket(tmpBuf, sockfd);
		retVal = -1;
	}
	return retVal;
}


/** @file services-utilities.c
 *  @brief Execute the dut command requests
 *
 *  Dut Incoming commands can be run in foreground nd background. 
 *  Foregound commands return actual return values
 *  Background commands return always success
 *
 *  @param request (commands) and sockfd (socket descriptor)
 *  @return retVal system retun value
 */

int execute_request(char* request, int sockfd)
{	
	char tmpBuf[1024];	

	/* Append additional parameters to collect the console logs */
	/* sprintf(tmpBuf, "timeout 10s %s 2>&1 | tee %s", request, CONSOLE_LOG_FILE); */
	/* sprintf(tmpBuf, "%s 2>&1 | tee %s &", request, CONSOLE_LOG_FILE); */
	/* sprintf(tmpBuf, "%s 2>&1 > %s &", request, CONSOLE_LOG_FILE); */
	/* Following ">" truncate the logs. Change it. We are giving this option to client */

	if(strchr(request, '&') != NULL)
	{	
		request[strlen(request) - 1] = '\0';
		sprintf(tmpBuf, "%s 2>&1 >> %s &", request, CONSOLE_LOG_FILE);
	}
	else
	{
		sprintf(tmpBuf, "%s 2>&1 >> %s", request, CONSOLE_LOG_FILE);	
	}

	/* Execute the request_final in assist board */
	int retVal = system(tmpBuf);

	if(retVal !=0)
	{
		printf("\nAssist : Execution of %s fail. Return value is %d\n", tmpBuf, retVal);
		sprintf(tmpBuf, "Assist : Execution of \"%s\" fail. Return value is %d. AssistDataEnds\n", request, retVal); 
		write_socket(tmpBuf, sockfd);
	}
	else
	{
		printf("\nAssist : Execution of %s pass. Return value is %d\n", tmpBuf, retVal);
		sprintf(tmpBuf, "Assist : Execution of \"%s\" pass. Return value is %d. AssistDataEnds\n", request, retVal);
		write_socket(tmpBuf, sockfd);
	}
	return retVal;
}