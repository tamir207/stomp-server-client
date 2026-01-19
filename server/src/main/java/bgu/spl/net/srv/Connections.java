package bgu.spl.net.srv;

import java.io.IOException;
import java.util.Map;

public interface Connections<T> {

    boolean send(int connectionId, T msg);

    void send(String channel, T msg);

    void disconnect(int connectionId);

    boolean addNewClient(int connectionId, ConnectionHandler<T> handler);

    int addUser(int connectionId, String username, String password);

    boolean addSubscriber(int connectionId, String topic, String subscriptionId);

    boolean unsubscribe(int connectionId, String subscriptionId);

    boolean isUserConnected(int connectionId);

    Map<Integer, String> getSubscribers(String channel);

    int incrementAndGetId();

}
