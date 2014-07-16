<?php

include_once 'utils.php';

function person_create($pname, $nickname, $email, $webpage)
{
	$sql = "INSERT INTO rpm_person (pname,nickname,email,webpage) VALUES ('$pname','$nickname','$email','$webpage')";
	$result = mysql_query($sql) or die("query failed: " . mysql_error());

	if ($result) {
		return mysql_insert_id();
	} else {
		return 0;
	}
}

function person_update($pid, $pname, $nickname, $email, $webpage)
{
	$sql = "UPDATE rpm_person SET pname='$pname',nickname='$nickname',email='$email',webpage='$webpage' WHERE pid=$pid";
	$result = mysql_query($sql) or die("query failed: " . mysql_error());

	return $result;
}

function person_delete($pid)
{
	$sql = "DELETE FROM rpm_person WHERE pid = $pid";
	$result = mysql_query($sql) or die("query failed: " . mysql_error());

	return $result;
}

function person_display($action, $pid, $pname, $nickname, $email, $webpage, $readonly)
{
	$output = "";

	if ($readonly) {
		$ro = "readonly";
	} else {
		$ro = "";
	}

	$output .= "<form name='focus' action='" . $_SERVER['PHP_SELF'] . "' method='post'>";
	$output .= "<table summary='$action person' border='0' cellspacing='0'>";
	$output .= "<tr>";
	$output .= "<th>Name</th>";
	$output .= "<td><input size='30' type='text' name='pname' $ro value='$pname'></td>";
	$output .= "</tr>";
	$output .= "<tr>";
	$output .= "<th>Nickname</th>";
	$output .= "<td><input size='30' type='text' name='nickname' $ro value='$nickname'></td>";
	$output .= "</tr>";
	$output .= "<tr>";
	$output .= "<th>Email</th>";
	$output .= "<td><input size='60' type='text' name='email' $ro value='$email'></td>";
	$output .= "</tr>";
	$output .= "<tr>";
	$output .= "<th>Web page</th>";
	$output .= "<td><input size='60' type='text' name='webpage' $ro value='$webpage'></td>";
	$output .= "</tr>";
	$output .= "<tr>";
	$output .= "<td colspan=2'>";
	$output .= "<input type='hidden' name='action' value='$action'>";
	$output .= "<input type='hidden' name='pid'    value='$pid'>";
	$output .= "<input type='submit' name='button' value='OK'>";
	$output .= "<input type='submit' name='button' value='Cancel'>";
	$output .= "</td>";
	$output .= "</tr>";
	$output .= "</form>";

	return $output;
}

function person_main()
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
	$output .= "function sf(){document.focus.pname.focus();}\n";
	$output .= "// -->\n";
	$output .= "</script>\n";
	$output .= '<link rel="stylesheet" href="style.css" type="text/css">';
	$output .= '<title>Person Manager</title>';
	$output .= "</head>\n";
	$output .= "<body onload='sf()'>\n";

	$output .= layout_header();
	$output .= layout_menu($_SERVER['PHP_SELF']);

	$output .= '<div class="content">';
	$output .= '<h2>Person Manager</h2>';

	$action   = get_post_variable('action');
	$button   = get_post_variable('button');
	$pid      = get_post_variable('pid');
	$pname    = get_post_variable('pname');
	$nickname = get_post_variable('nickname');
	$email    = get_post_variable('email');
	$webpage  = get_post_variable('webpage');
	$message  = "";

	if ($action == "create") {
		if ($button == "OK") {
			$pid = person_create($pname, $nickname, $email, $webpage);
			if ($pid > 0) {
				$message = "Created new user id $pid ($pname).";
				$action = "search";
			} else {
				$message = "Could not create new user: $pname.";
			}
		} elseif ($button == "Cancel") {
			$message = "Create cancelled.";
			$action = "";
		} else {
			$output .= person_display($action, 0, "", "", "", "", FALSE);
		}
	}

	if ($action == "edit") {
		if ($button == "edit") {
			$sql = "SELECT pid,pname,nickname,email,webpage FROM rpm_person WHERE pid = '$pid'";
			$table = get_table($sql);
			$person = $table[$pid];
			$output .= person_display($action, $pid, $person['pname'], $person['nickname'], $person['email'], $person['webpage'], FALSE);
		} elseif ($button == "OK") {
			if (person_update($pid, $pname, $nickname, $email, $webpage)) {
				$message = "Changes saved for user id $pid ($pname).";
			} else {
				$message = "Could not save changes to user id $pid ($pname).";
			}
			$action = "search";
		} elseif ($button == "Cancel") {
			$message = "Edit cancelled.";
			$action = "";
		}
	}

	if ($action == "delete") {
		if ($button == "delete") {
			$sql = "SELECT pid,pname,nickname,email,webpage FROM rpm_person WHERE pid = $pid";
			$table = get_table($sql);
			$person = $table[$pid];
			$message = "Delete this user?";
			$output .= person_display($action, $pid, $person['pname'], $person['nickname'], $person['email'], $person['webpage'], TRUE);
		} elseif ($button == "OK") {
			if (person_delete($pid)) {
				$message = "User id $pid deleted.";
			} else {
				$message = "Could not delete user id $pid.";
			}
			$action = "";
		} elseif ($button == "Cancel") {
			$message = "Delete cancelled.";
			$action = "";
		}
	}

	if (($action == "") || ($action == "search")) {
		$output .= '<form name="focus" action="' . $_SERVER['PHP_SELF'] . '" method="post">';
		$output .= '<input type="text"   name="pname"  value="' . $pname . '">';
		$output .= '<input type="submit" name="action" value="search" >';
		$output .= '<input type="submit" name="action" value="create" >';
		$output .= '</form>';
	}

	if (!empty($message)) {
		$output .= "<b style='color: red'>$message</b><br>";
	}

	if ($action == "search") {
		$sql = "SELECT pid,pname,nickname,email,webpage FROM rpm_person";

		if (!empty($pid)) {
			$sql .= " WHERE pid=$pid";
		} else {
			$pname = htmlentities($pname);
			$sql .= " WHERE pname LIKE '%$pname%' OR email LIKE '%$pname%' OR webpage LIKE '%$pname%'";
		}
		$sql .= " ORDER BY pname";

		$people = get_table($sql);

		$count = count($people);
		if ($count == 0) {
			$output .= "No matches."; 
		} else {
			if (empty($pid)) {
				$output .= $count . ' match';
				if ($count != 1) {
					$output .= 'es';
				}
			}

			$output .= '<table summary="search results" border="1" cellspacing="0" cellpadding="3">';
			$output .= '<tr><th>ID</th><th>Name</th><th>Nickname</th><th>Email address</th><th>Web page</th><th>Actions</th></tr>';

			foreach($people as $line) {
				$output .= '<tr>';
				$output .= '<td>' . $line['pid']      . '</td>';
				$output .= '<td>' . $line['pname']    . '</td>';
				$output .= '<td>' . $line['nickname'] . '&nbsp;</td>';
				$output .= '<td><a href="mailto:' . $line['email']    . '">' . $line['email'] . '</a></td>';
				if (empty($line['webpage'])) {
					$output .= '<td>&nbsp;</td>';
				} else {
					$output .= '<td><a href="' . $line['webpage']  . '">' . $line['webpage'] . '</a></td>';
				}
				$output .= '<td>';
				$output .= '<form action="' . $_SERVER['PHP_SELF'] . '" method="post">';
				$output .= '<input type="hidden" name="pid"    value="' . $line['pid'] . '">';
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


person_main();

?>

