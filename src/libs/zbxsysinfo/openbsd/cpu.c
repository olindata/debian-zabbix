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
#include "sysinfo.h"
#include "stats.h"

int	SYSTEM_CPU_NUM(const char *cmd, const char *param, unsigned flags, AGENT_RESULT *result)
{
#ifdef HAVE_FUNCTION_SYSCTL_HW_NCPU
	/* OpenBSD 4.2,4.3 i386 */
	size_t	len;
	int	mib[] = {CTL_HW, HW_NCPU}, ncpu;
	char	mode[MAX_STRING_LEN];

	if (num_param(param) > 1)
		return SYSINFO_RET_FAIL;

	if (0 != get_param(param, 1, mode, sizeof(mode)))
		*mode = '\0';

	if (*mode == '\0')
		zbx_snprintf(mode, sizeof(mode), "online");

	if (0 != strcmp(mode, "online"))
		return SYSINFO_RET_FAIL;

	len = sizeof(ncpu);

	if (0 != sysctl(mib, 2, &ncpu, &len, NULL, 0))
		return SYSINFO_RET_FAIL;

	SET_UI64_RESULT(result, ncpu);

	return SYSINFO_RET_OK;
#else
	return SYSINFO_RET_FAIL;
#endif /* HAVE_FUNCTION_SYSCTL_HW_NCPU */
}

int     SYSTEM_CPU_INTR(const char *cmd, const char *param, unsigned flags, AGENT_RESULT *result)
{
#if defined(HAVE_UVM_UVMEXP)
	/* OpenBSD 4.2 i386 */
	int		mib[] = {CTL_VM, VM_UVMEXP};
	size_t		len;
	struct uvmexp	v;

	len = sizeof(struct uvmexp);

	if (0 != sysctl(mib, 2, &v, &len, NULL, 0))
		return SYSINFO_RET_FAIL;

	SET_UI64_RESULT(result, v.intrs);

	return	SYSINFO_RET_OK;
#else
	return	SYSINFO_RET_FAIL;
#endif /* HAVE_UVM_UVMEXP2 */
}

int     SYSTEM_CPU_SWITCHES(const char *cmd, const char *param, unsigned flags, AGENT_RESULT *result)
{
#if defined(HAVE_UVM_UVMEXP)
	/* OpenBSD 4.2 i386 */
	int		mib[] = {CTL_VM, VM_UVMEXP};
	size_t		len;
	struct uvmexp	v;

	len = sizeof(struct uvmexp);

	if (0 != sysctl(mib, 2, &v, &len, NULL, 0))
		return SYSINFO_RET_FAIL;

	SET_UI64_RESULT(result, v.swtch);

	return	SYSINFO_RET_OK;
#else
	return	SYSINFO_RET_FAIL;
#endif /* HAVE_UVM_UVMEXP2 */
}

int	SYSTEM_CPU_UTIL(const char *cmd, const char *param, unsigned flags, AGENT_RESULT *result)
{
	char	tmp[32], type[32];
	int	cpu_num, mode;

	if (!CPU_COLLECTOR_STARTED(collector))
	{
		SET_MSG_RESULT(result, strdup("Collector is not started!"));
		return SYSINFO_RET_OK;
	}

	if (num_param(param) > 3)
		return SYSINFO_RET_FAIL;

	if (0 != get_param(param, 1, tmp, sizeof(tmp)))
		*tmp = '\0';

	if ('\0' == *tmp || 0 == strcmp(tmp, "all"))	/* default parameter */
		cpu_num = 0;
	else
	{
		cpu_num = atoi(tmp) + 1;
		if (cpu_num < 1 || cpu_num > collector->cpus.count)
			return SYSINFO_RET_FAIL;
	}

	if (0 != get_param(param, 2, type, sizeof(type)))
		*type = '\0';

	if (0 != get_param(param, 3, tmp, sizeof(tmp)))
		*tmp = '\0';

	if ('\0' == *tmp || 0 == strcmp(tmp, "avg1"))	/* default parameter */
		mode = ZBX_AVG1;
	else if (0 == strcmp(tmp, "avg5"))
		mode = ZBX_AVG5;
	else if (0 == strcmp(tmp, "avg15"))
		mode = ZBX_AVG15;
	else
		return SYSINFO_RET_FAIL;

	if ('\0' == *type || 0 == strcmp(type, "user"))	/* default parameter */
		SET_DBL_RESULT(result, collector->cpus.cpu[cpu_num].user[mode]);
	else if (0 == strcmp(type, "nice"))
		SET_DBL_RESULT(result, collector->cpus.cpu[cpu_num].nice[mode]);
	else if (0 == strcmp(type, "system"))
		SET_DBL_RESULT(result, collector->cpus.cpu[cpu_num].system[mode]);
	else if (0 == strcmp(type, "idle"))
		SET_DBL_RESULT(result, collector->cpus.cpu[cpu_num].idle[mode]);
	else if (0 == strcmp(type, "interrupt"))
		SET_DBL_RESULT(result, collector->cpus.cpu[cpu_num].interrupt[mode]);
	else
		return SYSINFO_RET_FAIL;

	return SYSINFO_RET_OK;
}

int	SYSTEM_CPU_LOAD(const char *cmd, const char *param, unsigned flags, AGENT_RESULT *result)
{
	/* OpenBSD 3.9 i386; OpenBSD 4.3 i386 */
	char	tmp[32];
	int	mode;
	double	load[ZBX_AVGMAX];

	if (num_param(param) > 2)
		return SYSINFO_RET_FAIL;

	if (0 != get_param(param, 1, tmp, sizeof(tmp)))
		*tmp = '\0';

	if ('\0' != *tmp && 0 != strcmp(tmp, "all"))	/* default parameter */
		return SYSINFO_RET_FAIL;

	if (0 != get_param(param, 2, tmp, sizeof(tmp)))
		*tmp = '\0';

	if ('\0' == *tmp || 0 == strcmp(tmp, "avg1"))	/* default parameter */
		mode = ZBX_AVG1;
	else if (0 == strcmp(tmp, "avg5"))
		mode = ZBX_AVG5;
	else if (0 == strcmp(tmp, "avg15"))
		mode = ZBX_AVG15;
	else
		return SYSINFO_RET_FAIL;

#ifdef HAVE_GETLOADAVG
	if (mode >= getloadavg(load, 3))
		return SYSINFO_RET_FAIL;
#else
	return SYSINFO_RET_FAIL;
#endif	/* HAVE_GETLOADAVG */

	SET_DBL_RESULT(result, load[mode]);

	return SYSINFO_RET_OK;
}
