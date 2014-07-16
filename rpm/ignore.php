<?php

include_once 'utils.php';

function ignore_add_to_database($fname)
{
	$sql = "SELECT fid FROM rpm_file WHERE fname = '$fname'";
	if (count (get_table($sql)) > 0) {
		return FALSE;
	}

	$sql = "INSERT INTO rpm_file (aid,sid,rid,pid,fname) VALUES (0,0,0,0,'$fname')";
	$result = mysql_query($sql) or die("query failed: " . mysql_error());

	if ($result) {
		return mysql_insert_id();
	} else {
		return 0;
	}
}

function ignore_main()
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
	$output .= "function sf(){document.focus.ignore.focus();}\n";
	$output .= "// -->\n";
	$output .= "</script>\n";
	$output .= '<link rel="stylesheet" href="style.css" type="text/css">';
	$output .= '<title>Ignore Files</title>';
	$output .= "</head>\n";
	$output .= "<body onload='sf()'>\n";

	$output .= layout_header();
	$output .= layout_menu($_SERVER['PHP_SELF']);

	$output .= '<div class="content">';
	$output .= '<h2>Ignore Files</h2>';

	$action  = get_post_variable('action');
	$button  = get_post_variable('button');
	$ignore  = get_post_variable('ignore');
	$message = "";

	if ($button == "ignore") {
		$list = explode("\r\n", $ignore);
		foreach ($list as $fname) {
			$fname = tidy_name($fname);
			if ($fname === FALSE) {
				continue;
			}
			$output .= "$fname ";
			$fid = ignore_add_to_database($fname);
			if ($fid === FALSE) {
				$output .= "<span style='color: red;'>already exists in database</span>";
			} else {
				$output .= "<span style='color: green;'>added to ignore list ($fid)</span>";
			}
			$output .= "<br>";
		}
	} else {
		$output .= "<form name='focus' action='" . $_SERVER['PHP_SELF'] . "' method='post'>";
		$output .= "<textarea name='ignore' cols='100' rows='20'>";
		if ($button == "sourceforge") {
			include "sf.php";

			$files = files_get_remainder();

			foreach ($files as $f) {
				$output .= "$f\n";
			}
		} else {
			$output .= $ignore;
		}
		$output .= "</textarea>";
		$output .= "<br>";
		$output .= "<input type='hidden' name='action' value='$action'>";
		$output .= "<input type='submit' name='button' value='ignore'>";
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


ignore_main();

?>

