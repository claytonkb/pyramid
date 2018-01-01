// pyramid.c
//

#include "pyramid.h"
#include "interp.h"
#include <getopt.h>


//main
//
int main(int argc, char **argv, char **envp){ // main#

    pyr_cache pyr;

#ifdef DEV_MODE

    int c;

    global_dev_bare_metal = 0;
    global_dev_seed  = EMPTY_CSTRING;
    global_dev_srand = EMPTY_CSTRING;

    while( (c = getopt(argc, argv, "ms:r:")) != -1 ){
        switch(c) {
        case 'm':
            global_dev_bare_metal = 1;
            break;
        case 's':
            global_dev_seed = optarg;
            break;
        case 'r':
            global_dev_srand = optarg;
            break;
        case '?':
            break;
        default:
            _pigs_fly; // Not supposed to happen...
        }
    }

#endif // DEV_MODE


#ifdef PROF_MODE
    int wall_clock_time = interp_pyramid(&pyr, argc, argv, envp);
    _dd(wall_clock_time);
#else
    interp_pyramid(&pyr, argc, argv, envp);
#endif // PROF_MODE

//    _dd(global_dev_abnormal_ctr);

    return NORMAL_EXIT_CODE;

}

// Clayton Bauman 2016

