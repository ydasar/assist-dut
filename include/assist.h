/** @file assist.h
 *  @brief Function prototypes for the assist server/daemon
 *
 *  This contains the include header files, data and function prototypes
 *
 *  @author Yashwanth Dasar (yashwanth_dasar@mentor.com)
 *  @bug No known bugs.
 */

#ifndef _ASSIST_HEADER
#define _ASSIST_HEADER


/* libc includes. */
#include <stdio.h> 
#include <netdb.h> 
#include <netinet/in.h> 
#include <stdlib.h> 
#include <string.h> 
#include <sys/socket.h> 
#include <sys/types.h> 
#include <unistd.h>
#include <time.h>
#include <arpa/inet.h>
#include <ifaddrs.h>
#include <netpacket/packet.h>

/* Defines or data */
#define MAX_SIZE 4096
#define PORT 5678 
#define SA struct sockaddr 
#define ASSIST_CONF_FILE "/opt/ltp/testcases/bin/assist_address.conf"
#define CONSOLE_LOG_FILE "/tmp/cmd_console_logs"


/* Uncomment below line and compile the code to get more informative logs */
/* #define DEBUG 1 */

/* Global variables */

int g_reserve_assist;

/* Create daemon with send/recv functions */
int create_socket(int port);
char* read_socket(int sockfd);
int write_socket(char* console_logs, int sockfd);

/* Service devider functions */
int service_request(int sockfd);

// Service functions
int request_console_logs(int sockfd);
char* collect_console_logs();
int clear_console_logs(int sockfd);
int reboot_assist_board(int sockfd);
int health_check_assist_board(int sockfd);
int start_process(char* request, int sockfd);
int check_process_running(char* request, int sockfd);
int kill_running_process(char* request, int sockfd);
int execute_request(char* request, int sockfd);
char* get_config_value(char* parameter);

#ifdef READY_TO_USE
int check_assistboard_reserve(int sockfd);
int reserve_assist_service(int sockfd);
int unreserve_assist_service(int sockfd);
int reboot_assist_board(int sockfd);
int start_process(char* request, int sockfd);
int check_process_running(char* request, int sockfd);
int kill_running_process(char* request, int sockfd);

#endif

#endif
