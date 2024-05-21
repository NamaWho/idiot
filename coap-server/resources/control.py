
from mysql.connector import Error
from coapthon.resources.resource import Resource
import json

class Control(Resource):
    def __init__(self, name="Control"):
        super(Control, self).__init__(name)
        self.payload = "Control Resource"

    
    def render_GET(self, request):
        self.payload = "GET Control Resource"
        return self
    
    def render_POST(self, request):
        self.payload = "POST Control Resource"

        # Extract the json payload from the request
        json_payload = request.payload.decode('utf-8')
        
        try:
            data = json.loads(json_payload)

        except (Error, json.JSONDecodeError) as e:
            print(f"Error processing Control: {e}")
            self.payload = f"Control failed: {e}"

        return self
