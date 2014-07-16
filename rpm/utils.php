<?php

function dump_table($array, $name)
{
	$output = "";
	$index = key($array);
	$keys = array_keys($array[$index]);
	$cols = count($keys);

	$output .= "<h2>$name</h2>\n";
	$output .= "<table border=1 cellspacing=0 cellpadding=3>\n";

	$output .= "  <tr>\n";
	for ($i = 0; $i < $cols; $i++) {
		$output .= "    <th>$keys[$i]</th>\n";
	}
	$output .= "  </tr>\n";

	foreach ($array as $line) {
		$output .= "  <tr>\n";
		foreach ($line as $value) {
			if (empty($value)) {
				$output .= "    <td>&nbsp;</td>\n";
			} else {
				$output .= "    <td>$value</td>\n";
			}
		}
		$output .= "  </tr>\n";
	}

	$output .= "</table>\n";
	$output .= "<br>\n";

	return $output;
}

function get_count($sql)
{
	$table = array();

	$result = mysql_query($sql) or die("query failed: " . mysql_error());

	$key = mysql_fetch_field($result, 0)->name;

	$line = mysql_fetch_array($result, MYSQL_ASSOC);
	return $line[$key];
}

function get_table($sql)
{
	$table = array();

	$result = mysql_query($sql) or die("query failed: " . mysql_error());

	$key = mysql_fetch_field($result, 0)->name;

	while ($line = mysql_fetch_array($result, MYSQL_ASSOC)) {
		$table[$line[$key]] = $line;
	}

	mysql_free_result($result);
	return $table;
}

function get_table_num($sql)
{
	$table = array();

	$result = mysql_query($sql) or die("query failed: " . mysql_error());

	$count = mysql_num_rows($result);

	for ($i = 0; $i < $count; $i++) {
		$table[$i] = mysql_fetch_array($result, MYSQL_ASSOC);
	}

	mysql_free_result($result);
	return $table;
}

function get_name($row)
{
	$name = $row['pname'];
	$nick = $row['nickname'];
	$web  = $row['webpage'];

	$result = $name;
	/*
	if (!empty($nick)) {
		$result .= " ($nick)";
	}
	*/
	if (!empty($web)) {
		$result = "<a href='$web'>$result</a>";
	}
	return $result;
}

function display_menu()
{
	$output = "";
	$output .= "<a href=\"people.php\">Person</a>\n";
	$output .= "<a href=\"thanks.php\">Thanks</a>\n";
	$output .= "<a href=\"generate.php\">Generate</a>\n";
	$output .= "<a href=\"identify.php\">Identify</a>\n";
	$output .= "<a href=\"new.php\">New</a>\n";
	$output .= "<a href=\"sf.php\">SF</a>\n";
	$output .= "<a href=\"distro.php\">Distro</a>\n";
	$output .= "<a href=\"release.php\">Release</a>\n";
	$output .= "<a href=\"section.php\">Section</a>\n";
	$output .= "<a href=\"menu.php\">Menu</a>\n";
	$output .= "<br>\n";

	return $output;
}

function get_arch_modifier($str)
{
	if (strstr($str, "bigmem")) {
		return "bigmem";
	} elseif (strstr($str, "BOOT")) {
		return "BOOT";
	} elseif (strstr($str, "hugemem")) {
		return "hugemem";
	} elseif (strstr($str, "smp")) {
		return "smp";
	} elseif (strstr($str, "xen0")) {
		return "xen0";
	} elseif (strstr($str, "xenU")) {
		return "xenU";
	} else {
		return "";
	}
}

function strip_file_suffix($file)
{
	$file = str_replace(".rpm", "", $file);
	$file = str_replace(".ko", "", $file);
	$file = str_replace(".o", "", $file);
	return $file;
}

function guess_rel_arch($file, &$rel, &$arch)
{
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
		return FALSE;
	}

	return TRUE;
}

function get_post_variable($name)
{
	$result = "";

	if (isset($_POST)) {
		if (array_key_exists($name, $_POST)) {
			$result = $_POST[$name];
		} else {
			if (($name == "button") && array_key_exists('action', $_POST)) {
				$result = $_POST['action'];
			}
		}
	}

	return $result;
}

function get_distros($did, $anydistro, $readonly)
{
	$sql = "SELECT did,vendor,version,dname FROM rpm_distro ORDER by vendor,version";

	$distros = get_table($sql);

	$output = "";

	if ($readonly === TRUE) {
		$ro = "disabled";
	} else {
		$ro = "";
	}
	$output .= "<select $ro name='did'>";

	if ($anydistro === TRUE) {
		$output .= "<option value='0'>Any Distro...</option>";
	}

	foreach ($distros as $key => $value) {
		if ($did == $value['did']) {
			$marker = "selected";
		} else {
			$marker = "";
		}
		$output .= "<option {$marker} value='{$value['did']}'>{$value['vendor']} {$value['version']} ({$value['dname']})</option>";
	}
	
	$output .= "</select>";

	return $output;
}

function get_releases($rid, $did, $anyrelease, $readonly)
{
	$sql = "SELECT rid,rname FROM rpm_release";
	if ($did > 0) {
		$sql .= " WHERE did = '$did'";
	}
	$sql .= " ORDER BY rname";

	$release = get_table($sql);

	$output = "";

	if ($readonly === TRUE) {
		$ro = "disabled";
	} else {
		$ro = "";
	}
	$output .= "<select $ro name='rid'>";

	if ($anyrelease === TRUE) {
		$output .= "<option value='0'>Any Release...</option>";
	}

	foreach ($release as $key => $value) {
		if ($rid == $value['rid']) {
			$marker = "selected";
		} else {
			$marker = "";
		}
		$output .= "<option {$marker} value='{$value['rid']}'>{$value['rname']}</option>";
	}
	
	$output .= "</select>";

	return $output;
}

function get_people($pid, $anyperson, $readonly)
{
	$sql = "SELECT pid,pname,nickname FROM rpm_person ORDER BY pname";

	$people = get_table($sql);

	$output = "";

	if ($readonly === TRUE) {
		$ro = "disabled";
	} else {
		$ro = "";
	}
	$output .= "<select $ro name='pid'>";

	if ($anyperson === TRUE) {
		$output .= "<option value='0'>Any Person...</option>";
	}

	foreach ($people as $key => $value) {
		if ($pid == $value['pid']) {
			$marker = "selected";
		} else {
			$marker = "";
		}
		$nick = $value['nickname'];
		if (!empty($nick)) {
			$nick = " ($nick)";
		}
		$output .= "<option {$marker} value='{$value['pid']}'>{$value['pname']}{$nick}</option>";
	}
	
	$output .= "</select>";

	return $output;
}

function get_sections($sid, $anysection, $readonly)
{
	$sql = "SELECT sid,brief,title FROM rpm_section";

	$sections = get_table($sql);

	$output = "";

	if ($readonly === TRUE) {
		$ro = "disabled";
	} else {
		$ro = "";
	}
	$output .= "<select $ro name='sid'>";

	if ($anysection === TRUE) {
		$output .= "<option value='0'>Any Section...</option>";
	}

	foreach ($sections as $key => $value) {
		if ($sid == $value['sid']) {
			$marker = "selected";
		} else {
			$marker = "";
		}
		$output .= "<option {$marker} value='{$value['sid']}'>{$value['brief']} - {$value['title']}</option>";
	}
	
	$output .= "</select>";

	return $output;
}

function tidy_name($fname)
{
	$fname = trim($fname);
	$fname = basename($fname);

	$kn  = (preg_match ("/^kernel-ntfs/",            $fname) == 1);
	$kmn = (preg_match ("/^kernel-module-ntfs/",     $fname) == 1);
	$ks1 = (preg_match ("/^kernel-sourcecode-/",     $fname) == 1);
	$ks2 = (preg_match ("/^kernel-source-/",         $fname) == 1);
	$ks3 = (preg_match ("/^kernel-.*\\.src\\.rpm$/", $fname) == 1);
	$o   = (preg_match ("/\\.o$/",                   $fname) == 1);
	$ko  = (preg_match ("/\\.ko$/",                  $fname) == 1);
	$rpm = (preg_match ("/\\.rpm$/",                 $fname) == 1);

	if (($kn || $kmn || $ks1 || $ks2 || $ks3) && ($o || $ko || $rpm)) {
		return $fname;
	} else {
		return FALSE;
	}
}


?>

