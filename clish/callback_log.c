/*
 * callback_log.c
 *
 * Callback hook to log users's commands
 */
#include <stdio.h>
#include <stdlib.h>
#include <syslog.h>
#include <unistd.h>
#include <sys/types.h>
#include <pwd.h>

#include "internal.h"

#define SYSLOG_IDENT "klish"

/*--------------------------------------------------------- */
int clish_log_callback(clish_context_t *context, const char *line,
	int retcode)
{
	clish_shell_t *this = context->shell;
	struct passwd *user = NULL;
	char *uname = NULL;

	/* Initialization */
	if (!line) {
		openlog(SYSLOG_IDENT, LOG_PID,
			clish_shell__get_facility(this));
		return 0;
	}

	/* Log the given line */
	/* Try to get username from environment variables
	 * USER and LOGNAME and then from /etc/passwd.
	 */
	user = clish_shell__get_user(this);
	if (!(uname = getenv("USER"))) {
		if (!(uname = getenv("LOGNAME")))
			uname = user ? user->pw_name : "unknown";
	}
	syslog(LOG_INFO, "%u(%s) %s : %d",
		user ? user->pw_uid : getuid(), uname, line, retcode);

	return 0;
}
