/** @file assist-server.c
 *
 *  Copyright 2019-2020 Mentor Graphics Corporation, A Siemens business
 *
 *  This file is licensed under the terms of the GNU General Public License
 *  version 2.  This program  is licensed "as is" without any warranty of any
 *  kind, whether express or implied.
 *
 *  @brief Starting point of execution for assist-server. Create socket and wait in while loop.
 *
 *  main() function create a socket (tcp/ip) and go in while loop for accepting connection.
 *  Once new request come from DUT, accept the conenction and start processing the request
 *
 *  @author Yashwanth Dasar (yashwanth_dasar@mentor.com)
 *  @bug No know bugs.
 */

#include <assist.h>


/** @file assist-server.c
 *  @brief Starting point of execution. Create socket and wait in while loop
 *
 *  main() function create a socket (tcp/ip) and go in while loop for accepting connection.
 *  Once new request come from DUT, accept the conenction and start processing the request
 *
 *  @param none
 *  @return 0 on success and -1 on error
 */

int main(void) 
{ 
    int sockfd = 0, connfd = 0, sockaddr_len = 0; 
    struct sockaddr_in dut_addr = {0}; 
    char* portBuf; 
    int port = 0;
    int reserve_assist = -1;

    /* Read the configuration file and get the port number */   
    if((portBuf = get_config_value("port")) == NULL)
    {
        printf("\nAssist : Get port number fail\n");
        return CONFIG_FILE_PORT_NOT_FOUND;   
    }
    if((port = atoi(portBuf)) == 0)
    {
        perror("Assist : Get port number fail.");
        return CONVERT_PORT_TO_INT_FAIL;
    }
    #ifdef DEBUG
        printf("\nAssist : Binding TCP port is \n%d\n", port);
    #endif

    /* Free the allocated memory */
    if(portBuf != NULL)
    {
        free(portBuf);
        portBuf = NULL;
    }

    if((sockfd = create_socket(port)) < 0)
    {
        printf("\nAssist : Create socket fail\n");
        return CREATE_SOCKET_FAIL;
    }
    #ifdef DEBUG
        printf("\nAssist : Create socket pass\n");
    #endif


    // loop indefinitely for wait or listen or receive mode
    while(1)
    {
        sockaddr_len = sizeof(dut_addr); 
        
        // Accept connection from peer
        if((connfd = accept(sockfd, (struct sockaddr *)&dut_addr, (socklen_t*)&sockaddr_len)) < 0) 
        { 
            perror("Assist : Accept fail."); 
            // continue;
            //return ACCEPT_FAIL ; 
            exit(ACCEPT_FAIL);
        } 
        printf("\n============New connection==============\n"); 

        // Start receiving the requests or commands from DUT board
        if(service_request(connfd, reserve_assist) != OK)
        {
            printf("\nAssist : Something went wrong at assist board. Please check\n");
            
            if(write_socket("\nAssist : Something went wrong at assist board. Please check. AssistDataEnds", connfd) != 0)
            {
                continue;
            }
        } 
        close(connfd);
    }
    //close(sockfd);
    return OK;
}
