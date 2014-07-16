<?php

include_once 'utils.php';

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
	$output .= "function sf(){document.f.list.focus();}\n";
	$output .= "// -->\n";
	$output .= "</script>\n";
	$output .= "<link rel='stylesheet' href='style.css' type='text/css'>";
	$output .= "<title>Table Generator</title>";
	$output .= "</head>\n";
	$output .= "<body onload='sf()'>\n";

	$output .= "<h1>Function</h1>\n";

	if ($_POST) {
		$text = $_POST['list'];

		$text = html_entity_decode($text);
	} else {
		$text = "";
	}

	$output .= "<form name='f' action='{$_SERVER["PHP_SELF"]}' method='post'>";
	$output .= "<textarea name='list' cols='100' rows='20'>$text</textarea>";
	$output .= "<input type='submit' value='function'>";
	$output .= "</form>";

	$output .= "</body>\n";
	$output .= "</html>\n";

	echo $output;

	mysql_close($connection);
}


main();

?>

