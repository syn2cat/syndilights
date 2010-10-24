/* BPacman: Pacman module for Blinkenlights
 *
 * Copyright (c) 2002  1stein <1stein@1stein.no-ip.com>
 *
 * based on Test implementation for a BModule by the Blinkenlights Crew
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */

#include "config.h"

#include <string.h>
#include <stdlib.h>

#include <glib-object.h>
#include <gmodule.h>

#include <blib/blib.h>

#define BPacmanVerMaj 1
#define BPacmanVerMin 0
#define BPacmanVerRev 3

//minimum size and color count of display
#define BPacmanSizeXMin 25
#define BPacmanSizeYMin 19
#define BPacmanColorCntMin 4
//number of lives for pacman
#define BPacmanLives 3
//speed of game
#define BPacmanTicks 150
//number of ticks pacman is dead after contact with monster
#define BPacmanDeadTime 10
//position of eaten monsters
#define BPacmanMonsterNirvana -100
//the colors
#define BPacmanColorEmpty( MaxColor ) (0)
#define BPacmanColorWall( MaxColor ) ((MaxColor) * 2 / 3)
#define BPacmanColorPoint( MaxColor ) ((MaxColor) / 3)
#define BPacmanColorPacmanDead( MaxColor ) ((MaxColor) / 2)
#define BPacmanColorPacmanAlive( MaxColor ) (MaxColor)
#define BPacmanColorMonsterDead( MaxColor ) ((MaxColor) / 2)
#define BPacmanColorMonsterAlive( MaxColor ) (MaxColor)
//start position and direction of pacman
#define BPacmanStartPosX -1
#define BPacmanStartPosY 8
#define BPacmanStartDirX 1
#define BPacmanStartDirY 0
//start positions and directions of monsters
#define BPacmanMonsterCnt 2
#define BPacmanMonster0StartPosX 12
#define BPacmanMonster0StartPosY -1
#define BPacmanMonster0StartDirX 0
#define BPacmanMonster0StartDirY 1
#define BPacmanMonster1StartPosX 12
#define BPacmanMonster1StartPosY 19
#define BPacmanMonster1StartDirX 0
#define BPacmanMonster1StartDirY -1
//the walls of the game-field
int BPacmanWalls[BPacmanSizeYMin][BPacmanSizeXMin] = {
  {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
  {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
  {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
  {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
  {1, 0, 0, 0, 1, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 1, 0, 0, 0, 1},
  {1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1},
  {1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1},
  {0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0},
  {0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0},
  {0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0},
  {1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1},
  {1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1},
  {1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1},
  {1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1},
  {1, 0, 0, 0, 1, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 1, 0, 0, 0, 1},
  {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
  {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
  {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
  {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}
};
//the positions where changing direction is allowed
int BPacmanChDir[BPacmanSizeYMin][BPacmanSizeXMin] = {
  {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 1, 0, 0, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 0, 0, 1, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 1, 0, 0, 0, 1, 0, 1, 0, 1, 0, 0, 0, 1, 0, 1, 0, 1, 0, 0, 0, 1, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 1, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}
};
//the points of the game-field
int BPacmanPoints[BPacmanSizeYMin][BPacmanSizeXMin] = {
  {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 1, 0, 0, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 0, 0, 1, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
  {1, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1},
  {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 1, 0, 0, 0, 1, 0, 1, 0, 1, 0, 0, 0, 1, 0, 1, 0, 1, 0, 0, 0, 1, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 1, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}
};

#ifdef PACMAN_DEBUG
#define dbg_print g_print
#else
static inline void
dbg_print (char * Fmt, ...)
{
}
#endif

#define B_TYPE_PACMAN_MODULE         (b_type_pacman_module)
#define B_PACMAN_MODULE(obj)         (G_TYPE_CHECK_INSTANCE_CAST ((obj), B_TYPE_PACMAN_MODULE, BPacmanModule))
#define B_PACMAN_MODULE_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST ((klass), B_TYPE_PACMAN_MODULE, BPacmanModuleClass))
#define B_IS_PACMAN_MODULE(obj)      (G_TYPE_CHECK_INSTANCE_TYPE ((obj), B_TYPE_PACMAN_MODULE))

typedef struct _BPacmanModule BPacmanModule;
typedef struct _BPacmanModuleClass BPacmanModuleClass;

struct _BPacmanModule
{
  BModule parent_instance;

  int OfsX, OfsY;		//offset of top left corner of game-field
  int MaxColor;			//maximum color value
  int Points[BPacmanSizeYMin][BPacmanSizeXMin];	//points in game field
  int PointsLeft;		//numer of points left
  int PosX, PosY, CurDirX, CurDirY, NextDirX, NextDirY;	//position, current direction and next direction of pacman
  int MouthOpen;		//pacman's mouth state
  int DeadCnt;			//counter how long pacman is dead, 0 if not dead
  int MonsterPosX[BPacmanMonsterCnt], MonsterPosY[BPacmanMonsterCnt];	//position of monsters
  int MonsterDirX[BPacmanMonsterCnt], MonsterDirY[BPacmanMonsterCnt];	//direction of monsters
  int MonstersDead;		//!0 if monsters are dead
  int Lives;                    //rest of pacman's lives
  int player_device_id;
};

struct _BPacmanModuleClass
{
  BModuleClass parent_class;
};

static GType    b_pacman_module_get_type   (GTypeModule         *module);
static void     b_pacman_module_class_init (BPacmanModuleClass  *klass);
static void     b_pacman_module_init       (BPacmanModule       *test_module);
static gboolean b_pacman_module_query      (gint                 width,
                                            gint                 height,
                                            gint                 channels,
                                            gint                 maxval);
static gboolean b_pacman_module_prepare    (BModule             *module,
                                            GError             **error);
static void     b_pacman_module_start      (BModule             *module);
static void     b_pacman_module_event      (BModule             *module,
                                            BModuleEvent        *event);
static gint     b_pacman_module_tick       (BModule             *module);
static void     b_pacman_module_describe   (BModule             *module,
                                            const gchar        **title,
                                            const gchar        **description,
                                            const gchar        **author);


static GType b_type_pacman_module = 0;


G_MODULE_EXPORT gboolean
b_module_register (GTypeModule * module)
{
  b_pacman_module_get_type (module);

  return TRUE;
}

GType
b_pacman_module_get_type (GTypeModule * module)
{
  if (!b_type_pacman_module)
    {
      static const GTypeInfo pacman_module_info = {
	sizeof (BPacmanModuleClass),
	NULL,			/* base_init */
	NULL,			/* base_finalize */
	(GClassInitFunc) b_pacman_module_class_init,
	NULL,			/* class_finalize */
	NULL,			/* class_data */
	sizeof (BPacmanModule),
	0,			/* n_preallocs */
	(GInstanceInitFunc) b_pacman_module_init,
      };

      /* !!!!!!!!! The name given in the next function MUST be unique! */

      b_type_pacman_module = g_type_module_register_type (module,
							  B_TYPE_MODULE,
							  "BPacman",
							  &pacman_module_info,
							  0);
    }

  return b_type_pacman_module;
}

//check a picel
//returns 1 if pixel in game field or 0 if it is not in game field
static inline int
BPacmanCheckPixel (int X, int Y)
{
  return (X >= 0 && X < BPacmanSizeXMin && Y >= 0 && Y < BPacmanSizeYMin);
}

//check a position (for pacman or monster)
//returns 1 if position is free of walls or 0 if there is a wall at this position
int
BPacmanCheckPos (int X, int Y)
{
  int XX, YY;

  for (YY = -1; YY <= 1; YY++)
    for (XX = -1; XX <= 1; XX++)
      if (BPacmanCheckPixel (X + XX, Y + YY))
	if (BPacmanWalls[Y + YY][X + XX])
	  return 0;
  return 1;
}

//output a pixel
void
BPacmanOutputPixel (BPacmanModule *pPacmanModule, int X, int Y, int Color)
{
  if (BPacmanCheckPixel (X, Y))
    b_module_draw_point ((BModule *) pPacmanModule,
			 X + pPacmanModule->OfsX, Y + pPacmanModule->OfsY,
			 Color);
}

//output current picture
void
BPacmanOutput (BPacmanModule *pPacmanModule)
{
  int X, Y, I, Color;

  //empty the screen
  b_module_fill ((BModule *) pPacmanModule,
		 BPacmanColorEmpty (pPacmanModule->MaxColor));

  //put walls and points onto screen
  for (Y = 0; Y < BPacmanSizeYMin; Y++)
    {
      for (X = 0; X < BPacmanSizeXMin; X++)
	{
	  //wall
	  if (BPacmanWalls[Y][X])
	    b_module_draw_point ((BModule *) pPacmanModule,
				 X + pPacmanModule->OfsX,
				 Y + pPacmanModule->OfsY,
				 BPacmanColorWall (pPacmanModule->MaxColor));
	  //point
	  if (pPacmanModule->Points[Y][X])
	    b_module_draw_point ((BModule *) pPacmanModule,
				 X + pPacmanModule->OfsX,
				 Y + pPacmanModule->OfsY,
				 BPacmanColorPoint (pPacmanModule->MaxColor));
	}
    }

  //get pacman color (dead or alive)
  if (pPacmanModule->DeadCnt > 0)
    Color = BPacmanColorPacmanDead (pPacmanModule->MaxColor);
  else
    Color = BPacmanColorPacmanAlive (pPacmanModule->MaxColor);
  //draw pacman
  if (!pPacmanModule->MouthOpen)
    BPacmanOutputPixel (pPacmanModule, pPacmanModule->PosX,
			pPacmanModule->PosY, Color);
  if (!pPacmanModule->MouthOpen || pPacmanModule->CurDirX != -1)
    BPacmanOutputPixel (pPacmanModule, pPacmanModule->PosX - 1,
			pPacmanModule->PosY, Color);
  if (!pPacmanModule->MouthOpen || pPacmanModule->CurDirX != 1)
    BPacmanOutputPixel (pPacmanModule, pPacmanModule->PosX + 1,
			pPacmanModule->PosY, Color);
  if (!pPacmanModule->MouthOpen || pPacmanModule->CurDirY != -1)
    BPacmanOutputPixel (pPacmanModule, pPacmanModule->PosX,
			pPacmanModule->PosY - 1, Color);
  if (!pPacmanModule->MouthOpen || pPacmanModule->CurDirY != 1)
    BPacmanOutputPixel (pPacmanModule, pPacmanModule->PosX,
			pPacmanModule->PosY + 1, Color);

  //get monster color (dead or alive)
  if (pPacmanModule->MonstersDead)
    Color = BPacmanColorMonsterDead (pPacmanModule->MaxColor);
  else
    Color = BPacmanColorMonsterAlive (pPacmanModule->MaxColor);
  //draw monsters
  for (I = 0; I < BPacmanMonsterCnt; I++)
    {
      BPacmanOutputPixel (pPacmanModule, pPacmanModule->MonsterPosX[I],
			  pPacmanModule->MonsterPosY[I] - 1, Color);
      BPacmanOutputPixel (pPacmanModule, pPacmanModule->MonsterPosX[I] - 1,
			  pPacmanModule->MonsterPosY[I], Color);
      BPacmanOutputPixel (pPacmanModule, pPacmanModule->MonsterPosX[I],
			  pPacmanModule->MonsterPosY[I], Color);
      BPacmanOutputPixel (pPacmanModule, pPacmanModule->MonsterPosX[I] + 1,
			  pPacmanModule->MonsterPosY[I], Color);
      BPacmanOutputPixel (pPacmanModule, pPacmanModule->MonsterPosX[I] - 1,
			  pPacmanModule->MonsterPosY[I] + 1, Color);
      BPacmanOutputPixel (pPacmanModule, pPacmanModule->MonsterPosX[I] + 1,
			  pPacmanModule->MonsterPosY[I] + 1, Color);
    }

  //update screen
  b_module_paint ((BModule *) pPacmanModule);
}

//start a new game
void
BPacmanNewGame (BPacmanModule *pPacmanModule)
{
  int X, Y;

  //copy points into local points array and count them
  pPacmanModule->PointsLeft = 0;
  for (Y = 0; Y < BPacmanSizeYMin; Y++)
    {
      for (X = 0; X < BPacmanSizeXMin; X++)
	{
	  pPacmanModule->Points[Y][X] = BPacmanPoints[Y][X];
	  if (BPacmanPoints[Y][X])
	    pPacmanModule->PointsLeft++;
	}
    }

  //set start position and direction of pacman
  pPacmanModule->PosX = BPacmanStartPosX;
  pPacmanModule->PosY = BPacmanStartPosY;
  pPacmanModule->CurDirX = BPacmanStartDirX;
  pPacmanModule->CurDirY = BPacmanStartDirY;
  pPacmanModule->NextDirX = BPacmanStartDirX;
  pPacmanModule->NextDirY = BPacmanStartDirY;
  pPacmanModule->MouthOpen = 0;
  pPacmanModule->DeadCnt = 0;
  //set start position and direction of monsters
  pPacmanModule->MonsterPosX[0] = BPacmanMonster0StartPosX;
  pPacmanModule->MonsterPosY[0] = BPacmanMonster0StartPosY;
  pPacmanModule->MonsterDirX[0] = BPacmanMonster0StartDirX;
  pPacmanModule->MonsterDirY[0] = BPacmanMonster0StartDirY;
  pPacmanModule->MonsterPosX[1] = BPacmanMonster1StartPosX;
  pPacmanModule->MonsterPosY[1] = BPacmanMonster1StartPosY;
  pPacmanModule->MonsterDirX[1] = BPacmanMonster1StartDirX;
  pPacmanModule->MonsterDirY[1] = BPacmanMonster1StartDirY;
  pPacmanModule->MonstersDead = 0;

  dbg_print ("BPacman: new game\n");

  //output current picture
  BPacmanOutput (pPacmanModule);
}

//key-procedure
void
BPacmanKey (BPacmanModule *pPacmanModule,
            BModuleKey     Key)
{
  switch (Key)
    {
    case B_KEY_2:
      //set pacman's next direction to upwards
      pPacmanModule->NextDirX = 0;
      pPacmanModule->NextDirY = -1;
      break;
    case B_KEY_4:
      //set pacman's next direction to leftwards
      pPacmanModule->NextDirX = -1;
      pPacmanModule->NextDirY = 0;
      break;
    case B_KEY_6:
      //set pacman's next direction to rightwards
      pPacmanModule->NextDirX = 1;
      pPacmanModule->NextDirY = 0;
      break;
    case B_KEY_8:
      //set pacman's next direction to downwards
      pPacmanModule->NextDirX = 0;
      pPacmanModule->NextDirY = 1;
      break;

    default:
      break;
    }

  //output current picture
  BPacmanOutput (pPacmanModule);
}

//tick-procedure
//returns 1 if game over or 0 if game not over
int
BPacmanTick (BPacmanModule *pPacmanModule)
{
  int X, Y, I, J, Left, Right, Up, Down, Cnt, DirX[4], DirY[4];

  //toggle mouth state
  pPacmanModule->MouthOpen = !pPacmanModule->MouthOpen;

  //mouth now open
  if (pPacmanModule->MouthOpen)
    {
      //pacman is dead
      if (pPacmanModule->DeadCnt > 0)
	{
	  //decrement dead count
	  pPacmanModule->DeadCnt--;
	  if (pPacmanModule->DeadCnt == 0)
          {
            //another life
            if( pPacmanModule->Lives > 0 )
            {
              dbg_print( "BPacman: Pacman is alive again - %d live(s) left\n", pPacmanModule->Lives );
            }
            //game over
            else
            {
              dbg_print( "BPacman: Pacman has no lives left - game over\n" );
              //request end
              b_module_request_stop( (BModule *)pPacmanModule );
	      //do nothing else and return game over
	      return 1;
            }
          }
	}
      //pacman is alive
      else
	{
	  //pacman is at a position where changing direction is allowed
	  if (BPacmanCheckPixel (pPacmanModule->PosX, pPacmanModule->PosY))
	    {
	      if (BPacmanChDir[pPacmanModule->PosY][pPacmanModule->PosX])
		{
		  //change direction to new one
		  pPacmanModule->CurDirX = pPacmanModule->NextDirX;
		  pPacmanModule->CurDirY = pPacmanModule->NextDirY;
		}
	    }

	  //get new position (move in current direction)
	  X = pPacmanModule->PosX + pPacmanModule->CurDirX;
	  Y = pPacmanModule->PosY + pPacmanModule->CurDirY;
	  //check if there is a wall at the new position
	  if (BPacmanCheckPos (X, Y))
	    {
	      //set pacman to new position
	      pPacmanModule->PosX = X;
	      pPacmanModule->PosY = Y;
	      //wrap around
	      if (pPacmanModule->PosX < -1 && pPacmanModule->CurDirX < 0)
		pPacmanModule->PosX = BPacmanSizeXMin;
	      if (pPacmanModule->PosX > BPacmanSizeXMin
		  && pPacmanModule->CurDirX > 0)
		pPacmanModule->PosX = -1;
	      if (pPacmanModule->PosY < -1 && pPacmanModule->CurDirY < 0)
		pPacmanModule->PosY = BPacmanSizeYMin;
	      if (pPacmanModule->PosY > BPacmanSizeYMin
		  && pPacmanModule->CurDirY > 0)
		pPacmanModule->PosY = -1;

	      //point at this position
	      if (BPacmanCheckPixel
		  (pPacmanModule->PosX, pPacmanModule->PosY))
		{
		  if (pPacmanModule->
		      Points[pPacmanModule->PosY][pPacmanModule->PosX])
		    {
		      //eat point
		      pPacmanModule->Points[pPacmanModule->
					    PosY][pPacmanModule->PosX] = 0;
		      pPacmanModule->PointsLeft--;
		      //all points eaten
		      if (pPacmanModule->PointsLeft == 0)
			{
			  //monsters are dead now
			  pPacmanModule->MonstersDead = 1;
			}
		    }
		}
	    }
	}

      //monsters are alive
      if (!pPacmanModule->MonstersDead)
	{
	  //move monsters
	  for (I = 0; I < BPacmanMonsterCnt; I++)
	    {
	      //monster is at a position where changing direction is allowed
	      if (BPacmanCheckPixel
		  (pPacmanModule->MonsterPosX[I],
		   pPacmanModule->MonsterPosY[I]))
		{
		  if (BPacmanChDir[pPacmanModule->MonsterPosY[I]]
		      [pPacmanModule->MonsterPosX[I]])
		    {
		      //get possible directions
		      Left =
			BPacmanCheckPos (pPacmanModule->MonsterPosX[I] - 1,
					 pPacmanModule->MonsterPosY[I]);
		      Right =
			BPacmanCheckPos (pPacmanModule->MonsterPosX[I] + 1,
					 pPacmanModule->MonsterPosY[I]);
		      Up =
			BPacmanCheckPos (pPacmanModule->MonsterPosX[I],
					 pPacmanModule->MonsterPosY[I] - 1);
		      Down =
			BPacmanCheckPos (pPacmanModule->MonsterPosX[I],
					 pPacmanModule->MonsterPosY[I] + 1);
		      //count directions
		      Cnt = 0;
		      if (Left)
			Cnt++;
		      if (Right)
			Cnt++;
		      if (Up)
			Cnt++;
		      if (Down)
			Cnt++;
		      //more than one direction possible
		      if (Cnt > 1)
			{
			  //exclude backward direction
			  if (pPacmanModule->MonsterDirX[I] == -1)
			    Right = 0;
			  if (pPacmanModule->MonsterDirX[I] == 1)
			    Left = 0;
			  if (pPacmanModule->MonsterDirY[I] == -1)
			    Down = 0;
			  if (pPacmanModule->MonsterDirY[I] == 1)
			    Up = 0;
			}
		      //put possible directions into an array
		      Cnt = 0;
		      if (Left)
			{
			  DirX[Cnt] = -1;
			  DirY[Cnt] = 0;
			  Cnt++;
			}
		      if (Right)
			{
			  DirX[Cnt] = 1;
			  DirY[Cnt] = 0;
			  Cnt++;
			}
		      if (Up)
			{
			  DirX[Cnt] = 0;
			  DirY[Cnt] = -1;
			  Cnt++;
			}
		      if (Down)
			{
			  DirX[Cnt] = 0;
			  DirY[Cnt] = 1;
			  Cnt++;
			}
		      //change direction to new one
		      if (Cnt > 0)
			{
			  J = rand () % Cnt;	//select one of the possible directions per coincidence
			  pPacmanModule->MonsterDirX[I] = DirX[J];
			  pPacmanModule->MonsterDirY[I] = DirY[J];
			}
		    }
		}

	      //get new position (move in current direction)
	      X =
		pPacmanModule->MonsterPosX[I] + pPacmanModule->MonsterDirX[I];
	      Y =
		pPacmanModule->MonsterPosY[I] + pPacmanModule->MonsterDirY[I];
	      //check if there is a wall at the new position
	      if (BPacmanCheckPos (X, Y))
		{
		  //set monster to new position
		  pPacmanModule->MonsterPosX[I] = X;
		  pPacmanModule->MonsterPosY[I] = Y;
		  //wrap around
		  if (pPacmanModule->MonsterPosX[I] < -1
		      && pPacmanModule->MonsterDirX[I] < 0)
		    pPacmanModule->MonsterPosX[I] = BPacmanSizeXMin;
		  if (pPacmanModule->MonsterPosX[I] > BPacmanSizeXMin
		      && pPacmanModule->MonsterDirX[I] > 0)
		    pPacmanModule->MonsterPosX[I] = -1;
		  if (pPacmanModule->MonsterPosY[I] < -1
		      && pPacmanModule->MonsterDirY[I] < 0)
		    pPacmanModule->MonsterPosY[I] = BPacmanSizeYMin;
		  if (pPacmanModule->MonsterPosY[I] > BPacmanSizeYMin
		      && pPacmanModule->MonsterDirY[I] > 0)
		    pPacmanModule->MonsterPosY[I] = -1;
		}
	    }
	}
    }

  //check if monster kills pacman / pacman kills monster
  for (I = 0; I < BPacmanMonsterCnt; I++)
    {
      //get distance
      X = pPacmanModule->PosX - pPacmanModule->MonsterPosX[I];
      if (X < 0)
	X = -X;
      Y = pPacmanModule->PosY - pPacmanModule->MonsterPosY[I];
      if (Y < 0)
	Y = -Y;
      //monster kills pacman / pacman kills monster
      if (X + Y < 4)
	{
	  //pacman kills monster
	  if (pPacmanModule->MonstersDead)
	    {
	      //move monster out of sight
	      pPacmanModule->MonsterPosX[I] = BPacmanMonsterNirvana;
	      pPacmanModule->MonsterPosY[I] = BPacmanMonsterNirvana;
	      dbg_print ("BPacman: Pacman ate Monster\n");
	      //check if all monsters are gone
	      for (J = 0; J < BPacmanMonsterCnt; J++)
		if (pPacmanModule->MonsterPosX[J] != BPacmanMonsterNirvana
		    || pPacmanModule->MonsterPosY[J] != BPacmanMonsterNirvana)
		  break;
	      //all monsters are gone
	      if (J >= BPacmanMonsterCnt)
		{
		  dbg_print ("BPacman: Pacman has won game\n");
		  //start new game
		  BPacmanNewGame (pPacmanModule);
		}
	    }
	  //monster kills pacman
	  else
	    {
              //decrease number of lives
              if( pPacmanModule->DeadCnt == 0 )
              {
                pPacmanModule->Lives--;
                dbg_print( "BPacman: Monster killed Pacman\n" );
              }
              //pacman is dead for the next few steps
              pPacmanModule->DeadCnt = BPacmanDeadTime;
	    }
	}
    }

  //output current picture
  BPacmanOutput (pPacmanModule);

  //return game not over
  return 0;
}

static void
b_pacman_module_class_init (BPacmanModuleClass *klass)
{
  BModuleClass *module_class;

  module_class = B_MODULE_CLASS (klass);

  module_class->max_players = 1;

  module_class->query    = b_pacman_module_query;
  module_class->prepare  = b_pacman_module_prepare;
  module_class->start    = b_pacman_module_start;
  module_class->event    = b_pacman_module_event;
  module_class->tick     = b_pacman_module_tick;
  module_class->describe = b_pacman_module_describe;
}

static void
b_pacman_module_init (BPacmanModule *pacman_module)
{
  pacman_module->player_device_id = -1;
}

static gboolean
b_pacman_module_query (gint width,
                       gint height,
                       gint channels,
                       gint maxval)
{
  return (width >= BPacmanSizeXMin && height >= BPacmanSizeYMin
	  && channels == 1 && maxval + 1 >= BPacmanColorCntMin);
}

static gboolean
b_pacman_module_prepare (BModule  *module,
                         GError  **error)
{
  BPacmanModule *pacman_module = B_PACMAN_MODULE (module);

  /* initialize the module values that depend on the output device */
  pacman_module->OfsX = (module->width - BPacmanSizeXMin) / 2;	/* offset of top left corner of game-field */
  pacman_module->OfsY = (module->height - BPacmanSizeYMin) / 2;
  pacman_module->MaxColor = module->maxval;	/* maximum color value */

  return TRUE;
}

static void
b_pacman_module_start (BModule *module)
{
  /* set pacman's lives */
  ((BPacmanModule *)module)->Lives = BPacmanLives;

  /* start new game */
  BPacmanNewGame ((BPacmanModule *) module);

  /* start the tick machinery */
  b_module_ticker_start (module, BPacmanTicks);
}

static void
b_pacman_module_event (BModule      *module,
                       BModuleEvent *event)
{
  BPacmanModule *pacman;

  pacman = B_PACMAN_MODULE (module);

  switch (event->type)
    {
    case B_EVENT_TYPE_KEY:
      if (event->device_id == pacman->player_device_id)
        BPacmanKey (pacman, event->key);
      break;

      case B_EVENT_TYPE_PLAYER_ENTERED:
      if (pacman->player_device_id == -1)
        {
          pacman->player_device_id = event->device_id;

          module->num_players++;
        }
      break;

    case B_EVENT_TYPE_PLAYER_LEFT:
      if (pacman->player_device_id == event->device_id)
        {
          pacman->player_device_id = -1;

          module->num_players--;
        }
      break;

    default:
      break;
    }
}

static gint
b_pacman_module_tick (BModule *module)
{
  int GameOver;

  /* call tick-procedure */
  GameOver = BPacmanTick ((BPacmanModule *) module);

  /* we want to be called again in some milliseconds, if game is not over */
  if( GameOver )
    return 0;
  else
    return BPacmanTicks;
}

static void
b_pacman_module_describe (BModule      *module,
                          const gchar **title,
                          const gchar **description,
                          const gchar **author)
{
  *title       = "BPacman";
  *description = "Pacman game";
  *author      = "1stein";
}
