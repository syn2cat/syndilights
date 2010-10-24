/* blib - Library of useful things to hack the Blinkenlights
 *
 * Copyright (c) 2001-2002  The Blinkenlights Crew
 *                          Michael Natterer <mitch@gimp.org>
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

#include <stdlib.h>
#include <string.h>

#include <glib-object.h>

#include <blib/blib.h>


#define TEXT_TIMEOUT   200
#define BLINK_TIMEOUT  200
#define SCROLL_TIMEOUT 100
#define CHAR_WIDTH     3
#define CHAR_HEIGHT    5
#define CHAR_ADVANCE   4
#define LINE_ADVANCE   6
#define BLINK_STEPS    4


typedef struct _BChar BChar;

struct _BChar
{
  gchar  character;
  gchar  width;
  guchar matrix[CHAR_WIDTH * CHAR_HEIGHT];
};

static const BChar unknown_char =
{
  0, 3,
  { 1, 1, 1,
    1, 1, 1,
    1, 1, 1,
    1, 1, 1,
    1, 1, 1
  }
};

static const BChar chars[] =
{
  { '0', 3,
    { 1, 1, 1,
      1, 0, 1,
      1, 0, 1,
      1, 0, 1,
      1, 1, 1
    }
  },
  { '1', 3,
    { 1, 1, 0,
      0, 1, 0,
      0, 1, 0,
      0, 1, 0,
      0, 1, 0
    }
  },
  { '2', 3,
    { 0, 1, 1,
      1, 0, 1,
      0, 0, 1,
      0, 1, 0,
      1, 1, 1
    }
  },
  { '3', 3,
    { 1, 1, 1,
      0, 0, 1,
      1, 1, 1,
      0, 0, 1,
      1, 1, 1
    }
  },
  { '4', 3,
    { 1, 0, 1,
      1, 0, 1,
      1, 1, 1,
      0, 0, 1,
      0, 0, 1
    }
  },
  { '5', 3,
    { 1, 1, 1,
      1, 0, 0,
      1, 1, 1,
      0, 0, 1,
      1, 1, 0
    }
  },
  { '6', 3,
    { 1, 1, 1,
      1, 0, 0,
      1, 1, 1,
      1, 0, 1,
      1, 1, 1
    }
  },
  { '7', 3,
    { 1, 1, 1,
      0, 0, 1,
      0, 1, 0,
      1, 0, 0,
      1, 0, 0
    }
  },
  { '8', 3,
    { 1, 1, 1,
      1, 0, 1,
      1, 1, 1,
      1, 0, 1,
      1, 1, 1
    }
  },
  { '9', 3,
    { 1, 1, 1, 
      1, 0, 1,
      1, 1, 1,
      0, 0, 1,
      1, 1, 1
    }
  },

  { ':', 1,
    { 0, 0, 0, 
      1, 0, 0,
      0, 0, 0,
      1, 0, 0,
      0, 0, 0
    }
  },
  { '.', 1,
    { 0, 0, 0, 
      0, 0, 0,
      0, 0, 0,
      0, 0, 0,
      1, 0, 0
    }
  },

  { 'A', 3,
    { 1, 1, 1, 
      1, 0, 1,
      1, 1, 1,
      1, 0, 1,
      1, 0, 1
    }
  },
  { 'B', 3,
    { 1, 1, 0, 
      1, 0, 1,
      1, 1, 0,
      1, 0, 1,
      1, 1, 0
    }
  },
  { 'C', 3,
    { 1, 1, 1, 
      1, 0, 0,
      1, 0, 0,
      1, 0, 0,
      1, 1, 1
    }
  },
  { 'D', 3,
    { 1, 1, 0, 
      1, 0, 1,
      1, 0, 1,
      1, 0, 1,
      1, 1, 0
    }
  },
  { 'E', 3,
    { 1, 1, 1, 
      1, 0, 0,
      1, 1, 0,
      1, 0, 0,
      1, 1, 1
    }
  },
  { 'F', 3,
    { 1, 1, 1, 
      1, 0, 0,
      1, 1, 0,
      1, 0, 0,
      1, 0, 0
    }
  },
  { 'G', 3,
    { 1, 1, 1, 
      1, 0, 0,
      1, 0, 1,
      1, 0, 1,
      1, 1, 1
    }
  },
  { 'H', 3,
    { 1, 0, 1, 
      1, 0, 1,
      1, 1, 1,
      1, 0, 1,
      1, 0, 1
    }
  },
  { 'I', 1,
    { 1, 0, 0, 
      1, 0, 0,
      1, 0, 0,
      1, 0, 0,
      1, 0, 0
    }
  },
  { 'J', 3,
    { 0, 0, 1, 
      0, 0, 1,
      0, 0, 1,
      1, 0, 1,
      0, 1, 0
    }
  },
  { 'K', 3,
    { 1, 0, 1, 
      1, 0, 1,
      1, 1, 0,
      1, 0, 1,
      1, 0, 1
    }
  },
  { 'L', 3,
    { 1, 0, 0, 
      1, 0, 0,
      1, 0, 0,
      1, 0, 0,
      1, 1, 1
    }
  },
  { 'M', 3,
    { 1, 0, 1, 
      1, 1, 1,
      1, 0, 1,
      1, 0, 1,
      1, 0, 1
    }
  },
  { 'N', 3,
    { 1, 1, 1, 
      1, 0, 1,
      1, 0, 1,
      1, 0, 1,
      1, 0, 1
    }
  },
  { 'O', 3,
    { 0, 1, 0, 
      1, 0, 1,
      1, 0, 1,
      1, 0, 1,
      0, 1, 0
    }
  },
  { 'P', 3,
    { 1, 1, 0, 
      1, 0, 1,
      1, 1, 0,
      1, 0, 0,
      1, 0, 0
    }
  },
  { 'Q', 3,
    { 0, 1, 0, 
      1, 0, 1,
      1, 0, 1,
      1, 0, 1,
      0, 1, 1
    }
  },
  { 'R', 3,
    { 1, 1, 0, 
      1, 0, 1,
      1, 1, 0,
      1, 0, 1,
      1, 0, 1
    }
  },
  { 'S', 3,
    { 1, 1, 1, 
      1, 0, 0,
      1, 1, 1,
      0, 0, 1,
      1, 1, 1
    }
  },
  { 'T', 3,
    { 1, 1, 1, 
      0, 1, 0,
      0, 1, 0,
      0, 1, 0,
      0, 1, 0
    }
  },
  { 'U', 3,
    { 1, 0, 1, 
      1, 0, 1,
      1, 0, 1,
      1, 0, 1,
      1, 1, 1
    }
  },
  { 'V', 3,
    { 1, 0, 1, 
      1, 0, 1,
      1, 0, 1,
      1, 0, 1,
      0, 1, 0
    }
  },
  { 'W', 3,
    { 1, 0, 1, 
      1, 0, 1,
      1, 0, 1,
      1, 1, 1,
      1, 0, 1
    }
  },
  { 'X', 3,
    { 1, 0, 1, 
      1, 0, 1,
      0, 1, 0,
      1, 0, 1,
      1, 0, 1
    }
  },
  { 'Y', 3,
    { 1, 0, 1, 
      1, 0, 1,
      0, 1, 0,
      0, 1, 0,
      0, 1, 0
    }
  },
  { 'Z', 3,
    { 1, 1, 1, 
      0, 0, 1,
      0, 1, 0,
      1, 0, 0,
      1, 1, 1
    }
  },
};

typedef enum
{
  SCROLL_UP,
  CURSOR_BLINK,
  FINISH
} AnimType;


enum
{
  PROP_0,
  PROP_STRING
};


#define B_TYPE_TEXT            (b_type_text)
#define B_TEXT(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), B_TYPE_TEXT, BText))
#define B_TEXT_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass), B_TYPE_TEXT, BTextClass))
#define B_IS_TEXT(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), B_TYPE_TEXT))
#define B_IS_TEXT_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), B_TYPE_TEXT))

typedef struct _BText      BText;
typedef struct _BTextClass BTextClass;

struct _BText
{
  BModule       parent_instance;

  gchar        *string;
  gchar        *cursor_pos;

  gint          cursor_x;
  gint          cursor_y;

  guchar        bg_color;
  guchar        fg_color;

  gint          player_device_id;

  gint          timeout;

  AnimType      anim;
  gint          anim_steps;

  gint          cursor_size;

  GRand        *rand;
};

struct _BTextClass
{
  BModuleClass  parent_class;
};


static GType      b_text_get_type      (GTypeModule   *module);

static void       b_text_class_init    (BTextClass    *klass);
static void       b_text_init          (BText         *text);

static void       b_text_finalize      (GObject       *object);
static void       b_text_set_property  (GObject       *object,
                                        guint          property_id,
                                        const GValue  *value,
                                        GParamSpec    *pspec);
static gboolean   b_text_query         (gint           width,
                                        gint           height,
                                        gint           channels,
                                        gint           maxval);
static gboolean   b_text_prepare       (BModule       *module,
                                        GError       **error);
static void       b_text_relax         (BModule       *module);
static void       b_text_start         (BModule       *module);
static void       b_text_stop          (BModule       *module);
static void       b_text_event         (BModule       *module,
                                        BModuleEvent  *event);
static gint       b_text_tick          (BModule       *module);
static void       b_text_describe      (BModule       *module,
                                        const gchar  **title,
                                        const gchar  **description,
                                        const gchar  **author);


static BModuleClass * parent_class = NULL;
static GType          b_type_text  = 0;


G_MODULE_EXPORT gboolean
b_module_register (GTypeModule *module)
{
  b_text_get_type (module);
  return TRUE;
}

static GType
b_text_get_type (GTypeModule *module)
{
  if (! b_type_text)
    {
      static const GTypeInfo text_info =
      {
        sizeof (BTextClass),
        NULL,           /* base_init */
        NULL,           /* base_finalize */
        (GClassInitFunc) b_text_class_init,
        NULL,           /* class_finalize */
        NULL,           /* class_data */
        sizeof (BText),
        0,              /* n_preallocs */
        (GInstanceInitFunc) b_text_init,
      };

      b_type_text = g_type_module_register_type (module,
                                                 B_TYPE_MODULE, "BText",
                                                 &text_info, 0);
    }

  return b_type_text;
}

static void
b_text_class_init (BTextClass *klass)
{
  GObjectClass *object_class;
  BModuleClass *module_class;
  GParamSpec   *param_spec;

  object_class = G_OBJECT_CLASS (klass);
  module_class = B_MODULE_CLASS (klass);

  parent_class = g_type_class_peek_parent (klass);

  object_class->finalize     = b_text_finalize;
  object_class->set_property = b_text_set_property;

  param_spec = g_param_spec_string ("string", NULL,
                                    "The string to draw.",
                                    "blinken\nbis die\nkotze\nkommt",
                                    G_PARAM_CONSTRUCT | G_PARAM_WRITABLE);
  g_object_class_install_property (object_class, PROP_STRING, param_spec);

  module_class->max_players = 1;

  module_class->query    = b_text_query;
  module_class->prepare  = b_text_prepare;
  module_class->relax    = b_text_relax;
  module_class->start    = b_text_start;
  module_class->stop     = b_text_stop;
  module_class->event    = b_text_event;
  module_class->tick     = b_text_tick;
  module_class->describe = b_text_describe;
}

static void
b_text_init (BText *text)
{
  text->string = NULL;

  text->anim_steps       = 0;
  text->player_device_id = -1;

  text->rand             = g_rand_new ();
}

static void
b_text_finalize (GObject *object)
{
  BText *text;

  text = B_TEXT (object);

  if (text->string)
    {
      g_free (text->string);
      text->string = NULL;
    }

  if (text->rand)
    {
      g_rand_free (text->rand);
      text->rand = NULL;
    }

  G_OBJECT_CLASS (parent_class)->finalize (object);
}

static void
b_text_set_property (GObject      *object,
                     guint         property_id,
                     const GValue *value,
                     GParamSpec   *pspec)
{
  BText *text;

  text = B_TEXT (object);

  switch (property_id)
    {
    case PROP_STRING:
      g_free (text->string);
      text->string = g_value_dup_string (value);
      break;

    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
      break;
    }
}

static gboolean
b_text_query (gint     width,
              gint     height,
              gint     channels,
              gint     maxval)
{
  return (width  >= 5 * CHAR_ADVANCE &&
          height >= 2 * LINE_ADVANCE &&
          channels == 1);
}

static gboolean
b_text_prepare (BModule  *module,
                GError  **error)
{
  BText *text;

  text = B_TEXT (module);

  text->bg_color    = 0;
  text->fg_color    = module->maxval;

  text->cursor_size = CHAR_WIDTH;

  return TRUE;
}

static void
b_text_relax (BModule *module)
{
}

static void
b_text_start (BModule *module)
{
  BText *text = B_TEXT (module);

  text->cursor_pos = text->string;

  text->cursor_x   = 0;
  text->cursor_y   = 0;

  text->anim       = CURSOR_BLINK;
  text->anim_steps = BLINK_STEPS;

  text->timeout    = TEXT_TIMEOUT;

  b_module_fill (module, text->bg_color);

  b_module_ticker_start (module, text->timeout);
}

static void
b_text_stop (BModule *module)
{
  BText *text;

  text = B_TEXT (module);

  text->player_device_id = -1;
}

static void
b_text_event (BModule      *module,
              BModuleEvent *event)
{
  BText *text;

  text = B_TEXT (module);

  switch (event->type)
    {
    case B_EVENT_TYPE_KEY:
      if (text->anim_steps)
        return;

      switch (event->key)
        {
        default:
          break;
        }
      break;

    case B_EVENT_TYPE_PLAYER_ENTERED:
      if (text->player_device_id == -1)
        {
          text->player_device_id = event->device_id;

          module->num_players++;
        }
      break;

    case B_EVENT_TYPE_PLAYER_LEFT:
      if (text->player_device_id == event->device_id)
        {
          text->player_device_id = -1;

          module->num_players--;
        }
      break;

    default:
      break;
    }
}

static gint
b_text_tick (BModule *module)
{
  BText *text;

  text = B_TEXT (module);

  if (text->anim_steps > 0)
    {
      text->anim_steps--;

      switch (text->anim)
        {
        case SCROLL_UP:
          memmove (module->buffer,
                   module->buffer + module->width * module->channels,
                   module->width * (module->height - 1) * module->channels);
          b_module_draw_line (module,
                              0, module->height - 1,
                              module->width - 1, module->height - 1,
                              text->bg_color);
          text->timeout = SCROLL_TIMEOUT;

          if (text->anim_steps == 0)
            {
              text->anim       = CURSOR_BLINK;
              text->anim_steps = BLINK_STEPS;
            }
          break;

        case CURSOR_BLINK:
        case FINISH:
          b_module_draw_line (module,
                              text->cursor_x,
                              text->cursor_y + CHAR_HEIGHT - 1,
                              text->cursor_x + text->cursor_size - 1,
                              text->cursor_y + CHAR_HEIGHT - 1,
                              text->anim_steps % 2 ?
                              text->fg_color : text->bg_color);

          if (text->anim == FINISH && text->anim_steps == 0)
            {
              b_module_request_stop (module);
              return 0;
            }

          text->timeout = BLINK_TIMEOUT;
          break;
        }
    }
  else
    {
      text->timeout = TEXT_TIMEOUT;

      if (text->cursor_pos && *text->cursor_pos)
        {
          gint advance = CHAR_WIDTH;

          b_module_draw_line (module,
                              text->cursor_x,
                              text->cursor_y + CHAR_HEIGHT - 1,
                              text->cursor_x + text->cursor_size - 1,
                              text->cursor_y + CHAR_HEIGHT - 1,
                              text->bg_color);

          if (! g_ascii_isspace (*text->cursor_pos))
            {
              const BChar *bchar = &unknown_char;
              gint         i, x, y;

              for (i = 0; i < G_N_ELEMENTS (chars); i++)
                {
                  if (chars[i].character == g_ascii_toupper (*text->cursor_pos))
                    bchar = &chars[i];
                }

              for (x = 0; x < CHAR_WIDTH; x++)
                for (y = 0; y < CHAR_HEIGHT; y++)
                  {
                    if (bchar->matrix[y * CHAR_WIDTH + x])
                      b_module_draw_point (module,
                                           text->cursor_x + x,
                                           text->cursor_y + y,
                                           text->fg_color);
                    else
                      b_module_draw_point (module,
                                           text->cursor_x + x,
                                           text->cursor_y + y,
                                           text->bg_color);
                  }

              advance = bchar->width + 1;
            }

          text->cursor_x += advance;

          if (*text->cursor_pos == '\n' ||
              (text->cursor_x + CHAR_WIDTH > module->width))
            {
              text->cursor_pos++;

              /* no whitespace at the start of a line */
              if (g_ascii_isspace (*text->cursor_pos) &&
                  (*text->cursor_pos != '\n' ||
                   (text->cursor_x + CHAR_WIDTH > module->width)))
                text->cursor_pos++;

              text->cursor_x  = 0;
              text->cursor_y += LINE_ADVANCE;

              text->anim       = CURSOR_BLINK;
              text->anim_steps = BLINK_STEPS;
            }
          else
            {
              text->cursor_pos++;

              if (g_rand_double (text->rand) > 0.5)
                b_module_draw_line (module,
                                    text->cursor_x,
                                    text->cursor_y + CHAR_HEIGHT - 1,
                                    text->cursor_x + text->cursor_size - 1,
                                    text->cursor_y + CHAR_HEIGHT - 1,
                                    text->fg_color);
            }

          if (text->cursor_y + CHAR_HEIGHT > module->height)
            {
              text->cursor_y  -= LINE_ADVANCE;

              text->anim       = SCROLL_UP;
              text->anim_steps = LINE_ADVANCE;
            }
        }
      else
        {
          text->anim       = FINISH;
          text->anim_steps = BLINK_STEPS * 4;
          text->timeout    = BLINK_TIMEOUT;
        }
    }

  b_module_paint (module);

  return text->timeout;
}

static void
b_text_describe (BModule      *module,
                 const gchar **title,
                 const gchar **description,
                 const gchar **author)
{
  *title       = "BText";
  *description = "Text display";
  *author      = "Michael Natterer";
}
