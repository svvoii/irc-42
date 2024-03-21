#include "CommandHandler.hpp"

void	CommandHandler::handleQUIT()
{
	std::cout << YELLOW << "QUIT command received.." << RESET << std::endl;

	// format : /QUIT [message]
	std::string msg;
	if (commandsFromClient["params"].empty() == false) {
		msg = commandsFromClient["params"];
	}
	if (user.getStatus() == DELETED) {
		return;
	}
	// set the user status to DELETED (do not delete here..)
	user.setStatus(DELETED);

	// if the user is in a channel, remove him from all channels
	// broadcast the QUIT message to all users in the channels
	std::map<std::string, Channel*>::iterator it = user._channels.begin();
	for ( ; it != user._channels.end(); ++it) {
		// server.channelMap[it->first].removeUser(user.getNickName());

		server.setBroadcast(it->first, user.getNickName(), msg);

		// user.removeChannel(it->first); // this SEGFAULTS
	}
}