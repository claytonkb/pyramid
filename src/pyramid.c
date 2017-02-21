// pyramid.c
//

#include "pyramid.h"
#include "interp.h"

//main
//
int main(int argc, char **argv, char **envp){ // main#

    pyr_cache pyr;

#ifdef PROF_MODE
    int wall_clock_time = interp_pyramid(&pyr, argc, argv, envp);
    _dd(wall_clock_time);
#else
    interp_pyramid(&pyr, argc, argv, envp);
#endif

    return NORMAL_EXIT_CODE;

}

// Clayton Bauman 2016

