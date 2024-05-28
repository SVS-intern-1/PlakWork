from flask import Flask, jsonify
from flask_cors import CORS
import mysql.connector
from mysql.connector import Error

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

@app.route('/api/all_data', methods=['GET'])
def get_all_data():
    conn = get_db_connection()
    if conn is None:
        return jsonify({"error": "Database connection failed"}), 500
    
    cursor = conn.cursor(dictionary=True)
    
    # Retrieve and order data from indoor_db by id in descending order
    cursor.execute("SELECT * FROM indoor_db ORDER BY id DESC LIMIT 15")
    indoor_data = cursor.fetchall()
    
    # Retrieve and order data from outdoor_db by id in descending order
    cursor.execute("SELECT * FROM outdoor_db ORDER BY id DESC LIMIT 15")
    outdoor_data = cursor.fetchall()
    
    cursor.close()
    conn.close()
    
    return jsonify({
        "indoor_data": indoor_data,
        "outdoor_data": outdoor_data
    })

if __name__ == '__main__':
    app.run(debug=True)
