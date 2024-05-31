from flask import Flask, jsonify, render_template, request
from flask_cors import CORS
import mysql.connector
from mysql.connector import Error
import requests

app = Flask(__name__)
CORS(app)  # This will enable CORS for all routes

# Database configuration
db_config = {
    'host': 'localhost',
    'user': 'root',
    'password': 'plak5212',
    'database': 'sensor_db'
}

def get_db_connection():
    try:
        conn = mysql.connector.connect(**db_config)
        if conn.is_connected():
            return conn
    except Error as e:
        print(f"Error: {e}")
        return None

@app.route('/home')
def home():
    return render_template('home.html')

@app.route('/api/indoor_data', methods=['GET'])
def get_indoor_data():
    conn = get_db_connection()
    if conn is None:
        return jsonify({"error": "Database connection failed"}), 500
    
    cursor = conn.cursor(dictionary=True)
    
    cursor.execute("SELECT * FROM indoor_db ORDER BY id DESC LIMIT 20")
    indoor_data = cursor.fetchall()
    
    cursor.close()
    conn.close()
    
    return jsonify(indoor_data)

@app.route('/api/outdoor_data', methods=['GET'])
def get_outdoor_data():
    conn = get_db_connection()
    if conn is None:
        return jsonify({"error": "Database connection failed"}), 500
    
    cursor = conn.cursor(dictionary=True)
    
    cursor.execute("SELECT * FROM outdoor_db ORDER BY id DESC LIMIT 20")
    outdoor_data = cursor.fetchall()
    
    cursor.close()
    conn.close()
    
    return jsonify(outdoor_data)

@app.route('/api/all_data', methods=['GET'])
def get_all_data():
    conn = get_db_connection()
    if conn is None:
        return jsonify({"error": "Database connection failed"}), 500
    
    cursor = conn.cursor(dictionary=True)
    
    cursor.execute("SELECT * FROM indoor_db ORDER BY id DESC LIMIT 20")
    indoor_data = cursor.fetchall()
    
    cursor.execute("SELECT * FROM outdoor_db ORDER BY id DESC LIMIT 20")
    outdoor_data = cursor.fetchall()
    
    cursor.close()
    conn.close()
    
    return jsonify({
        "indoor_data": indoor_data,
        "outdoor_data": outdoor_data
    })

@app.route('/api/indoor_temperature', methods=['GET'])
def get_indoor_temperature():
    conn = get_db_connection()
    if conn is None:
        return jsonify({"error": "Database connection failed"}), 500
    
    cursor = conn.cursor(dictionary=True)
    
    # Modify the query to select temperature measurements
    cursor.execute("SELECT rawdata FROM indoor_db WHERE measurement_type = 'temperature' ORDER BY id DESC LIMIT 15")
    indoor_temperature = cursor.fetchall()
    
    cursor.close()
    conn.close()
    
    return jsonify(indoor_temperature)

@app.route('/api/indoor_humidity', methods=['GET'])
def get_indoor_humidity():
    conn = get_db_connection()
    if conn is None:
        return jsonify({"error": "Database connection failed"}), 500
    
    cursor = conn.cursor(dictionary=True)
    
    cursor.execute("SELECT rawdata FROM indoor_db WHERE measurement_type = 'humidity' ORDER BY id DESC LIMIT 15")
    indoor_humidity = cursor.fetchall()
    
    cursor.close()
    conn.close()
    
    return jsonify(indoor_humidity)

@app.route('/api/indoor_pressure', methods=['GET'])
def get_indoor_pressure():
    conn = get_db_connection()
    if conn is None:
        return jsonify({"error": "Database connection failed"}), 500
    
    cursor = conn.cursor(dictionary=True)
    
    cursor.execute("SELECT rawdata FROM indoor_db WHERE measurement_type = 'pressure' ORDER BY id DESC LIMIT 15")
    indoor_pressure = cursor.fetchall()
    
    cursor.close()
    conn.close()
    
    return jsonify(indoor_pressure)

@app.route('/api/indoor_gas', methods=['GET'])
def get_indoor_gas():
    conn = get_db_connection()
    if conn is None:
        return jsonify({"error": "Database connection failed"}), 500
    
    cursor = conn.cursor(dictionary=True)
    
    cursor.execute("SELECT rawdata FROM indoor_db WHERE measurement_type = 'gas' ORDER BY id DESC LIMIT 15")
    indoor_gas = cursor.fetchall()
    
    cursor.close()
    conn.close()
    
    return jsonify(indoor_gas)

@app.route('/api/outdoor_temperature', methods=['GET'])
def get_outdoor_temperature():
    conn = get_db_connection()
    if conn is None:
        return jsonify({"error": "Database connection failed"}), 500
    
    cursor = conn.cursor(dictionary=True)
    
    cursor.execute("SELECT rawdata FROM outdoor_db WHERE measurement_type = 'temperature' ORDER BY id DESC LIMIT 15")
    outdoor_temperature = cursor.fetchall()
    
    cursor.close()
    conn.close()
    
    return jsonify(outdoor_temperature)

@app.route('/api/outdoor_humidity', methods=['GET'])
def get_outdoor_humidity():
    conn = get_db_connection()
    if conn is None:
        return jsonify({"error": "Database connection failed"}), 500
    
    cursor = conn.cursor(dictionary=True)
    
    cursor.execute("SELECT rawdata FROM outdoor_db WHERE measurement_type = 'humidity' ORDER BY id DESC LIMIT 15")
    outdoor_humidity = cursor.fetchall()
    
    cursor.close()
    conn.close()
    
    return jsonify(outdoor_humidity)

@app.route('/api/outdoor_pressure', methods=['GET'])
def get_outdoor_pressure():
    conn = get_db_connection()
    if conn is None:
        return jsonify({"error": "Database connection failed"}), 500
    
    cursor = conn.cursor(dictionary=True)
    
    cursor.execute("SELECT rawdata FROM outdoor_db WHERE measurement_type = 'pressure' ORDER BY id DESC LIMIT 15")
    outdoor_pressure = cursor.fetchall()
    
    cursor.close()
    conn.close()
    
    return jsonify(outdoor_pressure)

@app.route('/api/outdoor_gas', methods=['GET'])
def get_outdoor_gas():
    conn = get_db_connection()
    if conn is None:
        return jsonify({"error": "Database connection failed"}), 500
    
    cursor = conn.cursor(dictionary=True)
    
    cursor.execute("SELECT rawdata FROM outdoor_db WHERE measurement_type = 'gas' ORDER BY id DESC LIMIT 15")
    outdoor_gas = cursor.fetchall()
    
    cursor.close()
    conn.close()
    
    return jsonify(outdoor_gas)

@app.route('/api/indoor_bme280', methods=['GET'])
def get_indoor_dht11():
    conn = get_db_connection()
    if conn is None:
        return jsonify({"error": "Database connection failed"}), 500
    
    cursor = conn.cursor(dictionary=True)
    
    cursor.execute("SELECT measurement_type, rawdata, device_unit FROM indoor_db WHERE device_model = 'BME280' ORDER BY id DESC LIMIT 15;")
    get_indoor_dht11 = cursor.fetchall()
    
    cursor.close()
    conn.close()
    
    return jsonify(get_indoor_dht11)

@app.route('/api/outdoor_bme280', methods=['GET'])
def get_outdoor_dht11():
    conn = get_db_connection()
    if conn is None:
        return jsonify({"error": "Database connection failed"}), 500
    
    cursor = conn.cursor(dictionary=True)
    
    cursor.execute("SELECT measurement_type, rawdata, device_unit FROM outdoor_db WHERE device_model = 'BME280' ORDER BY id DESC LIMIT 15;")
    get_outdoor_dht11 = cursor.fetchall()
    
    cursor.close()
    conn.close()
    
    return jsonify(get_outdoor_dht11)

@app.route('/api/indoor_mq2', methods=['GET'])
def get_indoor_mq2():
    conn = get_db_connection()
    if conn is None:
        return jsonify({"error": "Database connection failed"}), 500
    
    cursor = conn.cursor(dictionary=True)
    
    cursor.execute("SELECT measurement_type, rawdata, device_unit FROM indoor_db WHERE device_model = 'MQ-2' ORDER BY id DESC LIMIT 15;")
    get_indoor_mq2 = cursor.fetchall()
    
    cursor.close()
    conn.close()
    
    return jsonify(get_indoor_mq2)

@app.route('/api/outdoor_mq2', methods=['GET'])
def get_outdoor_mq2():
    conn = get_db_connection()
    if conn is None:
        return jsonify({"error": "Database connection failed"}), 500
    
    cursor = conn.cursor(dictionary=True)
    
    cursor.execute("SELECT measurement_type, rawdata, device_unit FROM outdoor_db WHERE device_model = 'MQ-2' ORDER BY id DESC LIMIT 15;")
    get_outdoor_mq2 = cursor.fetchall()
    
    cursor.close()
    conn.close()
    
    return jsonify(get_outdoor_mq2)

def forward_data(url, data):
    # Send data to the specified URL and return the result
    response = requests.post(url, json=data)
    # print(response.status_code)
    if response.status_code == 200 or 201:
        return "Data forwarded successfully", 200
    else:
        return "Failed to forward data", 500

@app.route('/iot_project/indoor.php', methods=['POST'])
def indoor_php():
    data = request.get_json()  # Receive incoming JSON data
    if not data or 'data' not in data:
        return "Invalid data", 400  # Validate data

    # Separate data into individual sensor readings
    temperature_data = next((item for item in data['data'] if item['measurement_type'] == 'temperature'), None)
    humidity_data = next((item for item in data['data'] if item['measurement_type'] == 'humidity'), None)
    pressure_data = next((item for item in data['data'] if item['measurement_type'] == 'pressure'), None)
    gas_data = next((item for item in data['data'] if item['measurement_type'] == 'gas'), None)

    url = 'http://192.168.1.12/iot_project/indoor.php'
    responses = []

    # Forward each type of data if it exists
    if temperature_data:
        responses.append(forward_data(url, temperature_data))
    if humidity_data:
        responses.append(forward_data(url, humidity_data))
    if pressure_data:
        responses.append(forward_data(url, pressure_data))
    if gas_data:
        responses.append(forward_data(url, gas_data))

    return jsonify(responses), 200  # Return the results

@app.route('/iot_project/outdoor.php', methods=['POST'])
def outdoor_php():
    data = request.get_json()  # Receive incoming JSON data
    if not data or 'data' not in data:
        return "Invalid data", 400  # Validate data

    # Separate data into individual sensor readings
    temperature_data = next((item for item in data['data'] if item['measurement_type'] == 'temperature'), None)
    humidity_data = next((item for item in data['data'] if item['measurement_type'] == 'humidity'), None)
    pressure_data = next((item for item in data['data'] if item['measurement_type'] == 'pressure'), None)
    gas_data = next((item for item in data['data'] if item['measurement_type'] == 'gas'), None)

    url = 'http://192.168.1.12/iot_project/outdoor.php'
    responses = []

    # Forward each type of data if it exists
    if temperature_data:
        responses.append(forward_data(url, temperature_data))
    if humidity_data:
        responses.append(forward_data(url, humidity_data))
    if pressure_data:
        responses.append(forward_data(url, pressure_data))
    if gas_data:
        responses.append(forward_data(url, gas_data))

    return jsonify(responses), 200  # Return the results

# # URL configurations for forwarding
# indoor_url = 'http://192.168.1.12/iot_project/indoor.php'
# outdoor_url = 'http://192.168.1.12/iot_project/outdoor.php'

# @app.route('/iot_project/indoor.php', methods=['POST'])
# def indoor_php():
#     data = request.get_json()
#     response = requests.post(indoor_url, json=data)
#     if response.status_code == 200:
#         return "Data forwarded successfully", 200
#     else:
#         print(f"Failed to forward data to {indoor_url}. Status code: {response.status_code}, Response: {response.text}")
#         return "Failed to forward data", 500

# @app.route('/iot_project/outdoor.php', methods=['POST'])
# def outdoor_php():
#     data = request.get_json()
#     response = requests.post(outdoor_url, json=data)
#     if response.status_code == 200:
#         return "Data forwarded successfully", 200
#     else:
#         print(f"Failed to forward data to {outdoor_url}. Status code: {response.status_code}, Response: {response.text}")
#         return "Failed to forward data", 500
if __name__ == '__main__':
    app.run(debug=True)