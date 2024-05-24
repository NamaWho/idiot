
from mysql.connector import Error
from coapthon.resources.resource import Resource
import json
import time
import threading
from models.observer import ObserveSensor
from models.database import Database

class Registration(Resource):

    sensors = {
        "pressure": {"status": 0, "address": ""},
        "vibration": {"status": 0, "address": ""},
        "voltage": {"status": 0, "address": ""},
        "rotation": {"status": 0, "address": ""}
    }
    actuators = {
        "alarm": {"status": 0, "address": ""}
    }

    all_sensors_registered = threading.Event()

    def __init__(self, name="Registration"):
        super(Registration, self).__init__(name)
        self.payload = "Registration Resource"
        self.database = Database()
        self.connection = self.database.connect_db()
    
    def render_GET(self, request):
        print("GET Registration Resource")
        self.payload = "GET Registration Resource"
        
        try:
            ip_address = request.source
            type = request.payload.decode("utf-8")
            status = 1

            if type in self.sensors:
                self.register_sensor(type, ip_address)
            elif type in self.actuators:
                self.register_actuator(type, ip_address)
            else:   
                self.payload = "Invalid sensor/actuator type"
                return self
            
            if type in self.sensors and type == "alarm":
                self.wait_for_all_sensors(request)

        except (Error, json.JSONDecodeError) as e:
            print(f"Error processing registration: {e}")
            self.payload = f"Registration failed: {e}"

        return self
    
    def register_sensor(self, type, ip_address):
        self.sensors[type]["status"] = 1
        self.sensors[type]["address"] = ip_address
        ObserveSensor(ip_address, type)

        # Insert node info into Sensor table if not already present
        if self.connection and self.connection.is_connected():
            cursor = self.connection.cursor()
            insert_node_query = """
            INSERT INTO Sensor (type, status, ip_address) 
            VALUES (%s, %s, %s)
            ON DUPLICATE KEY UPDATE ip_address=%s, type=%s, status=%s
            """
            cursor.execute(insert_node_query, (type, 1, ip_address, type, 1, ip_address))
            self.coap_server.connection.commit()
            cursor.close()
            self.payload = "Node registration successful"
        else:
            self.payload = "Database connection not available"

        if all(sensor["status"] for sensor in self.sensors.values()):
            self.all_sensors_registered.set()

    def register_actuator(self, type, ip_address):
        self.actuators[type]["status"] = 1
        self.actuators[type]["address"] = ip_address

    def wait_for_all_sensors(self, request):
        self.all_sensors_registered.wait()

        sensor_data = {
            "pressure_ip": self.sensors["pressure"]["address"],
            "vibration_ip": self.sensors["vibration"]["address"],
            "voltage_ip": self.sensors["voltage"]["address"],
            "rotation_ip": self.sensors["rotation"]["address"]
        }

        self.payload = json.dumps(sensor_data)
        self.send_response(request)

    def send_response(self, request):
        response = request.response
        response.payload = self.payload
        self.coap_server.send_response(request, response)