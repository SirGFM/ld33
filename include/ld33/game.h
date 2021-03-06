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
#include <GFraMe/gfmGenericArray.h>
#include <GFraMe/gfmGroup.h>
#include <GFraMe/gfmObject.h>
#include <GFraMe/gfmQuadtree.h>
#include <GFraMe/gfmSprite.h>
#include <GFraMe/gfmSpriteset.h>
#include <GFraMe/gfmTypes.h>

/** Types... */
#define player       gfmType_reserved_2
#define npc          gfmType_reserved_3
#define shadow       gfmType_reserved_4
#define scan         gfmType_reserved_5
#define atk          gfmType_reserved_6
#define wall         gfmType_reserved_7
#define collideable  gfmType_reserved_8
#define win          gfmType_reserved_9

/** Create two array types: one for objects and another for sprites */
gfmGenArr_define(gfmObject);

enum enStateTypes {
    state_none = 0,
    state_introstate,
    state_blastate,
    state_playstate,
    state_max
};
typedef enum enStateTypes stateTypes;

/**  */
struct stGameCtx {
    /** The library's main context */
    gfmCtx *pCtx;
    /** Array of objects */
    gfmGenArr_var(gfmObject, pObjs);
    /** Group for rendering everything */
    gfmGroup *pRender;
    /** 4x4 spriteset */
    gfmSpriteset *pSset4x4;
    /** 8x8 spriteset */
    gfmSpriteset *pSset8x8;
    /** 32x32 spriteset */
    gfmSpriteset *pSset32x32;
    /** 256x128 spriteset */
    gfmSpriteset *pSset256x128;
    /** The game's quadtree */
    gfmQuadtreeRoot *pQt;
    /** Pointer to the current state's struct */
    void *pState;
    /** Definition of the current state's type */
    stateTypes state;
    /** Whether we should quit from the current state */
    int quitState;
    /** Maximum number of particles on screen */
    int maxParts;
    /** PRNG seed */
    unsigned int seed;
    int didLose;
    int didWin;
    /** Input handles */
    int handle_down;
    int handle_left;
    int handle_right;
    int handle_up;
    int handle_atk;
    int handle_quit;
    /** Input states */
    gfmInputState state_down;
    int num_down;
    gfmInputState state_left;
    int num_left;
    gfmInputState state_right;
    int num_right;
    gfmInputState state_up;
    int num_up;
    gfmInputState state_atk;
    int num_atk;
    gfmInputState state_quit;
    int num_quit;
    /** Audios */
    int audioFreq;
    int song;
    int expl;
    int wall_hit;
    int slime_hit;
    int slime_death;
    int pl_hit;
    int pl_death;
#ifdef EMSCRIPT
    /** Whether this state has been initialized in a previous loop */
    int isInit;
#endif
};
typedef struct stGameCtx gameCtx;

#endif /* __GAME_H__ */

