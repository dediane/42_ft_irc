/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   invite.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ddecourt <ddecourt@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/11/23 15:14:48 by ddecourt          #+#    #+#             */
/*   Updated: 2023/01/18 11:39:06 by ddecourt         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "main.hpp"

//:lala!diane@localhost 341 #koko :diane
//:balkis!diane@localhost 341 balkis tata diane
//:balkis!diane@localhost INVITE diane tata

void Command::invite(Message *msg, std::vector<std::string> message)
{
    User *user = msg->getuser();
    Server *server = msg->getserver();
    User *receiver;
    Channel *channel;
    std::string nick;

    
    if (message.size() < 3)
        return(send_reply(user->getFd(), user->getPrefix() + " 461 " + user->getNickname() + " " + ERR_NEEDMOREPARAMS("INVITE")));
    nick = message[1];
    if (!(receiver = server->get_user_by_nickname(nick)))
        return (send_reply(user->getFd(), user->getPrefix() + " 401 " + user->getNickname() + " " + ERR_NOSUCHNICK(message[1])));
    if(!(channel = server->get_channel_by_name(message[2])))
        return (send_reply(user->getFd(), user->getPrefix() + " 403 " + user->getNickname() + " " + ERR_NOSUCHCHANNEL(message[2])));
    if (channel->isUserinChannel(*receiver) == true)
        return (send_reply(user->getFd(), user->getPrefix() + " 443 " + user->getNickname() + " " + ERR_USERONCHANNEL(nick, channel->getName())));
    if (channel->is_mode('i') && (!channel->is_usermode(user->getFd(), 'o')))
        return (send_reply(user->getFd(), user->getPrefix() + " 482 " + user->getNickname() + " " + ERR_CHANOPRIVNEEDED(channel->getName())));
    send_reply(user->getFd(), RPL_INVITING(user, channel, receiver->getNickname()));
    send_reply(receiver->getFd(), user->getPrefix() + " INVITE " + receiver->getNickname() + " " + channel->getName() + END);
    channel->addUserInvited(*receiver);
    return;
}
