#include "CommandHandler.hpp"
#include "../User/User.hpp"

CommandHandler::CommandHandler(ServerManager& srv, User &usr, map<string, string> &commands) :
	server(srv), user(usr), commandsFromClient(commands) { 

	// Filling in the `mapEnumToString` map to convert enum to string, this will help to identify the command and execute when needed
	mapEnumToString[NONE] = "NONE";
	mapEnumToString[CAP] = "CAP";
	mapEnumToString[INFO] = "INFO";
	mapEnumToString[INVITE] = "INVITE";
	mapEnumToString[JOIN] = "JOIN";
	mapEnumToString[KICK] = "KICK";
	mapEnumToString[LIST] = "LIST";
	mapEnumToString[MODE] = "MODE";
	mapEnumToString[NAMES] = "NAMES";
	mapEnumToString[NICK] = "NICK";
	mapEnumToString[NOTICE] = "NOTICE";
	mapEnumToString[OPER] = "OPER";
	mapEnumToString[PART] = "PART";
	mapEnumToString[PASS] = "PASS";
	mapEnumToString[PING] = "PING";
	mapEnumToString[PONG] = "PONG";
	mapEnumToString[PRIVMSG] = "PRIVMSG";
	mapEnumToString[QUIT] = "QUIT";
	mapEnumToString[TOPIC] = "TOPIC";
	mapEnumToString[USER] = "USER";
	mapEnumToString[VERSION] = "VERSION";
	mapEnumToString[WHO] = "WHO";
	mapEnumToString[WHOIS] = "WHOIS";

	// Filling in the `cmdToHandler` map execute command given the command string
	cmdToHandler["NONE"] = &CommandHandler::handleNONE;
	cmdToHandler["CAP"] = &CommandHandler::handleCAP;
	cmdToHandler["PASS"] = &CommandHandler::handlePASS;
	cmdToHandler["NICK"] = &CommandHandler::handleNICK;
	cmdToHandler["USER"] = &CommandHandler::handleUSER;
	cmdToHandler["JOIN"] = &CommandHandler::handleJOIN;
	cmdToHandler["PRIVMSG"] = &CommandHandler::handlePRIVMSG;
	cmdToHandler["TOPIC"] = &CommandHandler::handleTOPIC;
	cmdToHandler["INVITE"] = &CommandHandler::handleINVITE;
	cmdToHandler["KICK"] = &CommandHandler::handleKICK;
	cmdToHandler["MODE"] = &CommandHandler::handleMODE;
	// .. and so on


	if (user.authenticated()) {
		// process the commands directly
		executeCommand();
	}
	else {
		// if user is not authenticated, we search for the PASS, NICK and USER commands first
		authenticateUser();
	}
}

CommandHandler::~CommandHandler() {
}

e_cmd	CommandHandler::getCMD(const std::string & str) {

	std::map<e_cmd, std::string>::iterator it = mapEnumToString.begin();

	for (; it != mapEnumToString.end(); it++) {
		if (it->second == str) {
			return it->first;
		}
	}
	return NONE;
}

/*
** This function verifies if user is authenticated by checking if the user has sent NICK, USER and PASS 
** set in `User` class. If so, the user is authenticated and can send other Commands to the server.
**
** This logic might be changed .. TO DISCUSS !!
*/
void	CommandHandler::authenticateUser() {

	// if user is not authenticated, we search for the PASS, NICK and USER commands first
	// if (commandsFromClient.find("CAP") != commandsFromClient.end()) {
	// 	handleCAP(); // this one might not be needed
	// }
	if (commandsFromClient.find("NICK") != commandsFromClient.end()) {
		handleNICK();
	}
	if (commandsFromClient.find("PASS") != commandsFromClient.end()) {
		handlePASS();
	}
	if (commandsFromClient.find("USER") != commandsFromClient.end()) {
		handleUSER();
	}

	// setting authenticated to true if the user has sent NICK, USER and PASS
	// NEED to add the PASS check as well !!!
	if (!user.getNickName().empty() && !user.getUserName().empty()) {
		user.setAuthenticated(true);
		std::cout << "user authenticated " << std::endl;
		// && !user.getPassword().empty() removed PASSWORD condition for now

		// Identifying if te user is a BOT ?!
		if (user.getNickName() == "NeoBot") { // (or any other condition/name to identify the bot)
			std::cout << CYAN << ".. this user is a BOT.. aha !!!" << std::endl;
			user.setAsBot();
		}
	}
}

void	CommandHandler::executeCommand() {

	// map<string, string>::iterator it = commandsFromClient.begin();

	string cmdStr = commandsFromClient["command"];
	/* DEBUG */
	std::cout << YELLOW << "Executing command: " << RESET << "[" << cmdStr << "]" << std::endl;
	/* ***** */

	// The synax is important here !! (`cmdToHandler[cmdStr]()` - won't work)
	// first we get the pointer to the handler method and then we call it on `this` object
	(this->*cmdToHandler[cmdStr])();
}

/*
** Command Hanlers
*/
void	CommandHandler::handleNONE() {
	// do nothing or/and print error message
	std::cout << RED << "[-] command not found.." << RESET << std::endl; 
}

void	CommandHandler::handleCAP() {
	std::cout << YELLOW << "CAP command received.." << RESET << std::endl;
}

void	CommandHandler::handlePASS() {
	std::cout << YELLOW << "PASS command received.." << RESET << std::endl;

	user.setPassword(commandsFromClient["PASS"]);
}

void	CommandHandler::handleNICK() {
	std::cout << YELLOW << "NICK command received.." << RESET << std::endl;

	user.setNickName(commandsFromClient["NICK"]);
}

void	CommandHandler::handleUSER() {
	std::cout << YELLOW << "USER command received.." << RESET << std::endl;

	user.setUserName(commandsFromClient["USER"]);
}

void	CommandHandler::handleJOIN() {
//need to handle the case where the channel name is wrong, eg two ##?
	std::cout << YELLOW << "JOIN command received.." << RESET << std::endl;

	// format : /join #channel (password)
	const std::string channel;
	const std::string password;

	// checker aussi si le channel est en mode invite only
	
	if (server.channelMap.find(channel) == server.channelMap.end())
	{
		Channel new_channel(channel);
		server.setChannel(new_channel);
		user.setChannel(new_channel);
		user.getChannel(channel).setUser(user);
		
		// if we still decide that the user that creates the channel is the op : 
		user.getChannel(channel).setOp(user.getNickName());

		if (password.empty() == false)
			user.getChannel(channel).setKey(password);
	}
	else
	{
		if (server.channelMap[channel]->getInvit() == true)
			return;
		if (server.channelMap[channel]->getProtected() == true)
		{
			if (server.channelMap[channel]->getKey() != password)
				return;
		}
		if (user._channels.find(channel) == user._channels.end())
		{
			user.setChannel(server.getChannel(channel));
			server.getChannel(channel).setUser(user);
		}
		else 
			return ; // une erreur ?
	}

}

void	CommandHandler::handlePRIVMSG() {

	std::cout << YELLOW << "PRIVMSG command received.." << RESET << std::endl;

	// format : /msg nickname <message>

	std::string nickname; // just for compile 

	// I have to verify that the other user-nickname is in the channel too

	if(server.getFdbyNickName(nickname) == -1)
		return;
}

void	CommandHandler::handleINVITE() {

	std::cout << YELLOW << "INVITE command received.." << RESET << std::endl;

	// format : /INVITE nickname #channel
	
	std::string channel;
	std::string nickname;

	// normalement envoie une notification au nickname et celui ci doit toujours join
	// mais peut-etre juste ajouter direct le nickname au channel ?

	if(user.getChannel(channel).isOp(user.getNickName()) == false)
		return;
	if(server.channelMap.find(channel) == server.channelMap.end())
		return;
	// check si le user est dans la usermap du server
	if(server.getFdbyNickName(nickname) == -1)
		return;
	// si le channel n'existe pas , le creer. // si le channel a un mot de passe ...
}

void	CommandHandler::handleTOPIC()	{

	std::cout << YELLOW << "TOPIC command received.." << RESET << std::endl;

 	// format : /TOPIC #channel 

	std::string channel; 
	std::string topic;	

	// without params after channel, simply print the topic of the channel
	if (topic.empty() == true)
	{
		std::cout << user.getChannel(channel).getTheme() << std::endl;
		return;
	}
	// else s' il y a un param apres le nom du channel
	// check si le channel est restricted dans la modif du topic
	if (user.getChannel(channel).getTopicRestricted() == true)
	{
		if(user.getChannel(channel).isOp(user.getNickName()) == false)
			return;
	}
	// si pas de restrictions ou user was op then modify topic
	user.getChannel(channel).setTheme(topic);
}

void	CommandHandler::handleKICK()
{
 	std::cout << YELLOW << "KICK command received.." << RESET << std::endl;

	// format de la commande : /KICK #channel nickname
	std::string channel;
	std::string nickname;
	vector<string>::iterator it;
	vector<string>:: iterator last = user.getChannel(channel)._ops.end();
	for(it = user.getChannel(channel)._ops.begin(); it != last; ++it)
	{
		if (*it == user.getNickName())
			break;
	}
	if (it == last)
		return;
	if (user._channels.find(channel) != user._channels.end())
	{
		user.getChannel(channel).getUser(nickname).removeChannel(channel);
		user.getChannel(channel).removeUser(nickname);
		server.getChannel(channel).removeUser(nickname); // dunnow if necessarry to remove it in both
	}
}

void	CommandHandler::handleMODE()
{
	std::cout << YELLOW << "MODE command received.." << RESET << std::endl;
	
	ModeHandler	mode_handler(commandsFromClient, server, user);
}
