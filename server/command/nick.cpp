/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   nick.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ddecourt <ddecourt@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/11/23 15:16:46 by ddecourt          #+#    #+#             */
/*   Updated: 2022/12/15 18:01:28 by ddecourt         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "main.hpp"

void Command::nick(Message *msg, std::vector<std::string> message)
{
    User *usr = msg->getuser();
    Server *server = msg->getserver();
    std::string oldnick;
    std::vector<std::string> nicknames = server->get_all_nicknames();
    (void) server;
    (void)message;
    if (message.size() == 1)
        return(send_reply(usr->getFd(), ERR_NONICKNAMEGIVEN(usr->getNickname())));
    std::string nickname = message[1];
    if (nickname.length() > 9)
        return(send_reply(usr->getFd(), ERR_ERRONEUSNICKNAME(nickname)));
    std::vector<std::string>::iterator it;
    for (it = nicknames.begin(); it != nicknames.end(); it++)
    {
        if ((*it) == nickname)
            return(send_reply(usr->getFd(), ERR_NICKNAMEINUSE(nickname)));
    }
    oldnick = usr->getNickname();
    usr->setNickname(nickname);
    if (usr->isRegistered())
    {
        // std::cout << "FD = " << usr->getFd() << std::endl;
        std::cout << "Je passe dans la commande NICK" << std::endl;
        send_reply(usr->getFd(),":" + oldnick + "!" + usr->getUsername() + "@" + usr->getHostname() + " NICK :" + usr->getNickname());
    }
} 