#include "CommandHandler.hpp"

void	CommandHandler::handleJOIN() {

	std::cout << YELLOW << "JOIN command received.." << RESET << std::endl;

	// format : /join #channel (password)

	std::vector<std::string> params = split(commandsFromClient["params"], " ");
	if (params.begin() + 1 == params.end() || params.begin() + 2 == params.end())
		;
	else
	{
		if (!params.empty())
			server.setBroadcast(ERR_TOOMANYTARGETS(*(params.end() - 1)), user.getSocket());
		return;
	}
	std::string channelName = parse_channelName(*params.begin());
	if (channelName.empty() == true)
	{
		server.setBroadcast(ERR_NOSUCHCHANNEL(user.getNickName(), channelName), user.getSocket());
		return; 
	}
	// check if the channel doesn't exist, creates it
	if (server.channelMap.find(channelName) == server.channelMap.end())
	{
		Channel new_channel(channelName);
		new_channel.setUser(user);
		// set the creator of the channel as operator
		new_channel.setOp(user.getNickName());
		if (params.begin() + 1  != params.end())
			new_channel.setKey(*(params.begin() + 1));
		server.setChannel(new_channel);
		user.setChannel(new_channel);
		server.setBroadcast(MODE_USERMSG(user.getNickName(), "+o"), user.getSocket());
	}
	// if channel already exists
	else
	{
		if (server.channelMap[channelName].getInvit() == true)
		{
			server.setBroadcast(ERR_INVITEONLYCHAN(channelName), user.getSocket());
			return; 
		}
		if (server.channelMap[channelName].getProtected() == true)
		{
			if (server.channelMap[channelName].getKey() != *(params.begin() + 1))
			{
				server.setBroadcast(ERR_BADCHANNELKEY(channelName), user.getSocket());
				return;
			}
		}
		if (user._channels.find(channelName) == user._channels.end())
		{
			if (server.channelMap[channelName].getLimited() == true)
			{
				if (server.channelMap[channelName].getNb() == server.channelMap[channelName].getLimit())
				{
					server.setBroadcast(ERR_CHANNELISFULL(channelName), user.getSocket());
					return; 
				}
			}
			// add the user
			user.setChannel(server.getChannel(channelName));
			server.channelMap[channelName].setUser(user);
		}
		else
		{
			std::string nickName = user.getNickName();
			server.setBroadcast(ERR_USERONCHANNEL(nickName, channelName), user.getSocket());
			return ; 
		}
	}
	// send response to client
	std::string reply = user.getPrefix() + " " + user.userMessageBuffer;
	server.setBroadcast(channelName, user.getNickName(), reply);
	std::string topic = server.channelMap[channelName].getTheme();
	if (topic.empty())
		reply += RPL_NOTOPIC(channelName);
	else
		reply += RPL_TOPIC(user.getNickName(), channelName, topic);
	server.setBroadcast(reply, user.getSocket());
}