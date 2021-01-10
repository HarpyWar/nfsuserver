<?php
/*
NFSU server tracker
(c) 2020 HarpyWar <harpywar@gmail.com>
*/

$filename = "servers.txt";
$userip = get_userip();

// add only servers with opened nfsug port
if ( !check_port($userip, 10800) )
	exit;

if ( !file_exists($filename) )
{
	file_put_contents($filename, get_entry($userip));
	exit;
}

$data = file_get_contents("servers.txt");
$servers = explode("\n", $data);
$new_servers = [];
$found = false;
for ($i = 0; $i < count($servers); $i++)
{
	list($ip, $time) = explode(",", $servers[$i]);
	if (trim($ip) == "")
		continue;

	if ($ip == $userip)
	{
		$found = true;
		$new_servers[] = trim(get_entry($ip)); // update last active time
	}
	else
	{
		$new_servers[] = $servers[$i]; // do not update time
	}
}
if (!$found)
	$new_servers[] = trim(get_entry($userip)); // add new entry

$new_servers_str = implode("\n", $new_servers);
file_put_contents($filename, $new_servers_str . "\n");


function get_entry($userip)
{
	return $userip . ',' . time() . "\n";
}

function get_userip()
{
	if (!empty($_SERVER['HTTP_CLIENT_IP'])) {
		$ip = $_SERVER['HTTP_CLIENT_IP'];
	} elseif (!empty($_SERVER['HTTP_X_FORWARDED_FOR'])) {
		$ip = $_SERVER['HTTP_X_FORWARDED_FOR'];
	} else {
		$ip = $_SERVER['REMOTE_ADDR'];
	}
	return $ip;
}

function check_port($ip, $port)
{
    $fp = @fsockopen($ip, $port, $errno, $errstr, 1);
    if (!$fp)
        return false;
    else
        fclose($fp);
    return true;
}
