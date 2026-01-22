package bgu.spl.net.impl.stomp;

import java.util.HashMap;

import bgu.spl.net.srv.ConnectionsImpl;
import bgu.spl.net.srv.Server;

public class StompServer {

    public static void main(String[] args) {
        // TODO: implement this
        // Server.threadPerClient(
        //         7777, // port
        //         () -> new StompMessagingProtocolImpl(), // protocol factory
        //         () -> new StompMessageEncoderDecoder()
        // ).serve();

        Server.reactor(
                Runtime.getRuntime().availableProcessors(),
                7777, // port
                () -> new StompMessagingProtocolImpl(), // protocol factory
                () -> new StompMessageEncoderDecoder() // message encoder decoder factory
        ).serve();
    }
}
