// 
// File: usbemergency.c
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
#include <sys/types.h>      /* WNOHANG */
#include <sys/wait.h>       /* WNOHANG */
#include <signal.h>         /* signal and SIG */

#include <errno.h>          /* errno */
#include <string.h>         /* strerror() strdup() */

#include <sys/stat.h>       /* umask() */
#include <stdbool.h>        /* booleans */
#include <linux/input.h>    /* EVIOCGVERSION ++ */

#include "eventmanager.h"

// ======== Global variables and DEFINEs
#define EXIT_SUCCESS 0
#define EXIT_FAILURE 1
#define PROGRAM_VERSION "0.2"
#define DEFAULT_VENDOR_ID   0x2704
#define DEFAULT_PRODUCT_ID  0x2018

// ======== Local functions
void display_help(char *program_name){
    printf("%s, version %s. Display key codes of an USB input device\n",program_name, PROGRAM_VERSION);
    printf("Presse Ctrl-C to stop.\n");
    printf("Usage: %s -v vendor_ID -p product_ID\n",program_name);
    printf("Parameters\n");
    printf("  -v USB vendor ID of input device, with hexadecimal with 0x prefix (ex: 0x%04x).\n", DEFAULT_VENDOR_ID);
    printf("  -p USB product ID of input device, with hexadecimal 0x prefix (ex: 0x%04x).\n", DEFAULT_PRODUCT_ID);
    printf("Options (or if parameters are missing)\n");
    printf("  -h print help and stop\n");
}

// ======== 
int main(int argc, char** argv){
    // Values for USB emergency button
    unsigned short vendor_id  = DEFAULT_VENDOR_ID;
    unsigned short product_id = DEFAULT_PRODUCT_ID;
    bool help_mode = false;
    bool vendor_id_ok = false;
    bool product_id_ok = false;
    int option = 0;
    char * found_event = NULL;
    bool halt_requested = false;
    int key_value;

    // ==== Handle parameters
    while((option = getopt (argc, argv, "hv:p:")) != -1){
        switch (option){
        case 'h':
            help_mode = true;
            break;
        case 'v':
            errno = 0;
            vendor_id = strtol(optarg, NULL, 0);
            vendor_id_ok = (errno == 0);
            if (!vendor_id_ok) printf("Wrong value for vendor ID: %s\n",optarg);
            break;
        case 'p':
            product_id = strtol(optarg, NULL, 0);
            product_id_ok = (errno == 0);
            if (!product_id_ok) printf("Wrong value for product ID: %s\n",optarg);
            break;
        default:
            printf("Undefined option '%c' found\n", option);
            break;
        }
    }
    if ((help_mode) || !vendor_id_ok || !product_id_ok) {
        display_help(argv[0]);
        exit(EXIT_SUCCESS);
    }
    printf("Trying to find device [VendorID=0x%04x|ProductID=0x%04x]\n", vendor_id, product_id);

    // ==== Find which event source in /dev/input/event* matches the keyboard design by vendorID and productID
    found_event = get_matching_event(vendor_id, product_id);
    if (found_event == NULL) {
        printf("NO device [VendorID=0x%04x|ProductID=0x%04x] found\n", vendor_id, product_id);
        exit(EXIT_FAILURE);
    } else {
        printf("Found device: start key press detection.\n");
    }

    // ==== Now that we know the right event source, let's read it in the main loop
    while (!halt_requested){
        key_value = get_key(found_event);
        if (key_value == -1){
            printf("Problem on input device: halting\n");
            halt_requested = true;
        } else {
            printf("\nKey value: 0x%04x\n",key_value);
        }
    }
}  
