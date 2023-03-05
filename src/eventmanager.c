/* 
 * File: eventmanager.h
 * Goal: gather all functions related to event management
 * Documentation: 
 *  * Article of Linux Journal on events https://www.linuxjournal.com/article/6429
 *  * Starting point https://stackoverflow.com/questions/20943322/accessing-keys-from-linux-input-device
 *  * Tips and tricks on events https://unix.stackexchange.com/questions/94322/is-it-possible-for-a-daemon-i-e-background-process-to-look-for-key-presses-fr/94329#94329
 */


# include "eventmanager.h"

#include <stdio.h>      /* stderr */
#include <syslog.h>

#include <glob.h>       /* glob()*/

#include <errno.h>      /* errno */
#include <string.h>     /* strerror() */

#include <fcntl.h>      /* open() */
#include <unistd.h>     /* close() */
#include <sys/ioctl.h>  /* ioctl() */
#include <linux/input.h>    /* EVIOCGVERSION ++ */

#define EV_BUF_SIZE 16
// Wait 0.25s before testing a new key press
#define ANTI_BOUNCE_FACTOR 250000

bool tstamp_set = false;
struct input_event ev[EV_BUF_SIZE]; /* Read up to N events at a time */
long timestamp_s;
long difference_t;

bool event_match_id(char *event_path, unsigned short vendor_id, unsigned short product_id){
    int fd;
    unsigned short event_id[4];                   /* info on event */
    int event_features;
    unsigned version;

    if ((fd = open(event_path, O_RDONLY)) < 0) {
        syslog(LOG_ERR, "Error %d (%s): unable to open %s",errno, strerror(errno), event_path);
    }

    ioctl(fd, EVIOCGVERSION, &version);
    ioctl(fd, EVIOCGID, &event_id); 
    ioctl(fd, EVIOCGBIT(0, EV_MAX), &event_features);

    close(fd);

    // A keyboard has always the following flag in the event features.
    // -- EV=120013 => 0001 0010 0000 0000 0001 0011 (Event types sup. in this device.)
    //                    |   |               |   ||
    //                    |   |               |   |+-- EV_SYN (0x00)
    //                    |   |               |   +--- EV_KEY (0x01)
    //                    |   |               +------- EV_MSC (0x04)
    //                    |   +----------------------- EV_LED (0x11)
    //                    +--------------------------- EV_REP (0x14)
    // Therefore, value 0x120013 is expected in addition to Vendor and product IDs.
    /*
    printf("              --> [%04x|%04x] EV=%06x -> %s \n", \
            event_id[ID_VENDOR], \
            event_id[ID_PRODUCT], \
            event_features, \
            event_path);
    */
    return ((vendor_id  == event_id[ID_VENDOR]) && \
            (product_id == event_id[ID_PRODUCT]) && \
            (event_features == 0x120013));
}

char * get_matching_event(unsigned short vendor_id, unsigned short product_id){
    // == List all entries in /dev/input and for each one check if criteria matched 
    glob_t glob_buffer;
    int glob_result;
    char glob_target[] = "/dev/input/event*";
    bool event_found;
    char ** event_path;
    char * found_event=NULL;

    glob_result = glob(glob_target, 0, NULL, &glob_buffer);
    if (glob_result != 0){
        syslog(LOG_ERR, "Error %d (%s): unable to glob %s",errno, strerror(errno), glob_target);
    }else{
        event_found = false;
        event_path = glob_buffer.gl_pathv;
        while(*event_path && !event_found){
            event_found = event_match_id(*event_path,vendor_id,product_id);
            if (event_found){
                found_event = *event_path;
            } else {
                event_path++;
            }
        }
    }
    syslog(LOG_DEBUG, "Matching of [%04x|%04x] is path %s", vendor_id, product_id, found_event);
    return(found_event);
}

bool key_pressed(char * found_event, bool check_key_value, int key_value, bool * halt_requested){
    int fd;
    unsigned int size;
    bool right_key;
    unsigned int j;
    if ((fd = open(found_event, O_RDONLY)) < 0) {
        syslog(LOG_ERR, "Error %d (%s): unable to open '%s'", errno, strerror(errno), found_event);
        return(false);
    }
    while (!*halt_requested){
        size = read(fd, ev, sizeof(struct input_event) * EV_BUF_SIZE);
        // If something wrong happened ot signal was caught, size < 0 and we need to skip
        if (size < 0){
            syslog(LOG_WARNING, "Problem while reading key: size of read chars is %d. Halting",size);
            *halt_requested = true;
            close(fd);
            return(false);
        } else {
            if (size < (int) sizeof(struct input_event)) {
                syslog(LOG_ERR, "Error %d (%s): reading of '%s' failed", errno, strerror(errno), found_event);
                *halt_requested=true;
            } else{
                // == Key pressed: what to do now?
                // First assume that the right key was pressed because we won't necessarily check its value
                if (check_key_value){
                    right_key = false;
                    // Empty the buffer 
                    j=0;
                    while(j < size / sizeof(struct input_event)) {
                        if (ev[j].value == key_value){
                            right_key = true;
                            j=size;
                        }
                        j++;
                    }
                } else{
                    right_key = true;
                }
                if (right_key){
                    /* Perform anti bounce */
                    if (!tstamp_set){
                        tstamp_set = true;
                        timestamp_s = (ev[0].time.tv_sec * 1000000) + ev[0].time.tv_usec ;
                        /*
                        printf("Tstamp s : %ld  us : %ld\n", ev[0].time.tv_sec, ev[0].time.tv_usec);
                        printf("Tstamp initial : %ld\n", timestamp_s);
                        */
                        /* == Right key pressed */
                        close(fd);
                        return(true);
                    } else {
                        /*
                        printf("Tstamp s : %ld  us : %ld\n", ev[0].time.tv_sec, ev[0].time.tv_usec);
                        printf("     Tstamp_s : %ld\n", timestamp_s);
                        printf("Tstamp actuel : %ld\n", (ev[0].time.tv_sec * 1000000 + ev[0].time.tv_usec));
                        */
                        difference_t = (ev[0].time.tv_sec * 1000000 + ev[0].time.tv_usec) - timestamp_s;
                        // printf("   Difference : %ld  \n", difference_t);
                        if (difference_t > ANTI_BOUNCE_FACTOR){
                            tstamp_set = false;
                        }
                    }
                }
            }
        }
    }
    close(fd);
    return(false);
}
