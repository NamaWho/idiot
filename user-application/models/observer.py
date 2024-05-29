import json
from coapthon.client.helperclient import HelperClient

class ObserveSensor:

    def __init__(self,source_address, resource):
        self.address = source_address
        self.resource = resource
        self.start_observing()
       
    def start_observing(self):
        print(f"Starting observation at {self.address[0]} and port {self.address[1]}, resource {self.resource}")
        self.client = HelperClient(self.address)
        self.client.observe(self.resource, self.observer)
    
    def observer(self, response):

        if not response.payload:
            print("Empty response payload")
            return
        
        data = json.loads(response.payload)
        
        try:
            if self.resource == "vibration":
                print("\n📳📳📳📳 VIBRATION TELEMETRY 📳📳📳📳 : " + str(data["value"]))
            elif self.resource == "rotation":
                print("\n🔄🔄🔄🔄 ROTATION TELEMETRY 🔄🔄🔄🔄 : " + str(data["value"]))
            elif self.resource == "pressure":
                print("\n🔘🔘🔘🔘 PRESSURE TELEMETRY 🔘🔘🔘🔘 : " + str(data["value"]))
            elif self.resource == "voltage":
                print("\n🔋🔋🔋🔋 VOLTAGE TELEMETRY 🔋🔋🔋🔋 : " + str(data["value"]))
            else:
                print("Unknown resource:", self.resource)
        except KeyError as e:
            print("KeyError:", e)

    def stop_observing(self):
        try:
            self.client.cancel_observing(self.resource, False)
            self.client.stop()
        except Exception as e:
            pass
        print(f"Stopped observation at {self.address[0]} and port {self.address[1]}, resource {self.resource}")