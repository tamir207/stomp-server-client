package bgu.spl.net.impl.stomp;

import java.util.HashMap;

public class StompServer {

    public static void main(String[] args) {
        // TODO: implement this
        StompMessagingProtocolImpl stomp = new StompMessagingProtocolImpl();
        String stompMessage = "MESSAGE\nsubscription:78\nmessage-id:20\ndestination:/topic/a\n\nHello Topic a\n^@";
        stomp.process(stompMessage);
    }
}
