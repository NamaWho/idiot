from coapServer import CoAPServer

#import coapClient


def main():
    host = "::"
    port = 5683
    server = CoAPServer(host, port)

    #thread = threading.Thread(target=periodic_coap_client, args=(60,))
    #thread.daemon = True  
    #thread.start()

    try:
        print("CoAP server start")
        server.listen(10)
    except KeyboardInterrupt:
        print("Server Shutdown")
        server.close()
        print("Exiting...")

if __name__ == "__main__":
    main()


#def periodic_coap_client(interval):
    #time.sleep(interval)
    
    #while True:
     #   print("CoAP client start")
      #  coapClient.main()
       # time.sleep(interval)

