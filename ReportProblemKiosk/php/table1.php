<!DOCTYPE html>
<?php
$page = $_SERVER['PHP_SELF'];
$sec = "10";
?>
<html>
    <head>
    <meta http-equiv="refresh" content="<?php echo $sec?>;URL='<?php echo $page?>'">
    </head>
<body>

<?php
echo "<center>";
echo date("m/d/Y H:i:s");
echo "</center>";

$servername = "192.168.42.85";
$username = "nodemcu1";
$password = "secret";
$dbname = "kpi_mech";
$style1 = "<td style = 'wlocationth: 150px; border: 4px solid black; background-color: #ff6666'; font-size:'40px'>";//format if StartTime exceed $limit in minutes
$style2 = "<td style = 'wlocationth: 150px; border: 4px solid black; background-color: #ffffff'; font-size:'40px'>";//format otherwise
$limit = 15;

try {
	// Create connection
	$conn = new mysqli($servername, $username, $password, $dbname);
	// Check connection
	if ($conn->connect_error) {
		die("Connection failed: " . $conn->connect_error);
	}
	//Stat connection
	$sql = "SELECT location, Assignee, StartTime, EndTime, details FROM task_db WHERE status < 3";
	$result = $conn->query($sql);
	if ($result->num_rows > 0) {
		echo "<span style='font-size: 32pt'>";
		echo "<table style='border:4px solid black; width: 100%'>";
		//echo "<font size='30'>";
		echo "<tr><th>Location</th><th>Assignee</th><th>StartTime</th><th>EndTime</th><th>Breakdown Type</th></tr>";
		// output data of each row
		while($row = $result->fetch_assoc())
		{
			$to_time = strtotime(date("H:i:s"));
			$from_time = strtotime($row["StartTime"]);
			$duration = round(abs($to_time - $from_time) / 60,2);
			if ($duration > $limit){$style=$style1;}
			else{$style=$style2;}
			echo "<tr>";
			echo $style.$row["location"]."</td>";
			echo $style.$row["Assignee"]."</td>";
			echo $style.$row["StartTime"]."</td>";
			echo $style.$row["EndTime"]."</td>";
			echo $style.$row["details"]."</td>";
			echo "</tr>";
    		}
		echo "</span>";
		echo "</table>";
	} else {
    		echo "No results";
	}
}
catch(PDOException $e) {
     echo "Error: " . $e->getMessage();
}
$conn = null;
?>

</body>
</html>
