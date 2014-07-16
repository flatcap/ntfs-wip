<?php

include_once 'utils.php';

function database_add_file($file, $aid, $release, $sid, $did, $pid)
{
	$sql = "SELECT fid FROM rpm_file WHERE fname='$file'";
	$result = mysql_query($sql) or die("query failed: " . mysql_error());

	$line = mysql_fetch_array($result, MYSQL_NUM);
	mysql_free_result($result);

	if ($line === FALSE) {
		$sql = "INSERT INTO rpm_file (fname) values ('$file')";
		$result = mysql_query($sql) or die("query failed: " . mysql_error());
		$fid = mysql_insert_id();
	} else {
		$fid = $line[0];
	}

	$sql = "SELECT rid FROM rpm_release WHERE rname='$release'";
	$result = mysql_query($sql) or die("query failed: " . mysql_error());

	$line = mysql_fetch_array($result, MYSQL_NUM);
	mysql_free_result($result);

	if ($line === FALSE) {
		$sql = "INSERT INTO rpm_release (did,rname) VALUES ($did,'$release')";
		$result = mysql_query($sql) or die("query failed: " . mysql_error());
		$rid = mysql_insert_id();
	} else {
		$rid = $line[0];
	}

	$sql = "UPDATE rpm_file SET aid=$aid, sid=$sid, rid=$rid, pid=$pid WHERE fid=$fid";
	$result = mysql_query($sql) or die("query failed: " . mysql_error());
}

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
	$output .= "function sf(){document.focus.distro.focus();}\n";
	$output .= "// -->\n";
	$output .= "</script>\n";
	$output .= "<link rel='stylesheet' href='style.css' type='text/css'>";
	$output .= "<title>New Files</title>";
	$output .= "</head>\n";
	$output .= "<body onload='sf()'>\n";

	$output .= display_menu();

	$output .= "<h1>New Files</h1>";

	$section_list = get_table("SELECT sid,brief,title FROM rpm_section ORDER BY sdo");
	//echo dump_table($section_list, "Sections");

	$person_list = get_table("SELECT pid,pname FROM rpm_person ORDER BY pname");
	//echo dump_table($person_list, "People");

	$distro_list = get_table("SELECT did,vendor,abbr,version,dname FROM rpm_distro");
	//echo dump_table($distro_list, "Distro");

	if ($_POST) {
		$arch_list = get_table("SELECT aname,aid FROM rpm_arch");
		//echo dump_table($arch_list, "Arches");

		$file_list = explode("\r\n", $_POST['list']);
		$file = $file_list[0];
		$release = "";
		$arch = "";
		guess_rel_arch($file, $release, $arch);

		$sid = $_POST['section'];
		$section = $section_list[$sid]['title'];

		$pid = $_POST['person'];
		$person = $person_list[$pid]['pname'];

		$did = $_POST['distro'];
		$distro = "{$distro_list[$did]['vendor']} {$distro_list[$did]['version']} ({$distro_list[$did]['dname']})";

		$output .= "<ul>";
		$output .= "<li><b>Distro:</b> $distro</li>";
		$output .= "<li><b>Section:</b> $section</li>";
		$output .= "<li><b>Release:</b> $release</li>";
		$output .= "<li><b>Person:</b> $person</li>";
		$output .= "</ul>";

		$output .= "<table border='1' cellspacing='0' cellpadding='3'>\n";
		$output .= "<tr>";
		$output .= "<th>File</th>";
		$output .= "<th>Release</th>";
		$output .= "<th>Arch</th>";
		$output .= "</tr>";

		$release = "";
		$arch = "";
		foreach ($file_list as $file) {
			if (empty($file)) {
				continue;
			}
			guess_rel_arch($file, $release, $arch);
			$aid = $arch_list[$arch]['aid'];

			$output .= "<tr>";
			$output .= "<td>$file</td>";
			$output .= "<td>$release</td>";
			$output .= "<td>$arch</td>";
			$output .= "</tr>";
			database_add_file($file, $aid, $release, $sid, $did, $pid);
		}
		$output .= "</table>";
	} else {
		$output .= "<form name='focus' action='{$_SERVER["PHP_SELF"]}' method='post'>";

		$output .= "<select name='distro'>";
		$output .= "<option value=''>select a distro...</option>";
		foreach ($distro_list as $line) {
			$menu = "{$line['vendor']} {$line['version']} ({$line['dname']})";
			$value = $line['did'];
			$output .= "<option value='$value'>$menu</option>";
		}
		$output .= "</select>";
		$output .= " ";

		$output .= "<select name='section'>";
		$output .= "<option value=''>select a section...</option>";
		foreach ($section_list as $sect => $line) {
			$menu = $line['brief'];
			$value = $line['sid'];
			$output .= "<option value='$value'>$menu</option>";
		}
		$output .= "</select>";
		$output .= " ";

		$output .= "<select name='person'>";
		$output .= "<option value=''>select a person...</option>";
		foreach ($person_list as $pid => $line) {
			$menu = $line['pname'];
			$output .= "<option value='$pid'>$menu</option>";
		}
		$output .= "</select>";

		$output .= "<textarea name='list' cols='100' rows='20'></textarea>";
		$output .= "<input type='submit' value='review'>";

		$output .= "</form>";
	}

	$output .= "</body>";
	$output .= "</html>";

	echo $output;

	mysql_close($connection);
}


main();

?>

