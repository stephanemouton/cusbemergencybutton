/* 
 * File: parameters.h
 * Goal: Header for parameters management
 * Author: Stéphane Mouton
 */

#ifndef _PARAMETERS_H
#define	_PARAMETERS_H

#define DEFAULT_SCRIPT      "/bin/true"
#define DEFAULT_VENDOR_ID   0x2704
#define DEFAULT_PRODUCT_ID  0x2018
#define DEFAULT_KEY_VALUE   0x7002c

// Value for CASE Logic USB keypad (Model CLNP-1)
// #define DEFAULT_VENDOR_ID   0x05a4
// #define DEFAULT_PRODUCT_ID  0x9759
// '5' key
// define DEFAULT_KEY_VALUE 0x7005d

#include <stdbool.h>    /* booleans */
#include <unistd.h>     /* file detection */
#include <iniparser.h>  /* parsing configuration file */

bool load_parameters_from( char* config_file_name,
                            char* script_name,
                            bool *asynchronous_launch,
                            unsigned short *vendor_id,
                            unsigned short *product_id,
                            bool *check_key_value,
                            int *key_value);

#endif	/* _PARAMETERS_H */
