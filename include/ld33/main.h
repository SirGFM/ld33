/**
 * @file src/main.c
 * 
 * The game's entry point
 */
#ifndef __MAIN_H_
#define __MAIN_H_

#include <ld33/game.h>

int main_getPRNG(gameCtx *pGame);

/**
 * Update all key's states (and the quit flag)
 */
gfmRV main_getKeyStates(gameCtx *pGame);

#endif /* __MAIN_H_ */

