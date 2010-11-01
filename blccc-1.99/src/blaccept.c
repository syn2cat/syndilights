/* blccc - Blinkenlights Chaos Control Center
 *
 * Copyright (c) 2001-2002  Sven Neumann <sven@gimp.org>
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
#include <unistd.h>
#include <errno.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

#include <blib/blib.h>

#include "bltypes.h"

#include "blaccept.h"
#include "blccc.h"
#include "bllisten.h"


const gchar *welcome_msg = 
"--------------------------------------------------\n"
"Welcome to the Blinkenlights Chaos Control Center.\n"
"--------------------------------------------------\n"
"Type 'help' to get a list of supported commands.\n";

const gchar *help_msg =
"Supported commands are:\n"
"  bye              - closes the connection\n"
"  help             - shows this list of supported commands\n"
"  isdn block       - block control through isdn lines\n"
"  isdn unblock     - allow control through isdn lines\n"
"  enable <number>  - enable application\n"
"  disable <number> - disable application\n"
"  list             - list items in current playlist\n"
"  load <filename>  - load new playlist\n"
"  start <filename> - load new playlist and start immidiately\n"
"  next             - skip to next item\n"
"  reload           - reload current playlist\n"
"  status           - show status info\n"
"  add <host>       - add recipient\n"
"  remove <host>    - remove recipient\n"
#if 0
"  love <id>        - play loveletter <id>\n"
#endif
"  i                - invert display\n"
"  v                - flip display vertically\n"
"  h                - flip display horizontally\n"
"  l                - mirror left side\n"
"  r                - mirror right side\n"
"  0..9 # *         - fake isdn event\n";

const gchar *load_msg    = "Playlist loaded.\n";
const gchar *reload_msg  = "Playlist reloaded.\n";
const gchar *added_msg   = "Host added.\n";
const gchar *removed_msg = "Host removed.\n";
const gchar *love_msg    = "Made love.\n";
const gchar *event_msg   = "Event dispatched.\n";
const gchar *ok_msg      = "OK\n";
const gchar *failed_msg  = "Operation failed.\n";
const gchar *timeout_msg = "\nConnection timed out.\n";

const gchar *unknown_msg = 
"Unknown command, type 'help' to get a list of supported commands.\n";


static inline void
write_prompt (gint sock)
{
  write (sock, "> ", 2); 
}

void
bl_accept_new (BlListen *listen)
{
  gint            len;
  gint            sock;
  gchar           buf[1024];
  fd_set          set;
  struct timeval  tv;

  g_return_if_fail (BL_IS_LISTEN (listen));

  sock = accept (listen->sock, NULL, 0);

  g_thread_create ((GThreadFunc) bl_accept_new, listen, FALSE, NULL);

  write (sock, welcome_msg, strlen (welcome_msg));
  write_prompt (sock);

  while (TRUE)
    {
      /* wait for input with a timeout of 10 minutes */

      FD_ZERO (&set);
      FD_SET (sock, &set);
      tv.tv_sec  = 600;
      tv.tv_usec = 0;
      
      len = select (sock + 1, &set, NULL, NULL, &tv);

      if (len < 0)
        {
          if (errno == EINTR)  /* select() was interupted */
            continue;
          else
            break;
        }

      if (len == 0)         /* select() timed out      */ 
        {
          write (sock, timeout_msg, strlen (timeout_msg));
          break;
        }

      len = read (sock, &buf, sizeof (buf));

      if (len < 0)
        break;

      if ((len > 2 && strncmp (buf, "bye", 3) == 0) || 
          (len > 3 && strncmp (buf, "quit", 4) == 0))
        {
          write (sock, "Good Bye.\n", 10);
          break;
        }
      else if (len > 5 && strncmp (buf, "status", 6) == 0)
        {
          if (listen->ccc)
            {
              gchar *status = bl_ccc_status (listen->ccc);
              write (sock, status, strlen (status));
              g_free (status);
            }
        }
      else if (len > 4 && strncmp (buf, "add ", 4) == 0)
        {
          gchar *host = g_strndup (buf + 4, len - 4);
          g_strstrip (host);
          if (listen->ccc && bl_ccc_add (listen->ccc, host, NULL))
            write (sock, added_msg, strlen (added_msg));
          else
            write (sock, failed_msg, strlen (failed_msg));
          g_free (host);
        }
      else if (len > 7 && strncmp (buf, "remove ", 7) == 0)
        {
          gchar *host = g_strndup (buf + 7, len - 7);
          g_strstrip (host);
          if (listen->ccc && bl_ccc_remove (listen->ccc, host, NULL))
            write (sock, removed_msg, strlen (removed_msg));
          else
            write (sock, failed_msg, strlen (failed_msg));
          g_free (host);
        }
#if 0
      else if (len > 5 && strncmp (buf, "love ", 5) == 0)
        {
          gchar *id = g_strndup (buf + 5, len - 5);
          g_strstrip (id);
          if (listen->ccc && bl_ccc_on_demand (listen->ccc, id))
            write (sock, love_msg, strlen (love_msg));
          else
            write (sock, failed_msg, strlen (failed_msg));
          g_free (id);
        }
#endif
      else if (len > 5 && strncmp (buf, "reload", 6) == 0)
        {
          if (listen->ccc && bl_ccc_reload (listen->ccc))
            write (sock, reload_msg, strlen (reload_msg));
          else
            write (sock, failed_msg, strlen (failed_msg));
        }
     else if (len > 3 && strncmp (buf, "list", 4) == 0)
        {
          if (listen->ccc) 
            {
              gchar *list = bl_ccc_list (listen->ccc);
              write (sock, list, strlen (list));
              g_free (list);
            }
        }
     else if (len > 5 && strncmp (buf, "load ", 5) == 0)
        {
          gchar *name = g_strndup (buf + 5, len - 5);
          g_strstrip (name);
          if (listen->ccc && bl_ccc_load (listen->ccc, name, FALSE))
            write (sock, load_msg, strlen (load_msg));
          else
            write (sock, failed_msg, strlen (failed_msg));
          g_free (name);
        }
     else if (len > 6 && strncmp (buf, "start ", 6) == 0)
        {
          gchar *name = g_strndup (buf + 6, len - 6);
          g_strstrip (name);
          if (listen->ccc && bl_ccc_load (listen->ccc, name, TRUE))
            write (sock, load_msg, strlen (load_msg));
          else
            write (sock, failed_msg, strlen (failed_msg));
          g_free (name);
        }
      else if (len > 3 && strncmp (buf, "next", 4) == 0)
        {
          if (listen->ccc)
            {
              gchar *name = bl_ccc_next (listen->ccc);
              if (name)
                {
                  g_snprintf (buf, sizeof (buf),
                              "skipped to next item %s\n", name);
                  write (sock, buf, strlen (buf));
                }
              else
                write (sock, failed_msg, strlen (failed_msg));
              g_free (name);
            }
        }
      else if (len > 9 && strncmp (buf, "isdn block", 10) == 0)
        {
          if (listen->ccc)
            bl_ccc_isdn_block (listen->ccc);
        }
      else if (len > 11 && strncmp (buf, "isdn unblock", 12) == 0)
        {
          if (listen->ccc)
            bl_ccc_isdn_unblock (listen->ccc);
        }
      else if (len > 6 && strncmp (buf, "enable ", 7) == 0)
        {
          gchar *num = g_strndup (buf + 7, len - 7);
          g_strstrip (num);
          if (listen->ccc && bl_ccc_app_enable (listen->ccc, num))
            write (sock, ok_msg, strlen (ok_msg));
          else
            write (sock, failed_msg, strlen (failed_msg));
          g_free (num);
        }
      else if (len > 7 && strncmp (buf, "disable ", 8) == 0)
        {
          gchar *num = g_strndup (buf + 8, len - 8);
          g_strstrip (num);
          if (listen->ccc && bl_ccc_app_disable (listen->ccc, num))
            write (sock, ok_msg, strlen (ok_msg));
          else
            write (sock, failed_msg, strlen (failed_msg));
          g_free (num);
        }
      else if (len > 3 && strncmp (buf, "help", 4) == 0)
        {
          write (sock, help_msg, strlen (help_msg));
        }
      else if (len > 0 && strncmp (buf, "i", 1) == 0)
        {
          if (listen->ccc && listen->ccc->effects)
            listen->ccc->effects->invert ^= B_EFFECT_SCOPE_ALL;
        }
      else if (len > 0 && strncmp (buf, "v", 1) == 0)
        {
          if (listen->ccc && listen->ccc->effects)
            listen->ccc->effects->vflip ^= B_EFFECT_SCOPE_ALL;
        }
      else if (len > 0 && strncmp (buf, "h", 1) == 0)
        {
          if (listen->ccc && listen->ccc->effects)
            listen->ccc->effects->hflip ^= B_EFFECT_SCOPE_ALL;
        }
      else if (len > 0 && strncmp (buf, "l", 1) == 0)
        {
          if (listen->ccc && listen->ccc->effects)
            {
              listen->ccc->effects->lmirror ^= B_EFFECT_SCOPE_ALL;

              if (listen->ccc->effects->lmirror)
                listen->ccc->effects->rmirror = B_EFFECT_SCOPE_NONE;
            }
        }
      else if (len > 0 && strncmp (buf, "r", 1) == 0)
        {
          if (listen->ccc && listen->ccc->effects)
            {
              listen->ccc->effects->rmirror ^= B_EFFECT_SCOPE_ALL;

              if (listen->ccc->effects->rmirror)
                listen->ccc->effects->lmirror = B_EFFECT_SCOPE_NONE;
            }
        }
      else if (len > 1 &&
               ((*buf >= '0' && *buf <= '9') || *buf == '#' || *buf == '*'))
        {
          if (listen->ccc)
            {
              BModuleEvent event;

              event.device_id = 0;
              event.type      = B_EVENT_TYPE_KEY;

              switch (*buf)
                {
                case '#':
                  event.key = B_KEY_HASH;
                  break;

                case '*':
                  event.key = B_KEY_ASTERISK;
                  break;

                default:
                  event.key = *buf - '0' + B_KEY_0;
                }

              bl_ccc_event (listen->ccc, &event);
              write (sock, event_msg, strlen (event_msg));
            }
          else
            {
              write (sock, failed_msg, strlen (failed_msg));
            }
        }
      else
        {
          write (sock, unknown_msg, strlen (unknown_msg));
        }

      write_prompt (sock);
    }

  close (sock);
}
