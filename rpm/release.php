<?php

include_once 'utils.php';

function release_create($did, $rname)
{
	$sql = "INSERT INTO rpm_release (did,rname) VALUES ('$did','$rname')";
	$result = mysql_query($sql) or die("query failed: " . mysql_error());

	if ($result) {
		return mysql_insert_id();
	} else {
		return 0;
	}
}

function release_update($rid, $did, $rname)
{
	$sql = "UPDATE rpm_release SET did='$did', rname='$rname' WHERE rid='$rid'";
	$result = mysql_query($sql) or die("query failed: " . mysql_error());

	return $result;
}

function release_delete($rid)
{
	$sql = "DELETE FROM rpm_release WHERE rid = '$rid'";
	$result = mysql_query($sql) or die("query failed: " . mysql_error());

	return $result;
}

function release_display($action, $rid, $did, $rname, $readonly)
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
	$output .= "<th>Distro</th>";
	$output .= "<td>" . get_distros($did, FALSE, $readonly) . "</td>";
	$output .= "</tr>";

	$output .= "<tr>";
	$output .= "<th>Release Name</th>";
	$output .= "<td><input size='30' type='text' name='rname' $ro value='$rname'></td>";
	$output .= "</tr>";

	$output .= "<tr>";
	$output .= "<td colspan=2'>";
	$output .= "<input type='hidden' name='action' value='$action'>";
	$output .= "<input type='hidden' name='rid'    value='$rid'>";
	$output .= "<input type='submit' name='button' value='OK'>";
	$output .= "<input type='submit' name='button' value='Cancel'>";
	$output .= "</td>";
	$output .= "</tr>";
	$output .= "</form>";

	return $output;
}

function release_main()
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
	$output .= "function sf(){document.focus.rname.focus();}\n";
	$output .= "// -->\n";
	$output .= "</script>\n";
	$output .= '<link rel="stylesheet" href="style.css" type="text/css">';
	$output .= '<title>Release Manager</title>';
	$output .= "</head>\n";
	$output .= "<body onload='sf()'>\n";

	$output .= layout_header();
	$output .= layout_menu($_SERVER['PHP_SELF']);

	$output .= '<div class="content">';
	$output .= '<h2>Release Manager</h2>';

	$action  = get_post_variable('action');
	$button  = get_post_variable('button');
	$did     = get_post_variable('did');
	$rname   = get_post_variable('rname');
	$rid     = get_post_variable('rid');
	$message = "";

	if ($action == "create") {
		if ($button == "OK") {
			$rid = release_create($did, $rname);
			if ($rid > 0) {
				$message = "Created new release id $rid ($rname).";
				$action = "search";
			} else {
				$message = "Could not create new release: $rname.";
				$action = "";
			}
		} elseif ($button == "Cancel") {
			$message = "Create cancelled.";
			$action = "";
		} else {
			$output .= release_display($action, 0, $did, $rname, FALSE);
		}
	}

	if ($action == "edit") {
		if ($button == "edit") {
			$sql = "SELECT rid,did,rname FROM rpm_release WHERE rid = '{$rid}'";
			$table = get_table($sql);
			$rel = $table[$rid];
			$output .= release_display($action, $rid, $rel['did'], $rel['rname'], FALSE);
		} elseif ($button == "OK") {
			if (release_update($rid, $did, $rname)) {
				$message = "Saved changes to release id $rid ($rname).";
			} else {
				$message = "Could not save changes to release id $rid ($rname).";
			}
			$action = "search";
		} elseif ($button == "Cancel") {
			$message = "Edit cancelled.";
			$action = "";
		}
	}

	if ($action == "delete") {
		if ($button == "delete") {
			$sql = "SELECT rid,did,rname FROM rpm_release WHERE rid = '{$rid}'";
			$table = get_table($sql);
			$rel = $table[$rid];
			$message = "Delete this release?";
			$output .= release_display($action, $rid, $rel['did'], $rel['rname'], TRUE);
		} elseif ($button == "OK") {
			if (release_delete($rid)) {
				$message = "Release id $rid deleted.";
			} else {
				$message = "Could not delete release id $rid.";
			}
			$action = "";
		} elseif ($button == "Cancel") {
			$message = "Delete cancelled.";
			$action = "";
		}
	}

	if (($action == "") || ($action == "search")) {
		$output .= '<form name="focus" action="' . $_SERVER['PHP_SELF'] . '" method="post">';
		$output .= get_distros($did, TRUE, FALSE);
		$output .= '<input type="text"   name="rname"  value="' . $rname . '">';
		$output .= '<input type="submit" name="action" value="search" >';
		$output .= '<input type="submit" name="action" value="create" >';
		$output .= '</form>';
	}

	if (!empty($message)) {
		$output .= "<b style='color: red'>$message</b><br>";
	}

	if ($action == "search") {
		$sql = "SELECT rid,vendor,version,dname,rname FROM rpm_release LEFT JOIN rpm_distro ON rpm_release.did = rpm_distro.did";

		if (!empty($rid)) {
			$sql .= " WHERE rpm_release.rid='$rid'";
		} else {
			$rname = htmlentities($rname);
			if (!empty($rname) || ($did > 0)) {
				$sql .= " WHERE ";
			}
			if (!empty($rname)) {
				$sql .= "rname LIKE '%$rname%'";
			}
			if (!empty($rname) && ($did > 0)) {
				$sql .= " AND ";
			}
			if ($did > 0) {
				$sql .= "rpm_release.did='$did'";
			}
		}

		$sql .= " ORDER BY vendor,version,rid";

		$releases = get_table($sql);

		$count = count($releases);
		if ($count == 0) {
			$output .= 'No matches'; 
		} else {
			if (empty($rid)) {
				$output .= $count . ' match';
				if ($count != 1) {
					$output .= 'es';
				}
			}

			$output .= '<table summary="search results" border="1" cellspacing="0" cellpadding="3">';
			$output .= '<tr><th>ID</th><th>Distro</th><th>Name</th><th>Actions</th></tr>';

			foreach($releases as $line) {
				$distro = "{$line['vendor']} {$line['version']} ({$line['dname']})";
				$output .= '<tr>';
				$output .= '<td>' . $line['rid']     . '</td>';
				$output .= '<td>' . $distro          . '</td>';
				$output .= '<td>' . $line['rname']   . '</td>';
				$output .= '<td>';
				$output .= '<form action="' . $_SERVER['PHP_SELF'] . '" method="post">';
				$output .= '<input type="hidden" name="rid"  value="' . $line['rid'] . '">';
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


release_main();

?>

