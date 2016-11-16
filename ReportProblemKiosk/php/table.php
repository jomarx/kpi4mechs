<!DOCTYPE html>
<html>
<body>

<?php
echo "<table style='border: sollocation 1px black;'>";
  echo "<tr><th>location</th><th>Assignee</th><th>StartTime</th><th>EndTime</th><th>Job Code</th></tr>";

class TableRows extends RecursiveIteratorIterator { 
     function __construct($it) { 
         parent::__construct($it, self::LEAVES_ONLY); 
     }

     function current() {
         return "<td style='wlocationth: 150px; border: 1px sollocation black;'>" . parent::current(). "</td>";
     }

     function beginChildren() { 
         echo "<tr>"; 
     } 

     function endChildren() { 
         echo "</tr>" . "\n";
     } 
} 

$servername = "192.168.42.85";
$username = "nodemcu1";
$password = "secret";
$dbname = "kpi_mech";

try {
     $conn = new PDO("mysql:host=$servername;dbname=$dbname", $username, $password);
     $conn->setAttribute(PDO::ATTR_ERRMODE, PDO::ERRMODE_EXCEPTION);
     $stmt = $conn->prepare("SELECT location, Assignee, StartTime, EndTime, details FROM task_db"); 
     $stmt->execute();

     // set the resulting array to associative
     $result = $stmt->setFetchMode(PDO::FETCH_ASSOC); 

     foreach(new TableRows(new RecursiveArrayIterator($stmt->fetchAll())) as $k=>$v) { 
         echo $v;
     }
}
catch(PDOException $e) {
     echo "Error: " . $e->getMessage();
}
$conn = null;
echo "</table>";
?>  

</body>
</html>
// Create connection
$conn = new mysqli($servername, $username, $password, $dbname);
// Check connection
if ($conn->connect_error) {
     die("Connection failed: " . $conn->connect_error);
} 

$sql = "SELECT location, Assignee, StartTime, EndTime, details FROM task_db";
$result = $conn->query($sql);

if ($result->num_rows > 0) {
     echo "<table><tr><th>location</th><th>Name</th></tr>";
     // output data of each row
     while($row = $result->fetch_assoc()) {
         echo "<tr><td>" . $row["location"]. "</td><td>" . $row["Assignee"]. " </td><td>" . $row["StartTime"]. "</td><td>" . $row["EndTime"]. "</td><td>" . $row["details"]. "</td></tr>";
     }
     echo "</table>";
} else {
     echo "0 results";
}

$conn->close();
?>  

</body>
</html>