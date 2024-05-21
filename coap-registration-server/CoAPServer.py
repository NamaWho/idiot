import mysql.connector
from mysql.connector import Error
from coapthon.server.coap import CoAP
from resources.Registration import Registration
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

        # Database connection setup
        try:
            self.connection = mysql.connector.connect(
                host='your_mysql_host',
                database='your_database_name',
                user='your_username',
                password='your_password'
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
    server = CoAPServer("localhost", 5683)
    try:
        print("CoAP server start")
        server.listen(10)
    except KeyboardInterrupt:
        print("Server Shutdown")
        server.close()
        print("Exiting...")