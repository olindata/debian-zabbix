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

#include "sysinfo.h"
#include "zbxserver.h"

#include "daemon.h"
#include "zbxself.h"

#include "alerter/alerter.h"
#include "dbsyncer/dbsyncer.h"
#include "dbconfig/dbconfig.h"
#include "discoverer/discoverer.h"
#include "httppoller/httppoller.h"
#include "housekeeper/housekeeper.h"
#include "pinger/pinger.h"
#include "poller/poller.h"
#include "poller/checks_ipmi.h"
#include "timer/timer.h"
#include "trapper/trapper.h"
#include "nodewatcher/nodewatcher.h"
#include "watchdog/watchdog.h"
#include "utils/nodechange.h"
#include "escalator/escalator.h"
#include "proxypoller/proxypoller.h"
#include "selfmon/selfmon.h"

const char	*progname = NULL;
const char	title_message[] = "Zabbix Server";
const char	usage_message[] = "[-hV] [-c <file>] [-n <nodeid>]";

#ifndef HAVE_GETOPT_LONG
const char	*help_message[] = {
	"Options:",
	"  -c <file>       absolute path to the configuration file",
	"  -h              give this help",
	"  -n <nodeid>     convert database data to new nodeid",
	"  -V              display version number",
	0 /* end of text */
};
#else
const char	*help_message[] = {
	"Options:",
	"  -c --config <file>       absolute path to the configuration file",
	"  -h --help                give this help",
	"  -n --new-nodeid <nodeid> convert database data to new nodeid",
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
	{"new-nodeid",	1,	0,	'n'},
	{"version",	0,	0,	'V'},
	{0,0,0,0}
};

/* short options */

static char	shortopts[] = "c:n:hV";

/* end of COMMAND LINE OPTIONS */

int	threads_num = 0;
pid_t	*threads = NULL;

int		process_num		= 0;
unsigned char	process_type		= ZBX_PROCESS_TYPE_UNKNOWN;

int	CONFIG_ALERTER_FORKS		= 1;
int	CONFIG_DISCOVERER_FORKS		= 1;
int	CONFIG_HOUSEKEEPER_FORKS	= 1;
int	CONFIG_NODEWATCHER_FORKS	= 1;
int	CONFIG_PINGER_FORKS		= 1;
int	CONFIG_POLLER_FORKS		= 5;
int	CONFIG_UNREACHABLE_POLLER_FORKS	= 1;
int	CONFIG_HTTPPOLLER_FORKS		= 1;
int	CONFIG_IPMIPOLLER_FORKS		= 0;
int	CONFIG_TIMER_FORKS		= 1;
int	CONFIG_TRAPPER_FORKS		= 5;
int	CONFIG_ESCALATOR_FORKS		= 1;
int	CONFIG_SELFMON_FORKS		= 1;
int	CONFIG_WATCHDOG_FORKS		= 1;
int	CONFIG_DATASENDER_FORKS		= 0;
int	CONFIG_HEARTBEAT_FORKS		= 0;

int	CONFIG_LISTEN_PORT		= 10051;
char	*CONFIG_LISTEN_IP		= NULL;
char	*CONFIG_SOURCE_IP		= NULL;
int	CONFIG_TRAPPER_TIMEOUT		= 300;

int	CONFIG_HOUSEKEEPING_FREQUENCY	= 1;
int	CONFIG_MAX_HOUSEKEEPER_DELETE	= 500;		/* applies for every separate field value */
int	CONFIG_SENDER_FREQUENCY		= 30;
int	CONFIG_HISTSYNCER_FORKS		= 4;
int	CONFIG_HISTSYNCER_FREQUENCY	= 5;
int	CONFIG_CONFSYNCER_FORKS		= 1;
int	CONFIG_CONFSYNCER_FREQUENCY	= 60;
int	CONFIG_CONF_CACHE_SIZE		= 8388608;	/* 8MB */
int	CONFIG_HISTORY_CACHE_SIZE	= 8388608;	/* 8MB */
int	CONFIG_TRENDS_CACHE_SIZE	= 4194304;	/* 4MB */
int	CONFIG_TEXT_CACHE_SIZE		= 16777216;	/* 16MB */
int	CONFIG_DISABLE_HOUSEKEEPING	= 0;
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

int	CONFIG_NODEID			= 0;
int	CONFIG_MASTER_NODEID		= 0;
int	CONFIG_NODE_NOEVENTS		= 0;
int	CONFIG_NODE_NOHISTORY		= 0;

char	*CONFIG_SSH_KEY_LOCATION	= NULL;

int	CONFIG_LOG_SLOW_QUERIES		= 0;	/* ms; 0 - disable */

/* Global variable to control if we should write warnings to log[] */
int	CONFIG_ENABLE_LOG		= 1;

/* From table config */
int	CONFIG_REFRESH_UNSUPPORTED	= 0;

/* Zabbix server startup time */
int	CONFIG_SERVER_STARTUP_TIME	= 0;

/* Parameters for passive proxies */
int	CONFIG_PROXYPOLLER_FORKS	= 1;
/* How often Zabbix Server sends configuration data to Proxy in seconds */
int	CONFIG_PROXYCONFIG_FREQUENCY	= 3600; /* 1h */
int	CONFIG_PROXYDATA_FREQUENCY	= 1; /* 1s */

/* Mutex for node syncs */
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
	static struct cfg_line	cfg[] =
	{
		/* PARAMETER,			VAR,					FUNC,
			TYPE,		MANDATORY,	MIN,			MAX */
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
		{"MaxHousekeeperDelete",	&CONFIG_MAX_HOUSEKEEPER_DELETE,		NULL,
			TYPE_INT,	PARM_OPT,	0,			1000000},
		{"SenderFrequency",		&CONFIG_SENDER_FREQUENCY,		NULL,
			TYPE_INT,	PARM_OPT,	5,			SEC_PER_HOUR},
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
		{"DisableHousekeeping",		&CONFIG_DISABLE_HOUSEKEEPING,		NULL,
			TYPE_INT,	PARM_OPT,	0,			1},
		{"DebugLevel",			&CONFIG_LOG_LEVEL,			NULL,
			TYPE_INT,	PARM_OPT,	0,			4},
		{"PidFile",			&CONFIG_PID_FILE,			NULL,
			TYPE_STRING,	PARM_OPT,	0,			0},
		{"LogFile",			&CONFIG_LOG_FILE,			NULL,
			TYPE_STRING,	PARM_OPT,	0,			0},
		{"LogFileSize",			&CONFIG_LOG_FILE_SIZE,			NULL,
			TYPE_INT,	PARM_OPT,	0,			1024},
		{"AlertScriptsPath",		&CONFIG_ALERT_SCRIPTS_PATH,		NULL,
			TYPE_STRING,	PARM_OPT,	0,			0},
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
		{"NodeID",			&CONFIG_NODEID,				NULL,
			TYPE_INT,	PARM_OPT,	0,			999},
		{"NodeNoEvents",		&CONFIG_NODE_NOEVENTS,			NULL,
			TYPE_INT,	PARM_OPT,	0,			1},
		{"NodeNoHistory",		&CONFIG_NODE_NOHISTORY,			NULL,
			TYPE_INT,	PARM_OPT,	0,			1},
		{"SSHKeyLocation",		&CONFIG_SSH_KEY_LOCATION,		NULL,
			TYPE_STRING,	PARM_OPT,	0,			0},
		{"LogSlowQueries",		&CONFIG_LOG_SLOW_QUERIES,		NULL,
			TYPE_INT,	PARM_OPT,	0,			3600000},
		{"StartProxyPollers",		&CONFIG_PROXYPOLLER_FORKS,		NULL,
			TYPE_INT,	PARM_OPT,	0,			250},
		{"ProxyConfigFrequency",	&CONFIG_PROXYCONFIG_FREQUENCY,		NULL,
			TYPE_INT,	PARM_OPT,	1,			SEC_PER_WEEK},
		{"ProxyDataFrequency",		&CONFIG_PROXYDATA_FREQUENCY,		NULL,
			TYPE_INT,	PARM_OPT,	1,			SEC_PER_HOUR},
		{NULL}
	};

	CONFIG_SERVER_STARTUP_TIME = time(NULL);

	parse_cfg_file(CONFIG_FILE, cfg);

	if (NULL == CONFIG_DBNAME)
	{
		zabbix_log(LOG_LEVEL_CRIT, "DBName not in config file");
		exit(1);
	}

	if (NULL == CONFIG_PID_FILE)
	{
		CONFIG_PID_FILE = zbx_strdup(CONFIG_PID_FILE, "/tmp/zabbix_server.pid");
	}

	if (NULL == CONFIG_ALERT_SCRIPTS_PATH)
	{
		CONFIG_ALERT_SCRIPTS_PATH = zbx_strdup(CONFIG_ALERT_SCRIPTS_PATH, "/home/zabbix/bin");
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

	if (0 == CONFIG_NODEID)
	{
		CONFIG_NODEWATCHER_FORKS = 0;
	}

#ifdef HAVE_SQLITE3
	CONFIG_MAX_HOUSEKEEPER_DELETE = 0;
#endif

	if (1 == CONFIG_DISABLE_HOUSEKEEPING)
		CONFIG_HOUSEKEEPER_FORKS = 0;
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
	zbx_task_t	task = ZBX_TASK_START;
	char		ch = '\0';
	int		nodeid = 0;

	progname = get_program_name(argv[0]);

	/* Parse the command-line. */
	while ((char)EOF != (ch = (char)zbx_getopt_long(argc, argv, shortopts, longopts, NULL)))
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
			case 'n':
				nodeid=0;
				if(zbx_optarg)	nodeid = atoi(zbx_optarg);
				task = ZBX_TASK_CHANGE_NODEID;
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
		CONFIG_FILE = zbx_strdup(CONFIG_FILE, "/etc/zabbix/zabbix_server.conf");

	/* Required for simple checks */
	init_metrics();

	init_config();

#ifdef HAVE_OPENIPMI
	init_ipmi_handler();
#endif

	switch (task)
	{
		case ZBX_TASK_CHANGE_NODEID:
			change_nodeid(0, nodeid);
			exit(-1);
			break;
		default:
			break;
	}

	return daemon_start(CONFIG_ALLOW_ROOT);
}

int	MAIN_ZABBIX_ENTRY()
{
	DB_RESULT	result;
	DB_ROW		row;
	pid_t		pid;
	zbx_sock_t	listen_sock;
	int		i, server_num = 0;

	if (NULL == CONFIG_LOG_FILE || '\0' == *CONFIG_LOG_FILE)
	{
		zabbix_open_log(LOG_TYPE_SYSLOG, CONFIG_LOG_LEVEL, NULL);
	}
	else
	{
		zabbix_open_log(LOG_TYPE_FILE, CONFIG_LOG_LEVEL, CONFIG_LOG_FILE);
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
#ifdef	HAVE_LIBCURL
#	define LIBCURL_FEATURE_STATUS "YES"
#else
#	define LIBCURL_FEATURE_STATUS " NO"
#endif
#ifdef	HAVE_JABBER
#	define JABBER_FEATURE_STATUS "YES"
#else
#	define JABBER_FEATURE_STATUS " NO"
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

	zabbix_log(LOG_LEVEL_INFORMATION, "Starting Zabbix Server. Zabbix %s (revision %s).",
			ZABBIX_VERSION, ZABBIX_REVISION);

	zabbix_log(LOG_LEVEL_INFORMATION, "****** Enabled features ******");
	zabbix_log(LOG_LEVEL_INFORMATION, "SNMP monitoring:           " SNMP_FEATURE_STATUS);
	zabbix_log(LOG_LEVEL_INFORMATION, "IPMI monitoring:           " IPMI_FEATURE_STATUS);
	zabbix_log(LOG_LEVEL_INFORMATION, "WEB monitoring:            " LIBCURL_FEATURE_STATUS);
	zabbix_log(LOG_LEVEL_INFORMATION, "Jabber notifications:      " JABBER_FEATURE_STATUS);
	zabbix_log(LOG_LEVEL_INFORMATION, "Ez Texting notifications:  " LIBCURL_FEATURE_STATUS);
	zabbix_log(LOG_LEVEL_INFORMATION, "ODBC:                      " ODBC_FEATURE_STATUS);
	zabbix_log(LOG_LEVEL_INFORMATION, "SSH2 support:              " SSH2_FEATURE_STATUS);
	zabbix_log(LOG_LEVEL_INFORMATION, "IPv6 support:              " IPV6_FEATURE_STATUS);
	zabbix_log(LOG_LEVEL_INFORMATION, "******************************");

	if (0 != CONFIG_NODEID)
	{
		zabbix_log(LOG_LEVEL_INFORMATION, "NodeID:                    %3d", CONFIG_NODEID);
		zabbix_log(LOG_LEVEL_INFORMATION, "******************************");
	}

#ifdef	HAVE_SQLITE3
	zbx_create_sqlite3_mutex(CONFIG_DBNAME);
#endif /* HAVE_SQLITE3 */

	DBconnect(ZBX_DB_CONNECT_EXIT);

	result = DBselect("select refresh_unsupported from config where 1=1" DB_NODE,
			DBnode_local("configid"));

	if (NULL != (row = DBfetch(result)))
		CONFIG_REFRESH_UNSUPPORTED = atoi(row[0]);
	DBfree_result(result);

	if (0 != CONFIG_NODEID)
	{
		result = DBselect("select masterid from nodes where nodeid=%d",
				CONFIG_NODEID);

		if (NULL != (row = DBfetch(result)) && SUCCEED != DBis_null(row[0]))
			CONFIG_MASTER_NODEID = atoi(row[0]);
		DBfree_result(result);
	}

	init_database_cache(ZBX_PROCESS_SERVER);
	init_configuration_cache();
	init_selfmon_collector();

	/* Need to set trigger status to UNKNOWN since last run */
	DBupdate_triggers_status_after_restart();
	DBclose();

	if (ZBX_MUTEX_ERROR == zbx_mutex_create_force(&node_sync_access, ZBX_MUTEX_NODE_SYNC))
	{
		zbx_error("Unable to create mutex for node syncs");
		exit(FAIL);
	}

	threads_num = 1 + CONFIG_CONFSYNCER_FORKS + CONFIG_POLLER_FORKS + CONFIG_UNREACHABLE_POLLER_FORKS
			+ CONFIG_TRAPPER_FORKS + CONFIG_PINGER_FORKS + CONFIG_ALERTER_FORKS
			+ CONFIG_HOUSEKEEPER_FORKS + CONFIG_TIMER_FORKS + CONFIG_NODEWATCHER_FORKS
			+ CONFIG_HTTPPOLLER_FORKS + CONFIG_DISCOVERER_FORKS + CONFIG_HISTSYNCER_FORKS
			+ CONFIG_ESCALATOR_FORKS + CONFIG_IPMIPOLLER_FORKS + CONFIG_PROXYPOLLER_FORKS
			+ CONFIG_SELFMON_FORKS;
	threads = calloc(threads_num, sizeof(pid_t));

	if (CONFIG_TRAPPER_FORKS > 0)
	{
		if (FAIL == zbx_tcp_listen(&listen_sock, CONFIG_LISTEN_IP, (unsigned short)CONFIG_LISTEN_PORT))
		{
			zabbix_log(LOG_LEVEL_CRIT, "Listener failed with error: %s.", zbx_tcp_strerror());
			exit(1);
		}
	}

	for (i = 1; i <= CONFIG_CONFSYNCER_FORKS + CONFIG_POLLER_FORKS + CONFIG_UNREACHABLE_POLLER_FORKS
			+ CONFIG_TRAPPER_FORKS + CONFIG_PINGER_FORKS + CONFIG_ALERTER_FORKS
			+ CONFIG_HOUSEKEEPER_FORKS + CONFIG_TIMER_FORKS + CONFIG_NODEWATCHER_FORKS
			+ CONFIG_HTTPPOLLER_FORKS + CONFIG_DISCOVERER_FORKS + CONFIG_HISTSYNCER_FORKS
			+ CONFIG_ESCALATOR_FORKS + CONFIG_IPMIPOLLER_FORKS + CONFIG_PROXYPOLLER_FORKS
			+ CONFIG_SELFMON_FORKS; i++)
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

		process_type = ZBX_PROCESS_TYPE_WATCHDOG;
		process_num = 1;

		zabbix_log(LOG_LEVEL_WARNING, "server #%d started [%s]",
				server_num, get_process_type_string(process_type));

		main_watchdog_loop();
	}
	else if (server_num <= CONFIG_CONFSYNCER_FORKS)
	{
		process_type = ZBX_PROCESS_TYPE_CONFSYNCER;
		process_num = server_num;

		zabbix_log(LOG_LEVEL_WARNING, "server #%d started [%s]",
				server_num, get_process_type_string(process_type));

		main_dbconfig_loop();
	}
	else if (server_num <= CONFIG_CONFSYNCER_FORKS + CONFIG_POLLER_FORKS)
	{
#ifdef HAVE_SNMP
		init_snmp("zabbix_server");
#endif	/* HAVE_SNMP */

		process_type = ZBX_PROCESS_TYPE_POLLER;
		process_num = server_num - CONFIG_CONFSYNCER_FORKS;

		zabbix_log(LOG_LEVEL_WARNING, "server #%d started [%s]",
				server_num, get_process_type_string(process_type));

		main_poller_loop(ZBX_PROCESS_SERVER, ZBX_POLLER_TYPE_NORMAL);
	}
	else if (server_num <= CONFIG_CONFSYNCER_FORKS + CONFIG_POLLER_FORKS
			+ CONFIG_UNREACHABLE_POLLER_FORKS)
	{
#ifdef HAVE_SNMP
		init_snmp("zabbix_server");
#endif	/* HAVE_SNMP */

		process_type = ZBX_PROCESS_TYPE_UNREACHABLE;
		process_num = server_num - CONFIG_CONFSYNCER_FORKS - CONFIG_POLLER_FORKS;

		zabbix_log(LOG_LEVEL_WARNING, "server #%d started [%s]",
				server_num, get_process_type_string(process_type));

		main_poller_loop(ZBX_PROCESS_SERVER, ZBX_POLLER_TYPE_UNREACHABLE);
	}
	else if (server_num <= CONFIG_CONFSYNCER_FORKS + CONFIG_POLLER_FORKS
			+ CONFIG_UNREACHABLE_POLLER_FORKS + CONFIG_TRAPPER_FORKS)
	{
		process_type = ZBX_PROCESS_TYPE_TRAPPER;
		process_num = server_num - CONFIG_CONFSYNCER_FORKS - CONFIG_POLLER_FORKS
				- CONFIG_UNREACHABLE_POLLER_FORKS;

		zabbix_log(LOG_LEVEL_WARNING, "server #%d started [%s]",
				server_num, get_process_type_string(process_type));

		main_trapper_loop(ZBX_PROCESS_SERVER, &listen_sock);
	}
	else if (server_num <= CONFIG_CONFSYNCER_FORKS + CONFIG_POLLER_FORKS
			+ CONFIG_UNREACHABLE_POLLER_FORKS + CONFIG_TRAPPER_FORKS
			+ CONFIG_PINGER_FORKS)
	{
		process_type = ZBX_PROCESS_TYPE_PINGER;
		process_num = server_num - CONFIG_CONFSYNCER_FORKS - CONFIG_POLLER_FORKS
				- CONFIG_UNREACHABLE_POLLER_FORKS - CONFIG_TRAPPER_FORKS;

		zabbix_log(LOG_LEVEL_WARNING, "server #%d started [%s]",
				server_num, get_process_type_string(process_type));

		main_pinger_loop();
	}
	else if (server_num <= CONFIG_CONFSYNCER_FORKS + CONFIG_POLLER_FORKS
			+ CONFIG_UNREACHABLE_POLLER_FORKS + CONFIG_TRAPPER_FORKS
			+ CONFIG_PINGER_FORKS + CONFIG_ALERTER_FORKS)
	{
		process_type = ZBX_PROCESS_TYPE_ALERTER;
		process_num = server_num - CONFIG_CONFSYNCER_FORKS - CONFIG_POLLER_FORKS
				- CONFIG_UNREACHABLE_POLLER_FORKS - CONFIG_TRAPPER_FORKS
				- CONFIG_PINGER_FORKS;

		zabbix_log(LOG_LEVEL_WARNING, "server #%d started [%s]",
				server_num, get_process_type_string(process_type));

		main_alerter_loop();
	}
	else if (server_num <= CONFIG_CONFSYNCER_FORKS + CONFIG_POLLER_FORKS
			+ CONFIG_UNREACHABLE_POLLER_FORKS + CONFIG_TRAPPER_FORKS
			+ CONFIG_PINGER_FORKS + CONFIG_ALERTER_FORKS
			+ CONFIG_HOUSEKEEPER_FORKS)
	{
		process_type = ZBX_PROCESS_TYPE_HOUSEKEEPER;
		process_num = server_num - CONFIG_CONFSYNCER_FORKS - CONFIG_POLLER_FORKS
				- CONFIG_UNREACHABLE_POLLER_FORKS - CONFIG_TRAPPER_FORKS
				- CONFIG_PINGER_FORKS - CONFIG_ALERTER_FORKS;

		zabbix_log(LOG_LEVEL_WARNING, "server #%d started [%s]",
				server_num, get_process_type_string(process_type));

		main_housekeeper_loop();
	}
	else if (server_num <= CONFIG_CONFSYNCER_FORKS + CONFIG_POLLER_FORKS
			+ CONFIG_UNREACHABLE_POLLER_FORKS + CONFIG_TRAPPER_FORKS
			+ CONFIG_PINGER_FORKS + CONFIG_ALERTER_FORKS
			+ CONFIG_HOUSEKEEPER_FORKS + CONFIG_TIMER_FORKS)
	{
		process_type = ZBX_PROCESS_TYPE_TIMER;
		process_num = server_num - CONFIG_CONFSYNCER_FORKS - CONFIG_POLLER_FORKS
				- CONFIG_UNREACHABLE_POLLER_FORKS - CONFIG_TRAPPER_FORKS
				- CONFIG_PINGER_FORKS - CONFIG_ALERTER_FORKS
				- CONFIG_HOUSEKEEPER_FORKS;

		zabbix_log(LOG_LEVEL_WARNING, "server #%d started [%s]",
				server_num, get_process_type_string(process_type));

		main_timer_loop();
	}
	else if (server_num <= CONFIG_CONFSYNCER_FORKS + CONFIG_POLLER_FORKS
			+ CONFIG_UNREACHABLE_POLLER_FORKS + CONFIG_TRAPPER_FORKS
			+ CONFIG_PINGER_FORKS + CONFIG_ALERTER_FORKS
			+ CONFIG_HOUSEKEEPER_FORKS + CONFIG_TIMER_FORKS
			+ CONFIG_NODEWATCHER_FORKS)
	{
		process_type = ZBX_PROCESS_TYPE_NODEWATCHER;
		process_num = server_num - CONFIG_CONFSYNCER_FORKS - CONFIG_POLLER_FORKS
				- CONFIG_UNREACHABLE_POLLER_FORKS - CONFIG_TRAPPER_FORKS
				- CONFIG_PINGER_FORKS - CONFIG_ALERTER_FORKS
				- CONFIG_HOUSEKEEPER_FORKS - CONFIG_TIMER_FORKS;

		zabbix_log(LOG_LEVEL_WARNING, "server #%d started [%s]",
				server_num, get_process_type_string(process_type));

		main_nodewatcher_loop();
	}
	else if (server_num <= CONFIG_CONFSYNCER_FORKS + CONFIG_POLLER_FORKS
			+ CONFIG_UNREACHABLE_POLLER_FORKS + CONFIG_TRAPPER_FORKS
			+ CONFIG_PINGER_FORKS + CONFIG_ALERTER_FORKS
			+ CONFIG_HOUSEKEEPER_FORKS + CONFIG_TIMER_FORKS
			+ CONFIG_NODEWATCHER_FORKS + CONFIG_HTTPPOLLER_FORKS)
	{
		process_type = ZBX_PROCESS_TYPE_HTTPPOLLER;
		process_num = server_num - CONFIG_CONFSYNCER_FORKS - CONFIG_POLLER_FORKS
				- CONFIG_UNREACHABLE_POLLER_FORKS - CONFIG_TRAPPER_FORKS
				- CONFIG_PINGER_FORKS - CONFIG_ALERTER_FORKS
				- CONFIG_HOUSEKEEPER_FORKS - CONFIG_TIMER_FORKS
				- CONFIG_NODEWATCHER_FORKS;

		zabbix_log(LOG_LEVEL_WARNING, "server #%d started [%s]",
				server_num, get_process_type_string(process_type));

		main_httppoller_loop();
	}
	else if (server_num <= CONFIG_CONFSYNCER_FORKS + CONFIG_POLLER_FORKS
			+ CONFIG_UNREACHABLE_POLLER_FORKS + CONFIG_TRAPPER_FORKS
			+ CONFIG_PINGER_FORKS + CONFIG_ALERTER_FORKS
			+ CONFIG_HOUSEKEEPER_FORKS + CONFIG_TIMER_FORKS
			+ CONFIG_NODEWATCHER_FORKS + CONFIG_HTTPPOLLER_FORKS
			+ CONFIG_DISCOVERER_FORKS)
	{
#ifdef HAVE_SNMP
		init_snmp("zabbix_server");
#endif	/* HAVE_SNMP */

		process_type = ZBX_PROCESS_TYPE_DISCOVERER;
		process_num = server_num - CONFIG_CONFSYNCER_FORKS - CONFIG_POLLER_FORKS
				- CONFIG_UNREACHABLE_POLLER_FORKS - CONFIG_TRAPPER_FORKS
				- CONFIG_PINGER_FORKS - CONFIG_ALERTER_FORKS
				- CONFIG_HOUSEKEEPER_FORKS - CONFIG_TIMER_FORKS
				- CONFIG_NODEWATCHER_FORKS - CONFIG_HTTPPOLLER_FORKS;

		zabbix_log(LOG_LEVEL_WARNING, "server #%d started [%s]",
				server_num, get_process_type_string(process_type));

		main_discoverer_loop(ZBX_PROCESS_SERVER);
	}
	else if (server_num <= CONFIG_CONFSYNCER_FORKS + CONFIG_POLLER_FORKS
			+ CONFIG_UNREACHABLE_POLLER_FORKS + CONFIG_TRAPPER_FORKS
			+ CONFIG_PINGER_FORKS + CONFIG_ALERTER_FORKS
			+ CONFIG_HOUSEKEEPER_FORKS + CONFIG_TIMER_FORKS
			+ CONFIG_NODEWATCHER_FORKS + CONFIG_HTTPPOLLER_FORKS
			+ CONFIG_DISCOVERER_FORKS + CONFIG_HISTSYNCER_FORKS)
	{
		process_type = ZBX_PROCESS_TYPE_HISTSYNCER;
		process_num = server_num - CONFIG_CONFSYNCER_FORKS - CONFIG_POLLER_FORKS
				- CONFIG_UNREACHABLE_POLLER_FORKS - CONFIG_TRAPPER_FORKS
				- CONFIG_PINGER_FORKS - CONFIG_ALERTER_FORKS
				- CONFIG_HOUSEKEEPER_FORKS - CONFIG_TIMER_FORKS
				- CONFIG_NODEWATCHER_FORKS - CONFIG_HTTPPOLLER_FORKS
				- CONFIG_DISCOVERER_FORKS;

		zabbix_log(LOG_LEVEL_WARNING, "server #%d started [%s]",
				server_num, get_process_type_string(process_type));

		main_dbsyncer_loop();
	}
	else if (server_num <= CONFIG_CONFSYNCER_FORKS + CONFIG_POLLER_FORKS
			+ CONFIG_UNREACHABLE_POLLER_FORKS + CONFIG_TRAPPER_FORKS
			+ CONFIG_PINGER_FORKS + CONFIG_ALERTER_FORKS
			+ CONFIG_HOUSEKEEPER_FORKS + CONFIG_TIMER_FORKS
			+ CONFIG_NODEWATCHER_FORKS + CONFIG_HTTPPOLLER_FORKS
			+ CONFIG_DISCOVERER_FORKS + CONFIG_HISTSYNCER_FORKS
			+ CONFIG_ESCALATOR_FORKS)
	{
		process_type = ZBX_PROCESS_TYPE_ESCALATOR;
		process_num = server_num - CONFIG_CONFSYNCER_FORKS - CONFIG_POLLER_FORKS
				- CONFIG_UNREACHABLE_POLLER_FORKS - CONFIG_TRAPPER_FORKS
				- CONFIG_PINGER_FORKS - CONFIG_ALERTER_FORKS
				- CONFIG_HOUSEKEEPER_FORKS - CONFIG_TIMER_FORKS
				- CONFIG_NODEWATCHER_FORKS - CONFIG_HTTPPOLLER_FORKS
				- CONFIG_DISCOVERER_FORKS - CONFIG_HISTSYNCER_FORKS;

		zabbix_log(LOG_LEVEL_WARNING, "server #%d started [%s]",
				server_num, get_process_type_string(process_type));

		main_escalator_loop();
	}
	else if (server_num <= CONFIG_CONFSYNCER_FORKS + CONFIG_POLLER_FORKS
			+ CONFIG_UNREACHABLE_POLLER_FORKS + CONFIG_TRAPPER_FORKS
			+ CONFIG_PINGER_FORKS + CONFIG_ALERTER_FORKS
			+ CONFIG_HOUSEKEEPER_FORKS + CONFIG_TIMER_FORKS
			+ CONFIG_NODEWATCHER_FORKS + CONFIG_HTTPPOLLER_FORKS
			+ CONFIG_DISCOVERER_FORKS + CONFIG_HISTSYNCER_FORKS
			+ CONFIG_ESCALATOR_FORKS + CONFIG_IPMIPOLLER_FORKS)
	{
		process_type = ZBX_PROCESS_TYPE_IPMIPOLLER;
		process_num = server_num - CONFIG_CONFSYNCER_FORKS - CONFIG_POLLER_FORKS
				- CONFIG_UNREACHABLE_POLLER_FORKS - CONFIG_TRAPPER_FORKS
				- CONFIG_PINGER_FORKS - CONFIG_ALERTER_FORKS
				- CONFIG_HOUSEKEEPER_FORKS - CONFIG_TIMER_FORKS
				- CONFIG_NODEWATCHER_FORKS - CONFIG_HTTPPOLLER_FORKS
				- CONFIG_DISCOVERER_FORKS - CONFIG_HISTSYNCER_FORKS
				- CONFIG_ESCALATOR_FORKS;

		zabbix_log(LOG_LEVEL_WARNING, "server #%d started [%s]",
				server_num, get_process_type_string(process_type));

		main_poller_loop(ZBX_PROCESS_SERVER, ZBX_POLLER_TYPE_IPMI);
	}
	else if (server_num <= CONFIG_CONFSYNCER_FORKS + CONFIG_POLLER_FORKS
			+ CONFIG_UNREACHABLE_POLLER_FORKS + CONFIG_TRAPPER_FORKS
			+ CONFIG_PINGER_FORKS + CONFIG_ALERTER_FORKS
			+ CONFIG_HOUSEKEEPER_FORKS + CONFIG_TIMER_FORKS
			+ CONFIG_NODEWATCHER_FORKS + CONFIG_HTTPPOLLER_FORKS
			+ CONFIG_DISCOVERER_FORKS + CONFIG_HISTSYNCER_FORKS
			+ CONFIG_ESCALATOR_FORKS + CONFIG_IPMIPOLLER_FORKS
			+ CONFIG_PROXYPOLLER_FORKS)
	{
		process_type = ZBX_PROCESS_TYPE_PROXYPOLLER;
		process_num = server_num - CONFIG_CONFSYNCER_FORKS - CONFIG_POLLER_FORKS
				- CONFIG_UNREACHABLE_POLLER_FORKS - CONFIG_TRAPPER_FORKS
				- CONFIG_PINGER_FORKS - CONFIG_ALERTER_FORKS
				- CONFIG_HOUSEKEEPER_FORKS - CONFIG_TIMER_FORKS
				- CONFIG_NODEWATCHER_FORKS - CONFIG_HTTPPOLLER_FORKS
				- CONFIG_DISCOVERER_FORKS - CONFIG_HISTSYNCER_FORKS
				- CONFIG_ESCALATOR_FORKS - CONFIG_IPMIPOLLER_FORKS;

		zabbix_log(LOG_LEVEL_WARNING, "server #%d started [%s]",
				server_num, get_process_type_string(process_type));

		main_proxypoller_loop();
	}
	else if (server_num <= CONFIG_CONFSYNCER_FORKS + CONFIG_POLLER_FORKS
			+ CONFIG_UNREACHABLE_POLLER_FORKS + CONFIG_TRAPPER_FORKS
			+ CONFIG_PINGER_FORKS + CONFIG_ALERTER_FORKS
			+ CONFIG_HOUSEKEEPER_FORKS + CONFIG_TIMER_FORKS
			+ CONFIG_NODEWATCHER_FORKS + CONFIG_HTTPPOLLER_FORKS
			+ CONFIG_DISCOVERER_FORKS + CONFIG_HISTSYNCER_FORKS
			+ CONFIG_ESCALATOR_FORKS + CONFIG_IPMIPOLLER_FORKS
			+ CONFIG_PROXYPOLLER_FORKS + CONFIG_SELFMON_FORKS)
	{
		process_type = ZBX_PROCESS_TYPE_SELFMON;
		process_num = server_num - CONFIG_CONFSYNCER_FORKS - CONFIG_POLLER_FORKS
				- CONFIG_UNREACHABLE_POLLER_FORKS - CONFIG_TRAPPER_FORKS
				- CONFIG_PINGER_FORKS - CONFIG_ALERTER_FORKS
				- CONFIG_HOUSEKEEPER_FORKS - CONFIG_TIMER_FORKS
				- CONFIG_NODEWATCHER_FORKS - CONFIG_HTTPPOLLER_FORKS
				- CONFIG_DISCOVERER_FORKS - CONFIG_HISTSYNCER_FORKS
				- CONFIG_ESCALATOR_FORKS - CONFIG_IPMIPOLLER_FORKS
				- CONFIG_PROXYPOLLER_FORKS;

		zabbix_log(LOG_LEVEL_WARNING, "server #%d started [%s]",
				server_num, get_process_type_string(process_type));

		main_selfmon_loop();
	}

	return SUCCEED;
}

void	zbx_on_exit()
{
	zabbix_log(LOG_LEVEL_DEBUG, "zbx_on_exit() called");

	if (threads != NULL)
	{
		int	i;

		for (i = 1; i <= CONFIG_CONFSYNCER_FORKS + CONFIG_POLLER_FORKS
				+ CONFIG_UNREACHABLE_POLLER_FORKS + CONFIG_TRAPPER_FORKS
				+ CONFIG_PINGER_FORKS + CONFIG_ALERTER_FORKS
				+ CONFIG_HOUSEKEEPER_FORKS + CONFIG_TIMER_FORKS
				+ CONFIG_NODEWATCHER_FORKS + CONFIG_HTTPPOLLER_FORKS
				+ CONFIG_DISCOVERER_FORKS + CONFIG_HISTSYNCER_FORKS
				+ CONFIG_ESCALATOR_FORKS + CONFIG_IPMIPOLLER_FORKS
				+ CONFIG_PROXYPOLLER_FORKS + CONFIG_SELFMON_FORKS; i++)
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

	zbx_mutex_destroy(&node_sync_access);

#ifdef HAVE_OPENIPMI
	free_ipmi_handler();
#endif

#ifdef HAVE_SQLITE3
	php_sem_remove(&sqlite_access);
#endif	/* HAVE_SQLITE3 */

	free_selfmon_collector();

	zabbix_log(LOG_LEVEL_INFORMATION, "Zabbix Server stopped. Zabbix %s (revision %s).",
			ZABBIX_VERSION, ZABBIX_REVISION);

	zabbix_close_log();

	exit(SUCCEED);
}
