/** @file services.c
 *  @brief Receive incoming requests from DUT, device them based on the request.
 *
 *  Request can be following
 * 
 *  Command execution
 *  Request for console logs
 *  Clear console logs
 *  Reboot assist board
 *  Start process
 *  Kill a process
 *  Check the process state
 *  Assist board health
 * 
 *  @author Yashwanth Dasar (yashwanth_dasar@mentor.com)
 *  @bug No know bugs.
 */

#include "./include/assist.h"


/** @file services.c
 *  @brief DUT request is compared and appropriate function is called
 *
 *  DUT request is compared/analised. Based on request appropriate utility is called.
 *
 *  @param sockfd (socket descriptor)
 *  @return retVal (success 0, failure -1).
 */


int service_request(int sockfd) 
{ 		
	char* request = NULL;
	int retVal = 0;

	if((request = read_socket(sockfd)) == NULL)
	{
		printf("\nAssist : read_socket() fail\n");
		write_socket("Assist : read_socket() fail. AssistDataEnds", sockfd);
        retVal = -1;
	}
	#ifdef DEBUG
	else
	{
		printf("\nAssist : Recv/read pass\n");
	}
	#endif


	/* Receive console logs */
	if(strcmp(request, "ConsoleLogsRequest") == 0)
	{
		if(request_console_logs(sockfd) == -1)
		{
			printf("\nAssist : request_console_logs() fail\n");
			write_socket("Assist : request_console_logs() fail. AssistDataEnds", sockfd);
			retVal = -1;
		}
		#ifdef DEBUG
		else
		{
			printf("\nAssist : request_console_logs() pass\n");
		}
		#endif
	}

	/* Clear console logs */
	else if(strcmp(request, "ConsoleLogsClear") == 0)
	{
		if(clear_console_logs(sockfd) == -1)
		{
			printf("\nAssist : clear_console_logs() fail\n");
			write_socket("Assist : clear_console_logs() fail. AssistDataEnds", sockfd);
			retVal = -1;			
		}
		#ifdef DEBUG
		else
		{
			printf("\nAssist : clear_console_logs() pass\n");
		}
		#endif
	}

	/* Rebooting assist board */
	else if(strcmp(request, "AssistBoardReboot") == 0)
	{
		retVal = reboot_assist_board(sockfd);
	}

	/* Check the health of assit board */
	else if(strcmp(request, "AssistBoardHealth") == 0)
	{
		retVal = health_check_assist_board(sockfd);
	}

	/* Start a process */
	else if(strstr(request, "StartProcess") != 0)
	{
		retVal = start_process(request, sockfd);
	}

	/* Check a process is running */
	else if(strstr(request, "CheckProcessRunning") != 0)
	{
		retVal = check_process_running(request, sockfd);
	}

	/* Kill a running process */
	else if(strstr(request, "KillRunningProcess") != 0)
	{
		retVal = kill_running_process(request, sockfd);
	}	

	/* Execute the command */
	else 
	{
		retVal = execute_request(request, sockfd);
    }

    /* Free the allocated memory */ 
	if(request)
	{
		free(request); 
		request = NULL;
	}
	return retVal;
}