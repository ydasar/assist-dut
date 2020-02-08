/** @file services-utilities.c
 *
 *  Copyright 2019-2020 Mentor Graphics Corporation, A Siemens business
 *
 *  This file is licensed under the terms of the GNU General Public License
 *  version 2.  This program  is licensed "as is" without any warranty of any
 *  kind, whether express or implied.
 *
 *  @brief All DUT requests are handled
 *
 *  Request can be following
 * 
 *  Command execution : Execute a command, foreground or background
 *  Request for console logs : Send console logs to DUT
 *  Clear console logs : Truncate the console log file
 *  Reboot assist board : Reboot the assist board
 *  Start process : Start a process
 *  Kill a process : Kill a process
 *  Check the process state : Provide process state to DUT
 *  Assist board health : Provide assist board and assist server status to DUT
 *  
 *  NOTE : We can restart the assist server. But we can create systemd service.
 *
 *  @author Yashwanth Dasar (yashwanth_dasar@mentor.com)
 *  @bug No know bugs.
 */

#include <assist.h>



/** @file services-utilities.c
 *  @brief Read the console log file
 *
 *  Console logs are pushed to /tmp/cmd_console_logs during command execution. 
 *  Collect the console logs. 
 *
 *  @param sockfd (socket descriptor)
 *  @return retVal (success 0, failure negative numbers).
 */

int request_console_logs(int sockfd)
{
    int retVal = 0;
    char* console_logs = NULL;
    char tmpBuf[256] = {0};


    /* Read the log file and collect the content */
    if((console_logs = collect_console_logs()) == NULL)
    {
        printf("\nAssist : collect_console_logs() fail. Log file doesnt exist or no logs.\n");
        sprintf(tmpBuf, "Assist : collect_console_logs() fail. Log file doesnt exist. Return value is -1. AssistDataEnds");
        retVal = COLLECT_CONSOLE_LOGS_FAIL;
    }

    /* If the console logs length is zero byte or less, it is error */
    if((retVal != COLLECT_CONSOLE_LOGS_FAIL) && (strlen(console_logs) < 1))
    {
        printf("\nAssist : collect_console_logs() null\n");
        sprintf(tmpBuf, "Assist : collect_console_logs() fail. Log file doesnt has logs. Return value is -1. AssistDataEnds");
        retVal = CONSOLE_LOGS_ZERO_SIZE;
    }

    /* Send/write  console logs to socket */
    else if((retVal != CONSOLE_LOGS_ZERO_SIZE) && (write_socket(console_logs, sockfd) != SUCCESS))
    {
        printf("\nAssist : Write console logs failed\n");
        sprintf(tmpBuf, "Assist : collect_console_logs() fail. Write to socket fail. Return value is -1. AssistDataEnds");
        retVal = WRITE_SOCKET_FAIL;
    }       

    /* Free console_log memory */
    if(console_logs != NULL)
    {
        free(console_logs); 
        console_logs = NULL;    
    }
    return retVal;
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
    unsigned console_logs_len = 0; 
    FILE *fd_cmd_output = NULL;    

    /* Collect the console logs */
    /* Open console log file /tmp/cmd_output */
    if((fd_cmd_output = fopen(CONSOLE_LOG_FILE, "r")) == NULL)
    {
        perror("Assist : Cannot open the file.");
        /* Here console logs is NULL */
        return NULL;
    }
    /* Move to end of file */
    if(fseek(fd_cmd_output, 0 , SEEK_END) != SUCCESS)
    {
        printf("Assist : File seek fail.");
    }

    /* Find the size of console logs */
    if((console_logs_len = ftell(fd_cmd_output)) == 0)
    {
        perror("Assist : File size is zero");    
        fclose(fopen(CONSOLE_LOG_FILE, "w"));
        console_logs = NULL;
        return console_logs;            
    }

    /* Move to beginning of file */
    if(fseek(fd_cmd_output, 0 , SEEK_SET) != SUCCESS)
    {
        perror("Assist : File seek fail");
    }

    if(console_logs_len > 0)
    {
        printf("\nAssist : console log len is = %d\n", console_logs_len);
        console_logs = malloc(console_logs_len + strlen("\n") + strlen("AssistDataEnds"));
        printf("\nAssist : After adding tag, console log len is = %ld\n", (console_logs_len + strlen("\n") + strlen("AssistDataEnds")));
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
        perror("Assist : File read fail.");
    }
    console_logs[console_logs_len]='\0';
    strcat(console_logs, "\n");
    strcat(console_logs, "AssistDataEnds");
    
    fclose(fd_cmd_output);

    return console_logs;
}


/** @file services-utilities.c
 *  @brief Clear the console logs
 *
 *  Clear the console logs
 *
 *  @param sockfd (Socket descriptor)
 *  @return 0 on success and appropriate error on fail
 */

int clear_console_logs(int sockfd)
{
    char tmpBuf[256] = {0};
    FILE *fd_cmd_output = NULL;    

    if((fd_cmd_output = fopen(CONSOLE_LOG_FILE, "w")) == NULL)
    {
        perror("Assist : Cannot open the file.");
        sprintf(tmpBuf, "Cannot open the file. Return value is -1. AssistDataEnds");
        write_socket(tmpBuf, sockfd);

        return FILE_OPEN_FAIL;
    }   

    if(fclose(fd_cmd_output) != 0)
    {
        perror("Assist : Cannot truncate and close the file");
        sprintf(tmpBuf, "Cannot truncate and close the file. Return value is -1. AssistDataEnds");
        write_socket(tmpBuf, sockfd);

        return FILE_CLOSE_FAIL;
    }
    else
    {
        printf("\nAssist : clear_console_logs() = pass\n");
        sprintf(tmpBuf, "ConsoleLogsClear pass. Return value is 0. AssistDataEnds");
        write_socket(tmpBuf, sockfd);    
        return SUCCESS;
    }
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
    char tmpBuf[256] = {0};

    printf("\nAssist : Health check is OK\n");
    sprintf(tmpBuf, "Assist : Health check is OK. AssistDataEnds");
    write_socket(tmpBuf, sockfd);
    return SUCCESS;
}


/** @file services-utilities.c
 *  @brief Execute the dut command requests
 *
 *  Dut Incoming commands can be run in foreground nd background. 
 *  Foreground commands return actual return values
 *  Background commands return always success
 *
 *  @param request (commands) and sockfd (socket descriptor)
 *  @return retVal (system return value)
 */

int execute_request(char* request, int sockfd)
{   
    int retVal = 0;
    char tmpBuf[1024] = {0};  

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
    retVal = system(tmpBuf);

    if(retVal != SUCCESS)
    {
        printf("\nAssist : Execution of %s fail. Return value is %d\n", tmpBuf, retVal);
        perror("Assist : ");
        sprintf(tmpBuf, "Execution of \"%s\" fail. Return value is %d. AssistDataEnds\n", request, retVal); 
        write_socket(tmpBuf, sockfd);
    }
    else
    {
        printf("\nAssist : Execution of %s pass. Return value is %d\n", tmpBuf, retVal);
        sprintf(tmpBuf, "Execution of \"%s\" pass. Return value is %d. AssistDataEnds\n", request, retVal);
        write_socket(tmpBuf, sockfd);
    }
    return retVal;
}



/* Below function/features can be used in future. */
#ifdef USE_IN_FUTURE

/** @file services-utilities.c
 *  @brief Check assist board is blocked by some one.
 *
 *  At the beginning of test, reserve/block/book the assist board for continual test.
 *  Once the test is completed, un reserve/unblock/unbook the same.
 *
 *  @param none
 *  @return 0
 */

int check_assistboard_reserve(int sockfd, int* reserve_assist)
{
    char tmpBuf[256] = {0};


    if(*reserve_assist == RESERVE_ASSIST)
    {
        sprintf(tmpBuf, "Assist board is reserved. Lock value is 999. AssistDataEnds");
        write_socket(tmpBuf, sockfd);
    }
    else
    {
        sprintf(tmpBuf, "Assist board is not reserved. Lock value is 888. AssistDataEnds");
        write_socket(tmpBuf, sockfd);
    }
    return SUCCESS;
}

/** @file services-utilities.c
 *  @brief Reserve the assist board service.
 *
 *  At the beginning of test, block or book the assist board for continual test.
 *  Once the test is completed, unblock the same.
 *
 *  @param none
 *  @return 0
 */

int reserve_assist_service(int sockfd, int* reserve_assist)
{
    char tmpBuf[256] = {0};

    if(*reserve_assist != RESERVE_ASSIST)
    {
        *reserve_assist = RESERVE_ASSIST;
        sprintf(tmpBuf, "Assist board is reserved for you. Lock value is 999. AssistDataEnds");
        write_socket(tmpBuf, sockfd);
    }
    else
    {
        *reserve_assist = UNRESERVE_ASSIST;
        sprintf(tmpBuf, "Assist board is reserved for you. Lock value is 999. AssistDataEnds");
        write_socket(tmpBuf, sockfd);
    }
    return SUCCESS;
}


/** @file services-utilities.c
 *  @brief Unreserve the assist board service.
 *
 *  At the beginning of test, block or book the assist board for comtnual test.
 *  Once the test is completed, unblock the same.
 *
 *  @param none
 *  @return 0
 */

int unreserve_assist_service(int sockfd, int* reserve_assist)
{
    char tmpBuf[256] = {0};

    if(*reserve_assist != UNRESERVE_ASSIST)
    {
        *reserve_assist = UNRESERVE_ASSIST;
        sprintf(tmpBuf, "Assist board is unreserved. Lock value is 888. AssistDataEnds");
        write_socket(tmpBuf, sockfd);
    }
    else
    {
        *reserve_assist = RESERVE_ASSIST;
        sprintf(tmpBuf, "Assist board unreserv fail. Lock value is 999. AssistDataEnds");
        write_socket(tmpBuf, sockfd);
    }
    return SUCCESS;
}


/** @file services-utilities.c
 *  @brief Reboot the assist board
 *
 *  Reboot the assist board
 *
 *  @param sockfd (Socket descriptor)
 *  @return retVal (0 on success, appropriate error on fail).
 */

int reboot_assist_board(int sockfd)
{
    int retVal = -1;
    char tmpBuf[256] = {0};

    sprintf(tmpBuf, "reboot -h now");
    retVal = system(tmpBuf);

    if(SUCCESS == retVal)
    {
        printf("\nAssist : Board rebooting\n");
        sprintf(tmpBuf, "Assist : Board rebooting. Return value is %d. AssistDataEnds", retVal);
        write_socket(tmpBuf, sockfd);
        retVal = SUCCESS;
    }
    else
    {
        printf("\nAssist : Board reboot fail\n");
        perror("Assist : ");
        sprintf(tmpBuf, "Assist : Board reboot fail. Return value is %d. AssistDataEnds", retVal);
        write_socket(tmpBuf, sockfd);
        retVal = BOARD_REBOOT_FAIL;
    }
    return retVal;
}


/** @file services-utilities.c
 *  @brief Start a process/program
 *
 *  Start a process/program
 *
 *  @param request (commands) and sockfd (socket descriptor)
 *  @return retVal (0 on success, appropriate error on fail).
 */

int start_process(char* request, int sockfd)
{
    int retVal = -1;
    char tmpBuf[256] = {0};

    sprintf(tmpBuf, "%s", &request[13]);
    retVal = system(tmpBuf);

    if(0 == retVal)
    {
        printf("\nAssist : Process %s started successfully\n", &request[13]);
        sprintf(tmpBuf, "Assist : Process %s started successfully. Return value is %d. AssistDataEnds", &request[13], retVal);
        write_socket(tmpBuf, sockfd);
        retVal = SUCCESS;
    }
    else
    {
        printf("\nAssist : Process %s fail\n", &request[13]);
        perror("Assist : ");
        sprintf(tmpBuf, "Assist : Process %s failed to start. Return value is %d. AssistDataEnds", &request[13], retVal);
        write_socket(tmpBuf, sockfd);
        retVal = PROCESS_START_FAIL;
    }
    return retVal;  
}


/** @file services-utilities.c
 *  @brief Check process/program is running
 *
 *  Check the process is running, send the status
 *
 *  @param request (commands) and sockfd (socket descriptor)
 *  @return retVal (0 on success, appropriate error on fail).
 */

int check_process_running(char* request, int sockfd)
{
    int retVal = -1;
    char tmpBuf[256] = {0};

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
        retVal = SUCCESS;
    }
    else
    {
        printf("\nProcess do not exist\n");
        perror("Assist : ");        
        sprintf(tmpBuf, "Assist : Process \"%s\" do not exist. AssistDataEnds", &request[20]);
        write_socket(tmpBuf, sockfd);
        retVal = PROCESS_NOT_RUNNING;
    }
    return retVal;
}


/** @file services-utilities.c
 *  @brief Kill the program
 *
 *  Check requested program is running. If so kill it by name
 *
 *  @param request (commands) and sockfd (socket descriptor)
 *  @return retVal (0 on success, appropriate error on fail).
 */

int kill_running_process(char* request, int sockfd)
{
    int retVal = -1;
    char tmpBuf[256] = {0};


    sprintf(tmpBuf, "pkill %s", &request[19]);
    printf("\nCommand is %s\n", tmpBuf);
    retVal = system(tmpBuf);
    printf("\nsystem return value is %d\n", retVal);
    if(0 == retVal)
    {
        printf("\nAssist : Process killed\n");
        sprintf(tmpBuf, "Assist : Process \"%s\" killed. Return value is %d. AssistDataEnds", &request[19], retVal);
        write_socket(tmpBuf, sockfd);
        retVal = SUCCESS;
    }
    else
    {
        printf("\nProcess do not exist or not killed\n");
        perror("Assist : ");
        sprintf(tmpBuf, "Assist : Process \"%s\" do not exist or not killed. Return value is %d. AssistDataEnds", &request[19], retVal);
        write_socket(tmpBuf, sockfd);
        retVal = PROCESS_KILL_FAIL;
    }
    return retVal;
}



#endif // USE_IN_FUTURE
