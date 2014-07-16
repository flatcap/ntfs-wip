<?php

include_once 'utils.php';

function section_create($brief, $title, $body, $thanks, $sdo)
{
	$sql = "INSERT INTO rpm_section (brief,title,body,thanks,sdo) VALUES ('$brief','$title','$body','$thanks','$sdo')";
	$result = mysql_query($sql) or die("query failed: " . mysql_error());

	if ($result) {
		return mysql_insert_id();
	} else {
		return 0;
	}
}

function section_update($sid, $brief, $title, $body, $thanks, $sdo)
{
	$sql = "UPDATE rpm_section SET brief='$brief',title='$title',body='$body',thanks='$thanks',sdo='$sdo' WHERE sid='$sid'";
	$result = mysql_query($sql) or die("query failed: " . mysql_error());

	return $result;
}

function section_delete($sid)
{
	$sql = "DELETE FROM rpm_section WHERE sid = '$sid'";
	$result = mysql_query($sql) or die("query failed: " . mysql_error());

	return $result;
}

function section_display($action, $sid, $brief, $title, $body, $thanks, $sdo, $readonly)
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
	$output .= "<th>Brief</th>";
	$output .= "<td><input size='30' type='text' name='brief' $ro value='$brief'></td>";
	$output .= "</tr>";

	$output .= "<tr>";
	$output .= "<th>Thanks</th>";
	$output .= "<td><input size='30' type='text' name='thanks' $ro value='$thanks'></td>";
	$output .= "</tr>";

	$output .= "<tr>";
	$output .= "<th>Display Order</th>";
	$output .= "<td><input size='30' type='text' name='sdo' $ro value='$sdo'></td>";
	$output .= "</tr>";

	$output .= "<tr>";
	$output .= "<th>Title</th>";
	$output .= "<td><input size='30' type='text' name='title' $ro value='$title'></td>";
	$output .= "</tr>";

	$output .= "<tr>";
	$output .= "<th>Body</th>";
	$output .= "<td><input size='30' type='text' name='body' $ro value='$body'></td>";
	$output .= "</tr>";

	$output .= "<tr>";
	$output .= "<td colspan=2'>";
	$output .= "<input type='hidden' name='action' value='$action'>";
	$output .= "<input type='hidden' name='sid'    value='$sid'>";
	$output .= "<input type='submit' name='button' value='OK'>";
	$output .= "<input type='submit' name='button' value='Cancel'>";
	$output .= "</td>";
	$output .= "</tr>";
	$output .= "</form>";

	return $output;
}

function section_main()
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
	$output .= '<title>Section Manager</title>';
	$output .= "</head>\n";
	$output .= "<body onload='sf()'>\n";

	$output .= layout_header();
	$output .= layout_menu($_SERVER['PHP_SELF']);

	$output .= '<div class="content">';
	$output .= '<h2>Section Manager</h2>';

	$action  = get_post_variable('action');
	$button  = get_post_variable('button');
	$sid     = get_post_variable('sid');
	$brief   = get_post_variable('brief');
	$title   = get_post_variable('title');
	$body    = get_post_variable('body');
	$thanks  = get_post_variable('thanks');
	$sdo     = get_post_variable('sdo');
	$message = "";

	if ($action == "create") {
		if ($button == "OK") {
			$sid = section_create($brief, $title, $body, $thanks, $sdo);
			if ($sid > 0) {
				$message = "Created new section id $sid ($brief).";
				$action = "search";
			} else {
				$message = "Could not create new section: $brief.";
				$action = "";
			}
		} elseif ($button == "Cancel") {
			$message = "Create cancelled.";
			$action = "";
		} else {
			$output .= section_display($action, 0, $brief, $title, $body, $thanks, $sdo, FALSE);
		}
	}

	if ($action == "edit") {
		if ($button == "edit") {
			$sql = "SELECT sid,brief,title,body,thanks,sdo FROM rpm_section WHERE sid = '$sid'";
			$table = get_table($sql);
			$sect = $table[$sid];
			$output .= section_display($action, $sid, $sect['brief'], $sect['title'], $sect['body'], $sect['thanks'], $sect['sdo'], FALSE);
		} elseif ($button == "OK") {
			if (section_update($sid, $brief, $title, $body, $thanks, $sdo)) {
				$message = "Saved changes to section id $sid ($brief).";
			} else {
				$message = "Could not save changes to section id $sid ($brief).";
			}
			$action = "search";
		} elseif ($button == "Cancel") {
			$message = "Edit cancelled.";
			$action = "";
		}
	}

	if ($action == "delete") {
		if ($button == "delete") {
			$sql = "SELECT sid,brief,title,body,thanks,sdo FROM rpm_section WHERE sid = '$sid'";
			$table = get_table($sql);
			$sect = $table[$sid];
			$message = "Delete this section?";
			$output .= section_display($action, $sid, $sect['brief'], $sect['title'], $sect['body'], $sect['thanks'], $sect['sdo'], TRUE);
		} elseif ($button == "OK") {
			if (section_delete($sid)) {
				$message = "section id $sid deleted.";
			} else {
				$message = "Could not delete section id $sid.";
			}
			$action = "";
		} elseif ($button == "Cancel") {
			$message = "Delete cancelled.";
			$action = "";
		}
	}

	if (($action == "") || ($action == "search")) {
		$output .= '<form name="focus" action="' . $_SERVER['PHP_SELF'] . '" method="post">';
		$output .= '<input type="text"   name="brief"  value="' . $brief . '">';
		$output .= '<input type="submit" name="action" value="search" >';
		$output .= '<input type="submit" name="action" value="create" >';
		$output .= '</form>';
	}

	if (!empty($message)) {
		$output .= "<b style='color: red'>$message</b><br>";
	}

	if ($action == "search") {
		$sql = "SELECT sid,brief,title,body,thanks,sdo FROM rpm_section";

		if (!empty($sid)) {
			$sql .= " WHERE sid='$sid'";
		} else {
			$brief = htmlentities($brief);
			if (!empty($brief)) {
				$sql .= " WHERE brief LIKE '%$brief%'";
				$sql .= " OR title  LIKE '%$brief%'";
				$sql .= " OR body   LIKE '%$brief%'";
				$sql .= " OR thanks LIKE '%$brief%'";
			}
		}

		$sql .= " ORDER BY sdo";

		$sections = get_table($sql);

		$count = count($sections);
		if ($count == 0) {
			$output .= 'No matches'; 
		} else {
			if (empty($sid)) {
				$output .= $count . ' match';
				if ($count != 1) {
					$output .= 'es';
				}
			}

			$output .= '<table summary="search results" border="1" cellspacing="0" cellpadding="3">';
			$output .= '<tr><th>ID</th><th>Brief</th><th>Title</th><th>Body</th><th>Thanks</th><th>Display Order</th><th>Actions</th></tr>';

			foreach($sections as $line) {
				$output .= '<tr>';
				$output .= '<td>' . $line['sid']     . '</td>';
				$output .= '<td>' . $line['brief']   . '</td>';
				$output .= '<td>' . $line['title']   . '</td>';
				$output .= '<td>' . $line['body']   . '</td>';
				$output .= '<td>' . $line['thanks']   . '</td>';
				$output .= '<td>' . $line['sdo']   . '</td>';
				$output .= '<td>';
				$output .= '<form action="' . $_SERVER['PHP_SELF'] . '" method="post">';
				$output .= '<input type="hidden" name="sid"  value="' . $line['sid'] . '">';
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


section_main();

?>

