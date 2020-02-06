/** @file services.c
 *
 *  Copyright 2007-2020 Mentor Graphics Corporation, A Siemens business
 *
 *  This file is licensed under the terms of the GNU General Public License
 *  version 2.  This program  is licensed "as is" without any warranty of any
 *  kind, whether express or implied.
 *
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
        retVal = request_console_logs(sockfd);
    }

    /* Clear console logs */
    else if(strcmp(request, "ConsoleLogsClear") == 0)
    {
        retVal = clear_console_logs(sockfd);
    }

    /* Check the health of assit board */
    else if(strcmp(request, "AssistBoardHealth") == 0)
    {
        retVal = health_check_assist_board(sockfd);
    }

    #ifdef READY_TO_USE
    
        /* Check assist board is reserved */
        else if(strcmp(request, "AssistBoardReserveStatus") == 0)
        {
            retVal = check_assistboard_reserve(sockfd);
        }

        /* Reserve the assist board */
        else if(strcmp(request, "AssistBoardReserve") == 0)
        {
            retVal = reserve_assist_service(sockfd);
        }

        /* Unreserve the assost board */
        else if(strcmp(request, "AssistBoardUnreserve") == 0)
        {
            retVal = unreserve_assist_service(sockfd);
        }

        /* Rebooting assist board */
        else if(strcmp(request, "AssistBoardReboot") == 0)
        {
            retVal = reboot_assist_board(sockfd);
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
    #endif
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