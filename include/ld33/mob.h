/**
 * @file include/ld33/mob.h
 * 
 * Generic 'object'; May be either the player, a NPC or a shadow
 */
#ifndef __MOB_H__
#define __MOB_H__

#include <GFraMe/gfmError.h>

#include <ld33/game.h>

/** 'Export' the mob struct */
typedef struct stMob mob;

/**
 * Alloc a new mob
 */
gfmRV mob_getNew(mob **ppMob);
/**
 * Free a mob's memory
 */
gfmRV mob_free(mob **ppMob);

/**
 * Initializa a mob; To ease memory management, it doesn't alloc memory;
 * Instead, it used objects from the game's array (which are cleaned when the
 * game exits)
 * 
 * @param  pMob  The mob
 * @param  pGame The game's global contex
 */
gfmRV mob_init(mob *pMob, gameCtx *pGame, int type, int level);

gfmRV mob_setPosition(mob *pMob, int x, int y);

/**
 * Update the sprite and add it to the quadtree
 */
gfmRV mob_update(mob *pMob, gameCtx *pGame);

gfmRV mob_draw(mob *pMob, gameCtx *pGame);

#endif /* __MOB_H__ */

