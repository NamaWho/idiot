import mysql.connector
from mysql.connector import Error
import json

class Database:

    @classmethod
    def __new__(cls):
        if not hasattr(cls, 'instance'):
            cls.instance = super(Database, cls).__new__(cls)
        return cls.instance

    @classmethod
    def __init__(self):
       return
    
    @classmethod
    def connect_db(self):
        # Database connection setup
        try:
            # import credentials.key file to get MySQL credentials
            with open("private/credentials.json", "r") as file:
                self.credentials = json.load(file)

            self.connection = mysql.connector.connect(host= self.credentials["MYSQL_HOST"],
                                            user= self.credentials["MYSQL_USER"],
                                            password= self.credentials["MYSQL_PASSWORD"],
                                            database= self.credentials["MYSQL_DATABASE"])
            
            return self.connection
        
        except Error as e:
            print(f"Error connecting to MySQL database: {e}")
            return None

    @classmethod
    def __del__(self):
        print("MySQL connection closed")
    