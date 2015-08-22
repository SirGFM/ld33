/**
 * @file include/ld33/game.h
 * 
 * Game's basic struct; Keep track of everything used by different objects (like
 * the game's context, textures, sounds, the quadtree, etc...)
 */
#ifndef __GAME_H__
#define __GAME_H__

#include <GFraMe/gframe.h>
#include <GFraMe/gfmAssert.h>
#include <GFraMe/gfmError.h>
#include <GFraMe/gfmSpriteset.h>
#include <GFraMe/gfmQuadtree.h>

enum enStateTypes {
    state_none = 0,
    state_playstate,
    state_max
};
typedef enum enStateTypes stateTypes;

/**  */
struct stGameCtx {
    /** The library's main context */
    gfmCtx *pCtx;
    /** 8x8 spriteset */
    gfmSpriteset *pSset8x8;
    /** 32x32 spriteset */
    gfmSpriteset *pSset32x32;
    /** The game's quadtree */
    gfmQuadtreeRoot *pQt;
    /** Pointer to the current state's struct */
    void *pState;
    /** Definition of the current state's type */
    stateTypes state;
    /** Whether we should quit from the current state */
    int quitState;
};
typedef struct stGameCtx gameCtx;

#endif /* __GAME_H__ */

