
var count = 0;
var clients = {};

var http = require('http');
var server = http.createServer(function(request, response) {});

server.listen(1234, function() {
	console.log((new Date()) + ' Server is listening on port 1234');
});

var WebSocketServer = require('websocket').server;
wsServer = new WebSocketServer({
	httpServer: server
});


wsServer.on('request', function(r) {

	var connection = r.accept('echo-protocol', r.origin);

	// Specific id for this client & increment count
	var id = count++;

	// Store the connection method so we can loop through & contact all clients
	clients[id] = connection

	console.log((new Date()) + ' Connection accepted [' + id + ']');
	clients[id].sendUTF("Welcome to the server. You are connected. This message has been pushed to you.");

	// Create event listener
	connection.on('message', function(message) {

		// The string message that was sent to us
		var msgString = message.utf8Data;
		
		// Loop through all clients
		for(var i in clients){
			// Send a message to the client with the message
			clients[i].sendUTF(msgString);
		}
	});

	// Create event listener for close
	connection.on('close', function(reasonCode, description) {
		delete clients[id];
		console.log((new Date()) + ' Peer ' + connection.remoteAddress + ' disconnected.');
	});
});

var sendTime = function () {
	var now, i = 0;

	// get time now
	now = new Date();

	// send time to all clients
	for(i in clients) {
		// Send a message to the client with the message
		clients[i].sendUTF(now);
	}

	// repeat in 5 seconds
	setTimeout(sendTime, 5000);
};

process.stdin.resume();
process.stdin.on('data', function(chunk) {
  process.stdout.write('data: ' + chunk);
  for(i in clients) {
     // Send a message to the client with the message
     clients[i].sendUTF(chunk);
  }
});
// every 5 seconds send the date/time
