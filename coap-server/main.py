from coapServer import CoAPServer

def main():
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

if __name__ == "__main__":
    main()
