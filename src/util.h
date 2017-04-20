// util.h
// 

#ifndef UTIL_H
#define UTIL_H

/*****************************************************************************
 *                                                                           *
 *                               DEV UTILS                                   *
 *                                                                           *
 ****************************************************************************/

#ifdef DEV_MODE
void util_rsvd(void);
void util_bare_metal_prompt(pyr_cache *this_pyr, mword *init_ptr);
void util_get_line(char *buffer, FILE *stream);
void util_bare_metal_menu(void);
#endif

#endif //UTIL_H

// Clayton Bauman 2016

