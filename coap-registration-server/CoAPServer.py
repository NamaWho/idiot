import mysql.connector
from mysql.connector import Error
from coapthon.server.coap import CoAP
from resources.Registration import Registration
from resources.Telemetry import Telemetry
from resources.Control import Control
import json


class CoAPServer(CoAP):
    
    def __init__(self, host, port):
        """
        Initialize the CoAP server

        :param host: The host to bind the server to
        :param port: The port to bind the server to

        :return: None
        """
        CoAP.__init__(self, (host, port), False)
        self.add_resource("register/", Registration())
        self.add_resource("telemetry/", Telemetry())
        self.add_resource("control/", Control())

        # Database connection setup
        try:
            # import credentials.key file to get MySQL credentials
            with open("private/credentials.key", "r") as file:
                self.credentials = json.load(file)

            self.connection = mysql.connector.connect(
                host= self.credentials['host'],
                database= self.credentials['database'],
                user= self.credentials['user'],
                password= self.credentials['password']
            )
            if self.connection.is_connected():
                print("Connected to MySQL database")
        
        except Error as e:
            print(f"Error connecting to MySQL database: {e}")
            self.connection = None

        
    def close(self):
        if self.connection is not None and self.connection.is_connected():
            self.connection.close()
            print("MySQL connection closed")
        super(CoAPServer, self).close()

if __name__ == "__main__":
    host = "localhost"
    port = 5683
    server = CoAPServer(host, port)

    try:
        print("CoAP server start")
        server.listen(10)
    except KeyboardInterrupt:
        print("Server Shutdown")
        server.close()
        print("Exiting...")