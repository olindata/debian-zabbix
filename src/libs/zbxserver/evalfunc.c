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
#include "db.h"
#include "log.h"
#include "zlog.h"
#include "zbxserver.h"

#include "evalfunc.h"

static int	get_function_parameter_uint(DB_ITEM *item, const char *parameters, int Nparam, int *value, int *flag)
{
	const char	*__function_name = "get_function_parameter_uint";
	char		*parameter = NULL;
	int		res = FAIL;

	zabbix_log(LOG_LEVEL_DEBUG, "In %s() parameters:'%s' Nparam:%d", __function_name, parameters, Nparam);

	parameter = zbx_malloc(parameter, FUNCTION_PARAMETER_LEN_MAX);

	if (0 != get_param(parameters, Nparam, parameter, FUNCTION_PARAMETER_LEN_MAX))
		goto clean;

	if (SUCCEED == substitute_simple_macros(NULL, item, NULL, NULL, NULL, &parameter, MACRO_TYPE_FUNCTION_PARAMETER, NULL, 0))
	{
		if ('#' == *parameter)
		{
			*flag = ZBX_FLAG_VALUES;
			if (SUCCEED == is_uint(parameter + 1))
			{
				sscanf(parameter + 1, "%u", value);
				res = SUCCEED;
			}
		}
		else if (SUCCEED == is_uint_prefix(parameter))
		{
			*flag = ZBX_FLAG_SEC;
			*value = str2uint(parameter);
			res = SUCCEED;
		}
	}

	if (SUCCEED == res)
		zabbix_log(LOG_LEVEL_DEBUG, "%s() flag:%d value:%d", __function_name, *flag, *value);
clean:
	zbx_free(parameter);

	zabbix_log(LOG_LEVEL_DEBUG, "End of %s():%s", __function_name, zbx_result_string(res));

	return res;
}

static int	get_function_parameter_str(DB_ITEM *item, const char *parameters, int Nparam, char **value)
{
	const char	*__function_name = "get_function_parameter_str";
	int		res = FAIL;

	zabbix_log(LOG_LEVEL_DEBUG, "In %s() parameters:'%s' Nparam:%d", __function_name, parameters, Nparam);

	*value = zbx_malloc(*value, FUNCTION_PARAMETER_LEN_MAX);

	if (0 != get_param(parameters, Nparam, *value, FUNCTION_PARAMETER_LEN_MAX))
		goto clean;

	res = substitute_simple_macros(NULL, item, NULL, NULL, NULL, value, MACRO_TYPE_FUNCTION_PARAMETER, NULL, 0);
clean:
	if (SUCCEED == res)
		zabbix_log(LOG_LEVEL_DEBUG, "%s() value:'%s'", __function_name, *value);
	else
		zbx_free(*value);

	zabbix_log(LOG_LEVEL_DEBUG, "End of %s():%s", __function_name, zbx_result_string(res));

	return res;
}

/******************************************************************************
 *                                                                            *
 * Function: evaluate_LOGEVENTID                                              *
 *                                                                            *
 * Purpose: evaluate function 'logeventid' for the item                       *
 *                                                                            *
 * Parameters: item - item (performance metric)                               *
 *             parameter - regex string for event id matching                 *
 *                                                                            *
 * Return value: SUCCEED - evaluated successfully, result is stored in 'value'*
 *               FAIL - failed to evaluate function                           *
 *                                                                            *
 * Author: Alexei Vladishev, Rudolfs Kreicbergs                               *
 *                                                                            *
 * Comments:                                                                  *
 *                                                                            *
 ******************************************************************************/
static int	evaluate_LOGEVENTID(char *value, DB_ITEM *item, const char *function, const char *parameters, time_t now)
{
	const char	*__function_name = "evaluate_LOGEVENTID";
	DB_RESULT	result;
	DB_ROW		row;
	char		sql[128], *arg1 = NULL, *arg1_esc;
	int		res = FAIL;
	ZBX_REGEXP	*regexps = NULL;
	int		regexps_alloc = 0, regexps_num = 0;

	zabbix_log(LOG_LEVEL_DEBUG, "In %s()", __function_name);

	if (ITEM_VALUE_TYPE_LOG != item->value_type)
		goto clean;

	if (1 < num_param(parameters))
		goto clean;

	if (FAIL == get_function_parameter_str(item, parameters, 1, &arg1))
		goto clean;

	if ('@' == *arg1)
	{
		arg1_esc = DBdyn_escape_string(arg1 + 1);
		result = DBselect("select r.name,e.expression,e.expression_type,e.exp_delimiter,e.case_sensitive"
				" from regexps r,expressions e"
				" where r.regexpid=e.regexpid"
					" and r.name='%s'",
				arg1_esc);
		zbx_free(arg1_esc);

		while (NULL != (row = DBfetch(result)))
			add_regexp_ex(&regexps, &regexps_alloc, &regexps_num,
					row[0], row[1], atoi(row[2]), row[3][0], atoi(row[4]));
		DBfree_result(result);
	}

	zbx_snprintf(sql, sizeof(sql),
			"select logeventid"
			" from history_log"
			" where itemid=" ZBX_FS_UI64
			" order by id desc",
			item->itemid);

	result = DBselectN(sql, 1);

	if (NULL == (row = DBfetch(result)))
		zabbix_log(LOG_LEVEL_DEBUG, "Result for LOGEVENTID is empty");
	else
	{
		if (SUCCEED == regexp_match_ex(regexps, regexps_num, row[0], arg1, ZBX_CASE_SENSITIVE))
			zbx_strlcpy(value, "1", MAX_BUFFER_LEN);
		else
			zbx_strlcpy(value, "0", MAX_BUFFER_LEN);
		res = SUCCEED;
	}
	DBfree_result(result);
	if ('@' == *arg1)
		zbx_free(regexps);
	zbx_free(arg1);
clean:
	zabbix_log(LOG_LEVEL_DEBUG, "End of %s():%s", __function_name, zbx_result_string(res));

	return res;
}

/******************************************************************************
 *                                                                            *
 * Function: evaluate_LOGSOURCE                                               *
 *                                                                            *
 * Purpose: evaluate function 'logsource' for the item                        *
 *                                                                            *
 * Parameters: item - item (performance metric)                               *
 *             parameter - ignored                                            *
 *                                                                            *
 * Return value: SUCCEED - evaluated successfully, result is stored in 'value'*
 *               FAIL - failed to evaluate function                           *
 *                                                                            *
 * Author: Alexei Vladishev                                                   *
 *                                                                            *
 * Comments:                                                                  *
 *                                                                            *
 ******************************************************************************/
static int	evaluate_LOGSOURCE(char *value, DB_ITEM *item, const char *function, const char *parameters, time_t now)
{
	const char	*__function_name = "evaluate_LOGSOURCE";
	DB_RESULT	result;
	DB_ROW		row;
	char		sql[128], *arg1 = NULL;
	int		res = FAIL;

	zabbix_log(LOG_LEVEL_DEBUG, "In %s()", __function_name);

	if (ITEM_VALUE_TYPE_LOG != item->value_type)
		goto clean;

	if (1 < num_param(parameters))
		goto clean;

	if (FAIL == get_function_parameter_str(item, parameters, 1, &arg1))
		goto clean;

	zbx_snprintf(sql, sizeof(sql),
			"select source"
			" from history_log"
			" where itemid=" ZBX_FS_UI64
			" order by id desc",
			item->itemid);

	result = DBselectN(sql, 1);

	if (NULL == (row = DBfetch(result)))
		zabbix_log(LOG_LEVEL_DEBUG, "Result for LOGSOURCE is empty");
	else
	{
		if (0 == strcmp(row[0], arg1))
			zbx_strlcpy(value, "1", MAX_BUFFER_LEN);
		else
			zbx_strlcpy(value, "0", MAX_BUFFER_LEN);
		res = SUCCEED;
	}
	DBfree_result(result);

	zbx_free(arg1);
clean:
	zabbix_log(LOG_LEVEL_DEBUG, "End of %s():%s", __function_name, zbx_result_string(res));

	return res;
}

/******************************************************************************
 *                                                                            *
 * Function: evaluate_LOGSEVERITY                                             *
 *                                                                            *
 * Purpose: evaluate function 'logseverity' for the item                      *
 *                                                                            *
 * Parameters: item - item (performance metric)                               *
 *             parameter - ignored                                            *
 *                                                                            *
 * Return value: SUCCEED - evaluated successfully, result is stored in 'value'*
 *               FAIL - failed to evaluate function                           *
 *                                                                            *
 * Author: Alexei Vladishev                                                   *
 *                                                                            *
 * Comments:                                                                  *
 *                                                                            *
 ******************************************************************************/
static int	evaluate_LOGSEVERITY(char *value, DB_ITEM *item, const char *function, const char *parameters, time_t now)
{
	const char	*__function_name = "evaluate_LOGSEVERITY";
	DB_RESULT	result;
	DB_ROW		row;
	char		sql[128];
	int		res = FAIL;

	zabbix_log(LOG_LEVEL_DEBUG, "In %s()", __function_name);

	if (ITEM_VALUE_TYPE_LOG != item->value_type)
		goto clean;

	zbx_snprintf(sql, sizeof(sql),
			"select severity"
			" from history_log"
			" where itemid=" ZBX_FS_UI64
			" order by id desc",
			item->itemid);

	result = DBselectN(sql, 1);

	if (NULL == (row = DBfetch(result)))
		zabbix_log(LOG_LEVEL_DEBUG, "Result for LOGSEVERITY is empty");
	else
	{
		zbx_strlcpy(value, row[0], MAX_BUFFER_LEN);
		res = SUCCEED;
	}
	DBfree_result(result);
clean:
	zabbix_log(LOG_LEVEL_DEBUG, "End of %s():%s", __function_name, zbx_result_string(res));

	return res;
}

/******************************************************************************
 *                                                                            *
 * Function: evaluate_COUNT                                                   *
 *                                                                            *
 * Purpose: evaluate function 'count' for the item                            *
 *                                                                            *
 * Parameters: item - item (performance metric)                               *
 *             parameters - up to four comma-separated fields:                *
 *                            (1) number of seconds/values                    *
 *                            (2) value to compare with (optional)            *
 *                            (3) comparison operator (optional)              *
 *                            (4) time shift (optional)                       *
 *                                                                            *
 * Return value: SUCCEED - evaluated successfully, result is stored in 'value'*
 *               FAIL - failed to evaluate function                           *
 *                                                                            *
 * Author: Alexei Vladishev, Aleksandrs Saveljevs                             *
 *                                                                            *
 * Comments:                                                                  *
 *                                                                            *
 ******************************************************************************/
static int	evaluate_COUNT(char *value, DB_ITEM *item, const char *function, const char *parameters, time_t now)
{
#define OP_EQ 0
#define OP_NE 1
#define OP_GT 2
#define OP_GE 3
#define OP_LT 4
#define OP_LE 5
#define OP_LIKE 6
#define OP_MAX 7

	const char	*__function_name = "evaluate_COUNT";
	int		arg1, flag, op, numeric_search, nparams, count = 0, h_num, res = FAIL;
	zbx_uint64_t	value_uint64 = 0, dbvalue_uint64;
	double		value_double = 0, dbvalue_double;
	char		*arg2 = NULL, *arg3 = NULL;
	char		**h_value;

	zabbix_log(LOG_LEVEL_DEBUG, "In %s()", __function_name);

	numeric_search = (ITEM_VALUE_TYPE_UINT64 == item->value_type || ITEM_VALUE_TYPE_FLOAT == item->value_type);
	op = (numeric_search ? OP_EQ : OP_LIKE);

	if (4 < (nparams = num_param(parameters)))
		goto clean;

	if (FAIL == get_function_parameter_uint(item, parameters, 1, &arg1, &flag))
		goto clean;

	if (2 <= nparams)
	{
		if (FAIL == get_function_parameter_str(item, parameters, 2, &arg2))
			goto clean;
		else
			switch (item->value_type)
			{
				case ITEM_VALUE_TYPE_UINT64:
					ZBX_STR2UINT64(value_uint64, arg2);
					break;
				case ITEM_VALUE_TYPE_FLOAT:
					value_double = atof(arg2);
					break;
				default:
					;	/* nothing */
			}
	}

	if (3 <= nparams)
	{
		if (FAIL == get_function_parameter_str(item, parameters, 3, &arg3))
		{
			zbx_free(arg2);
			goto clean;
		}
		else
		{
			int fail = 0;

			if ('\0' == *arg3 && numeric_search) op = OP_EQ;
			else if ('\0' == *arg3 && !numeric_search) op = OP_LIKE;
			else if (0 == strcmp(arg3, "eq")) op = OP_EQ;
			else if (0 == strcmp(arg3, "ne")) op = OP_NE;
			else if (0 == strcmp(arg3, "gt")) op = OP_GT;
			else if (0 == strcmp(arg3, "ge")) op = OP_GE;
			else if (0 == strcmp(arg3, "lt")) op = OP_LT;
			else if (0 == strcmp(arg3, "le")) op = OP_LE;
			else if (0 == strcmp(arg3, "like")) op = OP_LIKE;
			else
			{
				zabbix_log(LOG_LEVEL_DEBUG, "operator \"%s\" is not supported for function COUNT", arg3);
				fail = 1;
			}

			if (!fail && numeric_search && OP_LIKE == op)
			{
				zabbix_log(LOG_LEVEL_DEBUG, "operator \"like\" is not supported for counting numeric values");
				fail = 1;
			}

			if (!fail && !numeric_search && !(OP_LIKE == op || OP_EQ == op || OP_NE == op))
			{
				zabbix_log(LOG_LEVEL_DEBUG, "operator \"%s\" is not supported for counting textual values", arg3);
				fail = 1;
			}

			if (fail)
			{
				zbx_free(arg2);
				zbx_free(arg3);
				goto clean;
			}

			zbx_free(arg3);
		}
	}

	if (4 <= nparams)
	{
		int	time_shift, time_shift_flag;

		if (FAIL == get_function_parameter_uint(item, parameters, 4, &time_shift, &time_shift_flag) ||
			ZBX_FLAG_SEC != time_shift_flag)
		{
			zbx_free(arg2);
			goto clean;
		}

		now -= time_shift;
	}

	if (NULL != arg2 && '\0' == *arg2 && (numeric_search || OP_LIKE == op))
		zbx_free(arg2);

	if (ZBX_FLAG_SEC == flag && NULL == arg2)
	{
		h_value = DBget_history(item->itemid, item->value_type, ZBX_DB_GET_HIST_COUNT,
				now - arg1, now, NULL, 0);

		if (NULL == h_value[0])
			zbx_strlcpy(value, "0", MAX_BUFFER_LEN);
		else
			zbx_snprintf(value, MAX_BUFFER_LEN, "%s", h_value[0]);
		DBfree_history(h_value);
	}
	else
	{
		if (ZBX_FLAG_VALUES == flag)
			h_value = DBget_history(item->itemid, item->value_type, ZBX_DB_GET_HIST_VALUE,
					0, now, NULL, arg1);
		else
			h_value = DBget_history(item->itemid, item->value_type, ZBX_DB_GET_HIST_VALUE,
					now - arg1, now, NULL, 0);

		for (h_num = 0; NULL != h_value[h_num]; h_num++)
		{
			if (NULL == arg2)
				goto count_inc;

			switch (item->value_type)
			{
				case ITEM_VALUE_TYPE_UINT64:
					ZBX_STR2UINT64(dbvalue_uint64, h_value[h_num]);

					switch (op)
					{
						case OP_EQ:
							if (dbvalue_uint64 == value_uint64)
								goto count_inc;
							break;
						case OP_NE:
							if (dbvalue_uint64 != value_uint64)
								goto count_inc;
							break;
						case OP_GT:
							if (dbvalue_uint64 > value_uint64)
								goto count_inc;
							break;
						case OP_GE:
							if (dbvalue_uint64 >= value_uint64)
								goto count_inc;
							break;
						case OP_LT:
							if (dbvalue_uint64 < value_uint64)
								goto count_inc;
							break;
						case OP_LE:
							if (dbvalue_uint64 <= value_uint64)
								goto count_inc;
							break;
					}
					break;
				case ITEM_VALUE_TYPE_FLOAT:
					dbvalue_double = atof(h_value[h_num]);

					switch (op)
					{
						case OP_EQ:
							if (dbvalue_double > value_double - 0.00001 &&
									dbvalue_double < value_double + 0.00001)
							{
								goto count_inc;
							}
							break;
						case OP_NE:
							if (!(dbvalue_double > value_double - 0.00001 &&
									dbvalue_double < value_double + 0.00001))
							{
								goto count_inc;
							}
							break;
						case OP_GT:
							if (dbvalue_double > value_double)
								goto count_inc;
							break;
						case OP_GE:
							if (dbvalue_double >= value_double)
								goto count_inc;
							break;
						case OP_LT:
							if (dbvalue_double < value_double)
								goto count_inc;
							break;
						case OP_LE:
							if (dbvalue_double <= value_double)
								goto count_inc;
							break;
					}
					break;
				default:
					switch (op)
					{
						case OP_EQ:
							if (0 == strcmp(h_value[h_num], arg2))
								goto count_inc;
							break;
						case OP_NE:
							if (0 != strcmp(h_value[h_num], arg2))
								goto count_inc;
							break;
						case OP_LIKE:
							if (NULL != strstr(h_value[h_num], arg2))
								goto count_inc;
							break;
					}
					break;
			}

			continue;
count_inc:
			count++;
		}
		DBfree_history(h_value);

		zbx_snprintf(value, MAX_BUFFER_LEN, "%d", count);
	}
	zbx_free(arg2);

	res = SUCCEED;

	zabbix_log(LOG_LEVEL_DEBUG, "%s() value:%s", __function_name, value);
clean:
	zabbix_log(LOG_LEVEL_DEBUG, "End of %s():%s", __function_name, zbx_result_string(res));

	return res;
}

/******************************************************************************
 *                                                                            *
 * Function: evaluate_SUM                                                     *
 *                                                                            *
 * Purpose: evaluate function 'sum' for the item                              *
 *                                                                            *
 * Parameters: item - item (performance metric)                               *
 *             parameters - number of seconds/values and time shift (optional)*
 *                                                                            *
 * Return value: SUCCEED - evaluated successfully, result is stored in 'value'*
 *               FAIL - failed to evaluate function                           *
 *                                                                            *
 * Author: Alexei Vladishev                                                   *
 *                                                                            *
 * Comments:                                                                  *
 *                                                                            *
 ******************************************************************************/
static int	evaluate_SUM(char *value, DB_ITEM *item, const char *function, const char *parameters, time_t now)
{
	const char	*__function_name = "evaluate_SUM";
	int		nparams, arg1, flag, h_num, res = FAIL;
	double		sum = 0;
	zbx_uint64_t	l, sum_uint64 = 0;
	char		**h_value;

	zabbix_log(LOG_LEVEL_DEBUG, "In %s()", __function_name);

	if (ITEM_VALUE_TYPE_FLOAT != item->value_type && ITEM_VALUE_TYPE_UINT64 != item->value_type)
		goto clean;

	if (2 < (nparams = num_param(parameters)))
		goto clean;

	if (FAIL == get_function_parameter_uint(item, parameters, 1, &arg1, &flag))
		goto clean;

	if (2 == nparams)
	{
		int	time_shift, time_shift_flag;

		if (FAIL == get_function_parameter_uint(item, parameters, 2, &time_shift, &time_shift_flag))
			goto clean;
		if (ZBX_FLAG_SEC != time_shift_flag)
			goto clean;

		now -= time_shift;
	}

	if (ZBX_FLAG_SEC == flag)
	{
		h_value = DBget_history(item->itemid, item->value_type, ZBX_DB_GET_HIST_SUM, now - arg1, now, NULL, 0);

		if (NULL != h_value[0])
		{
			zbx_strlcpy(value, h_value[0], MAX_BUFFER_LEN);
			res = SUCCEED;
		}
		else
			zabbix_log(LOG_LEVEL_DEBUG, "result for SUM is empty");
		DBfree_history(h_value);
	}
	else if (ZBX_FLAG_VALUES == flag)
	{
		h_value = DBget_history(item->itemid, item->value_type, ZBX_DB_GET_HIST_VALUE, 0, now, NULL, arg1);

		if (ITEM_VALUE_TYPE_UINT64 == item->value_type)
		{
			for (h_num = 0; NULL != h_value[h_num]; h_num++)
			{
				ZBX_STR2UINT64(l, h_value[h_num]);
				sum_uint64 += l;
			}
		}
		else
		{
			for (h_num = 0; NULL != h_value[h_num]; h_num++)
				sum += atof(h_value[h_num]);
		}
		DBfree_history(h_value);

		if (0 != h_num)
		{
			if (ITEM_VALUE_TYPE_UINT64 == item->value_type)
				zbx_snprintf(value, MAX_BUFFER_LEN, ZBX_FS_UI64, sum_uint64);
			else
				zbx_snprintf(value, MAX_BUFFER_LEN, ZBX_FS_DBL, sum);
			res = SUCCEED;
		}
		else
			zabbix_log(LOG_LEVEL_DEBUG, "result for SUM is empty");
	}
clean:
	zabbix_log(LOG_LEVEL_DEBUG, "End of %s():%s", __function_name, zbx_result_string(res));

	return res;
}

/******************************************************************************
 *                                                                            *
 * Function: evaluate_AVG                                                     *
 *                                                                            *
 * Purpose: evaluate function 'avg' for the item                              *
 *                                                                            *
 * Parameters: item - item (performance metric)                               *
 *             parameters - number of seconds/values and time shift (optional)*
 *                                                                            *
 * Return value: SUCCEED - evaluated successfully, result is stored in 'value'*
 *               FAIL - failed to evaluate function                           *
 *                                                                            *
 * Author: Alexei Vladishev                                                   *
 *                                                                            *
 * Comments:                                                                  *
 *                                                                            *
 ******************************************************************************/
static int	evaluate_AVG(char *value, DB_ITEM *item, const char *function, const char *parameters, time_t now)
{
	const char	*__function_name = "evaluate_AVG";
	int		nparams, arg1, flag, h_num, res = FAIL;
	double		sum = 0;
	char		**h_value;

	zabbix_log(LOG_LEVEL_DEBUG, "In %s()", __function_name);

	if (ITEM_VALUE_TYPE_FLOAT != item->value_type && ITEM_VALUE_TYPE_UINT64 != item->value_type)
		goto clean;

	if (2 < (nparams = num_param(parameters)))
		goto clean;

	if (FAIL == get_function_parameter_uint(item, parameters, 1, &arg1, &flag))
		goto clean;

	if (2 == nparams)
	{
		int	time_shift, time_shift_flag;

		if (FAIL == get_function_parameter_uint(item, parameters, 2, &time_shift, &time_shift_flag))
			goto clean;
		if (ZBX_FLAG_SEC != time_shift_flag)
			goto clean;

		now -= time_shift;
	}

	if (ZBX_FLAG_SEC == flag)
	{
		h_value = DBget_history(item->itemid, item->value_type, ZBX_DB_GET_HIST_AVG, now - arg1, now, NULL, 0);

		if (NULL != h_value[0])
		{
			zbx_strlcpy(value, h_value[0], MAX_BUFFER_LEN);
			res = SUCCEED;
		}
		else
			zabbix_log(LOG_LEVEL_DEBUG, "result for AVG is empty");
		DBfree_history(h_value);
	}
	else if (ZBX_FLAG_VALUES == flag)
	{
		h_value = DBget_history(item->itemid, item->value_type, ZBX_DB_GET_HIST_VALUE, 0, now, NULL, arg1);

		for (h_num = 0; NULL != h_value[h_num]; h_num++)
			sum += atof(h_value[h_num]);
		DBfree_history(h_value);

		if (0 != h_num)
		{
			zbx_snprintf(value, MAX_BUFFER_LEN, ZBX_FS_DBL, sum / (double)h_num);
			res = SUCCEED;
		}
		else
			zabbix_log(LOG_LEVEL_DEBUG, "result for AVG is empty");
	}
clean:
	zabbix_log(LOG_LEVEL_DEBUG, "End of %s():%s", __function_name, zbx_result_string(res));

	return res;
}

/******************************************************************************
 *                                                                            *
 * Function: evaluate_LAST                                                    *
 *                                                                            *
 * Purpose: evaluate functions 'last' and 'prev' for the item                 *
 *                                                                            *
 * Parameters: value - buffer of size MAX_BUFFER_LEN                          *
 *             item - item (performance metric)                               *
 *             parameters - Nth last value and time shift (optional)          *
 *                                                                            *
 * Return value: SUCCEED - evaluated successfully, result is stored in 'value'*
 *               FAIL - failed to evaluate function                           *
 *                                                                            *
 * Author: Alexander Vladishev                                                *
 *                                                                            *
 * Comments:                                                                  *
 *                                                                            *
 ******************************************************************************/
static int	evaluate_LAST(char *value, DB_ITEM *item, const char *function, const char *parameters, time_t now)
{
	const char	*__function_name = "evaluate_LAST";
	int		arg1, flag, time_shift = 0, time_shift_flag, res = FAIL, written_len, h_num;
	char		**h_value;

	zabbix_log(LOG_LEVEL_DEBUG, "In %s()", __function_name);

	if (0 == strcmp(function, "last"))
	{
		if (FAIL == get_function_parameter_uint(item, parameters, 1, &arg1, &flag) || ZBX_FLAG_VALUES != flag)
		{
			arg1 = 1;
			flag = ZBX_FLAG_VALUES;
		}

		if (num_param(parameters) == 2)
		{
			if (SUCCEED == get_function_parameter_uint(item, parameters, 2, &time_shift, &time_shift_flag) &&
				ZBX_FLAG_SEC == time_shift_flag)
			{
				now -= time_shift;
				time_shift = 1;
			}
			else
				goto clean;
		}
	}
	else if (0 == strcmp(function, "prev"))
	{
		arg1 = 2;
		flag = ZBX_FLAG_VALUES;
	}
	else
		goto clean;

	if (0 == time_shift && 1 == arg1)
	{
		if (1 != item->lastvalue_null)
		{
			res = SUCCEED;

			switch (item->value_type) {
			case ITEM_VALUE_TYPE_FLOAT:
				zbx_snprintf(value, MAX_BUFFER_LEN, ZBX_FS_DBL, item->lastvalue_dbl);
				break;
			case ITEM_VALUE_TYPE_UINT64:
				zbx_snprintf(value, MAX_BUFFER_LEN, ZBX_FS_UI64, item->lastvalue_uint64);
				break;
			default:
				written_len = zbx_snprintf(value, MAX_BUFFER_LEN, "%s", item->lastvalue_str);
				if (ITEM_LASTVALUE_LEN == written_len && ITEM_VALUE_TYPE_STR != item->value_type)
					goto history;
				break;
			}
		}
	}
	else if (0 == time_shift && 2 == arg1)
	{
		if (1 != item->prevvalue_null)
		{
			res = SUCCEED;

			switch (item->value_type) {
			case ITEM_VALUE_TYPE_FLOAT:
				zbx_snprintf(value, MAX_BUFFER_LEN, ZBX_FS_DBL, item->prevvalue_dbl);
				break;
			case ITEM_VALUE_TYPE_UINT64:
				zbx_snprintf(value, MAX_BUFFER_LEN, ZBX_FS_UI64, item->prevvalue_uint64);
				break;
			default:
				written_len = zbx_snprintf(value, MAX_BUFFER_LEN, "%s", item->prevvalue_str);
				if (ITEM_LASTVALUE_LEN == written_len && ITEM_VALUE_TYPE_STR != item->value_type)
					goto history;
				break;
			}
		}
	}
	else
	{
history:
		h_value = DBget_history(item->itemid, item->value_type, ZBX_DB_GET_HIST_VALUE, 0, now, NULL, arg1);

		for (h_num = 0; NULL != h_value[h_num]; h_num++)
		{
			if (arg1 == h_num + 1)
			{
				zbx_snprintf(value, MAX_BUFFER_LEN, "%s", h_value[h_num]);
				res = SUCCEED;
				break;
			}
		}
		DBfree_history(h_value);
	}
clean:
	zabbix_log(LOG_LEVEL_DEBUG, "End of %s():%s", __function_name, zbx_result_string(res));

	return res;
}

/******************************************************************************
 *                                                                            *
 * Function: evaluate_MIN                                                     *
 *                                                                            *
 * Purpose: evaluate function 'min' for the item                              *
 *                                                                            *
 * Parameters: item - item (performance metric)                               *
 *             parameters - number of seconds/values and time shift (optional)*
 *                                                                            *
 * Return value: SUCCEED - evaluated successfully, result is stored in 'value'*
 *               FAIL - failed to evaluate function                           *
 *                                                                            *
 * Author: Alexei Vladishev                                                   *
 *                                                                            *
 * Comments:                                                                  *
 *                                                                            *
 ******************************************************************************/
static int	evaluate_MIN(char *value, DB_ITEM *item, const char *function, const char *parameters, time_t now)
{
	const char	*__function_name = "evaluate_MIN";
	int		nparams, arg1, flag, h_num, res = FAIL;
	zbx_uint64_t	min_uint64 = 0, l;
	double		min = 0, f;
	char		**h_value;

	zabbix_log(LOG_LEVEL_DEBUG, "In %s()", __function_name);

	if (ITEM_VALUE_TYPE_FLOAT != item->value_type && ITEM_VALUE_TYPE_UINT64 != item->value_type)
		goto clean;

	if (2 < (nparams = num_param(parameters)))
		goto clean;

	if (FAIL == get_function_parameter_uint(item, parameters, 1, &arg1, &flag))
		goto clean;

	if (2 == nparams)
	{
		int	time_shift, time_shift_flag;

		if (FAIL == get_function_parameter_uint(item, parameters, 2, &time_shift, &time_shift_flag))
			goto clean;
		if (ZBX_FLAG_SEC != time_shift_flag)
			goto clean;

		now -= time_shift;
	}

	if (ZBX_FLAG_SEC == flag)
	{
		h_value = DBget_history(item->itemid, item->value_type, ZBX_DB_GET_HIST_MIN, now - arg1, now, NULL, 0);

		if (NULL != h_value[0])
		{
			zbx_strlcpy(value, h_value[0], MAX_BUFFER_LEN);
			res = SUCCEED;
		}
		else
			zabbix_log(LOG_LEVEL_DEBUG, "result for MIN is empty");
		DBfree_history(h_value);
	}
	else if (ZBX_FLAG_VALUES == flag)
	{
		h_value = DBget_history(item->itemid, item->value_type, ZBX_DB_GET_HIST_VALUE, 0, now, NULL, arg1);

		if (ITEM_VALUE_TYPE_UINT64 == item->value_type)
		{
			for (h_num = 0; NULL != h_value[h_num]; h_num++)
			{
				ZBX_STR2UINT64(l, h_value[h_num]);
				if (0 == h_num || l < min_uint64)
					min_uint64 = l;
			}
		}
		else
		{
			for (h_num = 0; NULL != h_value[h_num]; h_num++)
			{
				f = atof(h_value[h_num]);
				if (0 == h_num || f < min)
					min = f;
			}
		}
		DBfree_history(h_value);

		if (0 != h_num)
		{
			if (item->value_type == ITEM_VALUE_TYPE_UINT64)
				zbx_snprintf(value, MAX_BUFFER_LEN, ZBX_FS_UI64, min_uint64);
			else
				zbx_snprintf(value, MAX_BUFFER_LEN, ZBX_FS_DBL, min);
			res = SUCCEED;
		}
		else
			zabbix_log(LOG_LEVEL_DEBUG, "result for MIN is empty");
	}
clean:
	zabbix_log(LOG_LEVEL_DEBUG, "End of %s():%s", __function_name, zbx_result_string(res));

	return res;
}

/******************************************************************************
 *                                                                            *
 * Function: evaluate_MAX                                                     *
 *                                                                            *
 * Purpose: evaluate function 'max' for the item                              *
 *                                                                            *
 * Parameters: item - item (performance metric)                               *
 *             parameters - number of seconds/values and time shift (optional)*
 *                                                                            *
 * Return value: SUCCEED - evaluated successfully, result is stored in 'value'*
 *               FAIL - failed to evaluate function                           *
 *                                                                            *
 * Author: Alexei Vladishev                                                   *
 *                                                                            *
 * Comments:                                                                  *
 *                                                                            *
 ******************************************************************************/
static int	evaluate_MAX(char *value, DB_ITEM *item, const char *function, const char *parameters, time_t now)
{
	const char	*__function_name = "evaluate_MAX";
	int		nparams, arg1, flag, h_num, res = FAIL;
	zbx_uint64_t	max_uint64 = 0, l;
	double		max = 0, f;
	char		**h_value;

	zabbix_log(LOG_LEVEL_DEBUG, "In %s()", __function_name);

	if (ITEM_VALUE_TYPE_FLOAT != item->value_type && ITEM_VALUE_TYPE_UINT64 != item->value_type)
		goto clean;

	if (2 < (nparams = num_param(parameters)))
		goto clean;

	if (FAIL == get_function_parameter_uint(item, parameters, 1, &arg1, &flag))
		goto clean;

	if (2 == nparams)
	{
		int	time_shift, time_shift_flag;

		if (FAIL == get_function_parameter_uint(item, parameters, 2, &time_shift, &time_shift_flag))
			goto clean;
		if (ZBX_FLAG_SEC != time_shift_flag)
			goto clean;

		now -= time_shift;
	}

	if (ZBX_FLAG_SEC == flag)
	{
		h_value = DBget_history(item->itemid, item->value_type, ZBX_DB_GET_HIST_MAX, now - arg1, now, NULL, 0);

		if (NULL != h_value[0])
		{
			zbx_strlcpy(value, h_value[0], MAX_BUFFER_LEN);
			res = SUCCEED;
		}
		else
			zabbix_log(LOG_LEVEL_DEBUG, "result for MAX is empty");
		DBfree_history(h_value);
	}
	else if (ZBX_FLAG_VALUES == flag)
	{
		h_value = DBget_history(item->itemid, item->value_type, ZBX_DB_GET_HIST_VALUE, 0, now, NULL, arg1);

		if (ITEM_VALUE_TYPE_UINT64 == item->value_type)
		{
			for (h_num = 0; NULL != h_value[h_num]; h_num++)
			{
				ZBX_STR2UINT64(l, h_value[h_num]);
				if (0 == h_num || l > max_uint64)
					max_uint64 = l;
			}
		}
		else
		{
			for (h_num = 0; NULL != h_value[h_num]; h_num++)
			{
				f = atof(h_value[h_num]);
				if (0 == h_num || f > max)
					max = f;
			}
		}
		DBfree_history(h_value);

		if (0 != h_num)
		{
			if (ITEM_VALUE_TYPE_UINT64 == item->value_type)
				zbx_snprintf(value, MAX_BUFFER_LEN, ZBX_FS_UI64, max_uint64);
			else
				zbx_snprintf(value, MAX_BUFFER_LEN, ZBX_FS_DBL, max);
			res = SUCCEED;
		}
		else
			zabbix_log(LOG_LEVEL_DEBUG, "result for MAX is empty");
	}
clean:
	zabbix_log(LOG_LEVEL_DEBUG, "End of %s():%s", __function_name, zbx_result_string(res));

	return res;
}

/******************************************************************************
 *                                                                            *
 * Function: evaluate_DELTA                                                   *
 *                                                                            *
 * Purpose: evaluate function 'delta' for the item                            *
 *                                                                            *
 * Parameters: item - item (performance metric)                               *
 *             parameters - number of seconds/values and time shift (optional)*
 *                                                                            *
 * Return value: SUCCEED - evaluated successfully, result is stored in 'value'*
 *               FAIL - failed to evaluate function                           *
 *                                                                            *
 * Author: Alexei Vladishev                                                   *
 *                                                                            *
 * Comments:                                                                  *
 *                                                                            *
 ******************************************************************************/
static int	evaluate_DELTA(char *value, DB_ITEM *item, const char *function, const char *parameters, time_t now)
{
	const char	*__function_name = "evaluate_DELTA";
	int		nparams, arg1, flag, h_num, res = FAIL;
	zbx_uint64_t	min_uint64 = 0, max_uint64 = 0, l;
	double		min = 0, max = 0, f;
	char		**h_value;

	zabbix_log(LOG_LEVEL_DEBUG, "In %s()", __function_name);

	if (ITEM_VALUE_TYPE_FLOAT != item->value_type && ITEM_VALUE_TYPE_UINT64 != item->value_type)
		goto clean;

	if (2 < (nparams = num_param(parameters)))
		goto clean;

	if (FAIL == get_function_parameter_uint(item, parameters, 1, &arg1, &flag))
		goto clean;

	if (2 == nparams)
	{
		int	time_shift, time_shift_flag;

		if (FAIL == get_function_parameter_uint(item, parameters, 2, &time_shift, &time_shift_flag))
			goto clean;
		if (ZBX_FLAG_SEC != time_shift_flag)
			goto clean;

		now -= time_shift;
	}

	if (ZBX_FLAG_SEC == flag)
	{
		h_value = DBget_history(item->itemid, item->value_type, ZBX_DB_GET_HIST_DELTA, now - arg1, now, NULL, 0);

		if (NULL != h_value[0])
		{
			zbx_strlcpy(value, h_value[0], MAX_BUFFER_LEN);
			res = SUCCEED;
		}
		else
			zabbix_log(LOG_LEVEL_DEBUG, "result for DELTA is empty");
		DBfree_history(h_value);
	}
	else if (ZBX_FLAG_VALUES == flag)
	{
		h_value = DBget_history(item->itemid, item->value_type, ZBX_DB_GET_HIST_VALUE, 0, now, NULL, arg1);

		if (ITEM_VALUE_TYPE_UINT64 == item->value_type)
		{
			for (h_num = 0; NULL != h_value[h_num]; h_num++)
			{
				ZBX_STR2UINT64(l, h_value[h_num]);
				if (0 == h_num || l < min_uint64)
					min_uint64 = l;
				if (0 == h_num || l > max_uint64)
					max_uint64 = l;
			}
		}
		else
		{
			for (h_num = 0; NULL != h_value[h_num]; h_num++)
			{
				f = atof(h_value[h_num]);
				if (0 == h_num || f < min)
					min = f;
				if (0 == h_num || f > max)
					max = f;
			}
		}
		DBfree_history(h_value);

		if (0 != h_num)
		{
			if (ITEM_VALUE_TYPE_UINT64 == item->value_type)
				zbx_snprintf(value, MAX_BUFFER_LEN, ZBX_FS_UI64, max_uint64 - min_uint64);
			else
				zbx_snprintf(value, MAX_BUFFER_LEN, ZBX_FS_DBL, max - min);
			res = SUCCEED;
		}
		else
			zabbix_log(LOG_LEVEL_DEBUG, "result for DELTA is empty");
	}
clean:
	zabbix_log(LOG_LEVEL_DEBUG, "End of %s():%s", __function_name, zbx_result_string(res));

	return res;
}

/******************************************************************************
 *                                                                            *
 * Function: evaluate_NODATA                                                  *
 *                                                                            *
 * Purpose: evaluate function 'nodata' for the item                           *
 *                                                                            *
 * Parameters: item - item (performance metric)                               *
 *             parameter - number of seconds                                  *
 *                                                                            *
 * Return value: SUCCEED - evaluated successfully, result is stored in 'value'*
 *               FAIL - failed to evaluate function                           *
 *                                                                            *
 * Author: Alexei Vladishev                                                   *
 *                                                                            *
 * Comments:                                                                  *
 *                                                                            *
 ******************************************************************************/
static int	evaluate_NODATA(char *value, DB_ITEM *item, const char *function, const char *parameters)
{
	const char	*__function_name = "evaluate_NODATA";
	int		arg1, flag, now, res = FAIL;

	zabbix_log(LOG_LEVEL_DEBUG, "In %s()", __function_name);

	if (1 < num_param(parameters))
		goto clean;

	if (FAIL == get_function_parameter_uint(item, parameters, 1, &arg1, &flag))
		goto clean;

	if (ZBX_FLAG_SEC != flag)
		goto clean;

	now = (int)time(NULL);

	if (item->lastclock + arg1 > now)
		zbx_strlcpy(value, "0", MAX_BUFFER_LEN);
	else
	{
		if (CONFIG_SERVER_STARTUP_TIME + arg1 > now)
			goto clean;

		zbx_strlcpy(value, "1", MAX_BUFFER_LEN);
	}

	res = SUCCEED;
clean:
	zabbix_log(LOG_LEVEL_DEBUG, "End of %s():%s", __function_name, zbx_result_string(res));

	return res;
}

/******************************************************************************
 *                                                                            *
 * Function: compare_last_and_prev                                            *
 *                                                                            *
 * Purpose: compare lastvalue_str and prevvalue_str for an item               *
 *                                                                            *
 * Parameters: item - item (performance metric)                               *
 *                                                                            *
 * Return value: 0 - values are equal                                         *
 *               non-zero - otherwise                                         *
 *                                                                            *
 * Author: Aleksandrs Saveljevs, Alexander Vladishev                          *
 *                                                                            *
 * Comments: To be used by functions abschange(), change(), and diff().       *
 *                                                                            *
 ******************************************************************************/
static int	compare_last_and_prev(const DB_ITEM *item, time_t now)
{
	int	i, res;
	char	**h_value;

	for (i = 0; '\0' != item->lastvalue_str[i] || '\0' != item->prevvalue_str[i]; i++)
	{
		if (item->lastvalue_str[i] != item->prevvalue_str[i])
			return 1;
	}

	if (ITEM_LASTVALUE_LEN > i || ITEM_VALUE_TYPE_STR == item->value_type)
		return 0;

	res = 0;	/* if values are no longer in history, consider them equal */

	h_value = DBget_history(item->itemid, item->value_type, ZBX_DB_GET_HIST_VALUE, 0, now, NULL, 2);

	if (NULL != h_value[0] && NULL != h_value[1])
		res = strcmp(h_value[0], h_value[1]);
	DBfree_history(h_value);

	return res;
}

/******************************************************************************
 *                                                                            *
 * Function: evaluate_ABSCHANGE                                               *
 *                                                                            *
 * Purpose: evaluate function 'abschange' for the item                        *
 *                                                                            *
 * Parameters: item - item (performance metric)                               *
 *             parameter - number of seconds                                  *
 *                                                                            *
 * Return value: SUCCEED - evaluated successfully, result is stored in 'value'*
 *               FAIL - failed to evaluate function                           *
 *                                                                            *
 * Author: Alexei Vladishev                                                   *
 *                                                                            *
 * Comments:                                                                  *
 *                                                                            *
 ******************************************************************************/
static int	evaluate_ABSCHANGE(char *value, DB_ITEM *item, const char *function, const char *parameters, time_t now)
{
	const char	*__function_name = "evaluate_ABSCHANGE";
	int		res = FAIL;

	zabbix_log(LOG_LEVEL_DEBUG, "In %s()", __function_name);

	if (1 == item->lastvalue_null || 1 == item->prevvalue_null)
		goto clean;

	switch (item->value_type)
	{
		case ITEM_VALUE_TYPE_FLOAT:
			zbx_snprintf(value, MAX_BUFFER_LEN, ZBX_FS_DBL,
					fabs(item->lastvalue_dbl - item->prevvalue_dbl));
			break;
		case ITEM_VALUE_TYPE_UINT64:
			/* to avoid overflow */
			if (item->lastvalue_uint64 >= item->prevvalue_uint64)
				zbx_snprintf(value, MAX_BUFFER_LEN, ZBX_FS_UI64,
						item->lastvalue_uint64 - item->prevvalue_uint64);
			else
				zbx_snprintf(value, MAX_BUFFER_LEN, ZBX_FS_UI64,
						item->prevvalue_uint64 - item->lastvalue_uint64);
			break;
		default:
			if (0 == compare_last_and_prev(item, now))
				zbx_strlcpy(value, "0", MAX_BUFFER_LEN);
			else
				zbx_strlcpy(value, "1", MAX_BUFFER_LEN);
			break;
	}
	res = SUCCEED;
clean:
	zabbix_log(LOG_LEVEL_DEBUG, "End of %s():%s", __function_name, zbx_result_string(res));

	return res;
}

/******************************************************************************
 *                                                                            *
 * Function: evaluate_CHANGE                                                  *
 *                                                                            *
 * Purpose: evaluate function 'change' for the item                           *
 *                                                                            *
 * Parameters: item - item (performance metric)                               *
 *             parameter - number of seconds                                  *
 *                                                                            *
 * Return value: SUCCEED - evaluated successfully, result is stored in 'value'*
 *               FAIL - failed to evaluate function                           *
 *                                                                            *
 * Author: Alexei Vladishev                                                   *
 *                                                                            *
 * Comments:                                                                  *
 *                                                                            *
 ******************************************************************************/
static int	evaluate_CHANGE(char *value, DB_ITEM *item, const char *function, const char *parameters, time_t now)
{
	const char	*__function_name = "evaluate_CHANGE";
	int		res = FAIL;

	zabbix_log(LOG_LEVEL_DEBUG, "In %s()", __function_name);

	if (1 == item->lastvalue_null || 1 == item->prevvalue_null)
		goto clean;

	switch (item->value_type)
	{
		case ITEM_VALUE_TYPE_FLOAT:
			zbx_snprintf(value, MAX_BUFFER_LEN, ZBX_FS_DBL,
					item->lastvalue_dbl - item->prevvalue_dbl);
			break;
		case ITEM_VALUE_TYPE_UINT64:
			/* to avoid overflow */
			if (item->lastvalue_uint64 >= item->prevvalue_uint64)
				zbx_snprintf(value, MAX_BUFFER_LEN, ZBX_FS_UI64,
						item->lastvalue_uint64 - item->prevvalue_uint64);
			else
				zbx_snprintf(value, MAX_BUFFER_LEN, "-" ZBX_FS_UI64,
						item->prevvalue_uint64 - item->lastvalue_uint64);
			break;
		default:
			if (0 == compare_last_and_prev(item, now))
				zbx_strlcpy(value, "0", MAX_BUFFER_LEN);
			else
				zbx_strlcpy(value, "1", MAX_BUFFER_LEN);
			break;
	}

	res = SUCCEED;
clean:
	zabbix_log(LOG_LEVEL_DEBUG, "End of %s():%s", __function_name, zbx_result_string(res));

	return res;
}

/******************************************************************************
 *                                                                            *
 * Function: evaluate_DIFF                                                    *
 *                                                                            *
 * Purpose: evaluate function 'diff' for the item                             *
 *                                                                            *
 * Parameters: item - item (performance metric)                               *
 *             parameter - number of seconds                                  *
 *                                                                            *
 * Return value: SUCCEED - evaluated successfully, result is stored in 'value'*
 *               FAIL - failed to evaluate function                           *
 *                                                                            *
 * Author: Alexei Vladishev                                                   *
 *                                                                            *
 * Comments:                                                                  *
 *                                                                            *
 ******************************************************************************/
static int	evaluate_DIFF(char *value, DB_ITEM *item, const char *function, const char *parameters, time_t now)
{
	const char	*__function_name = "evaluate_DIFF";
	int		res = FAIL;

	zabbix_log(LOG_LEVEL_DEBUG, "In %s()", __function_name);

	if (1 == item->lastvalue_null || 1 == item->prevvalue_null)
		goto clean;

	switch (item->value_type)
	{
		case ITEM_VALUE_TYPE_FLOAT:
			if (0 == cmp_double(item->lastvalue_dbl, item->prevvalue_dbl))
				zbx_strlcpy(value, "0", MAX_BUFFER_LEN);
			else
				zbx_strlcpy(value, "1", MAX_BUFFER_LEN);
			break;
		case ITEM_VALUE_TYPE_UINT64:
			if (item->lastvalue_uint64 == item->prevvalue_uint64)
				zbx_strlcpy(value, "0", MAX_BUFFER_LEN);
			else
				zbx_strlcpy(value, "1", MAX_BUFFER_LEN);
			break;
		default:
			if (0 == compare_last_and_prev(item, now))
				zbx_strlcpy(value, "0", MAX_BUFFER_LEN);
			else
				zbx_strlcpy(value, "1", MAX_BUFFER_LEN);
			break;
	}

	res = SUCCEED;
clean:
	zabbix_log(LOG_LEVEL_DEBUG, "End of %s():%s", __function_name, zbx_result_string(res));

	return res;
}

/******************************************************************************
 *                                                                            *
 * Function: evaluate_STR                                                     *
 *                                                                            *
 * Purpose: evaluate function 'str' for the item                              *
 *                                                                            *
 * Parameters: item - item (performance metric)                               *
 *             parameters - <string>[,seconds]                                *
 *                                                                            *
 * Return value: SUCCEED - evaluated successfully, result is stored in 'value'*
 *               FAIL - failed to evaluate function                           *
 *                                                                            *
 * Author: Alexander Vladishev                                                *
 *                                                                            *
 * Comments:                                                                  *
 *                                                                            *
 ******************************************************************************/
static int	evaluate_STR(char *value, DB_ITEM *item, const char *function, const char *parameters, time_t now)
{
#define ZBX_FUNC_STR		1
#define ZBX_FUNC_REGEXP		2
#define ZBX_FUNC_IREGEXP	3

	const char	*__function_name = "evaluate_STR";
	DB_RESULT	result;
	DB_ROW		row;
	char		*arg1 = NULL, *arg1_esc;
	int		arg2, flag, func, rows, h_num, res = FAIL;
	ZBX_REGEXP	*regexps = NULL;
	int		regexps_alloc = 0, regexps_num = 0;
	char		**h_value;

	zabbix_log(LOG_LEVEL_DEBUG, "In %s()", __function_name);

	if (ITEM_VALUE_TYPE_STR != item->value_type && ITEM_VALUE_TYPE_TEXT != item->value_type &&
			ITEM_VALUE_TYPE_LOG != item->value_type)
		goto clean;

	if (0 == strcmp(function, "str"))
		func = ZBX_FUNC_STR;
	else if (0 == strcmp(function, "regexp"))
		func = ZBX_FUNC_REGEXP;
	else if (0 == strcmp(function, "iregexp"))
		func = ZBX_FUNC_IREGEXP;
	else
		goto clean;

	if (2 < num_param(parameters))
		goto clean;

	if (FAIL == get_function_parameter_str(item, parameters, 1, &arg1))
		goto clean;

	if (FAIL == get_function_parameter_uint(item, parameters, 2, &arg2, &flag))
	{
		arg2 = 1;
		flag = ZBX_FLAG_VALUES;
	}

	if ((ZBX_FUNC_REGEXP == func || ZBX_FUNC_IREGEXP == func) && '@' == *arg1)
	{
		arg1_esc = DBdyn_escape_string(arg1 + 1);
		result = DBselect("select r.name,e.expression,e.expression_type,e.exp_delimiter,e.case_sensitive"
				" from regexps r,expressions e"
				" where r.regexpid=e.regexpid"
					" and r.name='%s'",
				arg1_esc);
		zbx_free(arg1_esc);

		while (NULL != (row = DBfetch(result)))
			add_regexp_ex(&regexps, &regexps_alloc, &regexps_num,
					row[0], row[1], atoi(row[2]), row[3][0], atoi(row[4]));
		DBfree_result(result);
	}

	if (ZBX_FLAG_SEC == flag)
		h_value = DBget_history(item->itemid, item->value_type, ZBX_DB_GET_HIST_VALUE, now - arg2, 0, NULL, 0);
	else
		h_value = DBget_history(item->itemid, item->value_type, ZBX_DB_GET_HIST_VALUE, 0, 0, NULL, arg2);

	rows = 0;
	if (ZBX_FUNC_STR == func)
	{
		for (h_num = 0; NULL != h_value[h_num]; h_num++)
		{
			if (NULL != strstr(h_value[h_num], arg1))
			{
				rows = 2;
				break;
			}
			rows = 1;
		}
	}
	else if (ZBX_FUNC_REGEXP == func)
	{
		for (h_num = 0; NULL != h_value[h_num]; h_num++)
		{
			if (SUCCEED == regexp_match_ex(regexps, regexps_num, h_value[h_num], arg1, ZBX_CASE_SENSITIVE))
			{
				rows = 2;
				break;
			}
			rows = 1;
		}
	}
	else if (ZBX_FUNC_IREGEXP == func)
	{
		for (h_num = 0; NULL != h_value[h_num]; h_num++)
		{
			if (SUCCEED == regexp_match_ex(regexps, regexps_num, h_value[h_num], arg1, ZBX_IGNORE_CASE))
			{
				rows = 2;
				break;
			}
			rows = 1;
		}
	}
	DBfree_history(h_value);

	if ((ZBX_FUNC_REGEXP == func || ZBX_FUNC_IREGEXP == func) && '@' == *arg1)
		zbx_free(regexps);

	zbx_free(arg1);

	if (0 == rows)
	{
		zabbix_log(LOG_LEVEL_DEBUG, "result for STR is empty");
		res = FAIL;
	}
	else
	{
		if (2 == rows)
			zbx_strlcpy(value, "1", MAX_BUFFER_LEN);
		else
			zbx_strlcpy(value, "0", MAX_BUFFER_LEN);
	}

	res = SUCCEED;
clean:
	zabbix_log(LOG_LEVEL_DEBUG, "End of %s():%s", __function_name, zbx_result_string(res));

	return res;
}

/******************************************************************************
 *                                                                            *
 * Function: evaluate_STRLEN                                                  *
 *                                                                            *
 * Purpose: evaluate function 'strlen' for the item                           *
 *                                                                            *
 * Parameters: value - buffer of size MAX_BUFFER_LEN                          *
 *             item - item (performance metric)                               *
 *             parameters - Nth last value and time shift (optional)          *
 *                                                                            *
 * Return value: SUCCEED - evaluated successfully, result is stored in 'value'*
 *               FAIL - failed to evaluate function                           *
 *                                                                            *
 * Author: Aleksandrs Saveljevs                                               *
 *                                                                            *
 * Comments:                                                                  *
 *                                                                            *
 ******************************************************************************/
static int	evaluate_STRLEN(char *value, DB_ITEM *item, const char *function, const char *parameters, time_t now)
{
	const char	*__function_name = "evaluate_STRLEN";
	int		res = FAIL;

	zabbix_log(LOG_LEVEL_DEBUG, "In %s()", __function_name);

	if (ITEM_VALUE_TYPE_STR != item->value_type && ITEM_VALUE_TYPE_TEXT != item->value_type &&
			ITEM_VALUE_TYPE_LOG != item->value_type)
		goto clean;

	if (SUCCEED == evaluate_LAST(value, item, "last", parameters, now))
	{
		zbx_snprintf(value, MAX_BUFFER_LEN, "%d", zbx_strlen_utf8(value));
		res = SUCCEED;
	}
clean:
	zabbix_log(LOG_LEVEL_DEBUG, "End of %s():%s", __function_name, zbx_result_string(res));

	return res;
}

/******************************************************************************
 *                                                                            *
 * Function: evaluate_FUZZYTIME                                               *
 *                                                                            *
 * Purpose: evaluate function 'fuzzytime' for the item                        *
 *                                                                            *
 * Parameters: item - item (performance metric)                               *
 *             parameter - number of seconds                                  *
 *                                                                            *
 * Return value: SUCCEED - evaluated successfully, result is stored in 'value'*
 *               FAIL - failed to evaluate function                           *
 *                                                                            *
 * Author: Alexei Vladishev                                                   *
 *                                                                            *
 * Comments:                                                                  *
 *                                                                            *
 ******************************************************************************/
static int	evaluate_FUZZYTIME(char *value, DB_ITEM *item, const char *function, const char *parameters, time_t now)
{
	const char	*__function_name = "evaluate_FUZZYTIME";
	int		arg1, flag, fuzlow, fuzhig, res = FAIL;

	zabbix_log(LOG_LEVEL_DEBUG, "In %s()", __function_name);

	if (ITEM_VALUE_TYPE_FLOAT != item->value_type && ITEM_VALUE_TYPE_UINT64 != item->value_type)
		goto clean;

	if (1 < num_param(parameters))
		goto clean;

	if (FAIL == get_function_parameter_uint(item, parameters, 1, &arg1, &flag))
		goto clean;

	if (ZBX_FLAG_SEC != flag)
		goto clean;

	if (1 == item->lastvalue_null)
		goto clean;

	fuzlow = (int)(now - arg1);
	fuzhig = (int)(now + arg1);

	if (ITEM_VALUE_TYPE_UINT64 == item->value_type)
	{
		if (item->lastvalue_uint64 >= fuzlow && item->lastvalue_uint64 <= fuzhig)
			zbx_strlcpy(value, "1", MAX_BUFFER_LEN);
		else
			zbx_strlcpy(value, "0", MAX_BUFFER_LEN);
	}
	else
	{
		if (item->lastvalue_dbl >= fuzlow && item->lastvalue_dbl <= fuzhig)
			zbx_strlcpy(value, "1", MAX_BUFFER_LEN);
		else
			zbx_strlcpy(value, "0", MAX_BUFFER_LEN);
	}

	res = SUCCEED;
clean:
	zabbix_log(LOG_LEVEL_DEBUG, "End of %s():%s", __function_name, zbx_result_string(res));

	return res;
}

/******************************************************************************
 *                                                                            *
 * Function: evaluate_function                                                *
 *                                                                            *
 * Purpose: evaluate function                                                 *
 *                                                                            *
 * Parameters: item - item to calculate function for                          *
 *             function - function (for example, 'max')                       *
 *             parameter - parameter of the function                          *
 *                                                                            *
 * Return value: SUCCEED - evaluated successfully, value contains its value   *
 *               FAIL - evaluation failed                                     *
 *                                                                            *
 * Author: Alexei Vladishev                                                   *
 *                                                                            *
 * Comments:                                                                  *
 *                                                                            *
 ******************************************************************************/
int	evaluate_function(char *value, DB_ITEM *item, const char *function, const char *parameter, time_t now)
{
	const char	*__function_name = "evaluate_function";
	int		ret;
	struct tm	*tm = NULL;

	zabbix_log(LOG_LEVEL_DEBUG, "In %s() function:'%s.%s(%s)'", __function_name,
			zbx_host_key_string_by_item(item), function, parameter);

	*value = '\0';

	if (0 == strcmp(function, "last") || 0 == strcmp(function, "prev"))
	{
		ret = evaluate_LAST(value, item, function, parameter, now);
	}
	else if (0 == strcmp(function, "min"))
	{
		ret = evaluate_MIN(value, item, function, parameter, now);
	}
	else if (0 == strcmp(function, "max"))
	{
		ret = evaluate_MAX(value, item, function, parameter, now);
	}
	else if (0 == strcmp(function, "avg"))
	{
		ret = evaluate_AVG(value, item, function, parameter, now);
	}
	else if (0 == strcmp(function, "sum"))
	{
		ret = evaluate_SUM(value, item, function, parameter, now);
	}
	else if (0 == strcmp(function, "count"))
	{
		ret = evaluate_COUNT(value, item, function, parameter, now);
	}
	else if (0 == strcmp(function, "delta"))
	{
		ret = evaluate_DELTA(value, item, function, parameter, now);
	}
	else if (0 == strcmp(function, "nodata"))
	{
		ret = evaluate_NODATA(value, item, function, parameter);
	}
	else if (0 == strcmp(function, "date"))
	{
		tm = localtime(&now);
		zbx_snprintf(value, MAX_BUFFER_LEN, "%.4d%.2d%.2d", tm->tm_year + 1900, tm->tm_mon + 1, tm->tm_mday);
		ret = SUCCEED;
	}
	else if (0 == strcmp(function, "dayofweek"))
	{
		tm = localtime(&now);
		zbx_snprintf(value, MAX_BUFFER_LEN, "%d", 0 == tm->tm_wday ? 7 : tm->tm_wday);
		ret = SUCCEED;
	}
	else if (0 == strcmp(function, "dayofmonth"))
	{
		tm = localtime(&now);
		zbx_snprintf(value, MAX_BUFFER_LEN, "%d", tm->tm_mday);
		ret = SUCCEED;
	}
	else if (0 == strcmp(function, "time"))
	{
		tm = localtime(&now);
		zbx_snprintf(value, MAX_BUFFER_LEN, "%.2d%.2d%.2d", tm->tm_hour, tm->tm_min, tm->tm_sec);
		ret = SUCCEED;
	}
	else if (0 == strcmp(function, "abschange"))
	{
		ret = evaluate_ABSCHANGE(value, item, function, parameter, now);
	}
	else if (0 == strcmp(function, "change"))
	{
		ret = evaluate_CHANGE(value, item, function, parameter, now);
	}
	else if (0 == strcmp(function, "diff"))
	{
		ret = evaluate_DIFF(value, item, function, parameter, now);
	}
	else if (0 == strcmp(function, "str") || 0 == strcmp(function, "regexp") || 0 == strcmp(function, "iregexp"))
	{
		ret = evaluate_STR(value, item, function, parameter, now);
	}
	else if (0 == strcmp(function, "strlen"))
	{
		ret = evaluate_STRLEN(value, item, function, parameter, now);
	}
	else if (0 == strcmp(function, "now"))
	{
		zbx_snprintf(value, MAX_BUFFER_LEN, "%d", (int)now);
		ret = SUCCEED;
	}
	else if (0 == strcmp(function, "fuzzytime"))
	{
		ret = evaluate_FUZZYTIME(value, item, function, parameter, now);
	}
	else if (0 == strcmp(function, "logeventid"))
	{
		ret = evaluate_LOGEVENTID(value, item, function, parameter, now);
	}
	else if (0 == strcmp(function, "logseverity"))
	{
		ret = evaluate_LOGSEVERITY(value, item, function, parameter, now);
	}
	else if (0 == strcmp(function, "logsource"))
	{
		ret = evaluate_LOGSOURCE(value, item, function, parameter, now);
	}
	else
	{
		zabbix_log(LOG_LEVEL_WARNING, "Unsupported function:%s",
				function);
		zabbix_syslog("Unsupported function:%s",
				function);
		ret = FAIL;
	}

	if (SUCCEED == ret)
		del_zeroes(value);

	zabbix_log(LOG_LEVEL_DEBUG, "End of %s():%s value:'%s'", __function_name, zbx_result_string(ret), value);

	return ret;
}

/******************************************************************************
 *                                                                            *
 * Function: add_value_suffix_uptime                                          *
 *                                                                            *
 * Purpose: Process suffix 'uptime'                                           *
 *                                                                            *
 * Parameters: value - value for adjusting                                    *
 *             max_len - max len of the value                                 *
 *                                                                            *
 * Return value:                                                              *
 *                                                                            *
 * Author: Alexei Vladishev                                                   *
 *                                                                            *
 * Comments:                                                                  *
 *                                                                            *
 ******************************************************************************/
static void	add_value_suffix_uptime(char *value, size_t max_len)
{
	const char	*__function_name = "add_value_suffix_uptime";

	double	secs, days;
	int	hours, mins, offset = 0;

	zabbix_log(LOG_LEVEL_DEBUG, "In %s() value:'%s'", __function_name, value);

	if (0 > (secs = round(atof(value))))
	{
		offset += zbx_snprintf(value, max_len, "-");
		secs = -secs;
	}

	days = floor(secs / SEC_PER_DAY);
	secs -= days * SEC_PER_DAY;

	hours = (int)(secs / SEC_PER_HOUR);
	secs -= (double)hours * SEC_PER_HOUR;

	mins = (int)(secs / SEC_PER_MIN);
	secs -= (double)mins * SEC_PER_MIN;

	if (0 != days)
		offset += zbx_snprintf(value + offset, max_len - offset, ZBX_FS_DBL_EXT(0) " days, ", days);

	zbx_snprintf(value + offset, max_len - offset, "%02d:%02d:%02d", hours, mins, (int)secs);

	zabbix_log(LOG_LEVEL_DEBUG, "End of %s() value:'%s'", __function_name, value);
}

/******************************************************************************
 *                                                                            *
 * Function: add_value_suffix_s                                               *
 *                                                                            *
 * Purpose: Process suffix 's'                                                *
 *                                                                            *
 * Parameters: value - value for adjusting                                    *
 *             max_len - max len of the value                                 *
 *                                                                            *
 * Return value:                                                              *
 *                                                                            *
 * Author: Alexei Vladishev                                                   *
 *                                                                            *
 * Comments:                                                                  *
 *                                                                            *
 ******************************************************************************/
static void	add_value_suffix_s(char *value, size_t max_len)
{
	const char	*__function_name = "add_value_suffix_s";

	double	secs, n;
	int	n_unit = 0, offset = 0;

	zabbix_log(LOG_LEVEL_DEBUG, "In %s() value:'%s'", __function_name, value);

	secs = atof(value);

	if (0 == floor(fabs(secs) * 1000))
	{
		zbx_snprintf(value, max_len, "%s", (0 == secs ? "0s" : "< 1ms"));
		goto clean;
	}

	if (0 > (secs = round(secs * 1000) / 1000))
	{
		offset += zbx_snprintf(value, max_len, "-");
		secs = -secs;
	}
	else
		*value = '\0';

	if (0 != (n = floor(secs / SEC_PER_YEAR)))
	{
		offset += zbx_snprintf(value + offset, max_len - offset, ZBX_FS_DBL_EXT(0) "y ", n);
		secs -= n * SEC_PER_YEAR;
		if (0 == n_unit)
			n_unit = 4;
	}

	if (0 != (n = floor(secs / SEC_PER_MONTH)))
	{
		offset += zbx_snprintf(value + offset, max_len - offset, "%dm ", (int)n);
		secs -= n * SEC_PER_MONTH;
		if (0 == n_unit)
			n_unit = 3;
	}

	if (0 != (n = floor(secs / SEC_PER_DAY)))
	{
		offset += zbx_snprintf(value + offset, max_len - offset, "%dd ", (int)n);
		secs -= n * SEC_PER_DAY;
		if (0 == n_unit)
			n_unit = 2;
	}

	if (4 > n_unit && 0 != (n = floor(secs / SEC_PER_HOUR)))
	{
		offset += zbx_snprintf(value + offset, max_len - offset, "%dh ", (int)n);
		secs -= n * SEC_PER_HOUR;
		if (0 == n_unit)
			n_unit = 1;
	}

	if (3 > n_unit && 0 != (n = floor(secs / SEC_PER_MIN)))
	{
		offset += zbx_snprintf(value + offset, max_len - offset, "%dm ", (int)n);
		secs -= n * SEC_PER_MIN;
	}

	if (2 > n_unit && 0 != (n = floor(secs)))
	{
		offset += zbx_snprintf(value + offset, max_len - offset, "%ds ", (int)n);
		secs -= n;
	}

	if (1 > n_unit && 0 != (n = round(secs * 1000)))
		offset += zbx_snprintf(value + offset, max_len - offset, "%dms", (int)n);

	if (0 != offset && ' ' == value[--offset])
		value[offset] = '\0';
clean:
	zabbix_log(LOG_LEVEL_DEBUG, "End of %s() value:'%s'", __function_name, value);
}

/******************************************************************************
 *                                                                            *
 * Function: add_value_suffix_normal                                          *
 *                                                                            *
 * Purpose: Process normal values and add K,M,G,T                             *
 *                                                                            *
 * Parameters: value - value for adjusting                                    *
 *             max_len - max len of the value                                 *
 *             units - units (bps, b, B, etc)                                 *
 *                                                                            *
 * Return value:                                                              *
 *                                                                            *
 * Author: Alexei Vladishev                                                   *
 *                                                                            *
 * Comments:                                                                  *
 *                                                                            *
 ******************************************************************************/
static void	add_value_suffix_normal(char *value, int max_len, const char *units)
{
	const char	*__function_name = "add_value_suffix_normal";

	const char	*minus = "";
	char		kmgt[8];
	char		tmp[64];
	double		base;
	double		value_double;

	zabbix_log(LOG_LEVEL_DEBUG, "In %s() value:'%s' units:'%s'",
			__function_name, value, units);

	if (0 > (value_double = atof(value)))
	{
		minus = "-";
		value_double = -value_double;
	}

	base = (0 == strcmp(units, "B") || 0 == strcmp(units, "Bps") ? 1024 : 1000);

	if (value_double < base || SUCCEED == str_in_list("%,ms,rpm,RPM", units, ','))
	{
		strscpy(kmgt, "");
	}
	else if (value_double < base * base)
	{
		strscpy(kmgt, "K");
		value_double /= base;
	}
	else if (value_double < base * base * base)
	{
		strscpy(kmgt, "M");
		value_double /= base * base;
	}
	else if (value_double < base * base * base * base)
	{
		strscpy(kmgt, "G");
		value_double /= base * base * base;
	}
	else
	{
		strscpy(kmgt, "T");
		value_double /= base * base * base * base;
	}

	if (0 != cmp_double((int)(value_double + 0.5), value_double))
	{
		zbx_snprintf(tmp, sizeof(tmp), ZBX_FS_DBL_EXT(2), value_double);
		del_zeroes(tmp);
	}
	else
		zbx_snprintf(tmp, sizeof(tmp), ZBX_FS_DBL_EXT(0), value_double);

	zbx_snprintf(value, max_len, "%s%s %s%s",
			minus, tmp, kmgt, units);

	zabbix_log(LOG_LEVEL_DEBUG, "End of %s() value:'%s'",
			__function_name, value);
}

/******************************************************************************
 *                                                                            *
 * Function: add_value_suffix                                                 *
 *                                                                            *
 * Purpose: Add suffix for value                                              *
 *                                                                            *
 * Parameters: value - value for replacing                                    *
 *                                                                            *
 * Return value: SUCCEED - suffix added successfully, value contains new value*
 *               FAIL - adding failed, value contains old value               *
 *                                                                            *
 * Author: Alexei Vladishev                                                   *
 *                                                                            *
 * Comments: !!! Don't forget sync code with PHP (function convert_units) !!! *
 *                                                                            *
 ******************************************************************************/
int	add_value_suffix(char *value, int max_len, const char *units, int value_type)
{
	const char	*__function_name = "add_value_suffix";

	int		ret = FAIL;
	struct tm	*local_time = NULL;
	time_t		time;
	char		tmp[256];

	zabbix_log(LOG_LEVEL_DEBUG, "In %s() value:'%s' units:'%s'",
			__function_name, value, units);

	switch (value_type)
	{
		case ITEM_VALUE_TYPE_UINT64:
			if (0 == strcmp(units, "unixtime"))
			{
				time = (time_t)atoi(value);
				local_time = localtime(&time);
				strftime(tmp, sizeof(tmp), "%Y.%m.%d %H:%M:%S", local_time);
				zbx_strlcpy(value, tmp, max_len);
				ret = SUCCEED;
				break;
			}
		case ITEM_VALUE_TYPE_FLOAT:
			if (0 == strcmp(units, "s"))
			{
				add_value_suffix_s(value, max_len);
				ret = SUCCEED;
			}
			else if (0 == strcmp(units, "uptime"))
			{
				add_value_suffix_uptime(value, max_len);
				ret = SUCCEED;
			}
			else if (0 != strlen(units))
			{
				add_value_suffix_normal(value, max_len, units);
				ret = SUCCEED;
			}
			break;
		default:
			ret = FAIL;
			break;
	}

	zabbix_log(LOG_LEVEL_DEBUG, "End of %s():%s value:'%s'",
			__function_name, zbx_result_string(ret), value);

	return ret;
}

/******************************************************************************
 *                                                                            *
 * Function: replace_value_by_map                                             *
 *                                                                            *
 * Purpose: replace value by mapping value                                    *
 *                                                                            *
 * Parameters: value - value for replacing                                    *
 *             valuemapid - index of value map                                *
 *                                                                            *
 * Return value: SUCCEED - evaluated successfully, value contains new value   *
 *               FAIL - evaluation failed, value contains old value           *
 *                                                                            *
 * Author: Eugene Grigorjev                                                   *
 *                                                                            *
 * Comments:                                                                  *
 *                                                                            *
 ******************************************************************************/
int	replace_value_by_map(char *value, int max_len, zbx_uint64_t valuemapid)
{
	const char	*__function_name = "replace_value_by_map";

	DB_RESULT	result;
	DB_ROW		row;
	char		new_value[MAX_STRING_LEN], orig_value[MAX_BUFFER_LEN], *value_esc;
	int		ret = FAIL;

	zabbix_log(LOG_LEVEL_DEBUG, "In %s()", __function_name);

	if (0 == valuemapid)
		goto clean;

	value_esc = DBdyn_escape_string(value);
	result = DBselect("select newvalue from mappings where valuemapid=" ZBX_FS_UI64 " and value='%s'",
			valuemapid,
			value_esc);
	zbx_free(value_esc);

	if (NULL != (row = DBfetch(result)) && FAIL == DBis_null(row[0]))
	{
		strscpy(new_value, row[0]);
		del_zeroes(new_value);

		strscpy(orig_value, value);

		zbx_snprintf(value, max_len, "%s (%s)", new_value, orig_value);

		ret = SUCCEED;
	}
	DBfree_result(result);
clean:
	zabbix_log(LOG_LEVEL_DEBUG, "End of %s() value:'%s'", __function_name, value);

	return ret;
}

/******************************************************************************
 *                                                                            *
 * Function: evaluate_macro_function                                          *
 *                                                                            *
 * Purpose: evaluate function used as a macro (e.g., in notifications)        *
 *                                                                            *
 * Parameters: host - host the key belongs to                                 *
 *             key - item's key (for example, 'system.cpu.load[,avg1]')       *
 *             function - function (for example, 'max')                       *
 *             parameter - parameter of the function                          *
 *                                                                            *
 * Return value: SUCCEED - evaluated successfully, value contains its value   *
 *               FAIL - evaluation failed                                     *
 *                                                                            *
 * Author: Alexei Vladishev                                                   *
 *                                                                            *
 * Comments: used for evaluation of notification macros                       *
 *           output buffer size should be MAX_BUFFER_LEN                      *
 *                                                                            *
 ******************************************************************************/
int	evaluate_macro_function(char *value, const char *host, const char *key, const char *function, const char *parameter)
{
	const char	*__function_name = "evaluate_macro_function";

	DB_ITEM		item;
	DB_RESULT	result;
	DB_ROW		row;
	char		*host_esc, *key_esc;
	int		res;

	zabbix_log(LOG_LEVEL_DEBUG, "In %s() function:'%s:%s.%s(%s)'",
			__function_name, host, key, function, parameter);

	host_esc = DBdyn_escape_string(host);
	key_esc = DBdyn_escape_string(key);

	result = DBselect(
			"select %s"
			" where h.host='%s'"
				" and h.hostid=i.hostid"
				" and i.key_='%s'"
				DB_NODE,
			ZBX_SQL_ITEM_SELECT, host_esc, key_esc, DBnode_local("h.hostid"));

	zbx_free(host_esc);
	zbx_free(key_esc);

	if (NULL == (row = DBfetch(result)))
	{
		DBfree_result(result);
		zabbix_log(LOG_LEVEL_WARNING, "Function [%s:%s.%s(%s)] not found. Query returned empty result",
				host, key, function, parameter);
		zabbix_syslog("Function [%s:%s.%s(%s)] not found. Query returned empty result",
				host, key, function, parameter);
		return FAIL;
	}

	DBget_item_from_db(&item, row);

	if (SUCCEED == (res = evaluate_function(value, &item, function, parameter, time(NULL))))
	{
		if (SUCCEED == str_in_list("last,prev", function, ','))
		{
			switch (item.value_type)
			{
				case ITEM_VALUE_TYPE_FLOAT:
				case ITEM_VALUE_TYPE_UINT64:
					if (SUCCEED != replace_value_by_map(value, MAX_BUFFER_LEN, item.valuemapid))
						add_value_suffix(value, MAX_BUFFER_LEN, item.units, item.value_type);
					break;
				case ITEM_VALUE_TYPE_STR:
					replace_value_by_map(value, MAX_BUFFER_LEN, item.valuemapid);
					break;
				default:
					;
			}
		}
		else if (SUCCEED == str_in_list("abschange,avg,change,delta,max,min,sum", function, ','))
		{
			switch (item.value_type)
			{
				case ITEM_VALUE_TYPE_FLOAT:
				case ITEM_VALUE_TYPE_UINT64:
					add_value_suffix(value, MAX_BUFFER_LEN, item.units, item.value_type);
					break;
				default:
					;
			}
		}
	}

	DBfree_result(result); /* cannot call DBfree_result until evaluate_FUNC */

	zabbix_log(LOG_LEVEL_DEBUG, "End of %s():%s value:'%s'", __function_name,
			zbx_result_string(res), value);

	return res;
}
