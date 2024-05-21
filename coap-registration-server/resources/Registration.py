
from mysql.connector import Error
from coapthon.resources.resource import Resource
import json

class Registration(Resource):
    def __init__(self, name="Registration", coap_server=None):
        super(Registration, self).__init__(name, coap_server, visible=True, observable=True, allow_children=True)
        self.payload = "Registration Resource"
        self.resource_type = "rt1"
        self.content_type = "json"
        self.interface_type = "if1"
    
    def render_GET(self, request):
        self.payload = "GET Registration Resource"
        return self
    
    def render_POST(self, request):
        self.payload = "POST Registration Resource"

        # Extract the json payload from the request
        json_payload = request.payload.decode('utf-8')
        print(json_payload)
        
        try:
            data = json.loads(json_payload)
            ip_address = data.get('ip_address', 'unknown')
            type = data.get('type', 'unknown')
            status = data.get('status', 'unknown')

            # Insert node info into lln_nodes table if not already present
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
