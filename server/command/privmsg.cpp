/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   privmsg.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ddecourt <ddecourt@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/12/19 13:27:00 by ddecourt          #+#    #+#             */
/*   Updated: 2023/01/18 13:18:30 by ddecourt         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "main.hpp"

void Command::privmsg(Message *msg, std::vector<std::string> message)
{
    Channel *channel;
    User *user = msg->getuser();
    Server *server = msg->getserver();
    
    std::vector<std::string>::iterator it;
    std::vector<std::string>::iterator it2;
    std::vector<std::string> channels_names = server->get_all_channels_names();
    std::string channel_name;
    std::string buffer;

    if (message.size() <= 3)
    {
        if (message[1][0] != '#') 
            return (send_reply(user->getFd(), "Error: channel must begin with '#'"));
        if (message.size() == 2)
            return (send_reply(user->getFd(), user->getPrefix() + " 412 " + ERR_NOTEXTTOSEND()));
        if (message[2] == ":")
            return (send_reply(user->getFd(), user->getPrefix() + " 404 " + ERR_CANNOTSENDTOCHAN(message[1])));
    }
    if (message[1][message[1].length() - 1] == ':' || message[2][0] != ':')
        return (send_reply(user->getFd(), user->getPrefix() + " 461 " + ERR_NEEDMOREPARAMS("PRIVMSG")));
    for (it = message.begin(); it != message.end(); ++it)
    {
        for(it2 = channels_names.begin(); it2 != channels_names.end(); ++it2)
        {
            if ((*it).find(*it2) != std::string::npos)
            {
                channel_name = (*it).substr(0, (*it2).length());
                (*it).erase(0, ((*it2).length() + 1));
            }
        }
        if (it != message.begin())
            buffer += (*it) + " ";
    }
    if(!(channel = server->get_channel_by_name(channel_name)))
        return (send_reply(user->getFd(), user->getPrefix() + " 403 " + ERR_NOSUCHCHANNEL(channel_name)));
    if (channel->is_mode('n'))
    {
        if (!channel->isUserinChannel(*user))
            return (send_reply(user->getFd(), user->getPrefix() + " 404 " + ERR_CANNOTSENDTOCHAN(channel->getName())));
    }
    if (channel->is_mode('m'))
    {
        if (!channel->is_usermode(user->getFd(), 'v') && !channel->is_usermode(user->getFd(), 'o'))
   		    return (send_reply(user->getFd(), user->getPrefix() + " 404 " + user->getNickname() + " " + ERR_CANNOTSENDTOCHAN(channel->getName())));
    }
    buffer.erase(0, 2);
    channel->broadcast_msg(user->getPrefix() + " PRIVMSG " + channel_name + " " + buffer + END, user);

}