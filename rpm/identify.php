<?php

include_once 'utils.php';

function main()
{
	include 'conf.php';

	$connection = mysql_connect($db_host, $db_user, $db_pass)
		or die("Could not connect: " . mysql_error());

	mysql_select_db("rpm")
		or die("Could not select database<br>");

	$output = "";

	$output .= "<!DOCTYPE html PUBLIC '-//W3C//DTD HTML 4.01 Transitional//EN'>\n";
	$output .= "<html>\n";
	$output .= "<head>\n";
	$output .= "<script type='text/javascript'>\n";
	$output .= "<!--\n";
	$output .= "function sf(){document.f.list.focus();}\n";
	$output .= "// -->\n";
	$output .= "</script>\n";
	$output .= "<link rel='stylesheet' href='style.css' type='text/css'>";
	$output .= "<title>Table Generator</title>";
	$output .= "</head>\n";
	$output .= "<body onload='sf()'>\n";

	$output .= display_menu();

	$output .= "<h1>Identify</h1>\n";

	if ($_POST) {
		$files = explode("\r\n", $_POST['list']);
		$output .= "<table border='1' cellspacing='0' cellpadding='3'>\n";
		$output .= "<tr>";
		$output .= "<th>filename</th>";
		$output .= "<th>release</th>";
		$output .= "<th>arch</th>";
		$output .= "</tr>";
		foreach ($files as $file) {
			if (empty($file)) {
				continue;
			}
			if (strncmp ($file, "kernel-sourcecode-", 18) == 0) {
				$rel = str_replace("kernel-sourcecode-", "", $file);
				$rel = str_replace(".noarch.rpm", "", $rel);
				$arch = "src";
			} elseif (strncmp ($file, "kernel-source-", 14) == 0) {
				$rel = str_replace("kernel-source-", "", $file);
				$rel = str_replace(".i386.rpm", "", $rel);
				$arch = "src";
			} elseif (strncmp ($file, "kernel-module-ntfs-", 19) == 0) {
				$bits = explode("-", $file);
				$rel = "{$bits['3']}-{$bits['4']}";
				$bits = explode(".", $file);
				$arch = $bits[count($bits) - 2];
				$mod = get_arch_modifier($rel);
				if (!empty($mod)) {
					$rel = str_replace($mod, "", $rel);
					$arch .= "-$mod";
				}
			} elseif (strncmp ($file, "kernel-ntfs-2", 13) == 0) {
				$rel = str_replace("kernel-ntfs-", "", $file);
				$rel = strip_file_suffix($rel);
				$bits = explode(".", $file);
				$arch = $bits[count($bits) - 2];
				$rel = str_replace(".$arch", "", $rel);
				$mod = get_arch_modifier($rel);
				if (!empty($mod)) {
					$rel = str_replace($mod, "", $rel);
					$arch .= "-$mod";
				}
			} elseif (strncmp ($file, "kernel-2", 8) == 0) {
				$rel = str_replace("kernel-", "", $file);
				$rel = str_replace(".src.rpm", "", $rel);
				$arch = "src";
			} elseif (strncmp ($file, "kernel-ntfs-", 12) == 0) {
				$rel = str_replace("kernel-ntfs-", "", $file);
				$rel = strip_file_suffix($rel);
				$mod = get_arch_modifier($rel);
				if (!empty($mod)) {
					$rel = str_replace("$mod-", "", $rel);
					$mod = "-$mod";
				}
				$arch = substr(strrchr($rel, "."), 1);
				$rel = str_replace(".$arch", "", $rel);
				$arch .= $mod;
			} else {
				$rel = "unknown";
				$arch = "unknown";
			}
			$output .= "<tr>";
			$output .= "<td>$file</td>";
			$output .= "<td>$rel</td>";
			$output .= "<td>$arch</td>";
			$output .= "</tr>";
		}
		$output .= "</table>";
	} else {
		$output .= "<form name='f' action='{$_SERVER["PHP_SELF"]}' method='post'>";
		$output .= "<textarea name='list' cols='100' rows='20'></textarea>";
		$output .= "<input type='submit' value='identify'>";
		$output .= "</form>";
	}

	$output .= "</body>\n";
	$output .= "</html>\n";

	echo $output;

	mysql_close($connection);
}


main();

?>

