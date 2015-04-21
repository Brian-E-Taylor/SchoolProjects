<?php

include('a3.rss-feeds.php');

$db_user = "user";
$db_pass = "pass";
$db_name = "db";

$db_obj = new mysqli("localhost", $db_user, $db_pass, $db_name);

if ($db_obj->connect_errno) {
    die("<hr/>Failed to connect to MySQL: " . $db_obj->connect_error);

}
$feed_url = array();
date_default_timezone_set("EST");

$query = "SELECT * FROM `Feeds`";
$result = $db_obj->query($query);
if (!$result)
{
	die("Invalid query.<br>\n");
}
else
{
	while ($row = $result->fetch_assoc())
	{
		$feed_id[$row['feed_id']] = $row['feed_id'];
		$feed_url[$row['feed_id']] = $row['feed_url'];
		$feed_name[$row['feed_id']] = $row['feed_name'];
	}
}

foreach ($rss_feeds as $feed)
{
	if (!in_array($feed, $feed_url))
	{
		$xml = simplexml_load_file($feed);
		$name = $db_obj->real_escape_string($xml->channel[0]->title);
		$query = "INSERT INTO `Feeds` (`feed_name`, `feed_url`, `feed_id`) VALUES ('$name', '$feed', NULL)";
		$result = $db_obj->query($query);
		if (!$result)
		{
			die("Invalid query.<br>\n");
		}
	}
}

$query = "SELECT * FROM `Feeds`";
$result = $db_obj->query($query);
if (!$result)
{
	die("Invalid query.<br>\n");
}
else
{
	while ($row = $result->fetch_assoc())
	{
		$feed_id[$row['feed_id']] = $row['feed_id'];
		$feed_url[$row['feed_id']] = $row['feed_url'];
		$feed_name[$row['feed_id']] = $row['feed_name'];
	}
}

foreach ($feed_id as $feed)
{
	$xml = simplexml_load_file($feed_url[$feed]);
	foreach ($xml->channel[0]->item as $item)
	{
		$title = $db_obj->real_escape_string($item->title);
		$link = $db_obj->real_escape_string($item->link);
		$date = date('Y-m-d H:i:s', strtotime($item->pubDate));
		$query = "SELECT * FROM `Entries` WHERE `feed_id` = '$feed' AND `item_link` = '$link'";
		$result = $db_obj->query($query);
		if ($result->num_rows == 0)
		{
			if (!empty($title))
			{
				$query = "INSERT INTO `Entries` (`feed_id`, `item_title`, `item_link`, `item_date`) VALUES ('$feed', '$title', '$link', '$date')";
				$result = $db_obj->query($query);
				if (!$result)
				{
					die("Invalid query.<br>\n");
				}
			}
		}
	}
}

echo "RSS feeds and entries imported.<br>\n";

?>
