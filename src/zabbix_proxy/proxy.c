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

#include "cfg.h"
#include "pid.h"
#include "db.h"
#include "dbcache.h"
#include "log.h"
#include "zbxgetopt.h"
#include "mutexs.h"
#include "proxy.h"

#include "sysinfo.h"
#include "zbxserver.h"

#include "daemon.h"
#include "zbxself.h"

#include "../zabbix_server/dbsyncer/dbsyncer.h"
#include "../zabbix_server/discoverer/discoverer.h"
#include "../zabbix_server/httppoller/httppoller.h"
#include "housekeeper/housekeeper.h"
#include "../zabbix_server/pinger/pinger.h"
#include "../zabbix_server/poller/poller.h"
#include "../zabbix_server/poller/checks_ipmi.h"
#include "../zabbix_server/trapper/trapper.h"
#include "proxyconfig/proxyconfig.h"
#include "datasender/datasender.h"
#include "heart/heart.h"
#include "../zabbix_server/selfmon/selfmon.h"

const char	*progname = NULL;
const char	title_message[] = "Zabbix Proxy";
const char	usage_message[] = "[-hV] [-c <file>]";

#ifndef HAVE_GETOPT_LONG
const char	*help_message[] = {
	"Options:",
	"  -c <file>       absolute path to the configuration file",
	"  -h              give this help",
	"  -V              display version number",
	0 /* end of text */
};
#else
const char	*help_message[] = {
	"Options:",
	"  -c --config <file>       absolute path to the configuration file",
	"  -h --help                give this help",
	"  -V --version             display version number",
	0 /* end of text */
};
#endif

/* COMMAND LINE OPTIONS */

/* long options */

static struct zbx_option longopts[] =
{
	{"config",	1,	0,	'c'},
	{"help",	0,	0,	'h'},
	{"version",	0,	0,	'V'},
	{0,0,0,0}
};

/* short options */

static char	shortopts[] = "c:n:hV";

/* end of COMMAND LINE OPTIONS */

int	threads_num = 0;
pid_t	*threads = NULL;

static unsigned char	zbx_process = ZBX_PROCESS_PROXY_ACTIVE;

int		process_num		= 0;
unsigned char	process_type		= ZBX_PROCESS_TYPE_UNKNOWN;

int	CONFIG_PROXYMODE		= ZBX_PROXYMODE_ACTIVE;
int	CONFIG_DATASENDER_FORKS		= 1;
int	CONFIG_DISCOVERER_FORKS		= 1;
int	CONFIG_HOUSEKEEPER_FORKS	= 1;
int	CONFIG_PINGER_FORKS		= 1;
int	CONFIG_POLLER_FORKS		= 5;
int	CONFIG_UNREACHABLE_POLLER_FORKS	= 1;
int	CONFIG_HTTPPOLLER_FORKS		= 1;
int	CONFIG_IPMIPOLLER_FORKS		= 0;
int	CONFIG_TRAPPER_FORKS		= 5;
int	CONFIG_SELFMON_FORKS		= 0;
int	CONFIG_PROXYPOLLER_FORKS	= 0;
int	CONFIG_ESCALATOR_FORKS		= 0;
int	CONFIG_ALERTER_FORKS		= 0;
int	CONFIG_TIMER_FORKS		= 0;
int	CONFIG_NODEWATCHER_FORKS	= 0;
int	CONFIG_WATCHDOG_FORKS		= 0;
int	CONFIG_HEARTBEAT_FORKS		= 1;

int	CONFIG_LISTEN_PORT		= 10051;
char	*CONFIG_LISTEN_IP		= NULL;
char	*CONFIG_SOURCE_IP		= NULL;
int	CONFIG_TRAPPER_TIMEOUT		= 300;

int	CONFIG_HOUSEKEEPING_FREQUENCY	= 1;
int	CONFIG_PROXY_LOCAL_BUFFER	= 0;
int	CONFIG_PROXY_OFFLINE_BUFFER	= 1;

int	CONFIG_HEARTBEAT_FREQUENCY	= 60;

int	CONFIG_PROXYCONFIG_FREQUENCY	= 3600; /* 1h */
int	CONFIG_PROXYDATA_FREQUENCY	= 1;

int	CONFIG_SENDER_FREQUENCY		= 30;
int	CONFIG_HISTSYNCER_FORKS		= 4;
int	CONFIG_HISTSYNCER_FREQUENCY	= 5;
int	CONFIG_CONFSYNCER_FORKS		= 1;
int	CONFIG_CONFSYNCER_FREQUENCY	= 60;
int	CONFIG_CONF_CACHE_SIZE		= 8388608;	/* 8MB */
int	CONFIG_HISTORY_CACHE_SIZE	= 8388608;	/* 8MB */
int	CONFIG_TRENDS_CACHE_SIZE	= 4194304;	/* 4MB */
int	CONFIG_TEXT_CACHE_SIZE		= 16777216;	/* 16MB */
int	CONFIG_UNREACHABLE_PERIOD	= 45;
int	CONFIG_UNREACHABLE_DELAY	= 15;
int	CONFIG_UNAVAILABLE_DELAY	= 60;
int	CONFIG_LOG_LEVEL		= LOG_LEVEL_WARNING;
char	*CONFIG_ALERT_SCRIPTS_PATH	= NULL;
char	*CONFIG_EXTERNALSCRIPTS		= NULL;
char	*CONFIG_TMPDIR			= NULL;
char	*CONFIG_FPING_LOCATION		= NULL;
#ifdef HAVE_IPV6
char	*CONFIG_FPING6_LOCATION		= NULL;
#endif /* HAVE_IPV6 */
char	*CONFIG_DBHOST			= NULL;
char	*CONFIG_DBNAME			= NULL;
char	*CONFIG_DBSCHEMA		= NULL;
char	*CONFIG_DBUSER			= NULL;
char	*CONFIG_DBPASSWORD		= NULL;
char	*CONFIG_DBSOCKET		= NULL;
int	CONFIG_DBPORT			= 0;
int	CONFIG_ENABLE_REMOTE_COMMANDS	= 0;
int	CONFIG_LOG_REMOTE_COMMANDS	= 0;
int	CONFIG_UNSAFE_USER_PARAMETERS	= 0;

char	*CONFIG_SERVER			= NULL;
int	CONFIG_SERVER_PORT		= 10051;
char	*CONFIG_HOSTNAME		= NULL;
int	CONFIG_NODEID			= -1;
int	CONFIG_MASTER_NODEID		= 0;
int	CONFIG_NODE_NOHISTORY		= 0;

char	*CONFIG_SSH_KEY_LOCATION	= NULL;

int	CONFIG_LOG_SLOW_QUERIES		= 0;	/* ms; 0 - disable */

/* Global variable to control if we should write warnings to log[] */
int	CONFIG_ENABLE_LOG		= 1;

int	CONFIG_REFRESH_UNSUPPORTED	= 600;

/* Zabbix server startup time */
int	CONFIG_SERVER_STARTUP_TIME	= 0;

/* Mutex for node syncs; not used in proxy */
ZBX_MUTEX	node_sync_access;

/******************************************************************************
 *                                                                            *
 * Function: init_config                                                      *
 *                                                                            *
 * Purpose: parse config file and update configuration parameters             *
 *                                                                            *
 * Parameters:                                                                *
 *                                                                            *
 * Return value:                                                              *
 *                                                                            *
 * Author: Alexei Vladishev                                                   *
 *                                                                            *
 * Comments: will terminate process if parsing fails                          *
 *                                                                            *
 ******************************************************************************/
static void	init_config()
{
	AGENT_RESULT	result;
	char		**value = NULL;

	static struct cfg_line	cfg[] =
	{
		/* PARAMETER,			VAR,					FUNC,
			TYPE,		MANDATORY,	MIN,			MAX */
		{"ProxyMode",			&CONFIG_PROXYMODE,			NULL,
			TYPE_INT,	PARM_OPT,	ZBX_PROXYMODE_ACTIVE,	ZBX_PROXYMODE_PASSIVE},
		{"Server",			&CONFIG_SERVER,				NULL,
			TYPE_STRING,	PARM_OPT,	0,			0},
		{"ServerPort",			&CONFIG_SERVER_PORT,			NULL,
			TYPE_INT,	PARM_OPT,	1024,			32767},
		{"Hostname",			&CONFIG_HOSTNAME,			NULL,
			TYPE_STRING,	PARM_OPT,	0,			0},
		{"StartDBSyncers",		&CONFIG_HISTSYNCER_FORKS,		NULL,
			TYPE_INT,	PARM_OPT,	1,			100},
		{"StartDiscoverers",		&CONFIG_DISCOVERER_FORKS,		NULL,
			TYPE_INT,	PARM_OPT,	0,			250},
		{"StartHTTPPollers",		&CONFIG_HTTPPOLLER_FORKS,		NULL,
			TYPE_INT,	PARM_OPT,	0,			1000},
		{"StartPingers",		&CONFIG_PINGER_FORKS,			NULL,
			TYPE_INT,	PARM_OPT,	0,			1000},
		{"StartPollers",		&CONFIG_POLLER_FORKS,			NULL,
			TYPE_INT,	PARM_OPT,	0,			1000},
		{"StartPollersUnreachable",	&CONFIG_UNREACHABLE_POLLER_FORKS,	NULL,
			TYPE_INT,	PARM_OPT,	0,			1000},
		{"StartIPMIPollers",		&CONFIG_IPMIPOLLER_FORKS,		NULL,
			TYPE_INT,	PARM_OPT,	0,			1000},
		{"StartTrappers",		&CONFIG_TRAPPER_FORKS,			NULL,
			TYPE_INT,	PARM_OPT,	0,			1000},
		{"CacheSize",			&CONFIG_CONF_CACHE_SIZE,		NULL,
			TYPE_INT,	PARM_OPT,	128 * ZBX_KIBIBYTE,	ZBX_GIBIBYTE},
		{"HistoryCacheSize",		&CONFIG_HISTORY_CACHE_SIZE,		NULL,
			TYPE_INT,	PARM_OPT,	128 * ZBX_KIBIBYTE,	ZBX_GIBIBYTE},
		{"TrendCacheSize",		&CONFIG_TRENDS_CACHE_SIZE,		NULL,
			TYPE_INT,	PARM_OPT,	128 * ZBX_KIBIBYTE,	ZBX_GIBIBYTE},
		{"HistoryTextCacheSize",	&CONFIG_TEXT_CACHE_SIZE,		NULL,
			TYPE_INT,	PARM_OPT,	128 * ZBX_KIBIBYTE,	ZBX_GIBIBYTE},
		{"CacheUpdateFrequency",	&CONFIG_CONFSYNCER_FREQUENCY,		NULL,
			TYPE_INT,	PARM_OPT,	1,			SEC_PER_HOUR},
		{"HousekeepingFrequency",	&CONFIG_HOUSEKEEPING_FREQUENCY,		NULL,
			TYPE_INT,	PARM_OPT,	1,			24},
		{"ProxyLocalBuffer",		&CONFIG_PROXY_LOCAL_BUFFER,		NULL,
			TYPE_INT,	PARM_OPT,	0,			720},
		{"ProxyOfflineBuffer",		&CONFIG_PROXY_OFFLINE_BUFFER,		NULL,
			TYPE_INT,	PARM_OPT,	1,			720},
		{"HeartbeatFrequency",		&CONFIG_HEARTBEAT_FREQUENCY,		NULL,
			TYPE_INT,	PARM_OPT,	0,			SEC_PER_HOUR},
		{"ConfigFrequency",		&CONFIG_PROXYCONFIG_FREQUENCY,		NULL,
			TYPE_INT,	PARM_OPT,	1,			SEC_PER_WEEK},
		{"DataSenderFrequency",		&CONFIG_PROXYDATA_FREQUENCY,		NULL,
			TYPE_INT,	PARM_OPT,	1,			SEC_PER_HOUR},
		{"TmpDir",			&CONFIG_TMPDIR,				NULL,
			TYPE_STRING,	PARM_OPT,	0,			0},
		{"FpingLocation",		&CONFIG_FPING_LOCATION,			NULL,
			TYPE_STRING,	PARM_OPT,	0,			0},
#ifdef HAVE_IPV6
		{"Fping6Location",		&CONFIG_FPING6_LOCATION,		NULL,
			TYPE_STRING,	PARM_OPT,	0,			0},
#endif	/* HAVE_IPV6 */
		{"Timeout",			&CONFIG_TIMEOUT,			NULL,
			TYPE_INT,	PARM_OPT,	1,			30},
		{"TrapperTimeout",		&CONFIG_TRAPPER_TIMEOUT,		NULL,
			TYPE_INT,	PARM_OPT,	1,			300},
		{"UnreachablePeriod",		&CONFIG_UNREACHABLE_PERIOD,		NULL,
			TYPE_INT,	PARM_OPT,	1,			SEC_PER_HOUR},
		{"UnreachableDelay",		&CONFIG_UNREACHABLE_DELAY,		NULL,
			TYPE_INT,	PARM_OPT,	1,			SEC_PER_HOUR},
		{"UnavailableDelay",		&CONFIG_UNAVAILABLE_DELAY,		NULL,
			TYPE_INT,	PARM_OPT,	1,			SEC_PER_HOUR},
		{"ListenIP",			&CONFIG_LISTEN_IP,			NULL,
			TYPE_STRING,	PARM_OPT,	0,			0},
		{"ListenPort",			&CONFIG_LISTEN_PORT,			NULL,
			TYPE_INT,	PARM_OPT,	1024,			32767},
		{"SourceIP",			&CONFIG_SOURCE_IP,			NULL,
			TYPE_STRING,	PARM_OPT,	0,			0},
		{"DebugLevel",			&CONFIG_LOG_LEVEL,			NULL,
			TYPE_INT,	PARM_OPT,	0,			4},
		{"PidFile",			&CONFIG_PID_FILE,			NULL,
			TYPE_STRING,	PARM_OPT,	0,			0},
		{"LogFile",			&CONFIG_LOG_FILE,			NULL,
			TYPE_STRING,	PARM_OPT,	0,			0},
		{"LogFileSize",			&CONFIG_LOG_FILE_SIZE,			NULL,
			TYPE_INT,	PARM_OPT,	0,			1024},
		{"ExternalScripts",		&CONFIG_EXTERNALSCRIPTS,		NULL,
			TYPE_STRING,	PARM_OPT,	0,			0},
		{"DBHost",			&CONFIG_DBHOST,				NULL,
			TYPE_STRING,	PARM_OPT,	0,			0},
		{"DBName",			&CONFIG_DBNAME,				NULL,
			TYPE_STRING,	PARM_MAND,	0,			0},
		{"DBSchema",			&CONFIG_DBSCHEMA,			NULL,
			TYPE_STRING,	PARM_OPT,	0,			0},
		{"DBUser",			&CONFIG_DBUSER,				NULL,
			TYPE_STRING,	PARM_OPT,	0,			0},
		{"DBPassword",			&CONFIG_DBPASSWORD,			NULL,
			TYPE_STRING,	PARM_OPT,	0,			0},
		{"DBSocket",			&CONFIG_DBSOCKET,			NULL,
			TYPE_STRING,	PARM_OPT,	0,			0},
		{"DBPort",			&CONFIG_DBPORT,				NULL,
			TYPE_INT,	PARM_OPT,	1024,			65535},
		{"SSHKeyLocation",		&CONFIG_SSH_KEY_LOCATION,		NULL,
			TYPE_STRING,	PARM_OPT,	0,			0},
		{"LogSlowQueries",		&CONFIG_LOG_SLOW_QUERIES,		NULL,
			TYPE_INT,	PARM_OPT,	0,			3600000},
		{NULL}
	};

	CONFIG_SERVER_STARTUP_TIME = time(NULL);

	parse_cfg_file(CONFIG_FILE, cfg);

	if (ZBX_PROXYMODE_ACTIVE == CONFIG_PROXYMODE &&
			(NULL == CONFIG_SERVER || '\0' == *CONFIG_SERVER))
	{
		zbx_error("Missing mandatory parameter [Server].");
	}

	if (ZBX_PROXYMODE_PASSIVE == CONFIG_PROXYMODE)
	{
		CONFIG_CONFSYNCER_FORKS = CONFIG_DATASENDER_FORKS = 0;
		zbx_process = ZBX_PROCESS_PROXY_PASSIVE;
	}
	else
		zbx_process = ZBX_PROCESS_PROXY_ACTIVE;

	if (NULL == CONFIG_HOSTNAME || '\0' == *CONFIG_HOSTNAME)
	{
		if (NULL != CONFIG_HOSTNAME)
			zbx_free(CONFIG_HOSTNAME);

		if (SUCCEED == process("system.hostname", 0, &result))
		{
			if (NULL != (value = GET_STR_RESULT(&result)))
			{
				CONFIG_HOSTNAME = zbx_strdup(CONFIG_HOSTNAME, *value);
				if (strlen(CONFIG_HOSTNAME) > HOST_HOST_LEN)
					CONFIG_HOSTNAME[HOST_HOST_LEN] = '\0';
			}
		}
		free_result(&result);

		if (NULL == CONFIG_HOSTNAME)
		{
			zabbix_log(LOG_LEVEL_CRIT, "Hostname is not defined");
			exit(1);
		}
	}
	else
	{
		if (strlen(CONFIG_HOSTNAME) > HOST_HOST_LEN)
		{
			zabbix_log(LOG_LEVEL_CRIT, "Hostname too long");
			exit(1);
		}
	}

	if (NULL == CONFIG_DBNAME)
	{
		zabbix_log(LOG_LEVEL_CRIT, "DBName not in config file");
		exit(1);
	}

	if (NULL == CONFIG_PID_FILE)
	{
		CONFIG_PID_FILE = zbx_strdup(CONFIG_PID_FILE, "/tmp/zabbix_proxy.pid");
	}

	if (NULL == CONFIG_TMPDIR)
	{
		CONFIG_TMPDIR = zbx_strdup(CONFIG_TMPDIR, "/tmp");
	}

	if (NULL == CONFIG_FPING_LOCATION)
	{
		CONFIG_FPING_LOCATION = zbx_strdup(CONFIG_FPING_LOCATION, "/usr/sbin/fping");
	}
#ifdef HAVE_IPV6
	if (NULL == CONFIG_FPING6_LOCATION)
	{
		CONFIG_FPING6_LOCATION = zbx_strdup(CONFIG_FPING6_LOCATION, "/usr/sbin/fping6");
	}
#endif /* HAVE_IPV6 */

	if (NULL == CONFIG_EXTERNALSCRIPTS)
	{
		CONFIG_EXTERNALSCRIPTS = zbx_strdup(CONFIG_EXTERNALSCRIPTS, "/etc/zabbix/externalscripts");
	}

	if (ZBX_PROXYMODE_ACTIVE != CONFIG_PROXYMODE || 0 == CONFIG_HEARTBEAT_FREQUENCY)
		CONFIG_HEARTBEAT_FORKS = 0;
}

/******************************************************************************
 *                                                                            *
 * Function: main                                                             *
 *                                                                            *
 * Purpose: executes server processes                                         *
 *                                                                            *
 * Parameters:                                                                *
 *                                                                            *
 * Return value:                                                              *
 *                                                                            *
 * Author: Eugene Grigorjev                                                   *
 *                                                                            *
 * Comments:                                                                  *
 *                                                                            *
 ******************************************************************************/
int	main(int argc, char **argv)
{
	char	ch;

	progname = get_program_name(argv[0]);

	/* Parse the command-line. */
	while ((ch = (char)zbx_getopt_long(argc, argv, shortopts, longopts,NULL)) != (char)EOF)
	{
		switch (ch)
		{
			case 'c':
				CONFIG_FILE = zbx_strdup(CONFIG_FILE, zbx_optarg);
				break;
			case 'h':
				help();
				exit(-1);
				break;
			case 'V':
				version();
				exit(-1);
				break;
			default:
				usage();
				exit(-1);
				break;
		}
	}

	if (NULL == CONFIG_FILE)
		CONFIG_FILE = zbx_strdup(CONFIG_FILE, "/etc/zabbix/zabbix_proxy.conf");

	/* Required for simple checks */
	init_metrics();

	init_config();

#ifdef HAVE_OPENIPMI
	init_ipmi_handler();
#endif

	return daemon_start(CONFIG_ALLOW_ROOT);
}

int	MAIN_ZABBIX_ENTRY()
{
	int	i;
	pid_t	pid;

	zbx_sock_t	listen_sock;

	int		server_num = 0;

	if (NULL == CONFIG_LOG_FILE || '\0' == *CONFIG_LOG_FILE)
	{
		zabbix_open_log(LOG_TYPE_SYSLOG,CONFIG_LOG_LEVEL,NULL);
	}
	else
	{
		zabbix_open_log(LOG_TYPE_FILE,CONFIG_LOG_LEVEL,CONFIG_LOG_FILE);
	}

#ifdef	HAVE_SNMP
#	define SNMP_FEATURE_STATUS "YES"
#else
#	define SNMP_FEATURE_STATUS " NO"
#endif
#ifdef	HAVE_OPENIPMI
#	define IPMI_FEATURE_STATUS "YES"
#else
#	define IPMI_FEATURE_STATUS " NO"
#endif
#ifdef	HAVE_ODBC
#	define ODBC_FEATURE_STATUS "YES"
#else
#	define ODBC_FEATURE_STATUS " NO"
#endif
#ifdef	HAVE_SSH2
#	define SSH2_FEATURE_STATUS "YES"
#else
#	define SSH2_FEATURE_STATUS " NO"
#endif
#ifdef	HAVE_IPV6
#	define IPV6_FEATURE_STATUS "YES"
#else
#	define IPV6_FEATURE_STATUS " NO"
#endif

	zabbix_log(LOG_LEVEL_INFORMATION, "Starting Zabbix Proxy. Zabbix %s (revision %s).",
			ZABBIX_VERSION, ZABBIX_REVISION);

	zabbix_log(LOG_LEVEL_INFORMATION, "**** Enabled features ****");
	zabbix_log(LOG_LEVEL_INFORMATION, "SNMP monitoring:       " SNMP_FEATURE_STATUS);
	zabbix_log(LOG_LEVEL_INFORMATION, "IPMI monitoring:       " IPMI_FEATURE_STATUS);
	zabbix_log(LOG_LEVEL_INFORMATION, "ODBC:                  " ODBC_FEATURE_STATUS);
	zabbix_log(LOG_LEVEL_INFORMATION, "SSH2 support:          " SSH2_FEATURE_STATUS);
	zabbix_log(LOG_LEVEL_INFORMATION, "IPv6 support:          " IPV6_FEATURE_STATUS);
	zabbix_log(LOG_LEVEL_INFORMATION, "**************************");

	DBinit();

	init_database_cache(zbx_process);
	init_configuration_cache();
	init_selfmon_collector();

	DBconnect(ZBX_DB_CONNECT_EXIT);
	DCsync_configuration();
	DBclose();

	threads_num = 1 + CONFIG_CONFSYNCER_FORKS + CONFIG_DATASENDER_FORKS + CONFIG_POLLER_FORKS
			+ CONFIG_UNREACHABLE_POLLER_FORKS + CONFIG_TRAPPER_FORKS + CONFIG_PINGER_FORKS
			+ CONFIG_HOUSEKEEPER_FORKS + CONFIG_HTTPPOLLER_FORKS + CONFIG_DISCOVERER_FORKS
			+ CONFIG_HISTSYNCER_FORKS + CONFIG_IPMIPOLLER_FORKS + CONFIG_SELFMON_FORKS;
	threads = calloc(threads_num, sizeof(pid_t));

	if (CONFIG_TRAPPER_FORKS > 0)
	{
		if (FAIL == zbx_tcp_listen(&listen_sock, CONFIG_LISTEN_IP, (unsigned short)CONFIG_LISTEN_PORT))
		{
			zabbix_log(LOG_LEVEL_CRIT, "Listener failed with error: %s.", zbx_tcp_strerror());
			exit(1);
		}
	}

	for (i = 1; i <= CONFIG_CONFSYNCER_FORKS + CONFIG_DATASENDER_FORKS + CONFIG_POLLER_FORKS
			+ CONFIG_UNREACHABLE_POLLER_FORKS + CONFIG_TRAPPER_FORKS + CONFIG_PINGER_FORKS
			+ CONFIG_HOUSEKEEPER_FORKS + CONFIG_HTTPPOLLER_FORKS + CONFIG_DISCOVERER_FORKS
			+ CONFIG_HISTSYNCER_FORKS + CONFIG_IPMIPOLLER_FORKS + CONFIG_SELFMON_FORKS;
		i++)
	{
		if (0 == (pid = zbx_fork()))
		{
			server_num = i;
			break;
		}
		else
			threads[i] = pid;
	}

	/* Main process */
	if (server_num == 0)
	{
		set_parent_signal_handler();

		if (0 != CONFIG_HEARTBEAT_FORKS)
		{
			process_type = ZBX_PROCESS_TYPE_HEARTBEAT;
			process_num = 1;

			zabbix_log(LOG_LEVEL_WARNING, "server #%d started [%s]",
					server_num, get_process_type_string(process_type));

			main_heart_loop();
		}
		else
		{
			zabbix_log(LOG_LEVEL_WARNING, "server #%d started");

			for (;;)
				zbx_sleep(SEC_PER_HOUR);
		}
	}
	else if (server_num <= CONFIG_CONFSYNCER_FORKS)
	{
		process_type = ZBX_PROCESS_TYPE_CONFSYNCER;
		process_num = server_num;

		zabbix_log(LOG_LEVEL_WARNING, "server #%d started [%s]",
				server_num, get_process_type_string(process_type));

		main_proxyconfig_loop(server_num);
	}
	else if (server_num <= CONFIG_CONFSYNCER_FORKS + CONFIG_DATASENDER_FORKS)
	{
		process_type = ZBX_PROCESS_TYPE_DATASENDER;
		process_num = server_num - CONFIG_CONFSYNCER_FORKS;

		zabbix_log(LOG_LEVEL_WARNING, "server #%d started [%s]",
				server_num, get_process_type_string(process_type));

		main_datasender_loop();
	}
	else if (server_num <= CONFIG_CONFSYNCER_FORKS + CONFIG_DATASENDER_FORKS
			+ CONFIG_POLLER_FORKS)
	{
#ifdef HAVE_SNMP
		init_snmp("zabbix_server");
#endif	/* HAVE_SNMP */

		process_type = ZBX_PROCESS_TYPE_POLLER;
		process_num = server_num - CONFIG_CONFSYNCER_FORKS - CONFIG_DATASENDER_FORKS;

		zabbix_log(LOG_LEVEL_WARNING, "server #%d started [%s]",
				server_num, get_process_type_string(process_type));

		main_poller_loop(zbx_process, ZBX_POLLER_TYPE_NORMAL);
	}
	else if (server_num <= CONFIG_CONFSYNCER_FORKS + CONFIG_DATASENDER_FORKS
			+ CONFIG_POLLER_FORKS + CONFIG_UNREACHABLE_POLLER_FORKS)
	{
#ifdef HAVE_SNMP
		init_snmp("zabbix_server");
#endif	/* HAVE_SNMP */

		process_type = ZBX_PROCESS_TYPE_UNREACHABLE;
		process_num = server_num - CONFIG_CONFSYNCER_FORKS - CONFIG_DATASENDER_FORKS
				- CONFIG_POLLER_FORKS;

		zabbix_log(LOG_LEVEL_WARNING, "server #%d started [%s]",
				server_num, get_process_type_string(process_type));

		main_poller_loop(zbx_process, ZBX_POLLER_TYPE_UNREACHABLE);
	}
	else if (server_num <= CONFIG_CONFSYNCER_FORKS + CONFIG_DATASENDER_FORKS
			+ CONFIG_POLLER_FORKS + CONFIG_UNREACHABLE_POLLER_FORKS
			+ CONFIG_TRAPPER_FORKS)
	{
		process_type = ZBX_PROCESS_TYPE_TRAPPER;
		process_num = server_num - CONFIG_CONFSYNCER_FORKS - CONFIG_DATASENDER_FORKS
				- CONFIG_POLLER_FORKS - CONFIG_UNREACHABLE_POLLER_FORKS;

		zabbix_log(LOG_LEVEL_WARNING, "server #%d started [%s]",
				server_num, get_process_type_string(process_type));

		main_trapper_loop(zbx_process, &listen_sock);
	}
	else if (server_num <= CONFIG_CONFSYNCER_FORKS + CONFIG_DATASENDER_FORKS
			+ CONFIG_POLLER_FORKS + CONFIG_UNREACHABLE_POLLER_FORKS
			+ CONFIG_TRAPPER_FORKS + CONFIG_PINGER_FORKS)
	{
		process_type = ZBX_PROCESS_TYPE_PINGER;
		process_num = server_num - CONFIG_CONFSYNCER_FORKS - CONFIG_DATASENDER_FORKS
				- CONFIG_POLLER_FORKS - CONFIG_UNREACHABLE_POLLER_FORKS
				- CONFIG_TRAPPER_FORKS;

		zabbix_log(LOG_LEVEL_WARNING, "server #%d started [%s]",
				server_num, get_process_type_string(process_type));

		main_pinger_loop();
	}
	else if (server_num <= CONFIG_CONFSYNCER_FORKS + CONFIG_DATASENDER_FORKS
			+ CONFIG_POLLER_FORKS + CONFIG_UNREACHABLE_POLLER_FORKS
			+ CONFIG_TRAPPER_FORKS + CONFIG_PINGER_FORKS
			+ CONFIG_HOUSEKEEPER_FORKS)
	{
		process_type = ZBX_PROCESS_TYPE_HOUSEKEEPER;
		process_num = server_num - CONFIG_CONFSYNCER_FORKS - CONFIG_DATASENDER_FORKS
				- CONFIG_POLLER_FORKS - CONFIG_UNREACHABLE_POLLER_FORKS
				- CONFIG_TRAPPER_FORKS - CONFIG_PINGER_FORKS;

		zabbix_log(LOG_LEVEL_WARNING, "server #%d started [%s]",
				server_num, get_process_type_string(process_type));

		main_housekeeper_loop();
	}
	else if (server_num <= CONFIG_CONFSYNCER_FORKS + CONFIG_DATASENDER_FORKS
			+ CONFIG_POLLER_FORKS + CONFIG_UNREACHABLE_POLLER_FORKS
			+ CONFIG_TRAPPER_FORKS + CONFIG_PINGER_FORKS
			+ CONFIG_HOUSEKEEPER_FORKS + CONFIG_HTTPPOLLER_FORKS)
	{
		process_type = ZBX_PROCESS_TYPE_HTTPPOLLER;
		process_num = server_num - CONFIG_CONFSYNCER_FORKS - CONFIG_DATASENDER_FORKS
				- CONFIG_POLLER_FORKS - CONFIG_UNREACHABLE_POLLER_FORKS
				- CONFIG_TRAPPER_FORKS - CONFIG_PINGER_FORKS
				- CONFIG_HOUSEKEEPER_FORKS;

		zabbix_log(LOG_LEVEL_WARNING, "server #%d started [%s]",
				server_num, get_process_type_string(process_type));

		main_httppoller_loop();
	}
	else if (server_num <= CONFIG_CONFSYNCER_FORKS + CONFIG_DATASENDER_FORKS
			+ CONFIG_POLLER_FORKS + CONFIG_UNREACHABLE_POLLER_FORKS
			+ CONFIG_TRAPPER_FORKS + CONFIG_PINGER_FORKS
			+ CONFIG_HOUSEKEEPER_FORKS + CONFIG_HTTPPOLLER_FORKS
			+ CONFIG_DISCOVERER_FORKS)
	{
#ifdef HAVE_SNMP
		init_snmp("zabbix_server");
#endif	/* HAVE_SNMP */

		process_type = ZBX_PROCESS_TYPE_DISCOVERER;
		process_num = server_num - CONFIG_CONFSYNCER_FORKS - CONFIG_DATASENDER_FORKS
				- CONFIG_POLLER_FORKS - CONFIG_UNREACHABLE_POLLER_FORKS
				- CONFIG_TRAPPER_FORKS - CONFIG_PINGER_FORKS
				- CONFIG_HOUSEKEEPER_FORKS - CONFIG_HTTPPOLLER_FORKS;

		zabbix_log(LOG_LEVEL_WARNING, "server #%d started [%s]",
				server_num, get_process_type_string(process_type));

		main_discoverer_loop(zbx_process);
	}
	else if (server_num <= CONFIG_CONFSYNCER_FORKS + CONFIG_DATASENDER_FORKS
			+ CONFIG_POLLER_FORKS + CONFIG_UNREACHABLE_POLLER_FORKS
			+ CONFIG_TRAPPER_FORKS + CONFIG_PINGER_FORKS
			+ CONFIG_HOUSEKEEPER_FORKS + CONFIG_HTTPPOLLER_FORKS
			+ CONFIG_DISCOVERER_FORKS + CONFIG_HISTSYNCER_FORKS)
	{
		process_type = ZBX_PROCESS_TYPE_HISTSYNCER;
		process_num = server_num - CONFIG_CONFSYNCER_FORKS - CONFIG_DATASENDER_FORKS
				- CONFIG_POLLER_FORKS - CONFIG_UNREACHABLE_POLLER_FORKS
				- CONFIG_TRAPPER_FORKS - CONFIG_PINGER_FORKS
				- CONFIG_HOUSEKEEPER_FORKS - CONFIG_HTTPPOLLER_FORKS
				- CONFIG_DISCOVERER_FORKS;

		zabbix_log(LOG_LEVEL_WARNING, "server #%d started [%s]",
				server_num, get_process_type_string(process_type));

		main_dbsyncer_loop();
	}
	else if (server_num <= CONFIG_CONFSYNCER_FORKS + CONFIG_DATASENDER_FORKS
			+ CONFIG_POLLER_FORKS + CONFIG_UNREACHABLE_POLLER_FORKS
			+ CONFIG_TRAPPER_FORKS + CONFIG_PINGER_FORKS
			+ CONFIG_HOUSEKEEPER_FORKS + CONFIG_HTTPPOLLER_FORKS
			+ CONFIG_DISCOVERER_FORKS + CONFIG_HISTSYNCER_FORKS
			+ CONFIG_IPMIPOLLER_FORKS)
	{
		process_type = ZBX_PROCESS_TYPE_IPMIPOLLER;
		process_num = server_num - CONFIG_CONFSYNCER_FORKS - CONFIG_DATASENDER_FORKS
				- CONFIG_POLLER_FORKS - CONFIG_UNREACHABLE_POLLER_FORKS
				- CONFIG_TRAPPER_FORKS - CONFIG_PINGER_FORKS
				- CONFIG_HOUSEKEEPER_FORKS - CONFIG_HTTPPOLLER_FORKS
				- CONFIG_DISCOVERER_FORKS - CONFIG_HISTSYNCER_FORKS;

		zabbix_log(LOG_LEVEL_WARNING, "server #%d started [%s]",
				server_num, get_process_type_string(process_type));

		main_poller_loop(zbx_process, ZBX_POLLER_TYPE_IPMI);
	}
	else if (server_num <= CONFIG_CONFSYNCER_FORKS + CONFIG_DATASENDER_FORKS
			+ CONFIG_POLLER_FORKS + CONFIG_UNREACHABLE_POLLER_FORKS
			+ CONFIG_TRAPPER_FORKS + CONFIG_PINGER_FORKS
			+ CONFIG_HOUSEKEEPER_FORKS + CONFIG_HTTPPOLLER_FORKS
			+ CONFIG_DISCOVERER_FORKS + CONFIG_HISTSYNCER_FORKS
			+ CONFIG_IPMIPOLLER_FORKS + CONFIG_SELFMON_FORKS)
	{
		process_type = ZBX_PROCESS_TYPE_SELFMON;
		process_num = server_num - CONFIG_CONFSYNCER_FORKS - CONFIG_DATASENDER_FORKS
				- CONFIG_POLLER_FORKS - CONFIG_UNREACHABLE_POLLER_FORKS
				- CONFIG_TRAPPER_FORKS - CONFIG_PINGER_FORKS
				- CONFIG_HOUSEKEEPER_FORKS - CONFIG_HTTPPOLLER_FORKS
				- CONFIG_DISCOVERER_FORKS - CONFIG_HISTSYNCER_FORKS
				- CONFIG_IPMIPOLLER_FORKS;

		zabbix_log(LOG_LEVEL_WARNING, "server #%d started [%s]",
				server_num, get_process_type_string(process_type));

		main_selfmon_loop();
	}

	return SUCCEED;
}

void	zbx_on_exit()
{
	zabbix_log(LOG_LEVEL_DEBUG, "zbx_on_exit() called");

	if (NULL != threads)
	{
		int	i;

		for (i = 1; i <= CONFIG_CONFSYNCER_FORKS + CONFIG_DATASENDER_FORKS
				+ CONFIG_POLLER_FORKS + CONFIG_UNREACHABLE_POLLER_FORKS
				+ CONFIG_TRAPPER_FORKS + CONFIG_PINGER_FORKS
				+ CONFIG_HOUSEKEEPER_FORKS + CONFIG_HTTPPOLLER_FORKS
				+ CONFIG_DISCOVERER_FORKS + CONFIG_HISTSYNCER_FORKS
				+ CONFIG_IPMIPOLLER_FORKS + CONFIG_SELFMON_FORKS; i++)
		{
			if (threads[i])
			{
				kill(threads[i], SIGTERM);
				threads[i] = ZBX_THREAD_HANDLE_NULL;
			}
		}

		zbx_free(threads);
	}

#ifdef USE_PID_FILE

	daemon_stop();

#endif /* USE_PID_FILE */

	free_metrics();

	zbx_sleep(2); /* wait for all threads closing */

	DBconnect(ZBX_DB_CONNECT_EXIT);
	free_database_cache();
	free_configuration_cache();
	DBclose();

#ifdef HAVE_OPENIPMI
	free_ipmi_handler();
#endif

#ifdef HAVE_SQLITE3
	php_sem_remove(&sqlite_access);
#endif	/* HAVE_SQLITE3 */

	free_selfmon_collector();

	zabbix_log(LOG_LEVEL_INFORMATION, "Zabbix Proxy stopped. Zabbix %s (revision %s).",
			ZABBIX_VERSION, ZABBIX_REVISION);

	zabbix_close_log();

	exit(SUCCEED);
}
