<?php

include_once 'utils.php';

function count_contrib_ppl()
{
	$sql = "SELECT COUNT(*) AS count FROM rpm_person";

	return get_count($sql);
}

function count_contrib_rpm()
{
	$sql = "SELECT COUNT(*) AS count FROM rpm_file WHERE pid!=0 AND pid!=46 AND pid!=8 AND fname NOT LIKE '%src%' AND fname NOT LIKE '%source%' AND fname LIKE '%.rpm'";

	return get_count($sql);
}

function thanks_top_contrib($max)
{
	$max += 2;
	$sql = "SELECT COUNT(*) as count,pname,nickname,email,webpage FROM rpm_file " .
		"LEFT JOIN rpm_person ON rpm_file.pid=rpm_person.pid WHERE fname LIKE " .
		"'%.rpm' AND fname NOT LIKE '%src%' AND fname NOT LIKE '%source%' AND " .
		"aid!=0 GROUP BY rpm_file.pid ORDER BY count DESC LIMIT $max";
		
	$list = get_table_num($sql);

	$output = "";
	$output .= "Rich ({$list[0]['count']}) &amp; ";
	$output .= "Chris ({$list[1]['count']}), blah blah";

	$output .= "<table border='1' cellspacing='0' cellpadding='3'>";
	$output .= "<tr>";
	$output .= "<th>Count</th>";
	$output .= "<th>Name</th>";
	$output .= "<th>Email</th>";
	$output .= "</tr>";

	for ($i = 2; $i < $max; $i++) {
		$line = $list[$i];
		$output .= "<tr>";
		$output .= "<td>" . $line['count'] . "</td>";
		$output .= "<td>" . get_name($line) . "</td>";
		$output .= "<td><a href='mailto:" . $line['email'] . "'>" . $line['email'] . "</a></td>";
		$output .= "</tr>";
	}

	$output .= "</table>";

	return $output;
}

function thanks_get_names()
{
	$cols = 3;
	$output = "";

	$sql = "SELECT pid,pname,nickname,email,webpage FROM rpm_person ORDER BY pname";
	$people = get_table($sql);
	$count = intval((count($people) + ($cols-1)) / $cols);

	$col1 = array_slice($people, 0,        $count);
	$col2 = array_slice($people, 1*$count, $count);
	$col3 = array_slice($people, 2*$count, $count);
	//$col4 = array_slice($people, 3*$count, $count);

	$output .= "<table width='100%' border='0' cellspacing='0' cellpadding='0'>\n";

	for ($i = 0; $i < $count; $i++) {
		$output .= "  <tr>\n";
		$output .= "    <td>" . get_name(current($col1)) . "</td>\n";
		$output .= "    <td>" . get_name(current($col2)) . "</td>\n";
		$output .= "    <td>" . get_name(current($col3)) . "</td>\n";
		//$output .= "    <td>" . get_name(current($col4)) . "</td>\n";
		$output .= "  </tr>\n";
		next($col1);
		next($col2);
		next($col3);
		//next($col4);
	}
	$output .= "</table>\n";

	return $output;
}

function thanks_main()
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
	$output .= "function sf(){document.focus.html.focus();}\n";
	$output .= "// -->\n";
	$output .= "</script>\n";
	$output .= '<link rel="stylesheet" href="style.css" type="text/css">';
	$output .= '<title>Thanks</title>';
	$output .= "</head>\n";
	$output .= "<body onload='sf()'>\n";

	$output .= layout_header();
	$output .= layout_menu($_SERVER['PHP_SELF']);

	$output .= '<div class="content">';
	$output .= '<h2>Thanks</h2>';

	$output .= "<h3>Top Ten</h3>";
	$output .= thanks_top_contrib(10);
	$output .= "<br>";

	$output .= count_contrib_ppl() . " contributors<br>";
	$output .= count_contrib_rpm() . " contributed rpms<br>";
	$output .= "<br>";

	/*
	$thanks = thanks_get_names();
	$output .= $thanks;

	$output .= "<br>";
	$output .= "<br>";

	$output .= "<form name='focus' action='' method='post'>";
	$output .= "<textarea name='html' cols='100' rows='20'>";
	$output .= htmlentities($thanks);
	$output .= "</textarea>";
	$output .= "</form>";
	*/

	mysql_close($connection);

	$output .= '</div>';
	$output .= "</body>\n";
	$output .= "</html>\n";

	echo $output;
}


thanks_main();

?>

