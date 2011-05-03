/*
** ZABBIX
** Copyright (C) 2000-2005 SIA Zabbix
**
** This program is free software; you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation; either version 2 of the License, or
** (at your option) any later version.
**
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with this program; if not, write to the Free Software
** Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
**/

#include "common.h"
#include "log.h"
#include "zlog.h"
#include "comms.h"
#include "base64.h"

#include "zbxmedia.h"

/*
 * smtp_readln reads until '\n'
 */
ssize_t smtp_readln(int fd, char *buf, int buf_len)
{
	ssize_t	nbytes, read_bytes;

	buf_len--;	/* '\0' */

	do
	{
		read_bytes = 0;

		do
		{
			if (-1 == (nbytes = read(fd, &buf[read_bytes], 1)))
				return nbytes;			/* error */

			read_bytes += nbytes;
		}
		while (nbytes > 0 &&				/* end of file (socket closed) */
				read_bytes < buf_len &&		/* end of buffer */
				buf[read_bytes - 1] != '\n' );	/* new line */

		buf[read_bytes] = '\0';
	}
	while (read_bytes >= 4 && isdigit(buf[0]) && isdigit(buf[1]) && isdigit(buf[2]) && buf[3] == '-');

	return read_bytes;
}

/*
 * Send email
 */
int	send_email(const char *smtp_server, const char *smtp_helo, const char *smtp_email, const char *mailto,
		const char *mailsubject, const char *mailbody, char *error, int max_error_len)
{
	const char	*__function_name = "send_email";

	zbx_sock_t	s;
	int		err, ret = FAIL;
	char		cmd[MAX_STRING_LEN], *cmdp = NULL;
	char		*tmp = NULL, *base64 = NULL;
	char		*localsubject = NULL, *localbody = NULL;

	char		str_time[MAX_STRING_LEN];
	struct tm	*local_time = NULL;
	time_t		email_time;

	const char	*OK_220 = "220";
	const char	*OK_250 = "250";
	const char	*OK_251 = "251";
	const char	*OK_354 = "354";

	zabbix_log(LOG_LEVEL_DEBUG, "In %s() smtp_server:'%s'", smtp_server);

	assert(error);
	*error = '\0';

	/* connect to and receive an initial greeting from SMTP server */

	if (FAIL == zbx_tcp_connect(&s, CONFIG_SOURCE_IP, smtp_server, ZBX_DEFAULT_SMTP_PORT, 0))
	{
		zbx_snprintf(error, max_error_len, "Cannot connect to SMTP server [%s] [%s]", smtp_server, zbx_tcp_strerror());
		goto close;
	}
	if (-1 == smtp_readln(s.socket, cmd, sizeof(cmd)))
	{
		zbx_snprintf(error, max_error_len, "Error receiving initial string from SMTP server [%s]", strerror(errno));
		goto out;
	}
	if (0 != strncmp(cmd, OK_220, strlen(OK_220)))
	{
		zbx_snprintf(error, max_error_len, "No welcome message 220* from SMTP server [%s]", cmd);
		goto out;
	}

	/* send HELO */

	if (0 != strlen(smtp_helo))
	{
		zbx_snprintf(cmd, sizeof(cmd), "HELO %s\r\n", smtp_helo);
		if (-1 == write(s.socket, cmd, strlen(cmd)))
		{
			zbx_snprintf(error, max_error_len, "Error sending HELO to mailserver [%s]", strerror(errno));
			goto out;
		}
		if (-1 == smtp_readln(s.socket, cmd, sizeof(cmd)))
		{
			zbx_snprintf(error, max_error_len, "Error receiving answer on HELO request [%s]", strerror(errno));
			goto out;
		}
		if (0 != strncmp(cmd, OK_250, strlen(OK_250)))
		{
			zbx_snprintf(error, max_error_len, "Wrong answer on HELO [%s]", cmd);
			goto out;
		}
	}

	/* send MAIL FROM */

	zbx_snprintf(cmd, sizeof(cmd), "MAIL FROM: <%s>\r\n", smtp_email);
	if (-1 == write(s.socket, cmd, strlen(cmd)))
	{
		zbx_snprintf(error, max_error_len, "Error sending MAIL FROM to mailserver [%s]", strerror(errno));
		goto out;
	}
	if (-1 == smtp_readln(s.socket, cmd, sizeof(cmd)))
	{
		zbx_snprintf(error, max_error_len, "Error receiving answer on MAIL FROM request [%s]", strerror(errno));
		goto out;
	}
	if (0 != strncmp(cmd, OK_250, strlen(OK_250)))
	{
		zbx_snprintf(error, max_error_len, "Wrong answer on MAIL FROM [%s]", cmd);
		goto out;
	}

	/* send RCPT TO */

	zbx_snprintf(cmd, sizeof(cmd), "RCPT TO: <%s>\r\n", mailto);
	if (-1 == write(s.socket, cmd, strlen(cmd)))
	{
		zbx_snprintf(error, max_error_len, "Error sending RCPT TO to mailserver [%s]", strerror(errno));
		goto out;
	}
	if (-1 == smtp_readln(s.socket, cmd, sizeof(cmd)))
	{
		zbx_snprintf(error, max_error_len, "Error receiving answer on RCPT TO request [%s]", strerror(errno));
		goto out;
	}
	/* May return 251 as well: User not local; will forward to <forward-path>. See RFC825. */
	if (0 != strncmp(cmd, OK_250, strlen(OK_250)) && 0 != strncmp(cmd, OK_251, strlen(OK_251)))
	{
		zbx_snprintf(error, max_error_len, "Wrong answer on RCPT TO [%s]", cmd);
		goto out;
	}

	/* send DATA */

	zbx_snprintf(cmd, sizeof(cmd), "DATA\r\n");
	if (-1 == write(s.socket, cmd, strlen(cmd)))
	{
		zbx_snprintf(error, max_error_len, "Error sending DATA to mailserver [%s]", strerror(errno));
		goto out;
	}
	if (-1 == smtp_readln(s.socket, cmd, sizeof(cmd)))
	{
		zbx_snprintf(error, max_error_len, "Error receiving answer on DATA request [%s]", strerror(errno));
		goto out;
	}
	if (0 != strncmp(cmd, OK_354, strlen(OK_354)))
	{
		zbx_snprintf(error, max_error_len, "Wrong answer on DATA [%s]", cmd);
		goto out;
	}

	/* prepare subject */

	tmp = string_replace(mailsubject, "\r\n", " ");
	localsubject = string_replace(tmp, "\n", " ");
	zbx_free(tmp);

	if (FAIL == is_ascii_string(localsubject))
	{
		int	len;

		str_base64_encode_dyn(localsubject, &base64, strlen(localsubject));
		zbx_free(localsubject);

		len = strlen(base64) + 13;
		localsubject = zbx_malloc(NULL, len);
		zbx_snprintf(localsubject, len, "=?UTF-8?B?%s?=", base64);
		zbx_free(base64);
		base64 = NULL;
	}

	/* prepare body */

	tmp = string_replace(mailbody, "\r\n", "\n");
	localbody = string_replace(tmp, "\n", "\r\n");
	zbx_free(tmp);

	str_base64_encode_dyn(localbody, &base64, strlen(localbody));
	zbx_free(localbody);
	localbody = base64;
	base64 = NULL;

	/* prepare date */

	time(&email_time);
	local_time = localtime(&email_time);
	strftime(str_time, MAX_STRING_LEN, "%a, %d %b %Y %H:%M:%S %z", local_time);

	/* e-mails are sent in 'SMTP/MIME e-mail' format because UTF-8 is used both in mailsubject and mailbody */
	/* =?charset?encoding?encoded text?= format must be used for subject field */

	cmdp = zbx_dsprintf(cmdp,
			"From: <%s>\r\n"
			"To: <%s>\r\n"
			"Date: %s\r\n"
			"Subject: %s\r\n"
			"MIME-Version: 1.0\r\n"
			"Content-Type: text/plain; charset=\"UTF-8\"\r\n"
			"Content-Transfer-Encoding: base64\r\n"
			"\r\n"
			"%s",
			smtp_email, mailto, str_time, localsubject, localbody);

	err = write(s.socket, cmdp, strlen(cmdp));

	zbx_free(cmdp);
	zbx_free(localsubject);
	zbx_free(localbody);

	if (-1 == err)
	{
		zbx_snprintf(error, max_error_len, "Error sending headers and mail body to mailserver [%s]", strerror(errno));
		goto out;
	}

	/* send . */

	zbx_snprintf(cmd, sizeof(cmd), "\r\n.\r\n");
	if (-1 == write(s.socket, cmd, strlen(cmd)))
	{
		zbx_snprintf(error, max_error_len, "Error sending . to mailserver [%s]", strerror(errno));
		goto out;
	}
	if (-1 == smtp_readln(s.socket, cmd, sizeof(cmd)))
	{
		zbx_snprintf(error, max_error_len, "Error receiving answer on . request [%s]", strerror(errno));
		goto out;
	}
	if (0 != strncmp(cmd, OK_250, strlen(OK_250)))
	{
		zbx_snprintf(error, max_error_len, "Wrong answer on end of data [%s]", cmd);
		goto out;
	}

	/* send QUIT */

	zbx_snprintf(cmd, sizeof(cmd), "QUIT\r\n");
	if (-1 == write(s.socket, cmd, strlen(cmd)))
	{
		zbx_snprintf(error, max_error_len, "Error sending QUIT to mailserver [%s]", strerror(errno));
		goto out;
	}

	ret = SUCCEED;
out:
	zbx_tcp_close(&s);
close:
	if ('\0' != *error)
	{
		zabbix_log(LOG_LEVEL_DEBUG, "%s", error);
		zabbix_syslog("%s", error);
	}

	zabbix_log(LOG_LEVEL_DEBUG, "End of %s():%s", __function_name, zbx_result_string(ret));

	return ret;
}
