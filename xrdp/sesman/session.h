/*
   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

   xrdp: A Remote Desktop Protocol server.
   Copyright (C) Jay Sorg 2005-2008
*/

/**
 *
 * @file session.h
 * @brief Session management definitions
 * @author Jay Sorg, Simone Fedele
 *
 */


#ifndef SESSION_H
#define SESSION_H

#include "libscp_types.h"

#define SESMAN_SESSION_TYPE_XRDP  1
#define SESMAN_SESSION_TYPE_XVNC  2

#define SESMAN_SESSION_STATUS_ACTIVE        0x01
#define SESMAN_SESSION_STATUS_IDLE          0x02
#define SESMAN_SESSION_STATUS_DISCONNECTED  0x04
#define SESMAN_SESSION_STATUS_TO_DESTROY    0x05
#define SESMAN_SESSION_STATUS_UNKNOWN       0x06

/* future expansion
#define SESMAN_SESSION_STATUS_REMCONTROL    0x08
*/
#define SESMAN_SESSION_STATUS_ALL           0xFF

#define SESMAN_SESSION_KILL_OK        0
#define SESMAN_SESSION_KILL_NULLITEM  1
#define SESMAN_SESSION_KILL_NOTFOUND  2

#define XRDP_USER_PREF_DIRECTORY		"/var/cache/xrdp/user_pref"
#define XRDP_TEMP_DIR								"/var/cache/xrdp"

#define XRDP_TAG                    "XRDP_PROCESS"
#define XRDP_TAG_LEN                12

struct session_date
{
  tui16 year;
  tui8  month;
  tui8  day;
  tui8  hour;
  tui8  minute;
};

#define zero_time(s) { (s)->year=0; (s)->month=0; (s)->day=0; (s)->hour=0; (s)->minute=0; }

struct session_item
{
  char name[256];
  char homedir[1024];
  int pid; /* pid of sesman waiting for wm to end */
  int display;
  int width;
  int height;
  int bpp;
  long data;

  /* status info */
  unsigned char status;
  unsigned char type;

  /* time data  */
  struct session_date connect_time;
  struct session_date disconnect_time;
  struct session_date idle_time;
};

struct session_chain
{
  struct session_chain* next;
  struct session_item* item;
};

/**
 *
 * @brief finds a session matching the supplied parameters
 * @return session data or 0
 *
 */
struct session_item* DEFAULT_CC
session_get_bydata(char* name);
#ifndef session_find_item
  #define session_find_item(a, b, c, d, e) session_get_bydata(a, b, c, d, e);
#endif

/**
 *
 * @brief starts a session
 * @return 0 on error, display number if success
 *
 */
int DEFAULT_CC
session_start(int width, int height, int bpp, char* username, char* password,
              long data, tui8 type, char* domain, char* program,
              char* directory, int keylayout);

/**
 *
 * @brief starts a session
 * @return error
 *
 */
int APP_CC
session_sync_start(void);

/**
 *
 * @brief kills a session
 * @param pid the pid of the session to be killed
 * @return
 *
 */
int DEFAULT_CC
session_kill(int pid);

/**
 *
 * @brief sends sigkill to all sessions
 * @return
 *
 */
void DEFAULT_CC
session_sigkill_all();

/**
 *
 * @brief retrieves a session's descriptor
 * @param pid the session pid
 * @return a pointer to the session descriptor on success, NULL otherwise
 *
 */
struct session_item* DEFAULT_CC
session_get_bypid(int pid);

/**
 *
 * @brief retrieves a session's descriptor
 * @param pid the session pid
 * @return a pointer to the session descriptor on success, NULL otherwise
 *
 */
struct SCP_DISCONNECTED_SESSION*
session_get_byuser(char* user, int* cnt, unsigned char flags);

struct session_item*
session_get_by_display(int display);

char*
session_get_status_string(int i);

int DEFAULT_CC
session_set_user_pref(char* username, char* key, char* value);

int DEFAULT_CC
session_get_user_pref(char* username, char* key, char* value);

struct session_item*
session_list_session(int* count);
void
session_update_status_by_user(char* user, int status);
#endif

