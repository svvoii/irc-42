#include "UserResponse.hpp"

UserResponse::UserResponse(User & user, Server const & server) 
	: _user(user), _server(server) {

	std::cout << CYAN << "\t[UserResponse] constructor called" << RESET << std::endl;
	std::cout << YELLOW << "\tuser.handshaked(): " << _user.handshaked() << RESET << std::endl;

	if (_user.handshaked() == false) {
		
		handshakeResponse();
	}
	else {

		responceBuilder();
	}
}

UserResponse::~UserResponse() {
}

void UserResponse::handshakeResponse() {

	std::string serverName = _server.getServerName();
	std::string serverVersion = "_server.getServerVersion()";
	std::string serverCreated = "_server.getCreationDate()";
	std::string userModes = "<usr_modes>";
	std::string channModes = "<chann_modes>";
	std::string hostName = _user.getHostName();
	std::string nickName = _user.getNickName();
	std::string userName = _user.getUserName();

	_user.responseBuffer = RPL_WELCOME(nickName) + "\r\n";
	_user.responseBuffer += RPL_YOURHOST(serverName, serverVersion) + "\r\n";
	_user.responseBuffer += RPL_CREATED(serverCreated) + "\r\n";
	_user.responseBuffer += RPL_MYINFO(serverName, serverVersion, userModes, channModes) + "\r\n";
/*
	_user.responseBuffer = serverName + " 001 " + nickName + " :Welcome to the Internet Relay Network " + userName + "!" + "\r\n";
	_user.responseBuffer += serverName + " 002 " + nickName + " :Your host is " + serverName + ", running version 1.0" + "\r\n";
	_user.responseBuffer += serverName + " 003 " + nickName + " :This server was created " + "server.getCreationDate()" + "\r\n";
	_user.responseBuffer += serverName + " 004 " + nickName + " " + serverName + " 1.0 " + "ao" + " " + "O" + "\r\n";
*/
	_user.setHandshaked(true);
}

void UserResponse::responceBuilder() {

	// BUILDING RESPONSES BASED ON THE COMMANDS RECEIVED FROM THE CLIENT

	if (_user.userMessageBuffer.empty()) {

		_user.responseBuffer = "\t<empty request>\n";
	} 
	else {

		_user.responseBuffer = "\t..coucou. The Matrix has you.. waiting for command..\n";
	}
}
