/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   mode.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: parallels <parallels@student.42.fr>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/12/09 16:35:32 by ddecourt          #+#    #+#             */
/*   Updated: 2023/01/13 16:41:59 by parallels        ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "main.hpp"
#include "command.hpp"

//User modes
// - [o] - User is an operator (prefix @).
// - [w] - User receives wallops;
// - [r] - restricted user connection.


//Channel modes
// [O] - give "channel creator" status;
// [o] - give/take channel operator privilege;

// [a] - toggle the anonymous channel flag;
// [q] - toggle the quiet channel flag;
// [p] - toggle the private channel flag;
// [s] - toggle the secret channel flag;
// [l] - set/remove the user limit to channel;
// [r] - toggle the server reop channel flag;

// [t] - toggle the topic settable by channel operator only flag;
// [i] - toggle the invite-only channel flag;
// [k] - set/remove the channel key (password);
// [n] - toggle the no messages to channel from clients on the outside;
// [m] - toggle the moderated channel;
// [v] - give/take the ability to speak on a moderated channel;

// [b] - set/remove ban mask to keep users out;
// [e] - set/remove an exception mask to override a ban mask;
// [I] - set/remove an invitation mask to automatically override the invite-only flag;

bool isvalid_mode(std::string mode, std::string validmodes)
{
    //mode valid for channel : [t][i][k][n][m][v]  && [o] for user in channel
    //mode valid for user: [o][i][w]
    if (mode.size() > 1)
    {
        std::string modes = validmodes;
        std::string::iterator it;
        std::string::iterator it2;
        if (!(*mode.begin() == '+' || *mode.begin() == '-'))
            return(false); 
        for (it2 = mode.begin() + 1 ; it2 != mode.end(); it2++)
        {
            it = modes.begin();
            for (it = modes.begin(); it != modes.end(); it++)
                if (!mode.find(*it))
                    return(false); //"ERROR 472"
        }
        return (true);
    }
    return (false);
}

std::string Command::addmode(std::string mode, std::string oldmode)
{
    std::string::iterator it;
    std::string::iterator it2;
    mode.erase(0,1);
    std::string copy = mode;
    for (it = mode.begin(); it != mode.end(); it++)
    {
        for (it2 = oldmode.begin() + 1; it2 != oldmode.end(); it2++)
        {
            if (*it == *it2)
                copy.erase(copy.find(*it), 1);
        }
    }
    oldmode.append(copy);
    return (oldmode);
}

std::string Command::deletemode(std::string mode, std::string oldmode)
{
    std::string::iterator it;
    std::string::iterator it2;
    std::string copy = oldmode;
    mode.erase(0,1);
    for (it = mode.begin(); it != mode.end(); it++)
    {
        for (it2 = oldmode.begin() + 1; it2 != oldmode.end(); it2++)
        {
            if (*it == *it2)
               copy.erase(copy.find(*it), 1);
        }
    }
    return (copy);
}

void Command::mode_channel(Message *msg, std::vector<std::string> message)
{
    Server *server = msg->getserver();
    Channel *channel;
    User *user = msg->getuser();
    User *target;
    User *channel_user;
    std::string mode;

    if(!(channel = server->get_channel_by_name(message[1])))
        return (send_reply(user->getFd(), user->getPrefix() + " 403 " + ERR_NOSUCHCHANNEL(message[1])));
    if (message.size() == 4)
    {
        if(!channel->is_usermode(user->getFd(), 'o'))
            return (send_reply(user->getFd(), user->getPrefix() + " 482 " + user->getNickname() + " " + ERR_CHANOPRIVNEEDED(channel->getName())));
        mode = message[2];
        if (!(target = server->get_user_by_nickname(message[3])))
            return (send_reply(user->getFd(), user->getPrefix() + " 401 " + ERR_NOSUCHNICK(message[3])));
        if (!channel->isUserinChannel(*target))
            return (send_reply(user->getFd(), user->getPrefix() + " 442 " + ERR_NOTONCHANNEL(message[3])));
        channel_user = channel->get_user(target);
        if (isvalid_mode(mode, "o") == true)
        {
            std::string new_mode;
            if (!channel->getUserMode(channel_user->getFd()).empty() && *mode.begin() == '+')
                new_mode =  addmode(mode, channel->getUserMode(channel_user->getFd()));
            else if  (!channel->getUserMode(channel_user->getFd()).empty() && *mode.begin() == '-')
                new_mode = deletemode(mode, channel->getUserMode(channel_user->getFd()));
            channel->removeUserMode(channel_user->getFd());
            channel->addUserMode(channel_user->getFd(), new_mode);
            std::string tmp = mode;
            mode = user->getNickname() + " " + channel->getName() + " " + tmp + " " + target->getNickname(); 
            send_reply(user->getFd(), RPL_CHANNELMODEIS(user, channel, mode));
            send_reply(target->getFd(), RPL_CHANNELMODEIS(user, channel, mode));
            return;
        }
    }
    if (message.size() == 3)
    {
        if(!channel->is_usermode(user->getFd(), 'o'))
            return (send_reply(user->getFd(), user->getPrefix() + " 482 " + user->getNickname() + " " + ERR_CHANOPRIVNEEDED(channel->getName())));
        mode = message[2];
        if (isvalid_mode(mode, "tiknmv") == true)
        {
            std::cout << "Je suis ici" << std::endl;
            
            if (!channel->getMode().empty() && *mode.begin() == '+')
                channel->setMode(addmode(mode, channel->getMode()));
            else if (!channel->getMode().empty() && *mode.begin() == '-')
                channel->setMode(deletemode(mode, channel->getMode()));
        }
    }
    send_reply(user->getFd(), RPL_CHANNELMODEIS(user, channel, user->getNickname() + " " + channel->getName() + " " + channel->getMode()));
    return;
}

void Command::mode_user(Message *msg, std::vector<std::string> message)
{
    Server *server = msg->getserver();
    User *user = msg->getuser();
    User *target;
    std::string mode = message[2];

    if(!(target = server->get_user_by_nickname(message[1])))
        return (send_reply(user->getFd(), user->getPrefix() + " 401 " + ERR_NOSUCHNICK(message[2])));
    if (target->getNickname() != user->getNickname())
        return (send_reply(user->getFd(), user->getPrefix() + " 502 " + ERR_USERSDONTMATCH()));
    if (message.size() == 3)
    {
        mode = message[2];
        if (isvalid_mode(mode, "oiw") == true)
        {
            if (!user->getMode().empty() && *mode.begin() == '+')
            {
                user->setMode(addmode(mode, user->getMode()));
                if (mode.find('o'))
                    user->setMode(deletemode("o", user->getMode()));
            } 
            else if (!user->getMode().empty() && *mode.begin() == '-')
                user->setMode(deletemode(mode, user->getMode()));
        }
    }
    send_reply(user->getFd(),RPL_UMODEIS(user));
    return;
}

void Command::mode(Message *msg, std::vector<std::string> message)
{
    User    *usr = msg->getuser();
    Server  *server = msg->getserver();
    (void)server;
    if (message.size() == 1)
        return(send_reply(usr->getFd(), ERR_NEEDMOREPARAMS("MODE")));
    if (message[1].find("#") != std::string::npos)
        mode_channel(msg, message);
    else
        mode_user(msg, message);
}