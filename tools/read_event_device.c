// From https://unix.stackexchange.com/questions/94322/is-it-possible-for-a-daemon-i-e-background-process-to-look-for-key-presses-fr/94329#94329

#include <stdio.h>

#include <string.h>     /* strerror() */
#include <errno.h>      /* errno */

#include <fcntl.h>      /* open() */
#include <unistd.h>     /* close() */
#include <sys/ioctl.h>  /* ioctl() */

#include <linux/input.h>    /* EVIOCGVERSION ++ */

#define EV_BUF_SIZE 16

int main(int argc, char *argv[])
{
    int fd, sz;
    unsigned i;
    int ev_ev;

    /* A few examples of information to gather */
    unsigned version;
    unsigned short id[4];                   /* or use struct input_id */
    char name[256] = "N/A";

    struct input_event ev[EV_BUF_SIZE]; /* Read up to N events ata time */

    if (argc < 2) {
        fprintf(stderr,
            "Usage: %s /dev/input/eventN\n"
            "Where X = input device number\n",
            argv[0]
        );
        return EINVAL;
    }

    if ((fd = open(argv[1], O_RDONLY)) < 0) {
        fprintf(stderr,
            "ERR %d:\n"
            "Unable to open `%s'\n"
            "%s\n",
            errno, argv[1], strerror(errno)
        );
    }
    /* Error check here as well. */
    ioctl(fd, EVIOCGVERSION, &version);
    ioctl(fd, EVIOCGID, id); 
    ioctl(fd, EVIOCGNAME(sizeof(name)), name);
    ioctl(fd, EVIOCGBIT(0, EV_MAX), &ev_ev);
    
/*
// B: EV=120013 => 0001 0010 0000 0000 0001 0011 (Event types sup. in this device.)
//                    |   |               |   ||
//                    |   |               |   |+-- EV_SYN (0x00)
//                    |   |               |   +--- EV_KEY (0x01)
//                    |   |               +------- EV_MSC (0x04)
//                    |   +----------------------- EV_LED (0x11)
//                    +--------------------------- EV_REP (0x14)

*/



    fprintf(stderr,
        "Name      : %s\n"
        "Version   : %d.%d.%d\n"
        "ID        : Bus=%04x Vendor=%04x Product=%04x Version=%04x\n"
        "EV        : Bit array=%06x\n"
        "----------\n"
        ,
        name,

        version >> 16,
        (version >> 8) & 0xff,
        version & 0xff,

        id[ID_BUS],
        id[ID_VENDOR],
        id[ID_PRODUCT],
        id[ID_VERSION],
        ev_ev
    );

    /* Loop. Read event file and parse result. */
    for (;;) {
        sz = read(fd, ev, sizeof(struct input_event) * EV_BUF_SIZE);

        if (sz < (int) sizeof(struct input_event)) {
            fprintf(stderr,
                "ERR %d:\n"
                "Reading of `%s' failed\n"
                "%s\n",
                errno, argv[1], strerror(errno)
            );
            goto fine;
        }

        /* Implement code to translate type, code and value */
        for (i = 0; i < sz / sizeof(struct input_event); ++i) {
            fprintf(stderr,
                "%ld.%06ld: "
                "type=%02x "
                "code=%02x "
                "value=%02x\n",
                ev[i].time.tv_sec,
                ev[i].time.tv_usec,
                ev[i].type,
                ev[i].code,
                ev[i].value
            );
        }
    }

fine:
    close(fd);

    return errno;
}

