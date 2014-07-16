<?php

defined ('_VALID_MOS') or die ('Direct Access to this location is not allowed.');

function db_get_table($sql)
{
	global $database;

	$database->setQuery($sql);
	if (!$database->query()) {
		echo $database->stderr (TRUE);
		return FALSE;
	}

	$results = $database->loadObjectList('pid');

	$table = array();
	foreach ($results as $pid => $row) {
		$table[$pid] = array();
		foreach ($row as $name => $value) {
			$table[$pid][$name] = $value;
		}
	}

	return $table;
}

function db_set_template($id, $html)
{
	global $database;

	$sql = "UPDATE #__content as c SET c.fulltext='$html' WHERE c.id='$id'";
	$database->setQuery($sql);
	if (!$database->query()) {
		echo $database->stderr (TRUE);
		return FALSE;
	}
}


function person_create($name, $nick, $email, $web)
{
	$sql = "INSERT INTO rpm_person (pname,nickname,email,webpage) VALUES ('$name','$nick','$email','$web')";
	$result = mysql_query($sql) or die("query failed: " . mysql_error());

	if ($result) {
		return mysql_insert_id();
	} else {
		return 0;
	}
}

function person_update($id, $name, $nick, $email, $web)
{
	$sql = "update rpm_person set pname='$name',nickname='$nick',email='$email',webpage='$web' WHERE pid=$id";
	$result = mysql_query($sql) or die("query failed: " . mysql_error());

	return $result;
}

function person_delete($id)
{
	$sql = "DELETE FROM rpm_person WHERE pid = $id";
	$result = mysql_query($sql) or die("query failed: " . mysql_error());

	return $result;
}

function person_display($self, $action, $id, $name, $nick, $email, $web, $write)
{
	$output = "";

	if ($write) {
		$ro = "";
	} else {
		$ro = "readonly";
	}

	$output .= "<form name='focus' action='$self' method='post'>";
	$output .= "<table summary='$action person' border='0' cellspacing='0'>";
	$output .= "<tr>";
	$output .= "<th>Name</th>";
	$output .= "<td><input size='30' type='text' name='name' $ro value='$name'></td>";
	$output .= "</tr>";
	$output .= "<tr>";
	$output .= "<th>Nickname</th>";
	$output .= "<td><input size='30' type='text' name='nick' $ro value='$nick'></td>";
	$output .= "</tr>";
	$output .= "<tr>";
	$output .= "<th>Email</th>";
	$output .= "<td><input size='60' type='text' name='email' $ro value='$email'></td>";
	$output .= "</tr>";
	$output .= "<tr>";
	$output .= "<th>Web page</th>";
	$output .= "<td><input size='60' type='text' name='web' $ro value='$web'></td>";
	$output .= "</tr>";
	$output .= "<tr>";
	$output .= "<td colspan=2'>";
	$output .= "<input type='hidden' name='action' value='$action'>";
	$output .= "<input type='hidden' name='id'     value='$id'>";
	$output .= "<input type='submit' name='param'  value='OK'>";
	$output .= "<input type='submit' name='param'  value='Cancel'>";
	$output .= "</td>";
	$output .= "</tr>";
	$output .= "</form>";

	return $output;
}

function person_search($param)
{
	$sql = "SELECT pid,pname,nickname,email,webpage FROM rpm_person";

	$param = htmlentities($param);
	if (is_numeric($param)) {
		$sql .= " WHERE pid=$param";
	} else {
		$sql .= " WHERE pname LIKE '%$param%' OR email LIKE '%$param%' OR webpage LIKE '%$param%'";
	}

	$sql .= " ORDER BY pname";

	$people = db_get_table($sql);
	if ($people === FALSE) {
		return FALSE;
	}

	$output = "";

	$count = count($people);
	if ($count == 0) {
		$output .= 'No matches for "' . $param . '".'; 
	} else {
		if (!is_numeric($param)) {
			if (isset($param)) {
				$output .= $count . ' matches for "' . $param . '".';
			} else {
				$output .= $count . ' matches';
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
			$output .= '<input type="hidden" name="param"  value="' . $line['pid'] . '">';
			$output .= '<input type="submit" name="action" value="edit"   >';
			$output .= '<input type="submit" name="action" value="delete" >';
			$output .= '</form>';
			$output .= '</td>';
			$output .= '</tr>';
		}
		$output .= '</table>';
	}

	return $output;
}


function person_main()
{
	$output = "";
	$action  = "";
	$param   = "";
	$message = "";

	if (isset($_POST)) {
		if (array_key_exists('action', $_POST))
			$action = $_POST['action'];
		if (array_key_exists('param', $_POST))
			$param = $_POST['param'];
	}

	if ($action == "create") {
		if ($param == "OK") {
			$id = person_create($_POST['name'], $_POST['nick'], $_POST['email'], $_POST['web']);
			if ($id > 0) {
				$message = "Created new user id $id (" . $_POST['name'] . ").";
				$action = "search";
				$param  = $id;
			} else {
				$message = "Could not create new user: " . $_POST['name'] . ".";
			}
		} elseif ($param == "Cancel") {
			$message = "Create cancelled.";
			$action = "";
		} else {
			person_display($action, 0, "", "", "", "", TRUE);
		}
	}

	if ($action == "edit") {
		if (is_numeric($param)) {
			$sql = "SELECT pid,pname,nickname,email,webpage FROM rpm_person WHERE pid = $param";
			$table = get_table($sql);
			$person = $table[$param];
			person_display($action, $param, $person['pname'], $person['nickname'], $person['email'], $person['webpage'], TRUE);
		} elseif ($param == "OK") {
			$id   = $_POST['id'];
			$name = $_POST['name'];
			if (person_update($id, $name, $_POST['nick'], $_POST['email'], $_POST['web'])) {
				$message = "Changes saved for user id $id ($name).";
			} else {
				$message = "Could not save changes to user id $id ($name).";
			}
			$action = "search";
			$param  = $_POST['id'];
		} elseif ($param == "Cancel") {
			$message = "Edit cancelled.";
			$action = "";
		}
	}

	if ($action == "delete") {
		if (is_numeric($param)) {
			$sql = "SELECT pid,pname,nickname,email,webpage FROM rpm_person WHERE pid = $param";
			$table = get_table($sql);
			$person = $table[$param];
			$message = "Delete this user?";
			person_display($action, $param, $person['pname'], $person['nickname'], $person['email'], $person['webpage'], FALSE);
		} elseif ($param == "OK") {
			$id = $_POST['id'];
			if (person_delete($id)) {
				$message = "User id $id deleted.";
			} else {
				$message = "Could not delete user id $id.";
			}
			$action = "";
		} elseif ($param == "Cancel") {
			$message = "Delete cancelled.";
			$action = "";
		}
	}

	if (($action == "") || ($action == "search")) {
		$output .= '<form name="focus" action="' . $_SERVER['PHP_SELF'] . '" method="post">';
		$output .= '<input type="text"   name="param"  value="">';
		$output .= '<input type="submit" name="action" value="search" >';
		$output .= '<input type="submit" name="action" value="create" >';
		$output .= '</form>';
	}

	if (!empty($message)) {
		$output .= "<b style='color: red'>$message</b><br>";
	}

	if ($action == "search") {
		$sql = "SELECT pid,pname,nickname,email,webpage FROM rpm_person";

		if (isset($param)) {
			$param = htmlentities($param);
			if (is_numeric($param)) {
				$sql .= " WHERE pid=$param";
			} else {
				$sql .= " WHERE pname LIKE '%$param%' OR email LIKE '%$param%' OR webpage LIKE '%$param%'";
			}
		}
		$sql .= " ORDER BY pname";

		$people = get_table($sql);

		$count = count($people);
		if ($count == 0) {
			$output .= 'No matches for "' . $param . '".'; 
		} else {
			if (!is_numeric($param)) {
				if (isset($param)) {
					$output .= $count . ' matches for "' . $param . '".';
				} else {
					$output .= $count . ' matches';
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
				$output .= '<input type="hidden" name="param"  value="' . $line['pid'] . '">';
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

	echo $output;
}


$self = sefRelToAbs ('index.php?option=com_rpm');

echo "<form action='$self' method='post'>";
echo "<input size='30' type='text' name='id' value=''>";
echo "<input type='submit' class='button' type='button' value='search'>";
echo "</form>";
echo "<br>";

$id = mosGetParam($_REQUEST, 'id', '');
$people = person_search($id);
if ($people === FALSE) {
	return;
}

db_set_template(27, $people);
//echo $people;

echo "<pre>";
var_dump($_REQUEST);
echo "</pre>";

?>
