/* 
 * File: parameters.c
 * Goal: Parameters management using iniparser library
 * Author: Stéphane Mouton
 */

#include "parameters.h"
#include <syslog.h>

char* load_parameters_from(char* config_file_name,
                          char* script_name,
                          bool *asynchronous_launch,
                          unsigned short *vendor_id,
                          unsigned short *product_id,
                          bool *check_key_value,
                          int *key_value){
        dictionary* ini ;
        syslog(LOG_INFO, "loading parameters.");
        ini = iniparser_load(config_file_name);
        if (ini==NULL) {
                syslog(LOG_ERR, "Cannot parse file %s. Exiting",config_file_name);
                exit(EXIT_FAILURE);
        }
        /* Get input device attributes */
        *vendor_id  = iniparser_getint(ini, "device:vendor_id", DEFAULT_VENDOR_ID);
        *product_id = iniparser_getint(ini, "device:product_id", DEFAULT_PRODUCT_ID);
        *check_key_value = iniparser_getboolean(ini, "device:expect_specific_key", false);
        *key_value = iniparser_getint(ini, "device:key_value", DEFAULT_KEY_VALUE);

        /* Get Action parameters, related to script to execute */
        free(script_name);
        script_name = strdup(iniparser_getstring(ini, "action:script", (char *)DEFAULT_SCRIPT));
        *asynchronous_launch = iniparser_getboolean(ini, "action:asynchronous_launch", false);

        syslog(LOG_DEBUG, "Parameters: Device = [Vendor:%04x|Product:%04x]",*vendor_id, *product_id);
        syslog(LOG_DEBUG, "Parameters: Expect specific key ? %d (key value = %04x)", *check_key_value, *key_value);
        syslog(LOG_DEBUG, "Parameters: Script='%s'",script_name);
        syslog(LOG_DEBUG, "Parameters: Asynchronous launch ? %d", *asynchronous_launch);

        // As script name is stored in allocated memory, we can free memory used to read configuration file
        iniparser_freedict(ini);
        return(script_name);
}
