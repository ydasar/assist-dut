/** @file communication.c
 *  @brief Socket communication functions available. Create socket, read/recevive, write/send data.
 *
 *  For this application, receive data is not bigger than 1K.
 *  But send data can be closer to 8K, 32K. For bigger data, a 
 *  special mechanism is required to send complete data.
 *
 *  @author Yashwanth Dasar (yashwanth_dasar@mentor.com)
 *  @bug No know bugs.
 */

#include "./include/assist.h"


/** @file communication.c
 *  @brief Create tcp/ip socket. And make it ready to use.
 *
 *  Create tcp/ip socket. And make it ready to use.
 *
 *  @param none
 *  @return sockfd on success and -1 on error.
 */

int create_socket(void)
{
	int sockfd; 
	struct sockaddr_in assist_addr; 

	/* Create socket  */
	sockfd = socket(AF_INET, SOCK_STREAM, 0); 
	if (sockfd == -1) 
	{ 
		printf("Assist : Create socket fail\n"); 
		return -1; 
	} 
	#ifdef DEBUG
	else
	{
		printf("Assist : Create socket pass\n"); 
	}
	#endif

	int enable = 1;
    setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int));

	/* Set zero in assist_addr to remove junk chars */
	bzero(&assist_addr, sizeof(assist_addr));

	/* Assign ip address and port to socket  */
	assist_addr.sin_family = AF_INET; 
	assist_addr.sin_addr.s_addr = htonl(INADDR_ANY); 
	assist_addr.sin_port = htons(PORT); 

	/* Bind socket with address and port */
	if ((bind(sockfd, (SA*)&assist_addr, sizeof(assist_addr))) != 0) 
	{ 
		printf("Assist : Bind socket fail\n"); 
		return -1; 
	} 
	#ifdef DEBUG
	else
	{
		printf("Assist : Bind socket pass\n"); 
	}
	#endif

	/* Listen using socket. Max connections can be 1 */
	if ((listen(sockfd, 1)) != 0) 
	{ 
		printf("Assist : Listen socket fail\n"); 
		return -1; 
	} 
	#ifdef DEBUG
	else
	{
		printf("Assist : Listen socket pass\n"); 
	}
	#endif

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
	char *request = NULL;
	int request_len = 0;

	/* Allocate dynamic memory. We know requests length dont cross 1024 bytes, so allocate with fix size */
	request = (char *)malloc(MAX_SIZE);
	if(request == NULL)
	{
		printf("\nAssist : Memory allocation fail at read_socket()\n");
		return NULL;
	}
	#ifdef DEBUG
	else
	{
		printf("\nAssist : Memory allocation pass\n");
	}
	#endif

	/* Recv/read data from socket */
	if((request_len = read(sockfd, request, MAX_SIZE)) == -1)
	{
		printf("\nAssist : Read fail\n");
		free(request); request = NULL;
		return NULL;
	}
	#ifdef DEBUG
	else
	{
		printf("\nAssist : Read pass\n");
	}
	#endif

	/* At end there is "\n". Remove it */
	request[request_len - 1]='\0';

	#ifdef DEBUG
		printf("\nAssist : Request/command from DUT is \n%s\n", request);
	#endif

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
 *  @param console_logs (console logs or errors or infomration) and sockfd (socket descriptor)
 *  @return retVal. 0 on success, -1 on error
 */

int write_socket(char* console_logs, int sockfd)
{
	int retVal = -1;
	int total_send_len = 0;
    int tmp_send_len = 0;	
    int tmp_total_send_len = 0;
	char tmpBuf[MAX_SIZE];

	#ifdef DEBUG
		printf("\nAssist : send data length is %ld\n", strlen(console_logs));
		printf("\nAssist : send data is \n%s\n", console_logs);
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
    while(1)
    {
    	#ifdef DEBUG
    		printf("\nSend temporary buffer is \n%s\n", tmpBuf);
    	#endif

    	/* send data through socket */
		tmp_send_len = write(sockfd, tmpBuf, tmp_send_len);

		/* Check the data send error */
		if(tmp_send_len == -1)
		{
			printf("\nAssist : Writing socket fail. Return value is (-1) %d\n", tmp_send_len);
			retVal = -1;
			break;
		}
		/* Check the data send error */
		else if (tmp_send_len < 0)
		{
			printf("\nAssist : Writing socket fail. Return value is (<0) %d\n", tmp_send_len);
			retVal = -1;
			break;
		}
		/* Check the data send error */
		else if(tmp_send_len == 0)
		{
			printf("\nAssist : Writing socket returned zero. Try again. Return value is (0) %d\n", tmp_send_len);
		}
		
		/* Know till now how much data is sent */
		tmp_total_send_len = tmp_total_send_len + tmp_send_len -1;

		#ifdef DEBUG
			printf("\nAssist : Send length is %d \n", tmp_send_len - 1);
			printf("\nAssist : Sent total len is %d\n", tmp_total_send_len);
			printf("\nAssist : Remaining send length is %d \n", total_send_len - tmp_total_send_len);
		#endif

		/* Deciding factor to know data send is end or complete */
		if((total_send_len - tmp_total_send_len) <= 0)
		{
			printf("\nAssist : Sent complete data\n");
			retVal = 0;
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
	return retVal;
}