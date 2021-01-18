<?php
/* --- [nfs:u lan server web interface script ---
   req. php4 or above with socket support
   full support for nfs:u lan server 0.97
   feel free to mod this script as much as u want.
   version 1.0 (eng) scripted by wippie.

	[some changes by vdl]
	1.1 avoided room string receive bug
	
	1.2 Frapl
		Updated to new stats data format since server version 0.9.8
	1.3 VDL 
		Fixed double connection bug

/* Set your nfs:u server ip adress here! */
$server_ip = "127.0.0.1";
$server_port = 10980;
/* ------------------------------------- */

class Room
{
	var $type; // A-H.
	var $name;
	var $roomcount; //count by server data
	var $count;	//count by num users added (should be the same)
	var $arrusers;

	function Room()
	{
		$this->count=0;
	}
	
	function add_user($username)
	{
		$this->arrusers[$this->count]=$username;
		$this->count++;
	}
}

$rooms = array();
$msg = 0;

function getRoomRowCount($roomtype){
	/* return number of rows (#rooms + #users per room)
	a specific room type has. ned for html */
	global $rooms;
	if(!is_array($rooms)) return 0;
	$cnt=0;
	for($i=0; $i<count($rooms); $i++){
		if(strcasecmp($rooms[$i]->type,$roomtype) == 0){
			$cnt+=($rooms[$i]->count + 1);
		}
	}
	return $cnt;
}
function getRoomTypeString($roomtype){
	/* to be translated to any other language.. */
	$a=array(
		"A" => 'Circuit',
		"B" => 'Sprint',
		"C" => 'Drift',
		"D" => 'Drag',
		"E" => 'Circuit',
		"F" => 'Sprint',
		"G" => 'Drift',
		"H" => 'Drag');
	return $a[$roomtype];
}
function printRoomTable($roomtype){
 global $rooms;
 if(!is_array($rooms)) return;
 echo "<table width=\"170\" border=\"0\" cellpadding=\"0\" cellspacing=\"0\">
        <tr> 
          <td width=\"12\" rowspan=\"".(getRoomRowCount($roomtype)+3)."\" class=\"tdleftbdr\"></td>
          <td height=\"12\" colspan=\"2\" class=\"tdtopbdr\"></td>
          <td width=\"10%\" height=\"12\" class=\"tdetopbdr\"></td>
          <td width=\"12\" rowspan=\"".(getRoomRowCount($roomtype)+3)."\" align=\"right\" class=\"tdrightbdr\"></td>
        </tr>
        <tr> 
          <td colspan=\"2\" class=\"tdroomhead\">".getRoomTypeString($roomtype)."</td>
          <td width=\"10%\" align=\"right\" class=\"tdroomhead\"></td>
        </tr>";
        
 for($i=0; $i<count($rooms); $i++){
	if(strcasecmp($rooms[$i]->type,$roomtype) == 0){
        	echo "<tr> 
          		<td colspan=\"2\" class=\"tdroom\">".($rooms[$i]->name)."</td>
          		<td width=\"10%\" align=\"right\" class=\"tdroom\">".($rooms[$i]->roomcount)."</td>
        	      </tr>";
		if($rooms[$i]->count > 0){
			for($j=0; $j<($rooms[$i]->count); $j++){
			        echo "<tr> 
          			<td width=\"10%\" class=\"tduser\">&nbsp;</td>
          			<td colspan=\"2\" class=\"tduser\">".($rooms[$i]->arrusers[$j])."</td>
        			</tr>";
			}	
		}
	}
 }

 echo "<tr> 
          <td width=\"10%\" height=\"12\" class=\"tdebottombdr\"></td>
          <td height=\"12\" colspan=\"2\" class=\"tdbottombdr\"></td>
        </tr>
      </table>";	
}

// get data from server
// data format usercount|roomcount|timeonlineinseconds|platform|version|servername~~~oldinformationaboutroomnamesandplayernames
$serverdata = "";
$usercount = 0;
$roomcount = 0;
$timeonlineinseconds = 0;
$platform = "";
$serverversion = "";
$servername = "";
$roomdata = "";

$fp = @fsockopen($server_ip, $server_port, $errno, $errstr, 30);
if (!$fp)
{
   $msg="$errstr ($errno)<br />\n";
} 
else
{
	$serverdata = fgets($fp);
	echo("<!-- ".$serverdata." -->");
	
	if (strlen(trim($serverdata)) > 0)
	{
		$serverdata = explode("~~~", $serverdata);
		$roomdata = $serverdata[1];

		$serverdata = explode("|", $serverdata[0]);
		$usercount = $serverdata[0];
		$roomcount = $serverdata[1];
		$timeonlineinseconds = $serverdata[2];
		$platform = $serverdata[3];
		$serverversion = $serverdata[4];
		$servername = $serverdata[5];
	}
}

//get room data
if (!$fp)
{
	$msg="$errstr ($errno)<br />\n";
}
else
{
	$arrdata=explode("]",$roomdata);
	
	for($i=0; $i<count($arrdata); $i++)
	{
		$rooms[$i]=new Room();
		$tdata=explode("[",$arrdata[$i]);	// split up to [type.room & count]-[users]
		$ndata=explode("|",$tdata[0]);		// split up to [type.room]-[count]

		if(count($ndata) < 2)
			$ndata[1] = 0;

		$rooms[$i]->roomcount=(int)$ndata[1];
		$roomname= str_replace("\"", "", $ndata[0]); //no quotes..
		$rooms[$i]->type=substr($roomname,0,1);
		$rooms[$i]->name=substr($roomname,2);
		
		if((int)$ndata[1] > 0)
		{
			$users=explode("|",$tdata[1]);
			for($j=0; $j<count($users); $j++)
			{
				$rooms[$i]->add_user($users[$j]);
			}
		}
	
	}
	$arrdata=null; //cleaning..
}
@fclose($fp);
?>

<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01 Transitional//EN">
<html>
<head>
<title>NFS:U LAN Server "<?php echo($servername); ?>" @ <?p$server_ip;?> : <?php 
	if($msg)
		echo "Offline";
	else
		echo "Usercount ".$usercount;
	?></title>
<meta http-equiv="Content-Type" content="text/html; charset=iso-8859-1">
<style type="text/css">
<!--
body {
	background-color: #000000;
	color: #FFFFFF;
}
td {
	font-family: Verdana, Arial, Helvetica, sans-serif;
	font-size: 10px;
	padding: 1px 3px;
	color: #C3DDE6;
}

.holder {
	color: black;
	background: url("title.jpg") no-repeat;
}
.tdhs{
	padding-top: 2em;
}
.tdroomhead{
	font-family: Verdana, Arial, Helvetica, sans-serif;
	font-size: 14px;
	padding: 1px 3px;
	color: #C3DDE6;
	font-style: italic;
	font-weight: bold;
	
	background-color: #01283B;
	border-bottom: 1px solid #032030;
	border-top: 1px solid #032030;
}
.tdroom {
	font-family: Verdana, Arial, Helvetica, sans-serif;
	font-size: 10px;
	padding: 1px 3px;
	color: #C3DDE6;
	font-weight: bold;

	background-color: #01283B;
	border-bottom: 1px solid #032030;
	border-top: 1px solid #032030;
}
.tduser {
	font-family: Verdana, Arial, Helvetica, sans-serif;
	font-size: 10px;
	padding: 1px 3px;
	color: #C3DDE6;

	background-color: #032030;
	border-bottom: 1px solid #01283B;
	border-top: 1px solid #01283B;	
}
.tdtopbdr {
	background: url("tbdr.gif") no-repeat;
	background-position: 0% 100%;
}
.tdbottombdr {
	background: url("bbdr.gif") no-repeat;
	background-position: 100% 100%;
}
.tdleftbdr {
	background: url("vbdrl.gif") no-repeat;
	background-position: 100% 0%;	
}
.tdrightbdr {
	background: url("vbdrr.gif") no-repeat;
	background-position: 0% 100%;	
}
.tdetopbdr {
	background: url("ebdr.gif") repeat-x;
	background-position: 0% 100%;	
}
.tdebottombdr {
	background: url("ebdr.gif") repeat-x;
	background-position: 0% 0%;	
}
-->
</style>
</head>

<body>
<table width="795" border="0" cellpadding="0" cellspacing="0" class="holder">
  <tr align="right" valign="bottom"> 
    <td height="135" colspan="5">NFS:U Server "<?php echo($servername); ?>" <?php echo($serverversion." (Platform: ".$platform.")"); ?> @ <?=$server_ip;?> : <?php 
	if($msg)
		echo "Offline";
	else
	{
		echo "Time online ".round($timeonlineinseconds/(60*60), 2)."h";
		echo " : Usercount ".$usercount;
		echo " : Roomcount ".$roomcount;
	}
	?></td>
  </tr>
  <tr align="right" valign="bottom">
    <td height="20" colspan="5" align="center"><?php
	if($msg) echo "<font size=+2><strong>Server is offline</font><br>".$msg."</strong>";
	?></td>
  </tr>
<?php if(!$msg) { ?>
  <tr align="center" valign="top"> 
    <td align="left"><img src="ranked.gif" width="41" height="199"></td>
    <td class="tdhs"><?php printRoomTable("A"); ?></td>
    <td class="tdhs"><?php printRoomTable("B"); ?></td>
    <td class="tdhs"><?php printRoomTable("C"); ?></td>
    <td class="tdhs"><?php printRoomTable("D"); ?></td>
  </tr>
  <tr align="center" valign="top"> 
    <td align="left"><img src="unranked.gif" width="41" height="199"></td>
    <td class="tdhs"><?php printRoomTable("E"); ?></td>
    <td class="tdhs"><?php printRoomTable("F"); ?></td>
    <td class="tdhs"><?php printRoomTable("G"); ?></td>
    <td class="tdhs"><?php printRoomTable("H"); ?></td>
  </tr>
<?php } ?>
</table>
</body>
</html>
