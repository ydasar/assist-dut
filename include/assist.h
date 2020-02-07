/** @file assist.h
 *
 *  Copyright 2019-2020 Mentor Graphics Corporation, A Siemens business
 *
 *  This file is licensed under the terms of the GNU General Public License
 *  version 2.  This program  is licensed "as is" without any warranty of any
 *  kind, whether express or implied.
 * 
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
#include <errno.h>

/* Defines or data */
#define MAX_SIZE 4096
#define ASSIST_CONF_FILE "/opt/ltp/testcases/bin/assist_address.conf"
#define CONSOLE_LOG_FILE "/tmp/cmd_console_logs"
#define RESERVE_ASSIST 999
#define UNRESERVE_ASSIST 888

#define USE_IN_FUTURE

/* Uncomment below line to get ebug messages */
#define DEBUG 1

/* Error enum types */
enum error {
	SUCCESS = 0,
	GENERAL_ERROR = -500,
	MISSING_ARGUMENTS,
    CONFIG_FILE_NOT_FOUND,    
    CONFIG_FILE_IP_NOT_FOUND,
    CONFIG_FILE_PORT_NOT_FOUND,
    CONVERT_PORT_TO_INT_FAIL,
    CREATE_SOCKET_FAIL,
    ACCEPT_FAIL,
    WRITE_SOCKET_FAIL,
    READ_SOCKET_FAIL,
    SOCKET_CLOSE_FAIL,
    SOCKET_FAIL,
    BIND_FAIL,
    LISTEN_FAIL,
    WRITE_FAIL1,
    WRITE_FAIL2,
    CONNECT_FAIL,
    COLLECT_CONSOLE_LOGS_FAIL,
    CONSOLE_LOGS_ZERO_SIZE,
    FILE_OPEN_FAIL,
    FILE_CLOSE_FAIL,
    BOARD_REBOOT_FAIL,
    PROCESS_START_FAIL,
    PROCESS_NOT_RUNNING,
    PROCESS_KILL_FAIL,
};


/* Create daemon with send/recv functions */
int create_socket(int port);
char* read_socket(int sockfd);
int write_socket(char* console_logs, int sockfd);

/* Service devider functions */
int service_request(int sockfd, int reserve_assist);

// Service functions
int request_console_logs(int sockfd);
char* collect_console_logs();
int clear_console_logs(int sockfd);
int health_check_assist_board(int sockfd);
int execute_request(char* request, int sockfd);
char* get_config_value(char* parameter);

/* Below function/features can be used in future. */
#ifdef USE_IN_FUTURE
	int check_assistboard_reserve(int sockfd, int reserve_assist);
	int reserve_assist_service(int sockfd, int reserve_assist);
	int unreserve_assist_service(int sockfd, int reserve_assist);
	int reboot_assist_board(int sockfd);
	int start_process(char* request, int sockfd);
	int check_process_running(char* request, int sockfd);
	int kill_running_process(char* request, int sockfd);
#endif //USE_IN_FUTURE
	

#endif // _ASSIST_HEADER
