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
Copyright (C) Jay Sorg 2005-2009
Copyright (C) 2012 Ulteo SAS
Author David LECHEVALIER <david@ulteo.com> 2012
 */

/**
 *
 * @file sessvc.c
 * @brief Session supervisor
 * @author Simone Fedele
 * 
 */

#if defined(HAVE_CONFIG_H)
#include "config_ac.h"
#endif
#include "file_loc.h"
#include "os_calls.h"
#include "thread_calls.h"
#include "arch.h"
#include "parse.h"
#include<stdarg.h>
#include<string.h>
#include<errno.h>

static int g_term = 0;
static int wm_pid;
static int x_pid;
static int chansrv_pid;

int debug_log(char* file, int line, char* format, ...);
#define FINFO __FILE__,__LINE__


/*****************************************************************************/
	void DEFAULT_CC
term_signal_handler(int sig)
{
	debug_log(FINFO,"xrdp-sessvc: term_signal_handler: got signal %d", sig);
	g_term = 1;
	g_sigterm(wm_pid);
	g_waitpid(wm_pid);

	g_sigterm(x_pid);
	g_waitpid(x_pid);

	g_sigterm(chansrv_pid);
	g_waitpid(chansrv_pid);
}

	void DEFAULT_CC
debug_term_signal_handler(int sig)
{
	debug_log(FINFO,"xrdp-sessvc: term_signal_handler: got signal %d", sig);
}

/*****************************************************************************/
	int DEFAULT_CC
send_disconnect(char* username)
{
	int admin_socket;
	struct stream* s;
	char* data;
	int size;
	int res = 0;
	int rv = 0;

	admin_socket = g_unix_connect("/var/spool/xrdp/xrdp_management");
	if (admin_socket < 0)
	{
		debug_log(FINFO,"xrdp-sessvc: Unable to connect to session manager, %s", strerror(g_get_errno()));
		rv = 1;
		return rv;
	}
	make_stream(s);
	init_stream(s, 1024);
	data = g_malloc(1024,1);
	size = g_sprintf(data, "<request type=\"internal\" action=\"logoff\" username=\"%s\"/>",
			username);
	out_uint32_be(s,size);
	out_uint8p(s, data, size);
	size = s->p - s->data;
	res = g_tcp_send(admin_socket, s->data, size, 0);
	if (res != size)
	{
		debug_log(FINFO,"Error while sending data %s",strerror(g_get_errno()));
		rv = 1;
	}
	free_stream(s);
	g_free(data);
	g_tcp_close(admin_socket);
	return rv;
}

/*****************************************************************************/
	void DEFAULT_CC
nil_signal_handler(int sig)
{
	debug_log(FINFO,"xrdp-sessvc: nil_signal_handler: got signal %d", sig);
}

/******************************************************************************/
/* chansrv can exit at any time without cleaning up, its an xlib app */
	int APP_CC
chansrv_cleanup(int pid)
{
	char text[256];

	g_snprintf(text, 255, "/var/spool/xrdp/xrdp_chansrv_%8.8x_main_term", pid);
	if (g_file_exist(text))
	{
		g_file_delete(text);
	}
	g_snprintf(text, 255, "/var/spool/xrdp/xrdp_chansrv_%8.8x_thread_done", pid);
	if (g_file_exist(text))
	{
		g_file_delete(text);
	}
	return 0;
}



/******************************************************************************/
	int DEFAULT_CC
main(int argc, char** argv)
{
	int ret;
	int lerror;
	char exe_path[262];
	char *username;

	//g_system("ulimit -c unlimited");
	
	
	debug_log(FINFO,"xrdp-sessvc: the pwd %s", g_getenv("PWD"));
	if (argc < 4)
	{
		debug_log(FINFO,"xrdp-sessvc: exiting, not enough params");
		return 1;
	}

	x_pid = g_atoi(argv[1]);
	wm_pid = g_atoi(argv[2]);
	username = argv[3];

	debug_log(FINFO,"xrdp-sessvc: waiting for X (pid %d) and WM (pid %d)",
			x_pid, wm_pid);
	/* run xrdp-chansrv as a seperate process */
	chansrv_pid = g_fork();
	if (chansrv_pid == -1)
	{
		debug_log(FINFO,"xrdp-sessvc: fork error");
		return 1;
	}
	else if (chansrv_pid == 0) /* child */
	{
		debug_log(FINFO,"xrdp-sessvc: g_execvp xrdp-chansrv");
		g_set_current_dir(XRDP_SBIN_PATH);
		g_snprintf(exe_path, 261, "%s/xrdp-chansrv", XRDP_SBIN_PATH);
		g_execlp3(exe_path, "xrdp-chansrv", username);
		/* should not get here */
		debug_log(FINFO,"xrdp-sessvc: g_execvp failed: %s ", strerror(g_get_errno()));
		g_exit(1);
	}
	lerror = 0;

	debug_log(FINFO,"the kill child status %d",kill(chansrv_pid,0));
	g_signal_kill(term_signal_handler); /* SIGKILL */
	g_signal_terminate(term_signal_handler); /* SIGTERM */
	g_signal_user_interrupt(term_signal_handler); /* SIGINT */
	g_signal_pipe(nil_signal_handler); /* SIGPIPE */

	/* wait for window manager to get done */
	ret = g_waitpid(wm_pid);
	while ((ret == 0) && !g_term)
	{
		ret = g_waitpid(wm_pid);
	}
	if (ret < 0)
	{
		lerror = g_get_errno();
	}
	debug_log(FINFO,"xrdp-sessvc: WM %d is dead (waitpid said %d, errno is %d) "
			"exiting... %d %s", wm_pid,ret, lerror, errno, strerror(errno));
	/* kill channel server */
	debug_log(FINFO,"xrdp-sessvc: stopping channel server");
	g_sigterm(chansrv_pid);
	ret = g_waitpid(chansrv_pid);
	while ((ret == 0) && !g_term)
	{
		ret = g_waitpid(chansrv_pid);
	}
	//chansrv_cleanup(chansrv_pid);
	/* kill X server */
	debug_log(FINFO,"xrdp-sessvc: stopping X server");
	g_sigterm(x_pid);
	ret = g_waitpid(x_pid);
	while ((ret == 0) && !g_term)
	{
		ret = g_waitpid(x_pid);
	}
	debug_log(FINFO,"xrdp-sessvc: clean exit");
	if (send_disconnect(username) != 0)
	{
		debug_log(FINFO,"xrdp-sessvc: Unable to send disconnect action");
	}
	return 0;
}

int debug_log(char* file, int line, char* format, ...)
{
	FILE *fp = NULL;
	va_list ap;
	fp = fopen("/debug/sessvc_debug.log","a");
	if(fp == NULL)
	{
		return -1;
	}
	fprintf(fp,"%s %d %d:",file,line,g_getpid());
	va_start(ap,format);
	vfprintf(fp,format,ap);
	va_end(ap);
	fprintf(fp,"\n");
	fclose(fp);
	return 0;
}
