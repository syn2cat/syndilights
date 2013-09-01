
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

wsServer.on('request', function(r){

	var connection = r.accept('echo-protocol', r.origin);

	// Specific id for this client & increment count
	var id = count++;

	// // Store the connection method so we can loop through & contact all clients
	clients[id] = connection

	console.log((new Date()) + ' Connection accepted [' + id + ']');

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



