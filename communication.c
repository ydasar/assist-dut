/** @file communication.c
 *
 *  Copyright 2019-2020 Mentor Graphics Corporation, A Siemens business
 *
 *  This file is licensed under the terms of the GNU General Public License
 *  version 2.  This program  is licensed "as is" without any warranty of any
 *  kind, whether express or implied.
 *
 *  @brief Socket communication functions available. Create socket, read/receive, write/send data.
 *
 *  For this application, receive data is not bigger than 1K.
 *  But send data can be closer to 8K, 32K. For bigger data, a 
 *  special mechanism is required to send complete data.
 *
 *  @author Yashwanth Dasar (yashwanth_dasar@mentor.com)
 *  @bug No know bugs.
 */

#include <assist.h>


/** @file communication.c
 *  @brief Create tcp/ip socket. And make it ready to use.
 *
 *  Create tcp/ip socket. And make it ready to use.
 *
 *  @param none
 *  @return sockfd on success and -1 on error.
 */

int create_socket(int port)
{
    int sockfd = 0;
    int enable = 0; 
    struct sockaddr_in assist_addr = {0}; 

    /* Create socket  */
    if((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) 
    { 
        perror("Assist : Create socket fail."); 
        return SOCKET_FAIL; 
    } 

    /* Set zero in assist_addr to remove junk chars */
    bzero(&assist_addr, sizeof(assist_addr));

    setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int));

    /* Assign ip address and port to socket  */
    assist_addr.sin_family = AF_INET; 
    assist_addr.sin_addr.s_addr = htonl(INADDR_ANY); 
    assist_addr.sin_port = htons(port); 

    /* Bind socket with address and port */
    if ((bind(sockfd, (struct sockaddr*)&assist_addr, sizeof(assist_addr))) != 0) 
    { 
        perror("Assist : Bind socket fail."); 
        return BIND_FAIL; 
    } 

    /* Listen using socket. Max connections can be 1 */
    if ((listen(sockfd, 1)) != SUCCESS) 
    { 
        perror("Assist : Listen socket fail"); 
        return LISTEN_FAIL; 
    } 
    return sockfd;
}



/** @file communication.c
 *  @brief Read data from socket.
 *
 *  Read data from socket. Data can be commands, console log request, console log clear request etc.
 *
 *  @param sockfd (Socket descriptor)
 *  @return request on success, NULL on error
 */


char* read_socket(int sockfd)
{
    int request_len = 0;
    char *request = NULL;

    /* Allocate dynamic memory. We know requests length dont cross 1024 bytes, so allocate with fix size */
    request = (char *)malloc(MAX_SIZE);
    if(request == NULL)
    {
        perror("Assist : Memory allocation fail at read_socket().");
        return NULL;
    }

    /* Recv/read data from socket */
    if((request_len = read(sockfd, request, MAX_SIZE)) == -1)
    {
        perror("Assist : Read fail.");
        free(request); 
        request = NULL;
        return NULL;
    }
    /* At end there is "\n". Remove it */
    request[request_len - 1]='\0';

    return request;
}


/** @file communication.c
 *  @brief Write data to socket
 *
 *  For every request from DUT, there has to a reply.
 *  For console logs request, logs has to be sent.
 *  In case of failure, error has to be sent.
 *  Other than console logs request, success or error has to be sent.
 *
 *  @param console_logs (console logs or errors or information) and sockfd (socket descriptor)
 *  @return retVal. 0 on success, -1 on error
 */

int write_socket(char* console_logs, int sockfd)
{
    int retVal = -1;
    int total_send_len = 0;
    int tmp_send_len = 0;   
    int tmp_total_send_len = 0;
    int wait_time = 600;
    char tmpBuf[MAX_SIZE] = {0};
    time_t end_wait = 0;

    #ifdef DEBUG
        printf("\nAssist : write-socket() : Send data is %s\n", console_logs);
        printf("\nAssist : write-socket() : Send data length is %ld\n", strlen(console_logs));
    #endif

    /* Send console logs or command execution status back to DUT.  */
    total_send_len = strlen(console_logs);

    /* Check the data length, if data length more than 1K, send data in chunks/fragments */
    if(total_send_len >= (MAX_SIZE-1))
    {
        strncpy(tmpBuf, &console_logs[0], MAX_SIZE-1);  
        tmp_send_len = MAX_SIZE ;
    }
    else
    {
        strcpy(tmpBuf, &console_logs[0]);
        tmp_send_len = total_send_len + 1;
    }
    
    /* Loop till last byte is sent */
    end_wait = time (NULL) + wait_time ;
    /* while(1) */
    while (time (NULL) < end_wait)
    {
        /* send data through socket */
        tmp_send_len = write(sockfd, tmpBuf, tmp_send_len);

        #ifdef DEBUG
            printf("\nAssist : write-socket() : Send chunk data is ...\n%s\n", tmpBuf);
            printf("\nAssist : write-socket() : Send data length is %ld\n", strlen(tmpBuf));
            printf("\nAssist : write-socket() : Sent data length is %d\n", tmp_send_len);            
        #endif

        /* Check the data send error */
        if(tmp_send_len == -1)
        {
            perror("Assist : Writing socket fail. Return value is (-1).");
            retVal = WRITE_FAIL1;
            break;
        }
        /* Check the data send error */
        else if (tmp_send_len < 0)
        {
            perror("Assist : Writing socket fail. Return value is (<0).");
            retVal = WRITE_FAIL2;
            break;
        }
        /* Check the data send error */
        else if(tmp_send_len == 0)
        {
            perror("Assist : Writing socket returned zero. Try again. Return value is (0)");
        }
        
        /* Know till now how much data is sent */
        tmp_total_send_len = tmp_total_send_len + tmp_send_len -1;

        /* Deciding factor to know data send is end or complete */
        if((total_send_len - tmp_total_send_len) <= 0)
        {
            printf("\nAssist : Sent complete data\n");
            retVal = SUCCESS;
            break;
        }

        /* Still data is there to send */
        if((total_send_len - tmp_total_send_len) >= (MAX_SIZE-1))
        {
            strncpy(tmpBuf, &console_logs[tmp_total_send_len], MAX_SIZE-1);
            tmp_send_len = MAX_SIZE ;
        }
        /* This is last chunk which is less then 1K */
        else
        {
            strncpy(tmpBuf, &console_logs[tmp_total_send_len], MAX_SIZE-1);
            tmp_send_len = total_send_len - tmp_total_send_len + 1;
        }
        //strncpy(tmpBuf, &console_logs[total_send_len], MAX_SIZE-1);
    }

    #ifdef DEBUG
        printf("\nAssist : write-socket() : Data send completed successfully\n");
    #endif

    return retVal;
}


/** @file dut-client.c
 *  @brief Read config file search for parameter and return the value
 *
 *  Read config file search for parameter and return the value
 *
 *  @param parameter
 *  @return value (can be IP address or port etc). On error return NULL.
 */

char* get_config_value(char* parameter)
{
    char line[256] = {0};
    char* value_address = NULL;
    FILE* assist_conf=NULL;

    if((assist_conf = fopen(ASSIST_CONF_FILE, "r")) == NULL)
    {
        perror("Cannot open the config file.");
        return NULL;
    }   

    /* Loop and find the assist ip address */
    while (fgets(line, sizeof(line), assist_conf)) 
    {
        if(strstr(line, parameter) != NULL) 
        {           
            sprintf(line, "%s", &line[strlen(parameter)+1]);
            value_address = malloc(strlen(line));
            strcpy(value_address, line);
            fclose(assist_conf);
            return value_address;
        }
    }
    fclose(assist_conf);
    return NULL;
}
