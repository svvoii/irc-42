#include "CommandHandler.hpp"

/*
** format : /NICK <nickname>
*/

void	CommandHandler::handleNICK() {

	std::cout << YELLOW << "NICK command received.." << RESET << std::endl;

	std::string nickname = commandsFromClient["params"];
	trim(nickname, " \t\r");
	
	// first check if NICK is valid
	if (nickname.empty()) {
		server.setBroadcast(ERR_NONICKNAMEGIVEN(server.hostname), user.getFd());
		return;
	}
	// if nickname starts with a digit, it is invalid
	if (std::isdigit(nickname[0]))
	{
		server.setBroadcast(ERR_ERRONEUSNICKNAME(server.hostname, user.getNickName(), nickname), user.getFd());
		return;
	}
	// check forbidden characters
	string::const_iterator it;
	for(it = nickname.begin(); it != nickname.end(); ++it) {
		if (std::isalnum(*it) == false && *it != '-' && *it != '_') {
			server.setBroadcast(ERR_ERRONEUSNICKNAME(server.hostname, user.getNickName(), nickname), user.getFd());
			return;
		}
	}
	std::string oldNick = user.getNickName();
	// if the nickname is already in use at registration : 
	if (oldNick.empty() && server.getFdbyNickName(commandsFromClient["params"]) != -1)
	{
		while (server.getFdbyNickName(nickname) != -1)
			nickname += '_';
	}
	// if the nickname is already in user after registration : 
	else if (server.getFdbyNickName(commandsFromClient["params"]) != -1) {
		server.setBroadcast(ERR_NICKNAMEINUSE(server.hostname, user.getNickName() ,commandsFromClient["params"]), user.getFd());
		return;
	}
	// check the length (31 chars max)
	if (nickname.length() > 31 || nickname.length() < 1) {
		server.setBroadcast(ERR_ERRONEUSNICKNAME(server.hostname, user.getNickName(), nickname), user.getFd());
		return;
	}
	// Once all the above passed setting nickname and updating it in all channels
	if (!oldNick.empty())
		server.setBroadcast(RPL_NICK(user.getPrefix(), nickname), user.getFd()); 
	server.usersMap[server.getFdbyNickName(oldNick)].setNickName(nickname);

	// the following part is to handle the initial registration of the user
	// if the user is already registered return
	if (user.getStatus() == REGISTERED) {
		// user.responseBuffer = "NICK set to " + nickname + "\r\n";
		return;
	} // if there is no PASS:
	if (user.getStatus() == PASS_MATCHED && !user.getUserName().empty()) {
		sendHandshake();
		user.setStatus(REGISTERED);
	}
	// The following logic is not necessary but nice to have anyway !!
	if (user.getStatus() == PASS_NEEDED && user._cap == false) {
		server.setBroadcast(ERR_NEEDPASSWORD(server.hostname), user.getFd());
	}
	if (user.getUserName().empty() && user._cap == false) {
		server.setBroadcast(ERR_NEEDUSERNAME(server.hostname), user.getFd());
	}
}