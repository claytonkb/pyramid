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
    _dd(global_mem_sys_alloc_total);
    _dd(global_mem_sys_free_total);
    _dd(global_mem_sys_alloc_count);
    _dd(global_mem_sys_free_count);
#else
    interp_pyramid(&pyr, argc, argv, envp);
#endif

#ifdef DEV_MODE
    _say("PYRAMID: exiting normally");
#endif

    return NORMAL_EXIT_CODE;

}

// Clayton Bauman 2016

