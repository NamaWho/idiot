
# from mysql.connector import Error
# from coapthon.resources.resource import Resource
# import json

# class Telemetry(Resource):
#     def __init__(self, name="Telemetry"):
#         super(Telemetry, self).__init__(name)
#         self.payload = "Telemetry Resource"
    
#     def render_GET(self, request):
#         self.payload = "GET Telemetry Resource"
#         return self
    
#     def render_POST(self, request):
#         self.payload = "POST Telemetry Resource"

#         # Extract the json payload from the request
#         json_payload = request.payload.decode('utf-8')
        
#         try:
#             data = json.loads(json_payload)

#         except (Error, json.JSONDecodeError) as e:
#             print(f"Error processing Telemetry: {e}")
#             self.payload = f"Telemetry failed: {e}"

#         return self
