<?php
//nfsuserver 0.9.8 monitoring v.0.0.3 by SeazoN
$server['ip'] = '127.0.0.1';
$server['port'] = 10980;
$string = '';

$fp = @fsockopen($server['ip'], $server['port'], $errno, $errstr, 30);
if(!$fp)
{
	echo "Server is down. $errstr ($errno)<br />\n";
}
else
{
	while(!feof($fp))
	{
		$string .= fread($fp,1);
	}
	//$string = "usercount|roomcount|timeonlineinseconds|platform|version|servername~~~A.LAN|0|[]B.LAN|0|[]C.LAN|0|[]D.LAN|0|[]E.LAN|1|[SeazoN|]F.LAN|0|[]G.LAN|0|[]H.LAN|0|[]";
	fclose($fp);
	preg_match("/(\d+)\|(\d+)\|(\d+)\|([A-Za-z0-9]{1,})\|([A-Za-z0-9\.]{1,})\|([A-Za-z0-9\.\s]{1,})\~\~\~/Ssi", $string, $array0);
	preg_match_all("/([ABCDEFGH]{1})\.([A-Za-z0-9\.]{0,})\|(\d+)\|\[([A-Za-z0-9]{0,})\|{0,}\]/Ssi", $string, $array);
//	preg_match_all("/([ABCDEFGH]{1})\.([A-Za-z0-9]{0,})\|(\d+)\|\[([A-Za-z0-9]{0,})\|{0,}\]/Ssi", $string, $array);
	$rooms = array(
	'A'=>'Ranked - Circuit:',
	'B'=>'Ranked - Sprint:',
	'C'=>'Ranked - Drift:',
	'D'=>'Ranked - Drag:',
	'E'=>'Unranked - Circuit:',
	'F'=>'Unranked - Sprint:',
	'G'=>'Unranked - Drift:',
	'H'=>'Unranked - Drag:',
);

$days = floor($array0[3]/86400);
$hours = floor(($array0[3] - $days*86400)/3600); 
$minutes = floor(($array0[3] - $days*86400 - $hours*3600)/60); 
$seconds = ($array0[3] - $days*86400 - $hours*3600 - $minutes*60); 
?>
<html>
<head>
<title><?php echo $array0[6]; ?> v.<?php echo $array0[5]." ".$array0[4] ; ?> - Monitoring IP: <?php echo $server['ip']; ?> - <?php echo $array0[1]; ?> players in <?php echo $array0[2]; ?> rooms [Uptime: <?php echo $days; ?>days, <?php echo $hours; ?>hr, <?php echo $minutes; ?>min, <?php echo $seconds; ?>secs]</title>
<meta http-equiv="Content-Type" content="text/html; charset=windows-1251">
<link rel="stylesheet" href="/games/cs-stats/pstats.css" type="text/css">
</head>
<body leftmargin="0" topmargin="0" marginwidth="0" marginheight="0">
<table width="98%" border="0" cellspacing="0" cellpadding="1" align="center" class="tablebg">
<tr>
  <td>
	<table width="100%" border="0" cellspacing="1" cellpadding="1">
	<tr class="listhead">
		<th align="center">In rooms</th><th align="center">Players</th><th align="center">Names</th>
</tr>
<?php
	$total = 0;
	foreach($array[1] as $k=>$v)
	{
?>
	<tr class="plrrow1">
		<td align="center"><?php echo $rooms[$array[1][$k]].' '.$array[2][$k]; ?></td><td align="center"><?php echo $array[3][$k]; ?></td><td align="center"><?php echo $array[4][$k]; ?></td>
	</tr>
<?php
		$total=$total+$array[3][$k]; 
	}
?>
	<tr class="listhead">
		<th align="center">Total in rooms</th><th align="center"><?php echo $total; ?></th><th align="center"></th>
	</tr>
      </table>
    </td>
  </tr>
</table>
</html>
<?php
}
echo "<!--$string-->";
?>