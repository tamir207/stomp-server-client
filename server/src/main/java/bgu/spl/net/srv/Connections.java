package bgu.spl.net.srv;

import java.io.IOException;
import java.util.Map;
import bgu.spl.net.impl.data.LoginStatus;

public interface Connections<T> {

    boolean send(int connectionId, T msg);

    void send(String channel, T msg);

    void disconnect(int connectionId);

    boolean addNewClient(int connectionId, ConnectionHandler<T> handler);

    LoginStatus addUser(int connectionId, String username, String password);

    boolean addSubscriber(int connectionId, String topic, String subscriptionId);

    boolean unsubscribe(int connectionId, String subscriptionId);

    boolean isUserConnected(int connectionId);

    Map<Integer, String> getSubscribers(String channel);

    int incrementAndGetId();

    String getUsername(int connectionId);

}
