/**
 * @file include/ld33/playstate.h
 * 
 * Game's main state, where all the fun should happen
 */
#ifndef __PLAYSTATE_H__
#define __PLAYSTATE_H__

#include <ld33/game.h>

gfmRV playstate_setWin(gameCtx *pGame);

/**
 * Initialize the playstate and loop it
 */
gfmRV playstate_loop(gameCtx *pGame);

#endif /* __PLAYSTATE_H__ */

