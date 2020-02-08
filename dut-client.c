/** @file dut-client.c
 *
 *  Copyright 2019-2020 Mentor Graphics Corporation, A Siemens business
 *
 *  This file is licensed under the terms of the GNU General Public License
 *  version 2.  This program  is licensed "as is" without any warranty of any
 *  kind, whether express or implied.
 *
 *  @brief Starting point of execution in dut-client. Establish tcp/ip communication with assist board and get services.
 *
 *  dut-client establish connection with assist board and get services.
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
 *  @return sockfs (socket descriptor). 0 on success and appropriate error on fail
 */

int client_create_socket(char* ip_addr, int port)
{
    int sockfd = 0; 
    struct sockaddr_in assist_addr = {0};

    /* Create socket */
    sockfd = socket(AF_INET, SOCK_STREAM, 0); 
    if (sockfd == -1) 
    { 
        perror("DUT : Create socket fail."); 
        return SOCKET_FAIL; 
    } 

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
    int error_count = 0;
    int wait_time = 600;
    int chunk_buffer_len = 0;
    char chunk_buffer[MAX_SIZE]={0};
    char* receive_data = NULL; 
    time_t end_wait=0;
    struct message recv_data = {0};

    /* Make buffer zero so that we can avoid accidental termination */
    bzero(chunk_buffer, MAX_SIZE);

    /* In case if there is big data to receive we have to use while loop */
    /* and data termination value check */
    
    end_wait = time (NULL) + wait_time ;
    while (time (NULL) < end_wait)
    {
        chunk_buffer_len = read(sockfd, (void*)&recv_data, sizeof(recv_data));
        
        #ifdef DEBUG
            printf("\nDUT : client_read_socket() : return length is = %d\n", chunk_buffer_len);
            printf("\nDUT : client_read_socket() : recv_data.current_length is = %d\n", recv_data.current_length);
            printf("\nDUT : client_read_socket() : recv_data.total_length is = %d\n", recv_data.total_length);
            printf("\nDUT : client_read_socket() : recv_data.data received is ... \n%s\n", recv_data.data);
            printf("\nDUT : client_read_socket() : recv_data received is ... \n%s\n", (char*) &recv_data);
        #endif

        if(chunk_buffer_len == -1)
        {
            perror("DUT : Error in read socket");
            if(receive_data)
            {
                free(receive_data); 
                receive_data = NULL;
            }
            break;
        }
        else if(chunk_buffer_len < 0)
        {
            perror("DUT : Chunk buffer is less than zero.");
            if(receive_data)
            {
                free(receive_data); 
                receive_data = NULL;            
            }
            break;
        }
        else if(chunk_buffer_len == 0)
        {
            perror("DUT : Chunk buffer is 0 bytes, try again.");
            error_count += 1;
            if(error_count >= 5)
            {
                free(receive_data); 
                receive_data = NULL; 
                break;
            }
        }

        /* If we are looping second time onwards */
        if((receive_data) && (strlen(receive_data) > 0))
        {
            char *tmp_receive_data = receive_data;
            receive_data = malloc(recv_data.current_length + strlen(receive_data) + 1);
            if(receive_data == NULL)
            {
                perror("Assist : malloc fail.");
                break;
            }

            strcpy(receive_data, tmp_receive_data);
            strcat(receive_data, recv_data.data); 

            if(tmp_receive_data)
            {
                free(tmp_receive_data);
                tmp_receive_data = NULL;
            }
        }
        /* First time we are looping */
        else
        {
            receive_data = malloc(recv_data.current_length);
            if(receive_data == NULL)
            {
                perror("Assist : malloc fail.");
                break;
            }
            strcpy(receive_data, recv_data.data);
        }

        /* Indication that this is the end of buffer */
        if(strstr(receive_data, "AssistDataEnds") != NULL)
        {
            #ifdef DEBUG
                printf("\nDUT : client_read_socket() : Observed \"AssistDataEnds\"\n");
            #endif          
            break;
        }

        bzero(chunk_buffer, MAX_SIZE);         
    }

    #ifdef DEBUG
        printf("\nDUT : client_read_socket() : Data receive completed successfully\n");
    #endif

    return receive_data ;
}


/** @file dut-client.c
 *  @brief Writing buffer to socket from DUT side
 *
 *  Write buffer to socket. Generally client commands are input.
 *
 *  @param sockfd (socket descriptor) request(client request or commands)
 *  @return 0 on success and error number (negative) on failure
 */

int client_write_socket(char* request, int sockfd)
{    
    struct message send_data = {0};

    send_data.current_length = strlen(request);
    send_data.total_length = send_data.current_length;

    strncpy(send_data.data, request, send_data.current_length);

    #ifdef DEBUG
        printf("\nDUT : send_data length is = %ld\n", sizeof(send_data));
        printf("\nDUT: send_data.data length is = %d\n", send_data.total_length);
        printf("\nDUT : send_data.data is ... \n%s\n", send_data.data);
    #endif

    if(write(sockfd, &send_data, sizeof(send_data)) == -1)
    {
        perror("DUT : Write socket fail.");
        return WRITE_SOCKET_FAIL;
    }
    return SUCCESS;
}



/** @file dut-client.c
 *  @brief Starting point for dut-client.
 *
 *  Starting point for dut-client. Create socket to establish communication with assist server
 *
 *  @param ip_addr (ip address of assist board)
 *  @return sockfs (socket descriptor) on success and appropriate error on fail
 */


int main(int argc, char* argv[]) 
{ 
    int sockfd = 0; 
    int port = 0;    
    char* ip_addr = NULL;
    char* portBuf = NULL;
    char* received_data = NULL;

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
        #endif // USE_IN_FUTURE
            
        return MISSING_ARGUMENTS;
    }

    if(strlen(argv[1]) > DATA_LEN)
    {
        printf("\nRequest length is expected\n");
        return REQUEST_BIGGER_FAIL;
    }
    printf("\n========New request start=======\n");

    /* Read the configuration file and get the assist board IP */   
    if((ip_addr = get_config_value("ip_address")) == NULL)
    {
        printf("\nDUT : Get assist IP address fail\n");
        return CONFIG_FILE_IP_NOT_FOUND;
    }

    /* Read the configuration file and get the port number */   
    if((portBuf = get_config_value("port")) == NULL)
    {
        printf("\nAssist : Get port number fail\n");
        return CONFIG_FILE_PORT_NOT_FOUND;   
    }    

    if((port = atoi(portBuf)) == SUCCESS)
    {
        printf("\nAssist : Get port number fail\n");
        return CONVERT_PORT_TO_INT_FAIL;
    }

    /* Create socket and connect */
    if((sockfd = client_create_socket(ip_addr, port)) == -1)
    {
        printf("\nDUT : Create socket fail\n");
        return CREATE_SOCKET_FAIL;
    }

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
    if(client_write_socket(argv[1], sockfd) != SUCCESS)
    {
        printf("\nDUT : Write socket fail\n");
        return WRITE_SOCKET_FAIL;
    }

    /* Receive the assist board data */
    received_data = NULL;
    if((received_data = client_read_socket(sockfd)) == NULL)
    {
        printf("\nDUT : Read socket fail\n");
        return READ_SOCKET_FAIL;
    }

    printf("\nDUT : Data received from assist board is \n%s\n\n", received_data);
    
    /* Free allocated memory */
    if(received_data)
    {
        free(received_data);        
    }   

    /* Close the socket */
    if(close(sockfd) != SUCCESS)
    {
        perror("DUT : Socket close fail.");
        return SOCKET_CLOSE_FAIL;
    }

    //return received_data;
    return SUCCESS;
}
