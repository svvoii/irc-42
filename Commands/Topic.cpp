#include "CommandHandler.hpp"

void	CommandHandler::handleTOPIC()	{

std::cout << YELLOW << "TOPIC command received.." << RESET << std::endl;

 	// format : /TOPIC #channel [topic]

	size_t i = commandsFromClient["params"].find_first_of(' ');
	std::string channelName = commandsFromClient["params"].substr(0, i);
	// if the channel does not exist

	// DEBUG //
	std::cout << "channelName : " << channelName << std::endl;
	//

	if (server.channelMap.find(channelName) == server.channelMap.end())
	{
		server.setBroadcast(ERR_NOSUCHCHANNEL(user.getNickName(), channelName), user.getSocket());
		return;
	}
	// if the user is not on the channel
	if (server.channelMap[channelName]._users.find(user.getNickName()) == server.channelMap[channelName]._users.end())
	{
		server.setBroadcast(ERR_USERNOTINCHANNEL(user.getNickName(), channelName), user.getSocket());
		return;
	}
	// if the user just wants to print the topic
	if (i == std::string::npos)
	{
		if (server.channelMap[channelName].getTheme().empty() == true)
			server.setBroadcast(RPL_NOTOPIC(channelName), user.getSocket());
		else 
			server.setBroadcast(RPL_TOPIC(user.getNickName(), channelName, server.channelMap[channelName].getTheme()), user.getSocket());
		return;
	}
	// if the user wants to change the topic
	else
	{
		std::string topic = commandsFromClient["params"].substr(i + 1);
		
		if (server.channelMap[channelName].getTopicRestricted() == true)
		{
			if(server.channelMap[channelName].isOp(user.getNickName()) == false)
			{
				server.setBroadcast(ERR_CHANOPRIVSNEEDED(channelName), user.getSocket());
				return;
			}
		}
		if (topic.empty())
			server.channelMap[channelName].removeTopic();
		server.channelMap[channelName].setTheme(topic);
		server.setBroadcast(RPL_TOPIC(user.getNickName(), channelName, topic), user.getSocket());
		server.setBroadcast(channelName, user.getNickName(), RPL_TOPIC(user.getNickName(), channelName, topic));
	}
}