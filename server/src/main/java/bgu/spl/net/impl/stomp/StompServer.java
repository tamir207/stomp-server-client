package bgu.spl.net.impl.stomp;

import java.util.HashMap;

import bgu.spl.net.srv.ConnectionsImpl;

public class StompServer {

    public static void main(String[] args) {
        // TODO: implement this
        StompMessagingProtocolImpl stomp = new StompMessagingProtocolImpl();
        int connection1 = 1;
        int connection2 = 2;
        ConnectionsImpl<String> connections = new ConnectionsImpl<>();
        String stompMessage = "MESSAGE\nsubscription:78\nmessage-id:20\ndestination:/topic/a\n\nHello Topic a\n^@";
        stomp.process(stompMessage);
    }
}
