<?php

function files_get_sourceforge()
{
	$URL_SFPAGE = "http://prdownloads.sourceforge.net/linux-ntfs";
	//$URL_SFPAGE = "sourceforge.html";

	$list = array();
	$file = fopen($URL_SFPAGE, "r");
	if (!$file) {
		echo "Unable to open $URL_SFPAGE on Sourceforge";
		return FALSE;
	}

	while (!feof($file)) {
		$line = fgets($file, 1024);
		if (!eregi('<a href="/linux-ntfs/kernel-[^"]*">(.*)</a>', $line, $tmp)) {
			continue;
		}

		$list[] = $tmp[1];
	}

	fclose($file);

	return $list;
}

function files_get_ignored()
{
	$sql = "SELECT fname FROM rpm_file WHERE aid = 0";

	$list = array();

	$result = mysql_query($sql) or die("query failed: " . mysql_error());

	$key = mysql_fetch_field($result, 0)->name;

	while ($line = mysql_fetch_array($result, MYSQL_ASSOC)) {
		$list[] = $line[$key];
	}

	mysql_free_result($result);

	return $list;
}

function files_get_released()
{
	$sql = "SELECT fname FROM rpm_file WHERE aid <> 0";

	$list = array();

	$result = mysql_query($sql) or die("query failed: " . mysql_error());

	$key = mysql_fetch_field($result, 0)->name;

	while ($line = mysql_fetch_array($result, MYSQL_ASSOC)) {
		$list[] = $line[$key];
	}

	mysql_free_result($result);

	return $list;
}

function files_get_remainder()
{
	$src = files_get_sourceforge();
	$rel = files_get_released();
	$ign = files_get_ignored();

	$all = array_merge ($rel, $ign);

	$rem = array_diff($src, $all);

	return $rem;
}


?>

