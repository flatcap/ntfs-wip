<?php

function layout_header()
{
	$output = "";

	$output .= '<div class="header">';
	$output .= '<h1>Linux-NTFS RPM Management</h1>';
	$output .= '</div>';

	return $output;
}

function layout_menu($page)
{
	$page = basename($page);

	if ($page == 'people.php')  $m1 = " class='selected'"; else $m1 = "";
	if ($page == 'distro.php')  $m2 = " class='selected'"; else $m2 = "";
	if ($page == 'section.php') $m3 = " class='selected'"; else $m3 = "";
	if ($page == 'release.php') $m4 = " class='selected'"; else $m4 = "";
	if ($page == 'addnew.php')  $m5 = " class='selected'"; else $m5 = "";
	if ($page == 'ignore.php')  $m6 = " class='selected'"; else $m6 = "";
	if ($page == 'review.php')  $m7 = " class='selected'"; else $m7 = "";
	if ($page == 'html.php')    $m8 = " class='selected'"; else $m8 = "";
	if ($page == 'thanks.php')  $m9 = " class='selected'"; else $m9 = "";

	$output = '';

	$output .= "<div class='menu'>";
	$output .= "<h1>Setup</h1>";
	$output .= "  <ul>";
	$output .= "    <li{$m1}><a href='people.php'>People</a></li>";
	$output .= "    <li{$m2}><a href='distro.php'>Distros</a></li>";
	$output .= "    <li{$m3}><a href='section.php'>Sections</a></li>";
	$output .= "    <li{$m4}><a href='release.php'>Releases</a></li>";
	$output .= "  </ul>";
	$output .= "<h1>Files</h1>";
	$output .= "  <ul>";
	$output .= "    <li{$m5}><a href='addnew.php'>Add New</a></li>";
	$output .= "    <li{$m6}><a href='ignore.php'>Ignore</a></li>";
	$output .= "  </ul>";
	$output .= "<h1>Output</h1>";
	$output .= "  <ul>";
	$output .= "    <li{$m7}><a href='review.php'>Review</a></li>";
	$output .= "    <li{$m8}><a href='html.php'>HTML</a></li>";
	$output .= "    <li{$m9}><a href='thanks.php'>Thanks</a></li>";
	$output .= "  </ul>";
	$output .= "<h1>Links</h1>";
	$output .= "  <ul>";
	$output .= "    <li><a href='http://linux-ntfs.org'>Linux-NTFS</a></li>";
	$output .= "    <li><a href='http://sourceforge.net/projects/linux-ntfs'>Sourceforge</a></li>";
	$output .= "  </ul>";
	$output .= "</div>";

	return $output;
}


?>

