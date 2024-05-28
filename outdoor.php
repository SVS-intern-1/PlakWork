<?php
$servername = "localhost";
$username = "root";
$password = "plak5212";
$dbname = "sensor_db";

// Create connection
$conn = new mysqli($servername, $username, $password, $dbname);

// Check connection
if ($conn->connect_error) {
    die("Connection failed: " . $conn->connect_error);
}

// Check if the request method is POST
if ($_SERVER["REQUEST_METHOD"] == "POST") {
    // Retrieve data from the POST request
    $asset_type = $_POST['asset_type'];
    $asset_uuid = $_POST['asset_uuid'];
    $device_controller = $_POST['device_controller'];
    $measurement_type = $_POST['measurement_type'];
    $device_uuid = $_POST['device_uuid'];
    $device_name = $_POST['device_name'];
    $device_model = $_POST['device_model'];
    $device_brand = $_POST['device_brand'];
    $rawdata = $_POST['rawdata'];
    $device_unit = $_POST['device_unit'];
    $device_status = $_POST['device_status'];

    // SQL query to insert data into the database
    $sql = "INSERT INTO outdoor_db (asset_type, asset_uuid, device_controller, measurement_type, device_uuid, device_name, device_model, device_brand, rawdata, device_unit, device_status)
            VALUES ('$asset_type', '$asset_uuid', '$device_controller', '$measurement_type', '$device_uuid', '$device_name', '$device_model', '$device_brand', '$rawdata', '$device_unit', '$device_status')";

    if ($conn->query($sql) === TRUE) {
        echo "New record created successfully";
    } else {
        echo "Error: " . $sql . "<br>" . $conn->error;
    }
}

$conn->close();
?>
