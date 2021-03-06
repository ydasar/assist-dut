/** @file dut-client.c
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
 *
 *  @author Yashwanth Dasar (yashwanth_dasar@mentor.com)
 *  @bug No know bugs.
 */

// Headers or data
#include "include/assist.h"



/** @file dut-client.c
 *  @brief Create socket to establish communication with assist server
 *
 *  Create socket to establish communication with assist server
 *
 *  @param ip_addr (ip address of assist board)
 *  @return sockfs (socket descriptor). 0 on success and -1 on error
 */

int client_create_socket(char* ip_addr)
{
	int sockfd; 
	struct sockaddr_in assist_addr;

	/* Create socket */
	sockfd = socket(AF_INET, SOCK_STREAM, 0); 
	if (sockfd == -1) 
	{ 
		printf("\nDUT : Create socket fail\n"); 
		return -1; 
	} 
	#ifdef DEBUG
	else
	{
		printf("\nDUT : Create socket pass\n"); 
	}
	#endif

	/* Set zero in assist_addr to remove junk chars */
	bzero(&assist_addr, sizeof(assist_addr)); 

	/* Assign ip address and port to socket */
	assist_addr.sin_family = AF_INET; 
	/* assist_addr.sin_addr.s_addr = inet_addr(ASSIST_ADDR); */
	assist_addr.sin_addr.s_addr = inet_addr(ip_addr); 
	assist_addr.sin_port = htons(PORT); 
	
	/* Connect to assist board */
	if (connect(sockfd, (SA*)&assist_addr, sizeof(assist_addr)) != 0) 
	{ 
		printf("\nDUT : Connect to assist board fail\n"); 
		return -1; 
	} 
	#ifdef DEBUG
	else
	{
		printf("\nDUT : Connect to assist board pass\n"); 
	}
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
	char chunk_buffer[MAX_SIZE];
	int chunk_buffer_len = 0;
	char* receive_data = NULL; 
	int error_count = 0;

	/* Make buffer zero so that we can avoid accidental termination */
	bzero(chunk_buffer, MAX_SIZE);

	/* In case if there is big data to receive we have to use while loop */
	/* and data termination value check */
	while(1)
	{
		chunk_buffer_len = read(sockfd, chunk_buffer, MAX_SIZE);
		chunk_buffer[chunk_buffer_len]='\0';
		
		#ifdef DEBUG
			printf("\nDUT : Chunk buffer is : \n%s\n", chunk_buffer);
			printf("\nDUT : Chunk buffer len is : %d\n", chunk_buffer_len);
		#endif

        if(chunk_buffer_len == -1)
        {
            printf("\nDUT : Error in read socket\n");
            if(receive_data)
            {
            	free(receive_data); receive_data = NULL;
            }
            break;
        }
		else if(chunk_buffer_len < 0)
        {
            printf("\nDUT : Chunk buffer is less than zero\n");
            if(receive_data)
            {
            	free(receive_data); receive_data = NULL;            
            }
            break;
        }
        else if(chunk_buffer_len == 0)
        {
            printf("\nDUT : Chunk buffer is 0 bytes, try again\n");
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
			strcpy(receive_data, chunk_buffer);
			#ifdef DEBUG
				printf("\nDUT : Accumalated1 chunk buffer is : \n%s\n", receive_data);
			#endif
		}

		/* Indication that this is the end of buffer */
        if(strstr(chunk_buffer, "AssistDataEnds") != NULL)
        {
        	printf("\nDUT : Socket read is completed\n");
        	/* receive_data = malloc(chunk_buffer_len); */
        	/* strcpy(receive_data, chunk_buffer); */
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



/** @file dut-client.c
 *  @brief Read config file and fetch the assist board IP
 *
 *  Read config file and fetch the assist board IP
 *
 *  @param none
 *  @return assist_ip_address. ip address on success and NULL on error.
 */

char* get_assist_ip()
{

	FILE *assist_conf;
	char line[200];
	char* assist_ip_address = NULL;

	if((assist_conf = fopen(ASSIST_CONF_FILE, "r")) == NULL)
	{
		printf("\nCannot open the file");
		return NULL;
	}	

	/* Loop and find the assist ip address */
	while (fgets(line, sizeof(line), assist_conf)) 
	{
		if(strstr(line, "ip_address=") != NULL) 
		{			
			sprintf(line, "%s", &line[strlen("ip_address=")]);
			assist_ip_address = malloc(strlen(line));
			strcpy(assist_ip_address, line);
			/* printf("\nIP address is %s\n", line); */
			return assist_ip_address;
		}
	}
	return NULL;
}


/** @file dut-client.c
 *  @brief Starting point for dut-client.
 *
 *  Starting point for dut-client. Create socket to establish communication with assist server
 *
 *  @param ip_addr (ip address of assist board)
 *  @return sockfs (socket descriptor) on success and -1 on failure
 */


int main(int argc, char* argv[]) 
{ 
	int sockfd; 
	char* ip_addr = NULL;

	/* Help in running the dut-client */
	if(( argc < 2 ) || (strstr(argv[1], "-h")))
	{
		printf("\nDUT : Help ./dut-client \"request\"\n");
		printf("\nDUT : Request can be ...\n\n");
		printf("DUT : ./dut-client \"ConsoleLogsClear\"\n");
		printf("DUT : ./dut-client \"any-command\"\n");
		printf("DUT : ./dut-client \"ConsoleLogsRequest\"\n");
		printf("DUT : ./dut-client \"AssistBoardReboot\"\n");
		printf("DUT : ./dut-client \"StartProcess\"\n");
		printf("DUT : ./dut-client \"CheckProcessRunning\"\n");
		printf("DUT : ./dut-client \"KillRunningProcess\"\n");
		return -1;
	}
	#ifdef DEBUG
	else
	{
		printf("\nDUT : The argument supplied is : \"%s\" \n", argv[1]);
	}
	#endif

	/* Read the configuration file and get the assist board IP */	
	if((ip_addr = get_assist_ip()) == NULL)
	{
		printf("\nDUT : Get assist IP address fail\n");
	}
	else
	{
		printf("\nDUT : Assist IP address is \n%s\n", ip_addr);
	}
	

	/* Create socket and connect */
	if((sockfd = client_create_socket(ip_addr)) == -1)
	{
		printf("\nDUT : Create socket fail\n");
		return -1;
	}
	#ifdef DEBUG
	else
	{
		printf("\nDUT : Create socket pass\n");
	}
	#endif

	/* Send request to assist board */
	if(write_socket(argv[1], sockfd) == -1)
	{
		printf("\nDUT : Write socket fail\n");
		return -1;
	}
	#ifdef DEBUG
	else
	{
		printf("\nDUT : Write socket pass\n");
	}
	#endif

	/* Receive the assist board data */
	char* received_data = NULL;
	if((received_data = client_read_socket(sockfd)) == NULL)
	{
		printf("\nDUT : Read socket fail\n");
		return -1;
	}
	#ifdef DEBUG
	else
	{
		printf("\nDUT : Read socket pass\n");
	}
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
	return -1;
}