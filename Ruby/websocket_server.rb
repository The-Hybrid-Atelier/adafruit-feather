require 'em-websocket'
require 'json'
def remote_ip(ws)
  if ws.get_peername
    ws.get_peername[2,6].unpack('nC4')[1..4].join('.')
  else
    return nil
  end
end

EventMachine.run do
  @channel = EM::Channel.new
  
  
  # START JSON WEBSOCKET SERVER
  ip= "192.168.1.252" # DO NOT USE LOCALHOST, BIND TO LOCAL IP
  port = 3001
  websockets = []

  EM::WebSocket.start(:host => ip, :port => port) do |ws|
    ws.onopen do |handshake|
      print "Server >> Socket opened\n"
      websockets << ws
      c_id = @channel.subscribe { |msg| ws.send msg }
      message = {name: "socket-server", event: "opened"}
      ws.send message.to_json
      
      # MULTICAST ONLY JSON-FORMATTED MESSAGES
      ws.onmessage do |msg, data|
          print "Server << " + msg + "\n"
          begin
           msg = JSON.parse(msg)

          websockets.each do |ms|
            if remote_ip(ws) != remote_ip(ms)  
              ms.send(msg.to_json)
            end
          end
           # msg["c_id"] = c_id
          # @channel.push msg.to_json
          rescue StandardError => bang
           print "Invalid JSON message received #{msg} : #{bang}.\n" 
          end    
      end

      ws.onclose do
        print "Server >> Socket closed\n"
        message = {name: "socket-server", event: "close"}
        @channel.push message.to_json
        @channel.unsubscribe c_id
        # print websockets
        
      end
    end
  end
  puts "Server: Started at #{ip}:#{port}"
end