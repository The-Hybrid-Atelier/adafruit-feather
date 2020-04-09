require 'em-websocket'
require 'json'

EventMachine.run do
  @channel = EM::Channel.new
  
  
  # START JSON WEBSOCKET SERVER
  ip= "192.168.1.29" # DO NOT USE LOCALHOST, BIND TO LOCAL IP
  port = 3001

  EM::WebSocket.start(:host => ip, :port => port) do |ws|
    ws.onopen do |handshake|
      print "Server >> Socket opened\n"
      c_id = @channel.subscribe { |msg| ws.send msg }
      message = {name: "socket-server", event: "opened"}
      ws.send message.to_json
      
      # MULTICAST ONLY JSON-FORMATTED MESSAGES
      ws.onmessage do |msg, data|
          print "Server << " + msg + "\n"
          begin
           msg = JSON.parse(msg)
           msg["c_id"] = c_id
          @channel.push msg.to_json
          rescue
           print "Invalid JSON message received #{msg}.\n" 
          end    
      end

      ws.onclose do
        print "Server >> Socket closed\n"
        message = {name: "socket-server", event: "close"}
        @channel.push message.to_json
        @channel.unsubscribe c_id
      end
    end
  end
  puts "Server: Started at #{ip}:#{port}"
end