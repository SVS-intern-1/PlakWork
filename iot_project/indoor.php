<?php
// Enable error reporting for debugging
ini_set('display_errors', 1);
ini_set('display_startup_errors', 1);
error_reporting(E_ALL);

// Database configuration
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
    // Get the raw POST data
    $json_data = file_get_contents('php://input');

    // Decode the JSON data
    $data = json_decode($json_data, true);

    if (json_last_error() !== JSON_ERROR_NONE) {
        http_response_code(400);
        echo "Invalid JSON data";
        exit;
    }

    // Validate data
    if (!isset($data['asset_type'], $data['asset_uuid'], $data['device_controller'], $data['measurement_type'], $data['device_uuid'], $data['device_name'], $data['device_model'], $data['device_brand'], $data['rawdata'], $data['device_unit'], $data['device_status'])) {
        http_response_code(400);
        echo "Invalid data format";
        exit;
    }

    // Extract data fields
    $asset_type = $data['asset_type'];
    $asset_uuid = $data['asset_uuid'];
    $device_controller = $data['device_controller'];
    $measurement_type = $data['measurement_type'];
    $device_uuid = $data['device_uuid'];
    $device_name = $data['device_name'];
    $device_model = $data['device_model'];
    $device_brand = $data['device_brand'];
    $rawdata = $data['rawdata'];
    $device_unit = $data['device_unit'];
    $device_status = $data['device_status'];

    // SQL query to insert data into the database using prepared statements
    $stmt = $conn->prepare("INSERT INTO indoor_db (asset_type, asset_uuid, device_controller, measurement_type, device_uuid, device_name, device_model, device_brand, rawdata, device_unit, device_status) VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?)");
    $stmt->bind_param("sssssssssss", $asset_type, $asset_uuid, $device_controller, $measurement_type, $device_uuid, $device_name, $device_model, $device_brand, $rawdata, $device_unit, $device_status);

    // Execute the SQL query
    if ($stmt->execute()) {
        http_response_code(201); // Created
        echo "New record created successfully";
    } else {
        http_response_code(500); // Internal Server Error
        echo "Error: " . $stmt->error;
    }

    // Close the statement
    $stmt->close();
} else {
    http_response_code(405); // Method Not Allowed
    echo "Invalid request method";
}

// Close the database connection
$conn->close();
?>
