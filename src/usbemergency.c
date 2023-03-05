// 
// File:   usbemergency.c
//
// Goal: main program, launched as daemon and in charge of launching actin on button press
//
// Author: Stéphane Mouton
//
//

#include <stdio.h>
#include <stdlib.h>    

#include <fcntl.h>          /* open() */
#include <unistd.h>         /* close() */
#include <getopt.h>         /* getopt(), optarg */
#include <syslog.h>         /* syslog() */
#include <sys/types.h>      /* WNOHANG */
#include <sys/wait.h>       /* WNOHANG */
#include <signal.h>         /* signal and SIG */

#include <errno.h>          /* errno */
#include <string.h>         /* strerror() strdup() */

#include <sys/stat.h>       /* umask() */
#include <stdbool.h>        /* booleans */
#include <linux/input.h>    /* EVIOCGVERSION ++ */

#include "parameters.h"
#include "eventmanager.h"

// ======== Global variables and DEFINEs
#define EXIT_SUCCESS 0
#define EXIT_FAILURE 1
#define PROGRAM_VERSION "0.1"
#define DEFAULT_CONFIGURATION_FILE "/etc/usbemergency.conf"

char * found_event = NULL;
bool halt_requested = false;

char* log_name;

// ======== Global signal_management
void signal_handler(int signal){
    switch (signal){
        case SIGCHLD :
            while (0 < waitpid(-1, NULL, WNOHANG));
        break;
        // re-read configuration file
        case SIGHUP :
            syslog(LOG_INFO, "Caught SIGHUP. Unable to reload parameters in this version. Please restart.");
            exit(EXIT_SUCCESS);
        break;
        // When requested to stop, then cleanly quit
        case SIGINT  :
        case SIGQUIT :
        case SIGTERM :
            syslog(LOG_INFO, "Caught signal %d, exiting.",signal);
            exit(EXIT_SUCCESS);
        break;
        default:
            syslog(LOG_ERR, "Caught unhandled signal %d", signal);
        break;
    }
    
}

// ======== Local functions
void display_help(char *program_name){
    printf("%s, version %s. Service to manage USB emergency button emulating single key keyboard\n",program_name, PROGRAM_VERSION);
    printf("Usage: %s [OPTIONS]\n",program_name);
    printf("Options\n");
    printf("-h print this help and exit\n");
    printf("-d debug mode, launch program in display mode (remain connected to console)\n");
    printf("-f FILE change configuration file (default is %s)\n",DEFAULT_CONFIGURATION_FILE);
    printf("-g generate configuration file and halt\n");
}

void generate_configuration_file(){
    printf("#\n"
            "# Configuration file for USB emergency button\n"
            "#\n"
            "[Device]\n"
            "# Input device, identified by vendor ID and product ID as seen in /proc/bus/input/devices\n"
            "# (it implies that IDs are not restricted to USB devices)\n"
            "# The goal is to find the associate device /dev/input/event??\n"
            "# Values are expressed in hexadecimal, as shown in /proc/bus/input/devices\n"
            "vendor_ID  = 0x%04x;\n"
            "product_ID = 0x%04x;\n"
            "# Do we expect a specific key and then which scan value?\n"
            "expect_specific_key = false;\n"
            "key_value = 0x%04x;\n"
            "\n"
            "[Action]\n"
            "# Script to be launched in asynchronous mode (i.e. in background)?\n"
            "# Default is false\n"
            "asynchronous_launch = false;\n"
            "# Script launched when the button is pressed\n"
            "# (Default is /bin/true)\n"
            "script = %s;\n",
            DEFAULT_VENDOR_ID, DEFAULT_PRODUCT_ID,DEFAULT_KEY_VALUE, DEFAULT_SCRIPT);
}

static void daemonize(void){
    pid_t pid, sid;

    /* already a daemon */
    if ( getppid() == 1 ) return;

    /* Fork off the parent process */
    pid = fork();
    if (pid < 0) {
        exit(EXIT_FAILURE);
    }
    /* If we got a good PID, then we can exit the parent process. */
    if (pid > 0) {
        exit(EXIT_SUCCESS);
    }

    /* At this point we are executing as the child process */

    /* Change the file mode mask */
    umask(0);

    /* Create a new SID for the child process */
    sid = setsid();
    if (sid < 0) {
        syslog(LOG_ERR, "Daemonization: cannot create a new SID for child process. Exiting");
        exit(EXIT_FAILURE);
    }

    /* Change the current working directory.  This prevents the current
       directory from being locked; hence not being able to remove it. */
    if ((chdir("/")) < 0) {
        syslog(LOG_ERR, "Daemonization: cannot change current working directory. Exiting");
        exit(EXIT_FAILURE);
    }

    /* Redirect standard files to /dev/null */
    freopen( "/dev/null", "r", stdin);
    freopen( "/dev/null", "w", stdout);
    freopen( "/dev/null", "w", stderr);
}

// ======== 
int main(int argc, char** argv){
    bool interactive_mode = false;
    bool help_mode = false;
    bool need_generate_config = false;
    int option = 0;
    pid_t pid;
    struct sigaction sa;

    int logopt = LOG_PID|LOG_DAEMON|LOG_CONS|LOG_NDELAY ;

    halt_requested = false;
    found_event = NULL;

    // ==== Information from configuration file
    char* config_file_name = (char *)DEFAULT_CONFIGURATION_FILE;
    // Script to launch
    char* script_name = (char *)DEFAULT_SCRIPT;
    // Scrit launched in background (asynchronous mode) or not?
    bool asynchronous_launch = false;
    // Values for USB emergency button
    unsigned short vendor_id  = DEFAULT_VENDOR_ID;
    unsigned short product_id = DEFAULT_PRODUCT_ID;
    // Defined but not useful since there is generally only one key ...
    // Do we check a specific key
    bool check_key_value = false;
    // If yes then which one?
    int key_value = DEFAULT_KEY_VALUE ;

    // ==== Handle parameters
    /*
     * -d : debug (interactive mode, no daemonize)
     * -h : usage
     * -f : alternate configuration file
     * -g : generate configuration file and halt
     */

    while((option = getopt (argc, argv, "dhgf:")) != -1){
        switch (option){
        case 'd':
            interactive_mode = true;
            break;
        case 'h':
            help_mode = true;
            break;
        case 'f':
            config_file_name = optarg;
            break;
        case 'g':
            need_generate_config = true;
            break;
        default:
            syslog(LOG_WARNING, "Undefined option '%c' found", option);
            break;
        }
    }
    if (help_mode){
        display_help(argv[0]);
        exit(EXIT_SUCCESS);
    }
    if (need_generate_config){
        generate_configuration_file();
        exit(EXIT_SUCCESS);
    }

    // Display also syslog messages in console
    if (interactive_mode) logopt|=LOG_PERROR;

    // Open Syslog communication
    openlog(argv[0], logopt, LOG_USER);
    syslog(LOG_INFO, "Starting with configuration file %s expected.", config_file_name);

    // ==== read parameters from configuration file
    // prepare defaut value for script name, allocated, before reading configuration file
    script_name = strdup((char *)DEFAULT_SCRIPT);

    if(access(config_file_name,R_OK) == -1){
        syslog(LOG_ERR, "Unable to open configuration file %s\n",config_file_name);
        exit(EXIT_FAILURE);
    }

    script_name = load_parameters_from( config_file_name,
                                        script_name,
                                        &asynchronous_launch,
                                        &vendor_id,
                                        &product_id,
                                        &check_key_value,
                                        &key_value);

    // ==== Find which event source in /dev/input/event* matches the keyboard design by vendorID and productID
    found_event = get_matching_event(vendor_id, product_id);

    if (found_event == NULL) {
        syslog(LOG_ERR, "No device [VendorID=%04x|ProductID=%04x] found", vendor_id, product_id);
        exit(EXIT_FAILURE);
    }

    // Cleanly run the program as a daemon
    if (!interactive_mode){
        daemonize();
    }

    // Configure signal handling to:
    // - avoid child zombie creation in asynchronous modeUSB communication
    // - allow re-reading of configuration file
    // - handle signal and cleanly stop
    memset (&sa, 0, sizeof sa);
    sa.sa_flags = 0;
    // signalHandler will deal with the 3 cases described previously
    sa.sa_handler = signal_handler;
    sigemptyset(&(sa.sa_mask));
    if(sigaction(SIGCHLD, &sa, NULL) != 0){
        syslog(LOG_ERR, "Unable to perform sigaction on SIGCHLD");
        exit(EXIT_FAILURE);
    }        
    if(sigaction(SIGHUP, &sa, NULL) != 0){
        syslog(LOG_ERR, "Unable to perform sigaction on SIGHUP");
        exit(EXIT_FAILURE);
    }        
    if(sigaction(SIGTERM, &sa, NULL) != 0){
        syslog(LOG_ERR, "Unable to perform sigaction on SIGTERM");
        exit(EXIT_FAILURE);
    }        
    if(sigaction(SIGINT, &sa, NULL) != 0){
        syslog(LOG_ERR, "Unable to perform sigaction on SIGINT");
        exit(EXIT_FAILURE);
    }        
    if(sigaction(SIGQUIT, &sa, NULL) != 0){
        syslog(LOG_ERR, "Unable to perform sigaction on SIGQUIT");
        exit(EXIT_FAILURE);
    }        

    // ==== Now that we know the right event source, let's read it in the main loop
    while (!halt_requested){
        if (key_pressed(found_event,check_key_value,key_value, &halt_requested)){
            // == perform action 
            syslog(LOG_INFO, "Key pressed: attempting to execute script");
            if (asynchronous_launch) {
                pid = fork();
                if (pid < 0) {
                    syslog(LOG_ERR, "Unable to launch asynchronously script '%s'",script_name);
                    exit(EXIT_FAILURE);
                }
                /* the child executes command and ends. */
                if (pid == 0) {
                    if(system(script_name) != 0){
                        syslog(LOG_ERR, "Asynchronous execution of script '%s' failed",script_name);
                    } else{
                        syslog(LOG_INFO, "Asynchronous execution of script '%s' succeeded",script_name);
                    }
                    exit(EXIT_SUCCESS);
                }
            }else {
                if (system(script_name) != 0){
                    syslog(LOG_ERR, "Synchronous execution of script '%s' failed",script_name);
                }else{
                    syslog(LOG_INFO, "Synchronous execution of script '%s' succeeded",script_name);
                }
            }
            // End of script execution
        } else{
            // Something wrong happened
            // Either we notify and continue or we stop
            // For now we just stop
            syslog(LOG_ERR, "Problem on input device: halting");
            halt_requested = true;
        }
    }
    free(script_name);
    syslog(LOG_INFO, "Stoping.");
}  
