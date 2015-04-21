<?php
$db_user = "user";
$db_pass = "pass";
$db_name = "db";

$db_obj = new mysqli("localhost", $db_user, $db_pass, $db_name);

if ($db_obj->connect_errno) {
    die("<hr/>Failed to connect to MySQL: " . $db_obj->connect_error);
}
date_default_timezone_set("EST");
	
?>

<HTML>
<BODY>
<FORM action="rss-search.php" method="post">
<INPUT TYPE="text" size="80" name="searchtext"><br>
<INPUT TYPE="submit" name="submit" value="Search for Text"><br>
</FORM>
<br>

<?php

if (isset($_POST['submit']))
{
	if (empty($_POST['searchtext']))
	{
		// Print all entries, sorted by most recent
		$i = 1;
		echo "<table border=\"1\" cellpadding=\"2\" cellspacing=\"0\">\n";
		echo "<th>#</th><th>Feed Name</th><th>Title</th><th>Date</th>\n";
		$query = "SELECT `Entries`.`feed_id`,`Entries`.`item_link`,`Entries`.`item_title`,`Entries`.`item_date`,`Feeds`.`feed_name` FROM `Entries` JOIN `Feeds` ON `Entries`.`feed_id` = `Feeds`.`feed_id` ORDER BY `item_date` DESC";
		$result = $db_obj->query($query);
		while ($row = $result->fetch_assoc())
		{
			$feed_name = $row['feed_name'];
			$url = $row['item_link'];
			$title = $row['item_title'];
			$date = date('Y-m-d H:i:s', strtotime($row['item_date']));
			echo "<tr>\n";
			echo "	<td>$i</td><td>$feed_name</td><td><a href=\"$url\">$title</a></td><td>$date EST</td>\n";
			echo "</tr>\n";
			$i++;
		}
		echo "</table>\n";
	}
	else
	{
		// Print matching entries, sorted by most recent
		$i = 1;
		$search = $db_obj->real_escape_string($_POST['searchtext']);
		echo "<table border=\"1\" cellpadding=\"2\" cellspacing=\"0\">\n";
		echo "<th>#</th><th>Feed Name</th><th>Title</th><th>Date</th>\n";
		$query = "SELECT `Entries`.`feed_id`,`Entries`.`item_link`,`Entries`.`item_title`,`Entries`.`item_date`,`Feeds`.`feed_name` FROM `Entries` JOIN `Feeds` ON `Entries`.`feed_id` = `Feeds`.`feed_id` WHERE `Entries`.`item_title` LIKE '%$search%' ORDER BY `item_date` DESC";
		$result = $db_obj->query($query);
		while ($row = $result->fetch_assoc())
		{
			$feed_name = $row['feed_name'];
			$url = $row['item_link'];
			$title = $row['item_title'];
			$date = date('Y-m-d H:i:s', strtotime($row['item_date']));
			echo "<tr>\n";
			echo "	<td>$i</td><td>$feed_name</td><td><a href=\"$url\">$title</a></td><td>$date EST</td>\n";
			echo "</tr>\n";
			$i++;
		}
		echo "</table>\n";
	
	}
}

?>
