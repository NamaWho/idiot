
from mysql.connector import Error
from coapthon.resources.resource import Resource
import json
from observe import ObserveSensor

class Registration(Resource):

    pressure_sensor = 0
    vibration_sensor = 0
    voltage_sensor = 0
    rotation_sensor = 0

    pressure_address = ""
    vibration_address = ""
    voltage_address = ""
    rotation_address = ""

    def __init__(self, name="Registration"):
        super(Registration, self).__init__(name)
        self.payload = "Registration Resource"
    
    def render_POST(self, request):
        self.payload = "POST Registration Resource"

        # Extract the json payload from the request
        json_payload = request.payload.decode('utf-8')
        
        try:
            data = json.loads(json_payload)
            ip_address = data.get('ip_address', '')
            type = data.get('type', '')
            status = data.get('status', '')

            if type == "pressure":
                self.pressure_sensor = 1
                self.pressure_address = ip_address
                ObserveSensor(ip_address, "pressure")
            elif type == "vibration":
                self.vibration_sensor = 1
                self.vibration_address = ip_address
                ObserveSensor(ip_address, "vibration")
            elif type == "voltage":
                self.voltage_sensor = 1
                self.voltage_address = ip_address
                ObserveSensor(ip_address, "voltage")
            elif type == "rotation":
                self.rotation_sensor = 1
                self.rotation_address = ip_address
                ObserveSensor(ip_address, "rotation")
            else:
                self.payload = "Invalid sensor type"
                return self

            # Insert node info into Sensor table if not already present
            if self.coap_server and self.coap_server.connection:
                cursor = self.coap_server.connection.cursor()
                insert_node_query = """
                INSERT INTO Sensor (type, status, ip_address) 
                VALUES (%s, %s, %s)
                ON DUPLICATE KEY UPDATE ip_address=%s, type=%s, status=%s
                """
                cursor.execute(insert_node_query, (type, status, ip_address, type, status, ip_address))
                self.coap_server.connection.commit()
                cursor.close()

                self.payload = "Node registration successful"
            else:
                self.payload = "Database connection not available"

        except (Error, json.JSONDecodeError) as e:
            print(f"Error processing registration: {e}")
            self.payload = f"Registration failed: {e}"

        return self
