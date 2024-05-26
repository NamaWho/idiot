import mysql.connector
from mysql.connector import Error
import json

class Database:
    connection = None

    def __init__(self):
       return
    
    def connect_db(self):
        if self.connection is not None:
            return self.connection
        else:
            # Database connection setup
            try:
                # import credentials.key file to get MySQL credentials
                with open("private/credentials.json", "r") as file:
                    self.credentials = json.load(file)

                self.connection = mysql.connector.connect(host= self.credentials["MYSQL_HOST"],
                                                user= self.credentials["MYSQL_USER"],
                                                password= self.credentials["MYSQL_PASSWORD"],
                                                database= self.credentials["MYSQL_DATABASE"])
                
                if self.connection.is_connected():
                    print("Connected to MySQL database")
                
                return self.connection
            
            except Error as e:
                print(f"Error connecting to MySQL database: {e}")
                return None


    def __del__(self):
        if self.connection is not None:
            self.connection.close()
            print("MySQL connection closed")
    