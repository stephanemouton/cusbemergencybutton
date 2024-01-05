/* 
 * File: parameters.c
 * Goal: Parameters management using iniparser library
 * Author: Stéphane Mouton
 */

#include "parameters.h"
#include <syslog.h>

bool load_parameters_from(char* config_file_name,
                            char** script_name,
                            bool *asynchronous_launch,
                            unsigned short *vendor_id,
                            unsigned short *product_id,
                            bool *check_key_value,
                            int *key_value){
    dictionary* ini ;
    syslog(LOG_INFO, "loading parameters.");
    if (access(config_file_name, F_OK) != 0) {
        // file doesn't exist
        syslog(LOG_ERR, "Cannot find file %s . Exiting",config_file_name);
        return(false);
    }
    if (access(config_file_name, R_OK) != 0) {
        // file is not readable
        syslog(LOG_ERR, "Not enough rights to read file %s . Exiting",config_file_name);
        return(false);
    }
    ini = iniparser_load(config_file_name);
    if (ini==NULL) {
        syslog(LOG_ERR, "Cannot parse file %s . Exiting",config_file_name);
        return(false);
    }
    /* Get input device attributes and action parameters */
    *vendor_id  = iniparser_getint(ini, "device:vendor_id", DEFAULT_VENDOR_ID);
    *product_id = iniparser_getint(ini, "device:product_id", DEFAULT_PRODUCT_ID);
    *check_key_value = iniparser_getboolean(ini, "device:expect_specific_key", false);
    *key_value = iniparser_getint(ini, "device:key_value", DEFAULT_KEY_VALUE);
    *asynchronous_launch = iniparser_getboolean(ini, "action:asynchronous_launch", false);
    // Special case of the string containing script to launch:
    // The iniparser library allocates memory either for the content of fil or for default value.
    // But the library will free all memory after being used.
    // Therefore we need to copy the returned string.
    *script_name = strdup(iniparser_getstring(ini, "action:script", (char *)DEFAULT_SCRIPT));

    syslog(LOG_DEBUG, "Parameters: Device = [Vendor:%04x|Product:%04x]", *vendor_id, *product_id);
    syslog(LOG_DEBUG, "Parameters: Expect specific key ? %d (key value = %04x)", *check_key_value, *key_value);
    syslog(LOG_DEBUG, "Parameters: Script='%s'", *script_name);
    syslog(LOG_DEBUG, "Parameters: Asynchronous launch ? %d", *asynchronous_launch);

    // As script name is stored in allocated memory, we can free memory used to read configuration file
    iniparser_freedict(ini);
    return(true);
}
