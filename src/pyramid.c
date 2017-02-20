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
    _dd(global_this_thread_mem->sys_alloc_count);
    _dd(global_this_thread_mem->sys_free_count);
#else
    interp_pyramid(&pyr, argc, argv, envp);
#endif

#ifdef DEV_MODE
    _say("PYRAMID: exiting normally");
#endif

    return NORMAL_EXIT_CODE;

}

// Clayton Bauman 2016

