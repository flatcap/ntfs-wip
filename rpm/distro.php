<?php

include_once 'utils.php';

function distro_create($vendor, $abbr, $version, $dname, $title, $body)
{
	$sql = "INSERT INTO rpm_distro (vendor,abbr,version,dname,title,body) VALUES ('$vendor','$abbr','$version','$dname','$title','$body')";
	$result = mysql_query($sql) or die("query failed: " . mysql_error());

	if ($result) {
		return mysql_insert_id();
	} else {
		return 0;
	}
}

function distro_update($did, $vendor, $abbr, $version, $dname, $title, $body)
{
	$sql = "UPDATE rpm_distro SET vendor='$vendor', abbr='$abbr', version='$version', dname='$dname', title='$title', body='$body' WHERE did='$did'";
	$result = mysql_query($sql) or die("query failed: " . mysql_error());

	return $result;
}

function distro_delete($did)
{
	$sql = "DELETE FROM rpm_distro WHERE did = $did";
	$result = mysql_query($sql) or die("query failed: " . mysql_error());

	return $result;
}

function distro_display($action, $did, $vendor, $abbr, $version, $dname, $title, $body, $readonly)
{
	$output = "";

	if ($readonly) {
		$ro = "readonly";
	} else {
		$ro = "";
	}

	$output .= "<form name='focus' action='" . $_SERVER['PHP_SELF'] . "' method='post'>";
	$output .= "<table summary='$action' border='0' cellspacing='0'>";

	$output .= "<tr>";
	$output .= "<th>Vendor</th>";
	$output .= "<td><input size='30' type='text' name='vendor' $ro value='$vendor'></td>";
	$output .= "</tr>";

	$output .= "<tr>";
	$output .= "<th>Abbreviation</th>";
	$output .= "<td><input size='30' type='text' name='abbr' $ro value='$abbr'></td>";
	$output .= "</tr>";

	$output .= "<tr>";
	$output .= "<th>Distro Version</th>";
	$output .= "<td><input size='60' type='text' name='version' $ro value='$version'></td>";
	$output .= "</tr>";

	$output .= "<tr>";
	$output .= "<th>Distro Name</th>";
	$output .= "<td><input size='60' type='text' name='dname' $ro value='$dname'></td>";
	$output .= "</tr>";

	$output .= "<tr>";
	$output .= "<th>Page Title</th>";
	$output .= "<td><input size='60' type='text' name='title' $ro value='$title'></td>";
	$output .= "</tr>";

	$output .= "<tr>";
	$output .= "<th>Page Body</th>";
	$output .= "<td><input size='60' type='text' name='body' $ro value='$body'></td>";
	$output .= "</tr>";

	$output .= "<tr>";
	$output .= "<td colspan=2'>";
	$output .= "<input type='hidden' name='action' value='$action'>";
	$output .= "<input type='hidden' name='did'    value='$did'>";
	$output .= "<input type='submit' name='button' value='OK'>";
	$output .= "<input type='submit' name='button' value='Cancel'>";
	$output .= "</td>";
	$output .= "</tr>";
	$output .= "</form>";

	return $output;
}

function distro_main()
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
	$output .= "function sf(){document.focus.vendor.focus();}\n";
	$output .= "// -->\n";
	$output .= "</script>\n";
	$output .= '<link rel="stylesheet" href="style.css" type="text/css">';
	$output .= '<title>Distro Manager</title>';
	$output .= "</head>\n";
	$output .= "<body onload='sf()'>\n";

	$output .= layout_header();
	$output .= layout_menu($_SERVER['PHP_SELF']);

	$output .= '<div class="content">';
	$output .= '<h2>Distro Manager</h2>';

	$action  = get_post_variable('action');
	$button  = get_post_variable('button');
	$did     = get_post_variable('did');
	$vendor  = get_post_variable('vendor');
	$abbr    = get_post_variable('abbr');
	$version = get_post_variable('version');
	$dname   = get_post_variable('dname');
	$title   = get_post_variable('title');
	$body    = get_post_variable('body');
	$message = "";

	if ($action == "create") {
		if ($button == "OK") {
			$did = distro_create($vendor, $abbr, $version, $dname, $title, $body);
			if ($did > 0) {
				$message = "Created new distro id $did ($vendor $version).";
				$action = "search";
			} else {
				$message = "Could not create new distro: $vendor.";
			}
		} elseif ($button == "Cancel") {
			$message = "Create cancelled.";
			$action = "";
		} else {
			$output .= distro_display($action, 0, "", "", "", "", "", "", FALSE);
		}
	}

	if ($action == "edit") {
		if ($button == "edit") {
			$sql = "SELECT did,vendor,abbr,version,dname,title,body FROM rpm_distro WHERE did = $did";
			$table = get_table($sql);
			$distro = $table[$did];
			$output .= distro_display($action, $did, $distro['vendor'], $distro['abbr'], $distro['version'], $distro['dname'], $distro['title'], $distro['body'], FALSE);
		} elseif ($button == "OK") {
			if (distro_update($did, $vendor, $abbr, $version, $dname, $title, $body, TRUE)) {
				$message = "Changes saved for distro did $did ($vendor $version).";
			} else {
				$message = "Could not save changes to distro did $did ($vendor $version).";
			}
			$action = "search";
		} elseif ($button == "Cancel") {
			$message = "Edit cancelled.";
			$action = "";
		}
	}

	if ($action == "delete") {
		if ($button == "delete") {
			$sql = "SELECT did,vendor,abbr,version,dname,title,body FROM rpm_distro WHERE did = $did";
			$table = get_table($sql);
			$distro = $table[$did];
			$message = "Delete this distro?";
			$output .= distro_display($action, $did, $distro['vendor'], $distro['abbr'], $distro['version'], $distro['dname'], $distro['title'], $distro['body'], FALSE);
		} elseif ($button == "OK") {
			if (distro_delete($did)) {
				$message = "Distro id $did deleted.";
			} else {
				$message = "Could not delete distro id $did.";
			}
			$action = "";
		} elseif ($button == "Cancel") {
			$message = "Delete cancelled.";
			$action = "";
		}
	}

	if (($action == "") || ($action == "search")) {
		$output .= '<form name="focus" action="' . $_SERVER['PHP_SELF'] . '" method="post">';
		$output .= '<input type="text"   name="vendor" value="' . $vendor . '">';
		$output .= '<input type="submit" name="action" value="search" >';
		$output .= '<input type="submit" name="action" value="create" >';
		$output .= '</form>';
	}

	if (!empty($message)) {
		$output .= "<b style='color: red'>$message</b><br>";
	}

	if ($action == "search") {
		$sql = "SELECT did,vendor,abbr,version,dname FROM rpm_distro";

		if (!empty($did)) {
			$sql .= " WHERE did=$did";
		} else {
			$vendor = htmlentities($vendor);
			$sql .= " WHERE vendor LIKE '%$vendor%' OR abbr LIKE '%$vendor%' OR version LIKE '%$vendor%' OR dname LIKE '%$vendor%'";
		}
		$sql .= " ORDER BY vendor,version";

		$distros = get_table($sql);

		$count = count($distros);
		if ($count == 0) {
			$output .= "No matches."; 
		} else {
			if (empty($did)) {
				$output .= $count . ' match';
				if ($count != 1) {
					$output .= 'es';
				}
			}

			$output .= '<table summary="search results" border="1" cellspacing="0" cellpadding="3">';
			$output .= '<tr><th>ID</th><th>Vendor</th><th>Abbr</th><th>Version</th><th>Name</th><th>Actions</th></tr>';

			foreach($distros as $line) {
				$output .= '<tr>';
				$output .= '<td>' . $line['did']     . '</td>';
				$output .= '<td>' . $line['vendor']  . '</td>';
				$output .= '<td>' . $line['abbr']    . '</td>';
				$output .= '<td>' . $line['version'] . '</td>';
				$output .= '<td>' . $line['dname']   . '</td>';
				$output .= '<td>';
				$output .= '<form action="' . $_SERVER['PHP_SELF'] . '" method="post">';
				$output .= '<input type="hidden" name="did"    value="' . $line['did'] . '">';
				$output .= '<input type="submit" name="action" value="edit"   >';
				$output .= '<input type="submit" name="action" value="delete" >';
				$output .= '</form>';
				$output .= '</td>';
				$output .= '</tr>';
			}

			$output .= '</table>';
		}
	}

	mysql_close($connection);

	$output .= '</div>';
	$output .= "</body>\n";
	$output .= "</html>\n";

	echo $output;
}


distro_main();

?>

