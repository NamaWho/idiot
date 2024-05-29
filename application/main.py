from coapServer import CoAPServer

from coapClient import CoapClient


def main():
    host = "::"
    port = 5683
    server = CoAPServer(host, port)
    client = CoapClient()

    try:
        print("CoAP server start")
        server.listen(10)
    except KeyboardInterrupt:
        print("Server Shutdown")
        server.close()
        print("Exiting...")

if __name__ == "__main__":
    main()




