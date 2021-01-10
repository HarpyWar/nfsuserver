<?php
/*
NFSU server tracker
(c) 2020 HarpyWar <harpywar@gmail.com>
*/

$filename = "servers.txt";
$expire_time = 60*5; // seconds when server expired and removed from list

// servers for always display (who do not use tracker)
$persistent_servers = [
];

if ( !file_exists($filename) )
{
	exit;
}

$data = file_get_contents("servers.txt");
$servers = explode("\n", $data);

// add persistent servers
foreach ($persistent_servers as $pip)
{
	$found = false;
	foreach ($servers as $entry)
	{
		list($ip, $time) = explode(",", $entry);
		if ($ip == $pip)
			$found = true;
	}
	if (!$found)
		$servers[] = $pip . ',' . time();
}

$new_servers = [];
$dirty = false;
foreach ($servers as $entry)
{
	list($ip, $time) = explode(",", $entry);
	if (trim($ip) == "")
		continue;
		
	// check for expired servers
	if (time() - $time > $expire_time)
	{
		$dirty = true;
		continue;
	}
	// fill actual servers
	if (!in_array($ip, $persistent_servers) )
		$new_servers[] = $entry;

	echo formatEndian( ip2long($ip) ) . "\n";
}

if ($dirty)
{
	$new_servers_str = implode("\n", $new_servers);
	file_put_contents($filename, $new_servers_str . "\n");
}




function formatEndian($endian, $format = 'N') {
    $endian = intval($endian, 16);      // convert string to hex
    $endian = pack('L', $endian);       // pack hex to binary sting (unsinged long, machine byte order)
    $endian = unpack($format, $endian); // convert binary sting to specified endian format

    return sprintf("%u", $endian[1]); // return endian as as int
}