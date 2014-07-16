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
	$output .= "function sf(){document.f.q.focus();}\n";
	$output .= "// -->\n";
	$output .= "</script>\n";
	$output .= "<link rel='stylesheet' href='style.css' type='text/css'>";
	$output .= "<title>Table Generator</title>";
	$output .= "</head>\n";
	$output .= "<body onload='sf()'>\n";

	$output .= display_menu();

	$distro_vendor = get_table("select did,vendor,abbr,version,dname from rpm_distro");
	//echo dump_table($distro_vendor, "Distro");

	$output .= "<h1>Table Generator</h1>\n";
	$output .= "<form name='f' action='{$_SERVER["PHP_SELF"]}' method='get'>";
	$output .= "<select name='q'>";

	if ($_GET) {
		$selection = $_GET['q'];
	} else {
		$selection = "";
	}

	if (empty($selection)) {
		$output .= "<option value=''>select a distro...</option>";
	}

	foreach ($distro_vendor as $line) {
		$menu = "{$line['vendor']} {$line['version']} ({$line['dname']})";
		$value = $line['abbr'] . $line['version'];
		if ($selection == $value) {
			$sel = "selected";
		} else {
			$sel = "";
		}
		$output .= "<option value='$value' $sel>$menu</option>";
	}

	$output .= '</select>';
	$output .= '<input type="submit" value="generate">';
	$output .= '</form>';

	if (!empty($selection)) {
		$distro = "";
		foreach ($distro_vendor as $line) {
			if ($selection == ($line['abbr'] . $line['version'])) {
				$distro =  $line['did'];
				break;
			}
		}

		// if !distro ...

		$arch_cat = get_table("select aid,aname,rpm_category.cid,cname from rpm_arch left join rpm_category on rpm_arch.cid=rpm_category.cid order by cid,aname");
		//echo dump_table($arch_cat, "Arch / Category");

		$sections = get_table("select sid,title,body,thanks from rpm_section order by sdo");
		//echo dump_table($sections, "Sections");

		$header = get_table("select did,body from rpm_distro where did = $distro");
		//var_dump($header);

		$output .= $header[$distro]['body'];

		foreach ($sections as $sect => $sline) {
			$name = $sline['title'];
			$sect = $sline['sid'];
			$thanks = $sline['thanks'];

			$arch_list = get_table("select distinct(rpm_file.aid) from rpm_file left join rpm_release on rpm_file.rid = rpm_release.rid left join rpm_distro on rpm_release.did = rpm_distro.did left join rpm_arch on rpm_file.aid=rpm_arch.aid where rpm_distro.did = $distro and rpm_file.sid = $sect order by aid");
			//echo dump_table($arch_list);

			if (count($arch_list) == 0)
				continue;

			$output .= "<h2>$name</h2>";
			$output .= $sline['body'];

			$cats = array();
			foreach ($arch_list as $aline) {
				$aid = $aline['aid'];
				$cat = $arch_cat[$aid]['cname'];
				//$output .= "aid = $aid, cat = $cat<br>";
				if (array_key_exists($cat, $cats)) {
					$cats[$cat]++;
				} else {
					$cats[$cat] = 1;
				}
			}

			$rels = get_table("select rname,rpm_file.rid,group_concat(aid) as aid from rpm_file left join rpm_release on rpm_file.rid=rpm_release.rid where sid=$sect and did=$distro group by rname order by rpm_release.rid desc");
			//echo dump_table($rels, "Releases");

			$output .= "<table summary='ntfs rpms' border='1' cellspacing='0'>";
			$output .= "<tr>";
			$output .= "<th>Version</th>";
			foreach ($cats as $type => $count) {
				$output .= "<th colspan=$count>$type</th>";
			}
			if ($thanks) {
				$output .= "<th>Thanks</th>";
			}
			$output .= "</tr>";

			foreach ($rels as $rline) {
				$output .= "<tr>";
				$rel = $rline['rname'];
				$rid = $rline['rid'];
				$output .= "<td>$rel</td>";
				$ra = explode(",", $rline['aid']);

				if ($thanks) {
					$thanks_list = get_table ("select distinct pname,nickname,webpage from rpm_file left join rpm_release on rpm_file.rid=rpm_release.rid left join rpm_person on rpm_file.pid = rpm_person.pid where rname='$rel' and sid = $sect");
					$thanks = "";
					foreach ($thanks_list as $t) {
						if (!empty($thanks)) {
							$thanks .= ", ";
						}
						$thanks .= get_name($t);
					}
				}

				$file_list = get_table("select aid,fname from rpm_file where rid=$rid and sid=$sect");
				//echo dump_table($file_list, "file_list");

				foreach ($arch_list as $aline) {
					$a = $aline['aid'];
					$n = $arch_cat[$a]['aname'];
					if (array_search($a, $ra) === FALSE) {
						$output .= "<td>&nbsp;</td>";
					} else {
						$base = "http://prdownloads.sourceforge.net/linux-ntfs";
						$n = "<a href='$base/{$file_list[$a]['fname']}'>$n</a>";
						$output .= "<td>$n</td>";
					}
				}

				if ($thanks) {
					$output .= "<td>$thanks</td>";
				}

				$output .= "</tr>";
			}

			$output .= "</table>";
		}
	}

	mysql_close($connection);

	$output .= "</body>\n";
	$output .= "</html>\n";

	echo $output;
	//echo "<pre>\n";
	//echo htmlentities($output);
	//echo "</pre>\n";
}


main();

?>

