#ifndef SERVERREPLIES_HPP
#define SERVERREPLIES_HPP

#define RPL_WELCOME(hostname, nick, host) (":" + hostname + " 001 " + nick + " :Welcome to the Internet Relay Network " + nick + "!" + nick + "@" + host + "\r\n")
#define RPL_YOURHOST(hostname, nick) (":" + hostname + " 002 " + nick + " :Your host is ircserv, running version ircserv.1\r\n")
#define RPL_CREATED(hostname, nick, date) (":" + hostname + " 003 " + nick + " :This server was created " + date + "\r\n")
#define RPL_MYINFO(hostname, nick) (":" + hostname + " 004 " + nick + " : ircserv.1 modes itkol\r\n")

/*PASS / USER*/
#define ERR_NOTREGISTERED(hostname) (":" + hostname + " 451 :You have not registered\r\n")
#define ERR_ALREADYREGISTRED(hostname) (":" + hostname + " 462 ::Unauthorized command (already registered)\r\n")
#define ERR_PASSWDMISMATCH(hostname) (":" + hostname + " 464 :Password incorrect\r\n")
#define PING(servername) ("PING :" + servername + "\r\n")
#define ERR_UNKNOWNCOMMAND(hostname, command) (":" + hostname + " 421 " + command + " :Unknown command\r\n")

/*PRIVMSG*/
#define ERR_NOSUCHNICK(hostname, user_nick, target_nick) (":" + hostname + " 401 " + user_nick + " " + target_nick + ":No such nick\r\n")
#define ERR_CANNOTSENDTOCHAN(hostname, channel) (":" + hostname + " 404 " channel + " :Cannot send to channel\r\n")
#define ERR_TOOMANYTARGETS(hostname, target) (":" + hostname + " 407 " + target + ":407 recipients. Use one target only.\r\n")
#define ERR_NORECIPIENT(hostname, command) (":" + hostname + " 411 :No recipient given " + command + "\r\n")
#define ERR_NOTEXTTOSEND(hostname) (":" + hostname + " 412 :No text to send\r\n")
#define RPL_PRIVMSG(hostname, nick, target, message) (nick + " PRIVMSG " + target + " " + message + "\r\n") // nick is with prefix

/*KICK COMMAND*/
#define ERR_NOSUCHCHANNEL(hostname, nick, channel) (":" + hostname + " 403 " + nick + " " + channel + " :No such channel\r\n")
#define ERR_USERNOTINCHANNEL(hostname, nick, channel) (":" + hostname + " 441 " + nick + " " + channel + " :They aren't on that channel\r\n")
#define ERR_NOTONCHANNEL(hostname, channel) (":" + hostname + " 442 " + channel + " :You're not on that channel\r\n")
#define ERR_NEEDMOREPARAMS(hostname, command) (":" + hostname + " 461 " + command + " :Not enough parameters\r\n")
#define ERR_CHANOPRIVSNEEDED(hostname, channel) (":" + hostname + " 482 " + channel + " :You're not channel operator\r\n")
#define RPL_KICK(hostname, user_id, channel, kicked, reason) (":" + hostname + " " + user_id + " KICK " + channel + " " + kicked + " " + reason + "\r\n")

/*INVITE COMMAND*/
#define RPL_INVITE(hostname, user, invited, channel) (":" + hostname + " " + user + " INVITE " + invited + " " + channel + "\r\n")
#define RPL_INVITING(hostname, user, channel, nick) (":" + hostname + " 341 " + user + " " + nick + " " + channel + "\r\n")
#define ERR_USERONCHANNEL(hostname, user, channel) (":" + hostname + " 442 " + user + " " + channel + " :is already on channel\r\n")

/* TOPIC COMMAND: */
#define RPL_NOTOPIC(hostname, channel) (":" + hostname + " 331 " + channel + " :No topic is set\r\n")
#define RPL_TOPIC(hostname, nick, channel, topic) (":" + hostname + " 332 " + nick + " " + channel + " " + topic + "\r\n")
#define ERR_NOCHANMODES(hostname, channel) (":" + hostname + " 477 " + channel + " :Channel doesn't support modes\r\n")

/*MODE COMMAND*/
#define RPL_CHANNELMODEIS(hostname, channel, mode, mode_params) (":" + hostname + " 324 " + channel + " " + mode + " " + mode_params + "\r\n")
#define ERR_KEYSET(hostname, channel) (":" + hostname + " 467 " + channel + " :Channel key already set\r\n")
#define ERR_UNKNOWNMODE(hostname, char, channel) (":" + hostname + " 472 " + char + " :is unknown mode char to me for " + channel + "\r\n")
#define ERR_UMODEUNKNOWNFLAG(hostname, client) (":" + hostname + " 501 " + client + " :Unknown MODE flag\r\n") // client is with prefix

/*NICK COMMAND*/
#define ERR_ERRONEUSNICKNAME(hostname, nick) (":" + hostname + " 432 " + nick + " :Erroneous nickname\r\n")
#define ERR_NICKNAMEINUSE(hostname, nick) (":" + hostname + " 433 " + nick + " :Nickname is already in use\r\n")
#define ERR_NONICKNAMEGIVEN(hostname) (":" + hostname + " 431 :No nickname given\r\n")
#define RPL_NICK(hostname, old_nick, username, new_nick) (":" + hostname + " " + old_nick + "!" + username + "@localhost NICK " +  new_nick + "\r\n")

/*JOIN COMMAND*/
#define ERR_CHANNELISFULL(hostname, channel) (":" + hostname + " 471 " + channel + " :Cannot join channel (+l)\r\n")
#define ERR_INVITEONLYCHAN(hostname, channel) (":" + hostname + " 473 " + channel + " :Cannot join channel (+i)\r\n")
#define ERR_BADCHANNELKEY(hostname, channel) (":" + hostname + " 475 " + channel + " :Cannot join channel (+k)\r\n")
//                 ERR_TOOMANYCHANNELS

/*PART COMMAND*/
#define RPL_PART(hostname, user_id, channel, reason) (":" + hostname + " " +user_id + " PART " + channel + " " + (reason.empty() ? "." : reason ) + "\r\n")
#define RPL_QUIT(user_id, reason) (user_id + " QUIT : " + reason + "\r\n")

/*OPERATOR REPLY*/
#define RPL_YOUREOPER(hostname, nick) (":" + hostname + " 381 " + nick + " :You are now an IRC operator\r\n")
#define MODE_USERMSG(client, mode) (":" + client + " MODE " + client + " :" + mode + "\r\n")

#endif