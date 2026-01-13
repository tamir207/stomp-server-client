package bgu.spl.net.srv;

import java.util.HashMap;
import java.util.List;
import java.util.Map;
import java.util.WeakHashMap;

public class ConnectionsImpl <T > implements Connections<T> {

    private final Map<Integer, ConnectionHandler<T>> con;
    private final Map<String, List<Integer>> channels;

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
        List<Integer> subscribers = channels.get(channel);
        for (Integer s: subscribers){
            ConnectionHandler<T> handler = con.get(s);
            if (handler != null)
                handler.send(msg);
        }
    }

    @Override
    public void disconnect(int connectionId){
        con.remove(connectionId);
    }
}
