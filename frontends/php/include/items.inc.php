<?php
/*
** ZABBIX
** Copyright (C) 2000-2010 SIA Zabbix
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
?>
<?php

/*
 * Function: get_item_logtype_description
 *
 * Description:
 *	 convert windows events type constant in to the string representation
 *
 * Author:
 *	 Alexander Vladishev
 *
 * Comments:
 *
 */
	function get_item_logtype_description($logtype){
		switch ($logtype){
			case ITEM_LOGTYPE_INFORMATION:		return S_INFORMATION;
			case ITEM_LOGTYPE_WARNING:		return S_WARNING;
			case ITEM_LOGTYPE_ERROR:		return S_ERROR;
			case ITEM_LOGTYPE_FAILURE_AUDIT:	return S_FAILURE_AUDIT;
			case ITEM_LOGTYPE_SUCCESS_AUDIT:	return S_SUCCESS_AUDIT;
			default:				return S_UNKNOWN;
		}
	}

/*
 * Function: get_item_logtype_style
 *
 * Description:
 *	 convert windows events type constant in to the CSS style name
 *
 * Author:
 *	 Alexander Vladishev
 *
 * Comments:
 *
 */
	function get_item_logtype_style($logtype){
		switch($logtype){
			case ITEM_LOGTYPE_INFORMATION:		return 'information';
			case ITEM_LOGTYPE_WARNING:		return 'warning';
			case ITEM_LOGTYPE_ERROR:		return 'high';
			case ITEM_LOGTYPE_FAILURE_AUDIT:	return 'high';
			case ITEM_LOGTYPE_SUCCESS_AUDIT:	return 'information';
			default:				return 'normal';
		}
	}

/*
 * Function: item_type2str
 *
 * Description:
 *     Represent integer value of item type as string
 *
 * Author:
 *     Eugene Grigorjev (eugene.grigorjev@zabbix.com)
 *
 * Comments:
 *
 */
	function item_type2str($type){
		switch($type){
			case ITEM_TYPE_ZABBIX:		$type = S_ZABBIX_AGENT;			break;
			case ITEM_TYPE_SNMPV1:		$type = S_SNMPV1_AGENT;			break;
			case ITEM_TYPE_TRAPPER:		$type = S_ZABBIX_TRAPPER;		break;
			case ITEM_TYPE_SIMPLE:		$type = S_SIMPLE_CHECK;			break;
			case ITEM_TYPE_SNMPV2C:		$type = S_SNMPV2_AGENT;			break;
			case ITEM_TYPE_INTERNAL:	$type = S_ZABBIX_INTERNAL;		break;
			case ITEM_TYPE_SNMPV3:		$type = S_SNMPV3_AGENT;			break;
			case ITEM_TYPE_ZABBIX_ACTIVE:	$type = S_ZABBIX_AGENT_ACTIVE;		break;
			case ITEM_TYPE_AGGREGATE:	$type = S_ZABBIX_AGGREGATE;		break;
			case ITEM_TYPE_HTTPTEST:	$type = S_WEB_MONITORING;		break;
			case ITEM_TYPE_EXTERNAL:	$type = S_EXTERNAL_CHECK;		break;
			case ITEM_TYPE_DB_MONITOR:	$type = S_ZABBIX_DATABASE_MONITOR;	break;
			case ITEM_TYPE_IPMI:		$type = S_IPMI_AGENT;			break;
			case ITEM_TYPE_SSH:		$type = S_SSH_AGENT;			break;
			case ITEM_TYPE_TELNET:		$type = S_TELNET_AGENT;			break;
			case ITEM_TYPE_CALCULATED:	$type = S_CALCULATED;			break;
			default:$type = S_UNKNOWN;			break;
		}
	return $type;
	}

	/*
	 * Function: item_value_type2str
	 *
	 * Description:
	 *     Represent integer value of item value type as string
	 *
	 * Author:
	 *     Eugene Grigorjev (eugene.grigorjev@zabbix.com)
	 *
	 * Comments:
	 *
	 */
	function item_value_type2str($value_type){
		switch($value_type){
			case ITEM_VALUE_TYPE_UINT64:	$value_type = S_NUMERIC_UNSIGNED;		break;
			case ITEM_VALUE_TYPE_FLOAT:	$value_type = S_NUMERIC_FLOAT;		break;
			case ITEM_VALUE_TYPE_STR:	$value_type = S_CHARACTER;		break;
			case ITEM_VALUE_TYPE_LOG:	$value_type = S_LOG;			break;
			case ITEM_VALUE_TYPE_TEXT:	$value_type = S_TEXT;			break;
			default:$value_type = S_UNKNOWN;			break;
		}
	return $value_type;
	}

	/*
	 * Function: item_data_type2str
	 *
	 * Description:
	 *     Represent integer value of item data type as string
	 *
	 * Author:
	 *     Eugene Grigorjev (eugene.grigorjev@zabbix.com)
	 *
	 * Comments:
	 *
	 */
	function item_data_type2str($data_type){
		switch($data_type){
			case ITEM_DATA_TYPE_DECIMAL:		$data_type = S_DECIMAL;		break;
			case ITEM_DATA_TYPE_OCTAL:		$data_type = S_OCTAL;		break;
			case ITEM_DATA_TYPE_HEXADECIMAL:	$data_type = S_HEXADECIMAL;	break;
			default:$data_type = S_UNKNOWN;		break;
		}
	return $data_type;
	}

	/*
	 * Function: item_status2str
	 *
	 * Description:
	 *     Represent integer value of item status as string
	 *
	 * Author:
	 *     Eugene Grigorjev (eugene.grigorjev@zabbix.com)
	 *
	 * Comments:
	 *
	 */
	function	item_status2str($status){
		switch($status){
			case ITEM_STATUS_ACTIVE:	$status = S_ACTIVE;		break;
			case ITEM_STATUS_DISABLED:	$status = S_DISABLED;		break;
			case ITEM_STATUS_NOTSUPPORTED:	$status = S_NOT_SUPPORTED;	break;
			default:
				$status = S_UNKNOWN;		break;
		}
	return $status;
	}

	/*
	 * Function: item_status2style
	 *
	 * Description:
	 *     Represent integer value of item status as CSS style name
	 *
	 * Author:
	 *     Eugene Grigorjev (eugene.grigorjev@zabbix.com)
	 *
	 * Comments:
	 *
	 */
	function	item_status2style($status){
		switch($status){
			case ITEM_STATUS_ACTIVE:	$status = 'off';	break;
			case ITEM_STATUS_DISABLED:	$status = 'on';		break;
			case ITEM_STATUS_NOTSUPPORTED:
			default:
				$status = 'unknown';	break;
		}
	return $status;
	}
	# Update Item definition for selected group

	function update_item_in_group($groupid,$itemid,$item){
/*		$description,$key,$hostid,$delay,$history,$status,$type,$snmp_community,$snmp_oid,$value_type,$trapper_hosts,$snmp_port,$units,$multiplier,$delta,$snmpv3_securityname,$snmpv3_securitylevel,$snmpv3_authpassphrase,$snmpv3_privpassphrase,$formula,$trends,$logtimefmt,$valuemapid,$delay_flex,$params,$ipmi_sensor,$applications)
//*/
		$sql='SELECT i.itemid,i.hostid '.
				' FROM hosts_groups hg,items i '.
				' WHERE hg.groupid='.$groupid.
					' and i.key_='.zbx_dbstr($item['key_']).
					' and hg.hostid=i.hostid';
		$result=DBSelect($sql);
		while($row=DBfetch($result)){
			$item['hostid'] = $row['hostid'];
			update_item($row['itemid'],$item);
		}
	return true;
	}

// Delete Item definition from selected group
	function delete_item_from_group($groupid,$itemid){
		if(!isset($itemid)){
			return 0;
		}

		$item=get_item_by_itemid($itemid);
		if(!$item){
			return 0;
		}

		$del_items = array();
		$sql='SELECT i.itemid '.
			' FROM hosts_groups hg,items i'.
			' WHERE hg.groupid='.$groupid.
				' AND i.key_='.zbx_dbstr($item["key_"]).
				' AND hg.hostid=i.hostid';
		$result=DBSelect($sql);
		while($row=DBfetch($result)){
			$del_items[$row['itemid']] = $row['itemid'];
		}
		if(!empty($del_items)){
			delete_item($del_items);
		}
	return 1;
	}

	# Add Item definition to selected group

	function add_item_to_group($groupid,$item){
/*	$description,$key,$hostid,$delay,$history,$status,$type,$snmp_community,$snmp_oid,$value_type,$trapper_hosts,$snmp_port,$units,$multiplier,$delta,$snmpv3_securityname,$snmpv3_securitylevel,$snmpv3_authpassphrase,$snmpv3_privpassphrase,$formula,$trends,$logtimefmt,$valuemapid,$delay_flex,$params,$ipmi_sensor,$applications)
//*/
		$sql='SELECT hostid FROM hosts_groups WHERE groupid='.$groupid;
		$result=DBSelect($sql);
		while($row=DBfetch($result)){
			$item['hostid'] = $row['hostid'];
			add_item($item);
		}
	return true;
	}

	/******************************************************************************
	 *                                                                            *
	 * Comments: !!! Don't forget sync code with C !!!                            *
	 *                                                                            *
	 ******************************************************************************/
	function add_item($item){
/*
		$item = array('description','key','hostid','delay','history','status','type',
		'snmp_community','snmp_oid','value_type','trapper_hosts','snmp_port','units','multiplier','delta',
		'snmpv3_securityname','snmpv3_securitylevel','snmpv3_authpassphrase','snmpv3_privpassphrase',
		'formula','trends','logtimefmt','valuemapid','delay_flex','params','ipmi_sensor','applications','templateid');
//*/

		$item_db_fields = array(
				'description'		=> null,
				'key_'			=> null,
				'hostid'		=> null,
				'delay'			=> 60,
				'history'		=> 7,
				'status'		=> ITEM_STATUS_ACTIVE,
				'type'			=> ITEM_TYPE_ZABBIX,
				'snmp_community'	=> '',
				'snmp_oid'		=> '',
				'value_type'		=> ITEM_VALUE_TYPE_STR,
				'data_type'		=> ITEM_DATA_TYPE_DECIMAL,
				'trapper_hosts'		=> 'localhost',
				'snmp_port'		=> 161,
				'units'			=> '',
				'multiplier'		=> 0,
				'delta'			=> 0,
				'snmpv3_securityname'	=> '',
				'snmpv3_securitylevel'	=> 0,
				'snmpv3_authpassphrase'	=> '',
				'snmpv3_privpassphrase'	=> '',
				'formula'		=> 0,
				'trends'		=> 365,
				'logtimefmt'		=> '',
				'valuemapid'		=> 0,
				'delay_flex'		=> '',
				'authtype'		=> 0,
				'username'		=> '',
				'password'		=> '',
				'publickey'		=> '',
				'privatekey'		=> '',
				'params'		=> '',
				'ipmi_sensor'		=> '',
				'applications'		=> array(),
				'templateid'		=> 0);

		if(!check_db_fields($item_db_fields, $item)){
			error(S_INCORRECT_ARGUMENTS_PASSED_TO_FUNCTION.SPACE.'[add_item]');
			return false;
		}

		$host=get_host_by_hostid($item['hostid']);
		if(!$host){
			return false;
		}

		if(($i = array_search(0,$item['applications'])) !== FALSE)
			unset($item['applications'][$i]);

		if(!preg_match('/^'.ZBX_PREG_ITEM_KEY_FORMAT.'$/u', $item['key_']) ){
			error(S_INCORRECT_KEY_FORMAT.SPACE."'key_name[param1,param2,...]'");
			return false;
		}

		if(($item['type'] == ITEM_TYPE_DB_MONITOR && $item['key_'] == 'db.odbc.select[<unique short description>]') ||
		   ($item['type'] == ITEM_TYPE_SSH && $item['key_'] == 'ssh.run[<unique short description>,<ip>,<port>,<encoding>]') ||
		   ($item['type'] == ITEM_TYPE_TELNET && $item['key_'] == 'telnet.run[<unique short description>,<ip>,<port>,<encoding>]')) {
		   	error(S_ITEMS_CHECK_KEY_DEFAULT_EXAMPLE_PASSED);
		   	return false;
		}

		$res = calculate_item_nextcheck(0, $item['type'], $item['delay'], $item['delay_flex'], time());
		if ($res['delay'] == SEC_PER_YEAR && $item['type'] != ITEM_TYPE_ZABBIX_ACTIVE && $item['type'] != ITEM_TYPE_TRAPPER){
			error(S_ITEM_WILL_NOT_BE_REFRESHED_PLEASE_ENTER_A_CORRECT_UPDATE_INTERVAL);
			return FALSE;
		}

		if(($item['snmp_port']<1 || $item['snmp_port']>65535) && in_array($item['type'],array(ITEM_TYPE_SNMPV1,ITEM_TYPE_SNMPV2C,ITEM_TYPE_SNMPV3))){
			error(S_INVALID_SNMP_PORT);
			return FALSE;
		}

		if(preg_match('/^(log|logrt|eventlog)\[/', $item['key_']) && ($item['value_type'] != ITEM_VALUE_TYPE_LOG)){
			error(S_TYPE_INFORMATION_BUST_LOG_FOR_LOG_KEY);
			return FALSE;
		}

		if($item['value_type'] == ITEM_VALUE_TYPE_STR){
			$item['delta']=0;
		}

		if ($item['value_type'] != ITEM_VALUE_TYPE_UINT64) {
			$item['data_type'] = 0;
		}

		if(($item['type'] == ITEM_TYPE_AGGREGATE) && ($item['value_type'] != ITEM_VALUE_TYPE_FLOAT)){
			error(S_VALUE_TYPE_MUST_FLOAT_FOR_AGGREGATE_ITEMS);
			return FALSE;
		}

		if($item['type'] == ITEM_TYPE_AGGREGATE){
			/* grpfunc['group','key','itemfunc','numeric param'] */
			if(preg_match('/^((.)*)(\[\"((.)*)\"\,\"((.)*)\"\,\"((.)*)\"\,\"([0-9]+)\"\])$/i', $item['key_'], $arr)){
				$g=$arr[1];
				if(!str_in_array($g,array("grpmax","grpmin","grpsum","grpavg"))){
					error(S_GROUP_FUNCTION.SPACE."[$g]".SPACE.S_IS_NOT_ONE_OF.SPACE."[grpmax, grpmin, grpsum, grpavg]");
					return FALSE;
				}
				// Group
				$g=$arr[4];
				// Key
				$g=$arr[6];
				// Item function
				$g=$arr[8];
				if(!str_in_array($g,array('last', 'min', 'max', 'avg', 'sum','count'))){
					error(S_ITEM_FUNCTION.SPACE.'['.$g.']'.SPACE.S_IS_NOT_ONE_OF.SPACE.'[last, min, max, avg, sum, count]');
					return FALSE;
				}
				// Parameter
				$g=$arr[10];
			}
			else{
				error(S_KEY_DOES_NOT_MATCH.SPACE.'grpfunc["group","key","itemfunc","numeric param"]');
				return FALSE;
			}
		}

		$sql = 'SELECT itemid, hostid, templateid '.
				' FROM items '.
				' WHERE hostid='.$item['hostid'].
					' AND key_='.zbx_dbstr($item['key_']);
		$db_item = DBfetch(DBselect($sql));
		if($db_item && (($item['templateid'] == 0) || (($db_item['templateid'] != 0) && ($item['templateid'] != 0) && ($item['templateid'] != $db_item['templateid'])))){
			error(S_AN_ITEM_WITH_THE_KEY.SPACE.'['.$item['key_'].']'.SPACE.S_ALREADY_EXISTS_FOR_HOST_SMALL.SPACE.'['.$host['host'].'].'.SPACE.S_THE_KEY_MUST_BE_UNIQUE);
			return FALSE;
		}
		else if ($db_item && $item['templateid'] != 0){
			$item['hostid'] = $db_item['hostid'];
			$item['applications'] = get_same_applications_for_host($item['applications'], $db_item['hostid']);

			$result = update_item($db_item['itemid'], $item);

			return $result;
		}

		//validating item key
		list($item_key_is_valid, $check_result) = check_item_key($item['key_']);
		if(!$item_key_is_valid){
			error(S_ERROR_IN_ITEM_KEY.SPACE.$check_result);
			return false;
		}

		// first add mother item
		$itemid=get_dbid('items','itemid');
		$result=DBexecute('INSERT INTO items '.
				' (itemid,description,key_,hostid,delay,history,status,type,'.
					'snmp_community,snmp_oid,value_type,data_type,trapper_hosts,'.
					'snmp_port,units,multiplier,'.
					'delta,snmpv3_securityname,snmpv3_securitylevel,snmpv3_authpassphrase,'.
					'snmpv3_privpassphrase,formula,trends,logtimefmt,valuemapid,'.
					'delay_flex,params,ipmi_sensor,templateid,authtype,username,password,publickey,privatekey)'.
			' VALUES ('.$itemid.','.zbx_dbstr($item['description']).','.zbx_dbstr($item['key_']).','.(!$item['hostid']? '0':$item['hostid']).','.
						(!$item['delay']? '0':$item['delay']).','.(!$item['history']? '0':$item['history']).','.(!$item['status']? '0':$item['status']).','.(!$item['type']? '0':$item['type']).','.
						zbx_dbstr($item['snmp_community']).','.zbx_dbstr($item['snmp_oid']).','.(!$item['value_type']? '0':$item['value_type']).','.(!$item['data_type']? '0':$item['data_type']).','.
						zbx_dbstr($item['trapper_hosts']).','.$item['snmp_port'].','.zbx_dbstr($item['units']).','.(!$item['multiplier'] ? '0':$item['multiplier']).','.
						intval($item['delta']).','.zbx_dbstr($item['snmpv3_securityname']).','.intval($item['snmpv3_securitylevel']).','.
						zbx_dbstr($item['snmpv3_authpassphrase']).','.zbx_dbstr($item['snmpv3_privpassphrase']).','.
						zbx_dbstr($item['formula']).','.(!$item['trends'] ? '0':$item['trends']).','.zbx_dbstr($item['logtimefmt']).','.(!$item['valuemapid'] ? '0':$item['valuemapid']).','.
						zbx_dbstr($item['delay_flex']).','.zbx_dbstr($item['params']).','.
						zbx_dbstr($item['ipmi_sensor']).','.$item['templateid'].','.intval($item['authtype']).','.
						zbx_dbstr($item['username']).','.zbx_dbstr($item['password']).','.
						zbx_dbstr($item['publickey']).','.zbx_dbstr($item['privatekey']).')'
			);

		if ($result)
			add_audit_ext(AUDIT_ACTION_ADD, AUDIT_RESOURCE_ITEM, $itemid, $host['host'].':'.$item['description'], NULL, NULL, NULL);
		else
			return $result;

		foreach($item['applications'] as $key => $appid){
			$itemappid=get_dbid('items_applications','itemappid');
			DBexecute('INSERT INTO items_applications (itemappid,itemid,applicationid) VALUES('.$itemappid.','.$itemid.','.$appid.')');
		}

		info(S_ADDED_NEW_ITEM.SPACE.$host['host'].':'.$item['key_']);

// add items to child hosts

		$db_hosts = get_hosts_by_templateid($host['hostid']);
		while($db_host = DBfetch($db_hosts)){
// recursion
			$item['hostid'] = $db_host['hostid'];
			$item['applications'] = get_same_applications_for_host($item['applications'], $db_host['hostid']);
			$item['templateid'] = $itemid;

			$result = add_item($item);
			if(!$result) break;
		}

		if($result)
			return $itemid;

		if($item['templateid'] == 0){
			delete_item($itemid);
		}

	return $result;
	}

// Update Item status

	function update_item_status($itemids, $status){
		zbx_value2array($itemids);
		$result = true;

		$db_items = DBselect('SELECT * FROM items WHERE '.DBcondition('itemid',$itemids));
		while($row = DBfetch($db_items)){
			$old_status=$row['status'];

			if($status != $old_status){
/*				unset($itemids[$row['itemid']]);*/
				if ($status==ITEM_STATUS_ACTIVE)
					$sql='UPDATE items SET status='.$status.",error='' ".
						' WHERE itemid='.$row['itemid'];
				else
					$sql='UPDATE items SET status='.$status.
						' WHERE itemid='.$row['itemid'];

				$result &= DBexecute($sql);
				if ($result){
					$host=get_host_by_hostid($row['hostid']);
					$item_new = get_item_by_itemid($row['itemid']);
					add_audit_ext(AUDIT_ACTION_UPDATE, AUDIT_RESOURCE_ITEM, $row['itemid'], $host['host'].':'.$row['description'], 'items', $row, $item_new);
				}
			}
		}
/*		if(!empty($itemids)){
			update_trigger_value_to_unknown_by_itemid($itemids);

			if($status==ITEM_STATUS_ACTIVE)
				$sql='UPDATE items SET status='.$status.",error='' ".
					' WHERE '.DBcondition('itemid',$itemids);
			else
				$sql='UPDATE items SET status='.$status.
					' WHERE '.DBcondition('itemid',$itemids);

			$result = DBexecute($sql);
		}*/

	return $result;
	}

	/******************************************************************************
	 *                                                                            *
	 * Comments: !!! Don't forget sync code with C !!!                            *
	 *                                                                            *
	 ******************************************************************************/
	function update_item($itemid,$item){
/*
		$item = array('description','key','hostid','delay','history','status','type',
		'snmp_community','snmp_oid','value_type','trapper_hosts','snmp_port','units','multiplier','delta',
		'snmpv3_securityname','snmpv3_securitylevel','snmpv3_authpassphrase','snmpv3_privpassphrase',
		'formula','trends','logtimefmt','valuemapid','delay_flex','params','ipmi_sensor','applications','templateid');
*/
		$upd_app = ((isset($item['applications'])) && !is_null($item['applications']));
		$item_in_params = $item;

		$item_data = get_item_by_itemid_limited($itemid);
		$item_data['applications'] = get_applications_by_itemid($itemid);

		if(!check_db_fields($item_data, $item)){
			error(S_INCORRECT_ARGUMENTS_PASSED_TO_FUNCTION.SPACE.'[update_item]');
			return false;
		}

		$host = get_host_by_hostid($item['hostid']);

		if(($i = array_search(0,$item['applications'])) !== FALSE) unset($item['applications'][$i]);

		if( !preg_match('/^'.ZBX_PREG_ITEM_KEY_FORMAT.'$/u', $item['key_']) ){
			error(S_INCORRECT_KEY_FORMAT.SPACE."'key_name[param1,param2,...]'");
			return false;
		}

		if(($item['type'] == ITEM_TYPE_DB_MONITOR && $item['key_'] == 'db.odbc.select[<unique short description>]') ||
		   ($item['type'] == ITEM_TYPE_SSH && $item['key_'] == 'ssh.run[<unique short description>,<ip>,<port>,<encoding>]') ||
		   ($item['type'] == ITEM_TYPE_TELNET && $item['key_'] == 'telnet.run[<unique short description>,<ip>,<port>,<encoding>]')) {
		   	error(S_ITEMS_CHECK_KEY_DEFAULT_EXAMPLE_PASSED);
			return false;
		}

		$res = calculate_item_nextcheck(0, $item['type'], $item['delay'], $item['delay_flex'], time());
		if ($res['delay'] == SEC_PER_YEAR && $item['type'] != ITEM_TYPE_ZABBIX_ACTIVE && $item['type'] != ITEM_TYPE_TRAPPER){
			error(S_ITEM_WILL_NOT_BE_REFRESHED_PLEASE_ENTER_A_CORRECT_UPDATE_INTERVAL);
			return FALSE;
		}

		if(($item['snmp_port'] < 1 || $item['snmp_port'] > 65535) && in_array($item['type'], array(ITEM_TYPE_SNMPV1,ITEM_TYPE_SNMPV2C,ITEM_TYPE_SNMPV3))){
			error(S_INVALID_SNMP_PORT);
			return FALSE;
		}

		if($item['value_type'] == ITEM_VALUE_TYPE_STR){
			$item['delta']=0;
		}

		if(preg_match('/^(log|logrt|eventlog)\[/', $item['key_']) && ($item['value_type'] != ITEM_VALUE_TYPE_LOG)){
			error(S_TYPE_INFORMATION_BUST_LOG_FOR_LOG_KEY);
			return FALSE;
		}

		if($item['value_type'] != ITEM_VALUE_TYPE_UINT64) {
			$item['data_type'] = 0;
		}

		$sql = 'SELECT itemid, hostid, templateid '.
				' FROM items '.
				' WHERE hostid='.$item['hostid'].
					' AND itemid<>'.$itemid.
					' AND key_='.zbx_dbstr($item['key_']);
		$db_item = DBfetch(DBselect($sql));
		if($db_item && (($db_item['templateid'] != 0) || ($item['templateid'] == 0))){
			error(S_AN_ITEM_WITH_THE_KEY.SPACE.'['.$item['key_'].']'.SPACE.S_ALREADY_EXISTS_FOR_HOST_SMALL.SPACE.'['.$host['host'].'].'.SPACE.S_THE_KEY_MUST_BE_UNIQUE);
			return FALSE;
		}
// first update child items
		$db_tmp_items = DBselect('SELECT itemid, hostid FROM items WHERE templateid='.$itemid);
		while($db_tmp_item = DBfetch($db_tmp_items)){
			$child_item_params = $item_in_params;

			$child_item_params['hostid'] = $db_tmp_item['hostid'];
			$child_item_params['templateid'] = $itemid;

			if($upd_app){
				$child_item_params['applications'] = get_same_applications_for_host($item['applications'], $db_tmp_item['hostid']);
			}
			else{
				$child_item_params['applications'] = null;
			}

			if(!check_db_fields($db_tmp_item, $child_item_params)){
				error(S_INCORRECT_ARGUMENTS_PASSED_TO_FUNCTION.SPACE.'[update_item]');
				return false;
			}

			$result = update_item($db_tmp_item['itemid'], $child_item_params);		// recursion!!!

			if(!$result)
				return $result;
		}

		if($db_item && $item['templateid'] != 0){
			$result = delete_item($db_item['itemid']);
			if(!$result) {
				error(S_CANNOT_UPDATE_ITEM.SPACE."'".$host["host"].':'.$item['key_']."'");
				return FALSE;
			}
		}

		//validating item key
		list($item_key_is_valid, $check_result) = check_item_key($item['key_']);
		if(!$item_key_is_valid){
			error(S_ERROR_IN_ITEM_KEY.SPACE.$check_result);
			return false;
		}

		$item_old = get_item_by_itemid($itemid);
		DBexecute('UPDATE items SET lastlogsize=0, mtime=0 WHERE itemid='.$itemid.' AND key_<>'.zbx_dbstr($item['key_']));


		if($upd_app){
			$result = DBexecute('DELETE FROM items_applications WHERE itemid='.$itemid);
			foreach($item['applications'] as $appid){
				$itemappid=get_dbid('items_applications','itemappid');
				DBexecute('INSERT INTO items_applications (itemappid,itemid,applicationid) VALUES ('.$itemappid.','.$itemid.','.$appid.')');
			}
		}

		if($item['status'] == ITEM_STATUS_ACTIVE)
			DBexecute("UPDATE items SET error='' WHERE itemid=".$itemid.' and status<>'.$item['status']);

		$result=DBexecute(
			'UPDATE items '.
			' SET description='.zbx_dbstr($item['description']).','.
				'key_='.zbx_dbstr($item['key_']).','.
				'hostid='.$item['hostid'].','.
				'delay='.$item['delay'].','.
				'history='.$item['history'].','.
				'type='.$item['type'].','.
				'snmp_community='.zbx_dbstr($item['snmp_community']).','.
				'snmp_oid='.zbx_dbstr($item['snmp_oid']).','.
				'value_type='.$item['value_type'].','.
				'data_type='.$item['data_type'].','.
				'trapper_hosts='.zbx_dbstr($item['trapper_hosts']).','.
				'snmp_port='.$item['snmp_port'].','.
				'units='.zbx_dbstr($item['units']).','.
				'multiplier='.$item['multiplier'].','.
				'delta='.$item['delta'].','.
				'snmpv3_securityname='.zbx_dbstr($item['snmpv3_securityname']).','.
				'snmpv3_securitylevel='.$item['snmpv3_securitylevel'].','.
				'snmpv3_authpassphrase='.zbx_dbstr($item['snmpv3_authpassphrase']).','.
				'snmpv3_privpassphrase='.zbx_dbstr($item['snmpv3_privpassphrase']).','.
				'formula='.zbx_dbstr($item['formula']).','.
				'trends='.$item['trends'].','.
				'logtimefmt='.zbx_dbstr($item['logtimefmt']).','.
				'valuemapid='.$item['valuemapid'].','.
				'delay_flex='.zbx_dbstr($item['delay_flex']).','.
				'params='.zbx_dbstr($item['params']).','.
				'ipmi_sensor='.zbx_dbstr($item['ipmi_sensor']).','.
				'templateid='.$item['templateid'].','.
				'authtype='.$item['authtype'].','.
				'username='.zbx_dbstr($item['username']).','.
				'password='.zbx_dbstr($item['password']).','.
				'publickey='.zbx_dbstr($item['publickey']).','.
				'privatekey='.zbx_dbstr($item['privatekey']).
			' WHERE itemid='.$itemid);

		if ($result){
			$item_new = get_item_by_itemid($itemid);
			add_audit_ext(AUDIT_ACTION_UPDATE, AUDIT_RESOURCE_ITEM, $itemid, $host['host'].':'.$item_old['description'], 'items', $item_old, $item_new);
		}

		update_item_status($itemid, $item['status']);

		if($result){
			info(S_ITEM.SPACE."'".$host['host'].':'.$item['key_']."'".SPACE.S_UPDATED_SMALL);
		}

	return $result;
	}

/*
 * Function: smart_update_item
 *
 * Description:
 *     Update specified fields of item
 *
 * Author:
 *     Aly
 *
 * Comments:
 *
 */
	function smart_update_item($itemid, $item=array()){
		$item_data = get_item_by_itemid_limited($itemid);

		$restore_rules= array(
					'description'		=> array(),
					'key_'			=> array(),
					'hostid'		=> array(),
					'delay'			=> array('template' => 1),
					'history'		=> array('template' => 1 , 'httptest' => 1),
					'status'		=> array('template' => 1 , 'httptest' => 1),
					'type'			=> array(),
					'snmp_community'	=> array('template' => 1),
					'snmp_oid'		=> array(),
					'snmp_port'		=> array('template' => 1),
					'snmpv3_securityname'	=> array('template' => 1),
					'snmpv3_securitylevel'	=> array('template' => 1),
					'snmpv3_authpassphrase'	=> array('template' => 1),
					'snmpv3_privpassphrase'	=> array('template' => 1),
					'value_type'		=> array(),
					'data_type'		=> array(),
					'trapper_hosts'		=> array('template' =>1 ),
					'units'			=> array(),
					'multiplier'		=> array(),
					'delta'			=> array('template' => 1 , 'httptest' => 1),
					'formula'		=> array(),
					'trends'		=> array('template' => 1 , 'httptest' => 1),
					'logtimefmt'		=> array(),
					'valuemapid'		=> array('httptest' => 1),
					'authtype'		=> array('template' => 1),
					'username'		=> array('template' => 1),
					'password'		=> array('template' => 1),
					'publickey'		=> array('template' => 1),
					'privatekey'		=> array('template' => 1),
					'params'		=> array('template' => 1),
					'delay_flex'		=> array('template' => 1),
					'ipmi_sensor'		=> array());

		foreach($restore_rules as $var_name => $info){
/*			if(($item_data['type'] == ITEM_TYPE_HTTPTEST) && !isset($info['httptest'])){
				$item[$var_name] = $item_data[$var_name];
			}*/

			if(!isset($info['template']) && (0 != $item_data['templateid'])){
				$item[$var_name] = $item_data[$var_name];
			}

			if(!array_key_exists($var_name,$item)){
				$item[$var_name] = $item_data[$var_name];
			}
		}

/*		if($item_data['type'] == ITEM_TYPE_HTTPTEST)
			$item['applications'] = get_applications_by_itemid($itemid);*/

	return update_item($itemid,$item);
	}

	/*
	 * Function: delete_template_items
	 *
	 * Description:
	 *     Delete items from host by templateid
	 *
	 * Author:
	 *     Eugene Grigorjev (eugene.grigorjev@zabbix.com)
	 *
	 * Comments: !!! Don't forget sync code with C !!!
	 *
	 */
	function delete_template_items($hostid, $templateids = null, $unlink_mode = false){
		zbx_value2array($templateids);

		$db_items = get_items_by_hostid($hostid);
		while($db_item = DBfetch($db_items)){
			if($db_item["templateid"] == 0)
				continue;

			if( !is_null($templateids)){
				$db_tmp_item = get_item_by_itemid($db_item["templateid"]);

				if(!uint_in_array($db_tmp_item["hostid"], $templateids)) continue;
			}

			if($unlink_mode){
				if(DBexecute('UPDATE items SET templateid=0 WHERE itemid='.$db_item["itemid"])){
					info(sprintf(S_ITEM_UNLINKED, $db_item["key_"]));
				}
			}
			else{
				delete_item($db_item["itemid"]);
			}
		}
	}

	/*
	 * Function: copy_item_to_host
	 *
	 * Description:
	 *     Copy specified item to the host
	 *
	 * Author:
	 *     Eugene Grigorjev (eugene.grigorjev@zabbix.com)
	 *
	 * Comments:
	 *
	 */
	function copy_item_to_host($itemid, $hostid, $copy_mode = false){
		$db_tmp_item = get_item_by_itemid_limited($itemid);

		$db_tmp_item['hostid'] = $hostid;
		$db_tmp_item['applications'] = get_same_applications_for_host(get_applications_by_itemid($db_tmp_item['itemid']),$hostid);
		$db_tmp_item['templateid'] = $copy_mode?0:$db_tmp_item['itemid'];

		$result = add_item($db_tmp_item);
	return $result;
	}

	/*
	 * Function: copy_template_items
	 *
	 * Description:
	 *     Copy items from template to the host
	 *
	 * Author:
	 *     Eugene Grigorjev (eugene.grigorjev@zabbix.com)
	 *
	 * Comments: !!! Don't forget sync code with C !!!
	 *
	 */
	function copy_template_items($hostid, $templateid = null, $copy_mode = false){
		if($templateid == null){
			$templateid = array_keys(get_templates_by_hostid($hostid));
		}

		if(is_array($templateid)){
			foreach($templateid as $id)
				copy_template_items($hostid, $id, $copy_mode); // attention recursion
			return;
		}

		$db_tmp_items = get_items_by_hostid($templateid);
		while($db_tmp_item = DBfetch($db_tmp_items)){
			$db_tmp_item['hostid'] = $hostid;
			$db_tmp_item['applications'] = get_same_applications_for_host(get_applications_by_itemid($db_tmp_item['itemid']),$hostid);
			$db_tmp_item['templateid'] = $copy_mode?0:$db_tmp_item['itemid'];

			add_item($db_tmp_item);
		}
	}

// Activate Item

	function activate_item($itemids){
		zbx_value2array($itemids);

// first update status for child items
		$chd_items = array();
		$db_tmp_items = DBselect('SELECT itemid, hostid FROM items WHERE '.DBcondition('templateid',$itemids));
		while($db_tmp_item = DBfetch($db_tmp_items)){
			$chd_items[$db_tmp_item['itemid']] = $db_tmp_item['itemid'];
		}
		if(!empty($chd_items)){
			activate_item($chd_items);  // Recursion !!!
		}

	return update_item_status($itemids, ITEM_STATUS_ACTIVE);
	}

// Disable Item
	function disable_item($itemids){
		zbx_value2array($itemids);

// first update status for child items
		$chd_items = array();
		$db_tmp_items = DBselect('SELECT itemid, hostid FROM items WHERE '.DBcondition('templateid',$itemids));
		while($db_tmp_item = DBfetch($db_tmp_items)){
			$chd_items[$db_tmp_item['itemid']] = $db_tmp_item['itemid'];
		}
		if(!empty($chd_items)){
			disable_item($chd_items);  // Recursion !!!
		}

		return update_item_status($itemids, ITEM_STATUS_DISABLED);
	}

	function get_items_by_hostid($hostids){
		zbx_value2array($hostids);
	return DBselect('SELECT * FROM items WHERE '.DBcondition('hostid',$hostids));
	}

	function get_item_by_key($key,$host=''){
		$item = false;

		$sql_from = '';
		$sql_where = '';
		if(!empty($host)){
			$sql_from = ',hosts h ';
			$sql_where = ' AND h.host='.zbx_dbstr($host).' AND i.hostid=h.hostid ';
		}
		$sql = 'SELECT DISTINCT i.* '.
			' FROM items i '.$sql_from.
			' WHERE i.key_='.zbx_dbstr($key).
				$sql_where;
		if($item = DBfetch(DBselect($sql))){
			$item = $item;
		}
	return $item;
	}

	function get_item_by_itemid($itemid){
		$row = DBfetch(DBselect('select * from items where itemid='.$itemid));
		if($row){
			return	$row;
		}
		error(S_NO_ITEM_WITH.SPACE.'itemid=['.$itemid.']');
	return	FALSE;
	}

	function get_item_by_itemid_limited($itemid){
		$sql = 'SELECT itemid,description,key_,hostid,delay,history,status,type,'.
					'snmp_community,snmp_oid,value_type,data_type,trapper_hosts,snmp_port,units,multiplier,delta,'.
					'snmpv3_securityname,snmpv3_securitylevel,snmpv3_authpassphrase,snmpv3_privpassphrase,'.
					'formula,trends,logtimefmt,valuemapid,delay_flex,params,ipmi_sensor,templateid,'.
					'authtype,username,password,publickey,privatekey '.
			' FROM items '.
			' WHERE itemid='.$itemid;
		$row = DBfetch(DBselect($sql));
		if($row){
			return	$row;
		}
		error(S_NO_ITEM_WITH.SPACE.'itemid=['.$itemid.']');
	return	FALSE;
	}

/*
 * Function: get_same_items_for_host
 *
 * Description:
 *		Replace items for specified host
 *
 * Author:
 *		Aly
 *
 * Comments:
 *		$error= true : rise Error if item doesn't exist (error generated), false: special processing (NO error generated)
 */
	function get_same_item_for_host($item,$dest_hostids){
		$return_array = is_array($dest_hostids);
		zbx_value2array($dest_hostids);

		if(!is_array($item)){
			$itemid = $item;
		}
		else if(isset($item['itemid'])){
			$itemid = $item['itemid'];
		}

		$same_item = null;
		$same_items = array();
		if(isset($itemid)){
			$sql = 'SELECT src.* '.
							' FROM items src, items dest '.
							' WHERE dest.itemid='.$itemid.
								' AND src.key_=dest.key_ '.
								' AND '.DBcondition('src.hostid',$dest_hostids);

			$res = DBselect($sql);
			while($db_item = DBfetch($res)){
				if(is_array($item)){
					$same_item = $db_item;
					$same_items[$db_item['itemid']] = $db_item;
				}
				else{
					$same_item = $db_item['itemid'];
					$same_items[$db_item['itemid']] = $db_item['itemid'];
				}
			}

			if($return_array)
				return $same_items;
			else
				return $same_item;
		}
	return false;
	}

	/******************************************************************************
	 *                                                                            *
	 * Comments: !!! Don't forget sync code with C !!!                            *
	 *                                                                            *
	 ******************************************************************************/
	function delete_item($itemids){
		zbx_value2array($itemids);
		if(empty($itemids)) return true;

// Get items INFO before delete them!
		$items = array();
		$item_res = DBselect('SELECT itemid, description, key_ FROM items WHERE '.DBcondition('itemid',$itemids));
		while($item_rows = DBfetch($item_res)){
			$items[$item_rows['itemid']] = $item_rows;
		}
// --
		$hosts = array();
		$hosts = get_host_by_itemid($itemids);
// first delete child items
		$del_cld_items = array();
		$db_items = DBselect('SELECT itemid FROM items WHERE '.DBcondition('templateid',$itemids));
		while($db_item = DBfetch($db_items)){		// recursion !!!!
			$del_cld_items[$db_item['itemid']] = $db_item['itemid'];
		}
		if(!empty($del_cld_items)){
			$result = delete_item($del_cld_items);
			if(!$result)	return	$result;
		}
//--
// triggers
		$result = delete_triggers_by_itemid($itemids);
		if(!$result)	return	$result;
//--

// delete graphs
		$del_graphs = array();
		$sql = 'SELECT gi.graphid'.
			' FROM graphs_items gi'.
			' WHERE '.DBcondition('gi.itemid', $itemids).
				' AND NOT EXISTS ('.
					' SELECT gii.gitemid'.
					' FROM graphs_items gii'.
					' WHERE gii.graphid=gi.graphid'.
						' AND '.DBcondition('gii.itemid', $itemids, true, false).
					' )';
		$db_graphs = DBselect($sql);
		while($db_graph = DBfetch($db_graphs)){
			$del_graphs[$db_graph['graphid']] = $db_graph['graphid'];
		}

		if(!empty($del_graphs)){
			$result = delete_graph($del_graphs);
			if(!$result)	return	$result;
		}

		DBexecute('DELETE FROM graphs_items WHERE '.DBcondition('itemid', $itemids));
//--

		$result = delete_history_by_itemid($itemids, 1 /* use housekeeper */);
		if(!$result)	return	$result;

		$temp_arr = array(SCREEN_RESOURCE_SIMPLE_GRAPH,SCREEN_RESOURCE_PLAIN_TEXT);

		DBexecute('DELETE FROM screens_items WHERE '.DBcondition('resourceid',$itemids).' AND '.DBcondition('resourcetype', $temp_arr));
		DBexecute('DELETE FROM items_applications WHERE '.DBcondition('itemid',$itemids));
		DBexecute("DELETE FROM profiles WHERE idx='web.favorite.graphids' AND source='itemid' AND ".DBcondition('value_id',$itemids));

		foreach ($itemids as $id) {	/* The section should be improved */
			$item_old = get_item_by_itemid($id);
			$result = DBexecute('DELETE FROM items WHERE itemid='.$id);
			if ($result)
				add_audit_ext(AUDIT_ACTION_DELETE, AUDIT_RESOURCE_ITEM, $id, $item_old['description'], 'items', NULL, NULL);
			else
				break;
		}

/*		$result = DBexecute('DELETE FROM items WHERE '.DBcondition('itemid',$itemids));*/
		if($result){
			foreach($items as $itemid => $item){
				info(S_ITEM.SPACE."'".$hosts[$itemid]['host'].':'.$item['key_']."'".SPACE.S_DELETED_SMALL);
			}
		}
	return $result;
	}

	/*
	 * Function: get_n_param
	 *
	 * Description:
	 *     Return key parameter by index
	 *
	 * Author:
	 *     Eugene Grigorjev (eugene.grigorjev@zabbix.com)
	 *
	 * Comments: indexes between 1-x
	 *
	 */
	function get_n_param($key, $num){
		$param="";

        $num--;
		if(preg_match('/^'.ZBX_PREG_ITEM_KEY_FORMAT.'$/', $key, $arr)){
			if(!isset($arr[ZBX_KEY_PARAM_ID])){
				$arr[ZBX_KEY_PARAM_ID] = false;
			}

			$params = zbx_get_params($arr[ZBX_KEY_PARAM_ID]);

			if(isset($params[$num])){
				$param = $params[$num];
			}
		}

		return $param;
	}

	function expand_item_key_by_data($item){
		$key =& $item['key_'];
		$macStack = array();

		$macroses = array('{HOSTNAME}', '{IPADDRESS}', '{HOST.DNS}', '{HOST.CONN}');

		foreach($macroses as $macro){
			$pos = 0;
			while($pos = zbx_strpos($key, $macro, $pos)){
				$pos++;
				$macStack[] = $macro;
			}
		}

		if(!empty($macStack)){
			$host = get_host_by_itemid($item['itemid']);

			foreach($macStack as $macro){
				switch($macro){
					case '{HOSTNAME}':
						$key = str_replace('{HOSTNAME}', $host['host'], $key);
					break;
					case '{IPADDRESS}':
						$key = str_replace('{IPADDRESS}', $host['ip'], $key);
					break;
					case '{HOST.DNS}':
						$key = str_replace('{HOST.DNS}', $host['dns'], $key);
					break;
					case '{HOST.CONN}':
						$key = str_replace('{HOST.CONN}', $host['useip'] ? $host['ip'] : $host['dns'], $key);
					break;
				}
			}
		}

		CUserMacro::resolveItem($item);

		return $item['key_'];
	}

	function item_description($item){
		$descr = $item['description'];
		$key = expand_item_key_by_data($item);

		/**
		 * Regular string functions used below are changed to zbx_*
		 * wrappers to allow users to name steps in non-ascii chars.
		 * Also $str[$i] calls were replased by zbx_substr($str, $i, 1)
		 * @see ZBX-2349
		 * @author Konstantin Buravcov
		 */
        for($i=9;$i>0;$i--){
            $descr = str_replace("$$i",get_n_param($key,$i),$descr);
        }

        if($res = preg_match_all('/'.ZBX_PREG_EXPRESSION_USER_MACROS.'/', $descr, $arr)){
            $macros = CuserMacro::getMacros($arr[1], array('itemid' => $item['itemid']));

            $search = array_keys($macros);
            $values = array_values($macros);
			$descr = str_replace($search, $values, $descr);
        }

	return $descr;
	}

	function get_realhost_by_itemid($itemid){
		$item = get_item_by_itemid($itemid);
		if($item['templateid'] <> 0)
			return get_realhost_by_itemid($item['templateid']);

	return get_host_by_itemid($itemid);
	}

/*
 * Function: get_items_data_overview
 *
 * Description:
 *     Retrieve overview table object for items
 *
 * Author:
 *     Eugene Grigorjev (eugene.grigorjev@zabbix.com)
 *
 * Comments:
 *
 */
	function get_items_data_overview($hostids,$view_style=null){
		global $USER_DETAILS;

		if(is_null($view_style)) $view_style = CProfile::get('web.overview.view.style',STYLE_TOP);

		$table = new CTableInfo(S_NO_ITEMS_DEFINED);

// COpt::profiling_start('prepare_data');
		$result = DBselect('SELECT DISTINCT h.hostid, h.host,i.itemid, i.key_, i.value_type, i.lastvalue, i.units, '.
				' i.description, t.priority, i.valuemapid, t.value as tr_value, t.triggerid '.
			' FROM hosts h, items i '.
				' LEFT JOIN functions f on f.itemid=i.itemid '.
				' LEFT JOIN triggers t on t.triggerid=f.triggerid and t.status='.TRIGGER_STATUS_ENABLED.
			' WHERE '.DBcondition('h.hostid',$hostids).
				' AND h.status='.HOST_STATUS_MONITORED.
				' AND h.hostid=i.hostid '.
				' AND i.status='.ITEM_STATUS_ACTIVE.
			' ORDER BY i.description,i.itemid');

		unset($items);
		unset($hosts);
// get rid of warnings about $triggers undefined
		$items = array();
		while($row = DBfetch($result)){
			$descr = item_description($row);
			$row['host'] = get_node_name_by_elid($row['hostid'], null, ': ').$row['host'];
			$hosts[zbx_strtolower($row['host'])] = $row['host'];

// A little tricky check for attempt to overwrite active trigger (value=1) with
// inactive or active trigger with lower priority.
			if (!isset($items[$descr][$row['host']]) ||
				(
					(($items[$descr][$row['host']]['tr_value'] == TRIGGER_VALUE_FALSE) && ($row['tr_value'] == TRIGGER_VALUE_TRUE)) ||
					(
						(($items[$descr][$row['host']]['tr_value'] == TRIGGER_VALUE_FALSE) || ($row['tr_value'] == TRIGGER_VALUE_TRUE)) &&
						($row['priority'] > $items[$descr][$row['host']]['severity'])
					)
				)
			)
			{
				$items[$descr][$row['host']] = array(
					'itemid'	=> $row['itemid'],
					'value_type'=> $row['value_type'],
					'lastvalue'	=> $row['lastvalue'],
					'units'		=> $row['units'],
					'description'=> $row['description'],
					'valuemapid' => $row['valuemapid'],
					'severity'	=> $row['priority'],
					'tr_value'	=> $row['tr_value'],
					'triggerid'	=> $row['triggerid']
				);
			}
		}

		if(!isset($hosts)){
			return $table;
		}

		ksort($hosts, SORT_STRING);
// COpt::profiling_stop('prepare_data');
// COpt::profiling_start('prepare_table');

		$css = getUserTheme($USER_DETAILS);
		$vTextColor = ($css == 'css_od.css')?'&color=white':'';
		if($view_style == STYLE_TOP){
			$header=array(new CCol(S_ITEMS,'center'));
			foreach($hosts as $hostname){
				$header = array_merge($header,array(new CImg('vtext.php?text='.$hostname.$vTextColor)));
			}

			$table->SetHeader($header,'vertical_header');
			$curr_rime = time();

			foreach($items as $descr => $ithosts){
				$table_row = array(nbsp($descr));
				foreach($hosts as $hostname){
					$table_row = get_item_data_overview_cells($table_row,$ithosts,$hostname);
				}
				$table->AddRow($table_row);
			}
		}
		else{
			$header=array(new CCol(S_HOSTS,'center'));
			foreach($items as $descr => $ithosts){
				$header = array_merge($header,array(new CImg('vtext.php?text='.$descr.$vTextColor)));
			}

			$table->SetHeader($header,'vertical_header');
			$curr_rime = time();

			foreach($hosts as $hostname){
				$table_row = array(nbsp($hostname));
				foreach($items as $descr => $ithosts){
					$table_row = get_item_data_overview_cells($table_row,$ithosts,$hostname);
				}
				$table->AddRow($table_row);
			}
		}
// COpt::profiling_stop('prepare_table');

	return $table;
	}

	function get_item_data_overview_cells(&$table_row,&$ithosts,$hostname){
		$css_class = '';
		unset($it_ov_menu);

		$value = '-';
		$ack = null;
		if(isset($ithosts[$hostname])){
			if($ithosts[$hostname]['tr_value'] == TRIGGER_VALUE_TRUE){
				$css_class = get_severity_style($ithosts[$hostname]['severity']);
				$ack = get_last_event_by_triggerid($ithosts[$hostname]['triggerid']);
				if ( 1 == $ack['acknowledged'] )
					$ack = array(SPACE, new CImg('images/general/tick.png','ack'));
				else
					$ack = null;
			}

			$value = format_lastvalue($ithosts[$hostname]);

			$it_ov_menu = array(
				array(S_VALUES,	null, null,
					array('outer'=> array('pum_oheader'), 'inner'=>array('pum_iheader'))),
				array(S_500_LATEST_VALUES, 'history.php?action=showlatest&itemid='.$ithosts[$hostname]['itemid'],
					array('tw'=>'_blank'))
				);

			switch($ithosts[$hostname]['value_type']){
				case ITEM_VALUE_TYPE_UINT64:
				case ITEM_VALUE_TYPE_FLOAT:
					$it_ov_menu = array_merge(array(
						/* name, url, (target [tw], statusbar [sb]), css, submenu */
						array(S_GRAPHS, null,  null,
							array('outer'=> array('pum_oheader'), 'inner'=>array('pum_iheader'))
							),
						array(S_LAST_HOUR_GRAPH, 'history.php?period=3600&action=showgraph&itemid='.
							$ithosts[$hostname]['itemid'], array('tw'=>'_blank')),
						array(S_LAST_WEEK_GRAPH, 'history.php?period=604800&action=showgraph&itemid='.
							$ithosts[$hostname]['itemid'], array('tw'=>'_blank')),
						array(S_LAST_MONTH_GRAPH, 'history.php?period=2678400&action=showgraph&itemid='.
							$ithosts[$hostname]['itemid'], array('tw'=>'_blank'))
						), $it_ov_menu);
					break;
				default:
					break;
			}
		}

		if($value != '-')	$value = new CSpan($value,'link');
		$value_col = new CCol(array($value,$ack),$css_class);

		if(isset($it_ov_menu)){
			$it_ov_menu  = new CPUMenu($it_ov_menu,170);
			$value_col->onClick($it_ov_menu->getOnActionJS());
			unset($it_ov_menu);
		}

		array_push($table_row,$value_col);
	return $table_row;
	}

	/******************************************************************************
	 *                                                                            *
	 * Comments: !!! Don't forget sync code with C !!!                            *
	 *                                                                            *
	 ******************************************************************************/
	function get_same_applications_for_host($applications, $hostid){
		$child_applications = array();

		foreach($applications as $appid){
			$db_apps = DBselect("select a1.applicationid from applications a1, applications a2".
					" where a1.name=a2.name and a1.hostid=".$hostid." and a2.applicationid=".$appid);
			$db_app = DBfetch($db_apps);
			if(!$db_app) continue;
			array_push($child_applications,$db_app["applicationid"]);
		}
	return $child_applications;
	}

	/******************************************************************************
	 *                                                                            *
	 * Comments: !!! Don't forget sync code with C !!!                            *
	 *                                                                            *
	 ******************************************************************************/
	function get_applications_by_itemid($itemids, $field='applicationid'){
		zbx_value2array($itemids);

		$result = array();

		$db_applications = DBselect('SELECT DISTINCT app.'.$field.' as result '.
										' FROM applications app, items_applications ia '.
										' WHERE app.applicationid=ia.applicationid '.
											' AND '.DBcondition('ia.itemid',$itemids));
		while($db_application = DBfetch($db_applications))
			array_push($result,$db_application['result']);

	return $result;
	}

	/******************************************************************************
	 *                                                                            *
	 * Comments: !!! Don't forget sync code with C !!!                            *
	 *                                                                            *
	 ******************************************************************************/
	function delete_history_by_itemid($itemids, $use_housekeeper=0){
		zbx_value2array($itemids);

		$result = delete_trends_by_itemid($itemids,$use_housekeeper);
		if(!$result)	return $result;

		if($use_housekeeper){
			foreach($itemids as $id => $itemid){
				$housekeeperid = get_dbid('housekeeper','housekeeperid');
				$sql = 'INSERT INTO housekeeper (housekeeperid,tablename,field,value)'.
							" VALUES ($housekeeperid,'history_text','itemid',$itemid)";
				DBexecute($sql);

				$housekeeperid = get_dbid('housekeeper','housekeeperid');
				$sql = 'INSERT INTO housekeeper (housekeeperid,tablename,field,value)'.
							" VALUES ($housekeeperid,'history_log','itemid',$itemid)";
				DBexecute($sql);

				$housekeeperid = get_dbid('housekeeper','housekeeperid');
				$sql = 'INSERT INTO housekeeper (housekeeperid,tablename,field,value)'.
							" VALUES ($housekeeperid,'history_uint','itemid',$itemid)";
				DBexecute($sql);

				$housekeeperid = get_dbid('housekeeper','housekeeperid');
				$sql = 'INSERT INTO housekeeper (housekeeperid,tablename,field,value)'.
							" VALUES ($housekeeperid,'history_str','itemid',$itemid)";
				DBexecute($sql);

				$housekeeperid = get_dbid('housekeeper','housekeeperid');
				$sql = 'INSERT INTO housekeeper (housekeeperid,tablename,field,value)'.
							" VALUES ($housekeeperid,'history','itemid',$itemid)";
				DBexecute($sql);
			}
			return TRUE;
		}

		DBexecute('DELETE FROM history_text WHERE '.DBcondition('itemid',$itemids));
		DBexecute('DELETE FROM history_log WHERE '.DBcondition('itemid',$itemids));
		DBexecute('DELETE FROM history_uint WHERE '.DBcondition('itemid',$itemids));
		DBexecute('DELETE FROM history_str WHERE '.DBcondition('itemid',$itemids));
		DBexecute('DELETE FROM history WHERE '.DBcondition('itemid',$itemids));
	return TRUE;
	}

/******************************************************************************
 *                                                                            *
 * Comments: !!! Don't forget sync code with C !!!                            *
 *                                                                            *
 ******************************************************************************/
	function delete_trends_by_itemid($itemids, $use_housekeeper=0){
		zbx_value2array($itemids);

		if($use_housekeeper){
			foreach($itemids as $id => $itemid){
				$housekeeperid = get_dbid('housekeeper','housekeeperid');
				DBexecute('INSERT INTO housekeeper (housekeeperid,tablename,field,value)'.
					" VALUES ($housekeeperid, 'trends','itemid',$itemid)");
				$housekeeperid = get_dbid('housekeeper','housekeeperid');
				DBexecute('INSERT INTO housekeeper (housekeeperid,tablename,field,value)'.
					" VALUES ($housekeeperid, 'trends_uint','itemid',$itemid)");
			}
			return TRUE;
		}
	return	DBexecute('DELETE FROM trends WHERE '.DBcondition('itemid',$itemids));
	}

	function format_lastvalue($db_item){
		if(isset($db_item["lastvalue"])){
			if($db_item["value_type"] == ITEM_VALUE_TYPE_FLOAT){
				$lastvalue=convert_units($db_item["lastvalue"],$db_item["units"]);
			}
			else if($db_item["value_type"] == ITEM_VALUE_TYPE_UINT64){
				$lastvalue=convert_units($db_item["lastvalue"],$db_item["units"]);
			}
			else if($db_item["value_type"] == ITEM_VALUE_TYPE_STR ||
					$db_item["value_type"] == ITEM_VALUE_TYPE_TEXT ||
					$db_item["value_type"] == ITEM_VALUE_TYPE_LOG){
				$lastvalue=$db_item["lastvalue"];
				if(zbx_strlen($lastvalue) > 20)
					$lastvalue = zbx_substr($lastvalue,0,20)." ...";
				$lastvalue = nbsp(htmlspecialchars($lastvalue));
			}
			else{
				$lastvalue=S_UNKNOWN_VALUE_TYPE;
			}
			if($db_item["valuemapid"] > 0);
				$lastvalue = replace_value_by_map($lastvalue, $db_item["valuemapid"]);

		}
		else{
			$lastvalue = "-";
		}
	return $lastvalue;
	}

/*
 * Function: item_get_history
 *
 * Description:
 *     Get value from history
 *
 * Parameters:
 *     itemid - item ID
 *     last  - 0 - last value (clock is used), 1 - last value
 *
 * Author:
 *     Alexei Vladishev
 *
 * Comments:
 *
 */
	function item_get_history($db_item, $last = 1, $clock = 0){
		$value = NULL;

		switch($db_item["value_type"]){
			case ITEM_VALUE_TYPE_FLOAT:
				$table = "history";
				break;
			case ITEM_VALUE_TYPE_UINT64:
				$table = "history_uint";
				break;
			case ITEM_VALUE_TYPE_TEXT:
				$table = "history_text";
				break;
			case ITEM_VALUE_TYPE_STR:
				$table = "history_str";
				break;
			case ITEM_VALUE_TYPE_LOG:
			default:
				$table = "history_log";
				break;
		}

		if($last == 0){
			$sql = 'select value from '.$table.' where itemid='.$db_item['itemid'].' and clock<='.$clock.
					' order by itemid,clock desc';
			$row = DBfetch(DBselect($sql, 1));
			if($row)
				$value = $row["value"];
		}
		else{
			$sql = "select max(clock) as clock from $table where itemid=".$db_item["itemid"];
			$row = DBfetch(DBselect($sql));
			if($row && !is_null($row["clock"])){
				$clock = $row["clock"];
				$sql = "select value from $table where itemid=".$db_item["itemid"]." and clock=$clock";
				$row = DBfetch(DBselect($sql, 1));
				if($row)
					$value = $row["value"];
			}
		}
	return $value;
	}

/*
 * Function: check_time_period
 *
 * Purpose: check if current time is within given period
 *
 * Parameters: period - [IN] time period in format [wd[-wd2],hh:mm-hh:mm]
 *             now    - [IN] timestamp for comparison
 *
 * Return value: 0 - out of period, 1 - within the period
 *
 * Author: Alexander Vladishev
 *
 * Comments:
 *        !!! Don't forget sync code with C !!!
 */
	function check_time_period($period, $now){
		$tm = localtime($now, true);
		$day = (0 == $tm['tm_wday']) ? 7 : $tm['tm_wday'];
		$sec = 3600 * $tm['tm_hour'] + 60 * $tm['tm_min'] + $tm['tm_sec'];

		$flag = (6 == sscanf($period, "%d-%d,%d:%d-%d:%d", $d1, $d2, $h1, $m1, $h2, $m2));

		if(!$flag){
			$flag = (5 == sscanf($period, "%d,%d:%d-%d:%d", $d1, $h1, $m1, $h2, $m2));
			$d2 = $d1;
		}

		if(!$flag){
			/* Delay period format is wrong - skip */;
		}
		else{
			if(($day >= $d1) &&
				($day <= $d2) &&
				($sec >= (3600*$h1+60*$m1)) &&
				($sec <= (3600*$h2+60*$m2)))
			{
				return true;
			}
		}

	return false;
	}

	function getItemDelay($delay, $flexIntervals){
		if(!empty($delay) || zbx_empty($flexIntervals)) return $delay;

		$minDelay = SEC_PER_YEAR;
		$flexIntervals = explode(';', $flexIntervals);
		foreach($flexIntervals as $fnum => $flexInterval){
			if(2 != sscanf($flexInterval, "%d/%29s", $flexDelay, $flexPeriod)) continue;

			$minDelay = min($minDelay, $flexDelay);
		}

	return $minDelay;
	}
/*
 * Function: get_current_delay
 *
 * Purpose: return delay value that is currently applicable
 *
 * Parameters: delay          - [IN] default delay
 *             flex_intervals - [IN] separated flexible intervals
 *
 *                                   +------------[;]<----------+
 *                                   |                          |
 *                                 ->+-[d/wd[-wd2],hh:mm-hh:mm]-+
 *
 *                                 d       - delay (0-n)
 *                                 wd, wd2 - day of week (1-7)
 *                                 hh      - hours (0-24)
 *                                 mm      - minutes (0-59)
 *
 *             now            - [IN] current time
 *
 * Return value: delay value - either default or minimum delay value
 *                             out of all applicable intervals
 *
 * Author: Alexander Vladishev
 */
	function get_current_delay($delay, $flex_intervals, $now){
		if(zbx_empty($flex_intervals)) return $delay;

		$current_delay = SEC_PER_YEAR;

		$arr_of_flex_intervals = explode(';', $flex_intervals);

		foreach($arr_of_flex_intervals as $fnum => $flex_interval){
			if(2 != sscanf($flex_interval, "%d/%29s", $flex_delay, $flex_period)) continue;

			if(($flex_delay < $current_delay) && check_time_period($flex_period, $now)){
				$current_delay = $flex_delay;
			}
		}

		if($current_delay == SEC_PER_YEAR) return $delay;

	return ($current_delay == 0) ? SEC_PER_YEAR : $current_delay;
	}

/*
 * Function: get_next_delay_interval
 *
 * Purpose: return time of next flexible interval
 *
 * Parameters: flex_intervals - [IN] separated flexible intervals
	 *
 *                                   +------------[;]<----------+
 *                                   |                          |
 *                                 ->+-[d/wd[-wd2],hh:mm-hh:mm]-+
	 *
 *                                 d       - delay (0-n)
 *                                 wd, wd2 - day of week (1-7)
 *                                 hh      - hours (0-24)
 *                                 mm      - minutes (0-59)
	 *
 *             now            - [IN] current time
 *
 * Return value: start of next interval
 *
 * Author: Alexei Vladishev, Alexander Vladishev
 */
	function get_next_delay_interval($flex_intervals, $now, &$next_interval){
		if(zbx_empty($flex_intervals)) return false;

		$next = 0;
		$tm = localtime($now, true);
		$day = (0 == $tm['tm_wday']) ? 7 : $tm['tm_wday'];
		$sec = 3600 * $tm['tm_hour'] + 60 * $tm['tm_min'] + $tm['tm_sec'];

		$arr_of_flex_intervals = explode(';', $flex_intervals);

		foreach($arr_of_flex_intervals as $flex_interval){
			if(7 != sscanf($flex_interval, "%d/%d-%d,%d:%d-%d:%d", $delay, $d1, $d2, $h1, $m1, $h2, $m2)){
				if(6 != sscanf($flex_interval, "%d/%d,%d:%d-%d:%d", $delay, $d1, $h1, $m1, $h2, $m2)) continue;

				$d2 = $d1;
			}

			$sec1 = 3600 * $h1 + 60 * $m1;
			$sec2 = 3600 * $h2 + 60 * $m2;

			if(($day >= $d1) && ($day <= $d2) && ($sec >= $sec1) && ($sec <= $sec2)){
// current period
				if(($next == 0) || ($next > ($now - $sec + $sec2)))	$next = $now - $sec + $sec2;
			}
			else if(($day >= $d1) && ($day <= $d2) && ($sec < $sec1)){
// will be active today
				if (($next == 0) || ($next > ($now - $sec + $sec1))) $next = $now - $sec + $sec1;
			}
			else{
				$next_day = (($day + 1 <= 7) ? ($day + 1) : 1);

				if(($next_day >= $d1) && ($next_day <= $d2)){
// will be active tomorrow
					if(($next == 0) || ($next > ($now - $sec + SEC_PER_DAY + $sec1)))
						$next = $now - $sec + SEC_PER_DAY + $sec1;
				}
				else{
					if($day < $d1) $day_diff = $d1 - $day;
					if($day >= $d2) $day_diff = ($d1 + 7) - $day;
					if(($day >= $d1) && ($day < $d2)){
// should never happen
// Could not deduce day difference
						$day_diff = -1;
					}

					if($day_diff != -1){
						if(($next == 0) || ($next > ($now - $sec + SEC_PER_DAY * $day_diff + $sec1)))
							$next = $now - $sec + SEC_PER_DAY * $day_diff + $sec1;
					}
				}
			}
		}

		if($next != 0) $next_interval = $next;

	return $next;
	}

/*
 * Function: calculate_item_nextcheck
 *
 * Description:
 *     calculate nextcheck timestamp for item
 *
 * Parameters:
 *     itemid - item ID
 *     item_type - item type
 *     delay - item's refresh rate in sec
 *     flex_intervals - item's flexible refresh rate
 *     now - current timestamp
 *
 * Author:
 *     Alexander Vladishev
 *
 * Comments:
 *     !!! Don't forget sync code with C !!!
 */
	function calculate_item_nextcheck($itemid, $item_type, $delay, $flex_intervals, $now){
		if(0 == $delay) $delay = SEC_PER_YEAR;

// Special processing of active items to see better view in queue
		if($item_type == ITEM_TYPE_ZABBIX_ACTIVE){
			$nextcheck = $now + $delay;
		}
		else{
			$current_delay = get_current_delay($delay, $flex_intervals, $now);

			if(get_next_delay_interval($flex_intervals, $now, $next_interval) && ($now + $current_delay) > $next_interval){
// next check falls out of the current interval
				do{
					$current_delay = get_current_delay($delay, $flex_intervals, $next_interval + 1);

					/* as soon as item check in the interval is not forbidden with delay=0, use it */
					if (SEC_PER_YEAR != $current_delay)
						break;

					get_next_delay_interval($flex_intervals, $next_interval + 1, $next_interval);
				}
				while($next_interval - $now < SEC_PER_WEEK);
// checking the nearest week for delay!=0

				$now = $next_interval;
			}

			$delay = $current_delay;
			$nextcheck = $delay * floor($now / $delay) + ($itemid % $delay);

			while($nextcheck <= $now) $nextcheck += $delay;
		}

	return array('nextcheck' => $nextcheck, 'delay' => $delay);
	}


	/**
	 * Check item key and return info about an error if one is present
	 *
	 * @param string $key item key, e.g. system.run[cat /etc/passwd | awk -F: '{ print $1 }']
	 * @return array
	 *
	 */
	function check_item_key($key){
		$key_strlen = zbx_strlen($key);

		//empty string
		if($key_strlen == 0){
			return array(
				false,   //is key valid?
				S_KEY_CANNOT_BE_EMPTY //result description
			);
		}

		//key is larger then 255 chars
		if($key_strlen > 255){
			return array(
				false,   //is key valid?
				sprintf(S_KEY_TOO_LARGE, 255) //result description
			);
		}

		$characters = array();

		//gathering characters into array, because we can't just work with them ar one if it's a unicode string
		for($i = 0; $i < $key_strlen; $i++){
			$characters[] = zbx_substr($key, $i, 1);
		}

		//checking every character, one by one
		for($current_char = 0; $current_char < $key_strlen; $current_char++) {
			if(!preg_match("/".ZBX_PREG_KEY_NAME."/", $characters[$current_char])) {
				break; //$current_char now points to a first 'not a key name' char
			}
		}

		//no function specified?
		if ($current_char == $key_strlen) {
			return array(
				true,   //is key valid?
				S_KEY_IS_VALID //result description
			);
		}
		//function with parameter, e.g. system.run[...]
		elseif($characters[$current_char] == '[') {

			$state = 0; //0 - initial, 1 - inside quoted param, 2 - inside unquoted param
			$nest_level = 0;

			//for every char, starting after '['
			for($i = $current_char+1; $i < $key_strlen; $i++) {
				switch($state){
					//initial state
					case 0:
						if($characters[$i] == ',') {
							//do nothing
						}
						//Zapcat: '][' is treated as ','
						elseif($characters[$i] == ']' && isset($characters[$i+1]) && $characters[$i+1] == '[' && $nest_level == 0) {
							$i++;
						}
						//entering quotes
						elseif($characters[$i] == '"') {
							$state = 1;
						}
						//next nesting level
						elseif($characters[$i] == '[') {
							$nest_level++;
						}
						//one of the nested sets ended
						elseif($characters[$i] == ']' && $nest_level != 0) {
							$nest_level--;
							//skipping spaces
							while(isset($characters[$i+1]) && $characters[$i+1] == ' ') {
								$i++;
							}
							//all nestings are closed correctly
							if ($nest_level == 0 && isset($characters[$i+1]) && $characters[$i+1] == ']' && !isset($characters[$i+2])) {
								return array(
									true,   //is key valid?
									S_KEY_IS_VALID //result description
								);
							}

							if((!isset($characters[$i+1]) || $characters[$i+1] != ',')
								&& !($nest_level !=0 && isset($characters[$i+1]) && $characters[$i+1] == ']')) {
								return array(
									false,   //is key valid?
									sprintf(S_INCORRECT_SYNTAX_NEAR, $characters[$current_char], $current_char) //result description
								);
							}
						}
						elseif($characters[$i] == ']' && $nest_level == 0) {
							if (isset($characters[$i+1])){
								return array(
									false,   //is key valid?
									sprintf(S_INCORRECT_USAGE_OF_BRACKETS, $characters[$i+1]) //result description
								);
							}
							else {
								return array(
									true,   //is key valid?
									S_KEY_IS_VALID //result description
								);
							}
						}
						elseif($characters[$i] != ' ') {
							$state = 2;
						}

					break;

					//quoted
					case 1:
						//ending quote is reached
						if($characters[$i] == '"')
						{
							//skipping spaces
							while(isset($characters[$i+1]) && $characters[$i+1] == ' ') {
								$i++;
							}

							//Zapcat
							if ($nest_level == 0 && isset($characters[$i+1]) && isset($characters[$i+2]) && $characters[$i+1] == ']' && $characters[$i+2] == '[')
							{
								$state = 0;
								break;
							}

							if ($nest_level == 0 && isset($characters[$i+1]) && $characters[$i+1] == ']' && !isset($characters[$i+2]))
							{
								return array(
									true,   //is key valid?
									S_KEY_IS_VALID //result description
								);
							}
							elseif($nest_level == 0 && $characters[$i+1] == ']' && isset($characters[$i+2])){
								return array(
									false,   //is key valid?
									sprintf(S_INCORRECT_USAGE_OF_BRACKETS, $characters[$i+2]) //result description
								);
							}

							if ((!isset($characters[$i+1]) || $characters[$i+1] != ',') //if next symbol is not ','
								&& !($nest_level != 0 && isset($characters[$i+1]) && $characters[$i+1] == ']'))
							{
								return array(
									false,   //is key valid?
									sprintf(S_INCORRECT_SYNTAX_NEAR, $characters[$current_char], $current_char) //result description
								);
							}

							$state = 0;
						}
						//escaped quote (\")
						elseif($characters[$i] == '\\' && isset($characters[$i+1]) && $characters[$i+1] == '"') {
							$i++;
						}

					break;

					//unquoted
					case 2:
						//Zapcat
						if($nest_level == 0 && $characters[$i] == ']' && isset($characters[$i+1]) && $characters[$i+1] =='[' )
						{
							$i--;
							$state = 0;
						}
						elseif($characters[$i] == ',' || ($characters[$i] == ']' && $nest_level != 0)) {
							$i--;
							$state = 0;
						}
						elseif($characters[$i] == ']' && $nest_level == 0) {
							if (isset($characters[$i+1])){
								return array(
									false,   //is key valid?
									sprintf(S_INCORRECT_USAGE_OF_BRACKETS, $characters[$i+1]) //result description
								);
							}
							else {
								return array(
									true,   //is key valid?
									S_KEY_IS_VALID //result description
								);
							}
						}
					break;
				}
			}

			return array(
				false,   //is key valid?
				S_INVALID_KEY_FORMAT //result description
			);

		}
		else {
			return array(
				false,   //is key valid?
				sprintf(S_INVALID_CHARACTER_AT_POSITION, $characters[$current_char], $current_char) //result description
			);
		}

	}
?>
