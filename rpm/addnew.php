<?php

include_once 'utils.php';

function addnew_get_arches()
{
	$sql = "SELECT aname,aid FROM rpm_arch";
	return get_table($sql);
}

function addnew_add_to_database($aid, $rid, $sid, $pid, $fname)
{
	$sql = "SELECT fid FROM rpm_file WHERE fname = '$fname'";
	if (count (get_table($sql)) > 0) {
		return FALSE;
	}

	$sql = "INSERT INTO rpm_file (aid,sid,rid,pid,fname) VALUES ('$aid','$sid','$rid','$pid','$fname')";
	$result = mysql_query($sql) or die("query failed: " . mysql_error());

	if ($result) {
		return mysql_insert_id();
	} else {
		return 0;
	}
}

function addnew_main()
{
	include 'conf.php';
	include 'menu.php';

	$connection = mysql_connect($db_host, $db_user, $db_pass)
		or die("Could not connect: " . mysql_error());

	mysql_select_db("rpm")
		or die("Could not select database<br>");

	$output = "";

	$output .= '<!DOCTYPE html PUBLIC "-//W3C//DTD HTML 4.01 Transitional//EN">';
	$output .= "<html>\n";
	$output .= "<head>\n";
	$output .= "<script type='text/javascript'>\n";
	$output .= "<!--\n";
	$output .= "function sf(){document.focus.addnew.focus();}\n";
	$output .= "// -->\n";
	$output .= "</script>\n";
	$output .= '<link rel="stylesheet" href="style.css" type="text/css">';
	$output .= '<title>Add New Files</title>';
	$output .= "</head>\n";
	$output .= "<body onload='sf()'>\n";

	$output .= layout_header();
	$output .= layout_menu($_SERVER['PHP_SELF']);

	$output .= '<div class="content">';
	$output .= '<h2>Add New Files</h2>';

	$button  = get_post_variable('button');
	$addnew  = get_post_variable('addnew');
	$did     = get_post_variable('did');
	$rid     = get_post_variable('rid');
	$pid     = get_post_variable('pid');
	$sid     = get_post_variable('sid');
	$message = "";

	if ($button == "add new") {
		$rel  = "";
		$arch = "";

		$arches = addnew_get_arches();

		$list = explode("\r\n", $addnew);
		foreach ($list as $fname) {
			$fname = tidy_name($fname);
			if ($fname === FALSE) {
				continue;
			}
			if (!guess_rel_arch($fname, $rel, $arch)) {
				continue;
			}
			$aid = $arches[$arch]['aid'];
			$output .= "$fname ";
			$fid = addnew_add_to_database($aid, $rid, $sid, $pid, $fname);
			if ($fid === FALSE) {
				$output .= "<span style='color: red;'>already exists in database</span>";
			} else {
				$output .= "<span style='color: green;'>added to file list ($fid)</span>";
			}
			$output .= "<br>";
		}
	} else {
		$output .= "<form name='focus' action='" . $_SERVER['PHP_SELF'] . "' method='post'>";
		$output .= "<textarea name='addnew' cols='100' rows='20'>";
		if ($button == "sourceforge") {
			include "sf.php";

			$files = files_get_remainder();

			foreach ($files as $f) {
				$output .= "$f\n";
			}
		} else {
			$output .= $addnew;
		}
		$output .= "</textarea>";
		$output .= "<br>";
		$output .= "Distro ";
		$output .= get_distros($did, FALSE, FALSE);
		$output .= "<input type='submit' name='button' value='refresh'>";
		$output .= "<br>";
		$output .= "Release ";
		$output .= get_releases($rid, $did, FALSE, FALSE);
		$output .= "<br>";
		$output .= "Section ";
		$output .= get_sections($sid, FALSE, FALSE);
		$output .= "<br>";
		$output .= "Person ";
		$output .= get_people($pid, FALSE, FALSE);
		$output .= "<br>";
		$output .= "<input type='submit' name='button' value='add new'>";
		$output .= "<input type='submit' name='button' value='sourceforge'>";
		$output .= "</td>";
		$output .= "</tr>";
		$output .= "</form>";
	}

	mysql_close($connection);

	$output .= '</div>';
	$output .= "</body>\n";
	$output .= "</html>\n";

	echo $output;
}


addnew_main();

?>

