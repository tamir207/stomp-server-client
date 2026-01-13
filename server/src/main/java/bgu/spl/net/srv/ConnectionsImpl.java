package bgu.spl.net.srv;

import java.util.HashMap;
import java.util.List;
import java.util.Map;
import java.util.WeakHashMap;

public class ConnectionsImpl <T> implements Connections<T> {

    
    private final Map<Integer, ConnectionHandler<T>> con;
    private final Map<String, Map<Integer, Integer>> channels;
    
    public ConnectionsImpl(){
        con = new WeakHashMap<>();
        channels = new HashMap<>();
    }

    @Override
    public boolean send(int connectionId, T msg){
        ConnectionHandler<T> handler = con.get(connectionId);
        if (handler == null)
            return false;
        handler.send(msg);
        return true;
    }

    @Override
    public void send(String channel, T msg){
        Map<Integer, Integer> subscribers = channels.get(channel);
        for (Map.Entry<Integer, Integer> subcriber: subscribers.entrySet()){
            ConnectionHandler<T> handler = con.get(subcriber.getKey());
            if (handler != null)
                handler.send(msg);
        }
    }

    public boolean addSubscriber(int connectionId, String topic, int subscriptionId) {
        if (topic == null) {
            return false;
        }

        Map<Integer, Integer> foundTopic = channels.get(topic);
        if (foundTopic == null) {
            return false;
        }
        
        foundTopic.put(connectionId, subscriptionId);
        return true;
    }

    @Override
    public void disconnect(int connectionId){
        con.remove(connectionId);
    }
}
