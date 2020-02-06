/** @file dut-client.c
 *
 *  Copyright 2019-2020 Mentor Graphics Corporation, A Siemens business
 *
 *  This file is licensed under the terms of the GNU General Public License
 *  version 2.  This program  is licensed "as is" without any warranty of any
 *  kind, whether express or implied.
 *
 *  @brief Starting point of execution in dut-client. Establsih tcp/ip communication with assist board and get services.
 *
 *  dut-client establish connection with assist board and get servces.
 *  Services can be ...
 *  
 *  Command execution
 *  Request for console logs
 *  Clear console logs
 *  Reboot assist board
 *  Start process
 *  Kill a process
 *  Check the process state
 *  Assist board health
 *  Block the assist board for test
 *  Unblock the assist board for test
 *
 *  @author Yashwanth Dasar (yashwanth_dasar@mentor.com)
 *  @bug No know bugs.
 */

// Headers or data
#include <assist.h>



/** @file dut-client.c
 *  @brief Create socket to establish communication with assist server
 *
 *  Create socket to establish communication with assist server
 *
 *  @param ip_addr (ip address of assist board)
 *  @return sockfs (socket descriptor). 0 on success and appripriate error on fail
 */

int client_create_socket(char* ip_addr, int port)
{
    int sockfd=0; 
    struct sockaddr_in assist_addr={0};

    /* Create socket */
    sockfd = socket(AF_INET, SOCK_STREAM, 0); 
    if (sockfd == -1) 
    { 
        perror("DUT : Create socket fail."); 
        return SOCKET_FAIL; 
    } 
    #ifdef DEBUG
        printf("\nDUT : Create socket pass\n"); 
    #endif

    /* Set zero in assist_addr to remove junk chars */
    bzero(&assist_addr, sizeof(assist_addr)); 

    /* Assign ip address and port to socket */
    assist_addr.sin_family = AF_INET; 
    /* assist_addr.sin_addr.s_addr = inet_addr(ASSIST_ADDR); */
    assist_addr.sin_addr.s_addr = inet_addr(ip_addr); 
    assist_addr.sin_port = htons(port); 
    
    /* Connect to assist board */
    if (connect(sockfd, (struct sockaddr*)&assist_addr, sizeof(assist_addr)) != 0) 
    { 
        perror("DUT : Connect to assist board fail"); 
        return CONNECT_FAIL; 
    } 
    #ifdef DEBUG
        printf("\nDUT : Connect to assist board pass\n"); 
    #endif

    return sockfd;
}


/** @file dut-client.c
 *  @brief Receive/read data/request from socket
 *
 *  Receive/read data/request from socket
 *
 *  @param sockfd (socket descriptor)
 *  @return receive_data. Data on success, NULL on error
 */

char* client_read_socket(int sockfd)
{
    char chunk_buffer[MAX_SIZE]={0};
    int chunk_buffer_len = 0;
    char* receive_data = NULL; 
    int error_count = 0;
    int wait_time = 600;
    time_t end_wait=0;

    /* Make buffer zero so that we can avoid accidental termination */
    bzero(chunk_buffer, MAX_SIZE);

    /* In case if there is big data to receive we have to use while loop */
    /* and data termination value check */
    end_wait = time (NULL) + wait_time ;
    /* while(1) */
    while (time (NULL) < end_wait)
    {
        chunk_buffer_len = read(sockfd, chunk_buffer, MAX_SIZE);
        chunk_buffer[chunk_buffer_len]='\0';
        
        #ifdef DEBUG
            printf("\nDUT : Chunk buffer is : \n%s\n", chunk_buffer);
            printf("\nDUT : Chunk buffer len is : %d\n", chunk_buffer_len);
        #endif

        if(chunk_buffer_len == -1)
        {
            perror("DUT : Error in read socket");
            if(receive_data)
            {
                free(receive_data); receive_data = NULL;
            }
            break;
        }
        else if(chunk_buffer_len < 0)
        {
            perror("DUT : Chunk buffer is less than zero.");
            if(receive_data)
            {
                free(receive_data); receive_data = NULL;            
            }
            break;
        }
        else if(chunk_buffer_len == 0)
        {
            perror("DUT : Chunk buffer is 0 bytes, try again.");
            sleep(2);
            error_count += 1;
            if(error_count >= 5)
            {
                free(receive_data); receive_data = NULL; 
                break;
            }
            /* continue; */
        }

        /* If we are looping second time onwards */
        if((receive_data) && (strlen(receive_data) > 0))
        {
            char *tmp_receive_data = receive_data;
            receive_data = malloc(chunk_buffer_len + strlen(receive_data) + 1);
            if(receive_data == NULL)
            {
                perror("Assist : malloc fail.");
                break;
            }

            strcpy(receive_data, tmp_receive_data);
            strcat(receive_data, chunk_buffer); 

            if(tmp_receive_data)
            {
                free(tmp_receive_data);
                tmp_receive_data = NULL;
            }

            #ifdef DEBUG
                printf("\nDUT : Accumalated2 chunk buffer is \n%s\n", receive_data);
            #endif      
        }
        /* First time we are looping */
        else
        {
            receive_data = malloc(chunk_buffer_len);
            if(receive_data == NULL)
            {
                perror("Assist : malloc fail.");
                break;
            }

            strcpy(receive_data, chunk_buffer);
            #ifdef DEBUG
                printf("\nDUT : Accumalated1 chunk buffer is : \n%s\n", receive_data);
            #endif
        }

        /* Indication that this is the end of buffer */
        if(strstr(receive_data, "AssistDataEnds") != NULL)
        {
            #ifdef DEBUG
                printf("\nDUT : Socket read is completed\n");
            #endif
            break;
        }

        bzero(chunk_buffer, MAX_SIZE);         
    }
    
    #ifdef DEBUG
        printf("\nDUT : Console logs length is : %ld\n", strlen(receive_data));
        printf("\nDUT : Console logs : \n%s\n", receive_data);
    #endif
    return receive_data ;
}



int client_write_socket(char* request, int sockfd)
{
    int request_len = 0;

    request_len = strlen(request) + 1;
    if(write(sockfd, request, request_len) == -1)
    {
        perror("DUT : Write socket fail.");
        return WRITE_SOCKET_FAIL;
    }
    #ifdef DEBUG
        printf("\nDUT : Write socket pass\n");
    #endif
    return OK;
}



/** @file dut-client.c
 *  @brief Starting point for dut-client.
 *
 *  Starting point for dut-client. Create socket to establish communication with assist server
 *
 *  @param ip_addr (ip address of assist board)
 *  @return sockfs (socket descriptor) on success and appripriate error on fail
 */


int main(int argc, char* argv[]) 
{ 
    int sockfd = 0; 
    char* ip_addr = NULL;
    char* portBuf = NULL;
    int port = 0;

    /* Help in running the dut-client */
    if(( argc < 2 ) || (strstr(argv[1], "-h")))
    {
        printf("\nDUT : Help ./dut-client \"request\"\n");
        printf("\nDUT : Request can be ...\n\n");
        printf("DUT : ./dut-client \"ConsoleLogsClear\"\n");
        printf("DUT : ./dut-client \"any-command\"\n");
        printf("DUT : ./dut-client \"ConsoleLogsRequest\"\n");
        
        /* Below function/features can be used in future. Begin */
        #ifdef USE_IN_FUTURE
            printf("DUT : ./dut-client \"AssistBoardReboot\"\n");
            printf("DUT : ./dut-client \"StartProcess\"\n");
            printf("DUT : ./dut-client \"CheckProcessRunning\"\n");
            printf("DUT : ./dut-client \"KillRunningProcess\"\n");
        #endif
        /* Above function/features can be used in future. End */
            
        return MISSING_ARGUMENTS;
    }
    #ifdef DEBUG
    else
    {
        printf("\nDUT : The argument supplied is : \"%s\" \n", argv[1]);
    }
    printf("\n========New request start=======\n");
    #endif

    /* Read the configuration file and get the assist board IP */   
    if((ip_addr = get_config_value("ip_address")) == NULL)
    {
        printf("\nDUT : Get assist IP address fail\n");
        return CONFIG_FILE_IP_NOT_FOUND;
    }
    #ifdef DEBUG
        printf("\nDUT : Assist IP address is \n%s\n", ip_addr);
    #endif    

    /* Read the configuration file and get the port number */   
    if((portBuf = get_config_value("port")) == NULL)
    {
        printf("\nAssist : Get port number fail\n");
        return CONFIG_FILE_PORT_NOT_FOUND;   
    }    
    if((port = atoi(portBuf)) == 0)
    {
        printf("\nAssist : Get port number fail\n");
        return CONVERT_PORT_TO_INT_FAIL;
    }
    #ifdef DEBUG
        printf("\nDUT : Connecting TCP port is \n%s\n", ip_addr);
    #endif

    /* Create socket and connect */
    if((sockfd = client_create_socket(ip_addr, port)) == -1)
    {
        printf("\nDUT : Create socket fail\n");
        return CREATE_SOCKET_FAIL;
    }
    #ifdef DEBUG
        printf("\nDUT : Create socket pass\n");
    #endif

    /* Free the allocated memory for port */
    if(portBuf != NULL)
    {
        free(portBuf);
        portBuf = NULL;
    }

    /* Free the allocated memory fir ip address*/
    if(ip_addr != NULL)
    {
        free(ip_addr);
        ip_addr = NULL;
    }

    /* Send request to assist board */
    if(client_write_socket(argv[1], sockfd) != OK)
    {
        printf("\nDUT : Write socket fail\n");
        return WRITE_SOCKET_FAIL;
    }
    #ifdef DEBUG
        printf("\nDUT : Write socket pass\n");
    #endif

    /* Receive the assist board data */
    char* received_data = NULL;
    if((received_data = client_read_socket(sockfd)) == NULL)
    {
        printf("\nDUT : Read socket fail\n");
        return READ_SOCKET_FAIL;
    }
    #ifdef DEBUG
        printf("\nDUT : Read socket pass\n");
    #endif

    /* Close the socket */
    close(sockfd); 
    
    printf("\nDUT : Data received from assist board is \n%s\n", received_data);
    
    /* Free allocated memory */
    if(received_data)
    {
        free(received_data);        
    }   

    //return received_data;
    return OK;
}
