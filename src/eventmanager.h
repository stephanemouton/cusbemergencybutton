/* 
 * File: eventmanager.h
 * Goal: gather all functions related to event management
 */

#ifndef _EVENTMANAGER_H
#define	_EVENTMANAGER_H

#include <stdbool.h>

bool event_match_id(char *event_path, unsigned short vendor_id, unsigned short product_id);
char * get_matching_event(unsigned short vendor_id, unsigned short product_id);
int  get_key(char * found_event);
bool key_pressed(char * found_event, bool check_key_value, int key_value, bool * halt_requested);

#endif	/* #ifndef _EVENTMANAGER_H */
