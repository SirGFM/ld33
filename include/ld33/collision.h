/**
 * @file include/ld33/collision.h
 */
#ifndef __COLLISION_H_
#define __COLLISION_H_

#include <ld33/game.h>

gfmRV collide_obj(gfmObject *pObj, gameCtx *pGame);
gfmRV collide_spr(gfmSprite *pSpr, gameCtx *pGame);

#endif /* __COLLISION_H_ */

