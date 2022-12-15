/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   reply.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ddecourt <ddecourt@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/12/05 13:15:20 by ddecourt          #+#    #+#             */
/*   Updated: 2022/12/15 18:09:07 by ddecourt         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "main.hpp"
#include "server.hpp"

std::string RPL_WELCOME(User user)
{
    std::string buffer = user.getPrefix() +  " 001 " + user.getNickname() + " :Welcome to the Internet Relay Network " + user.getNickname() + "!" + user.getUsername() + "@" + user.getHostname() + END;
    //std::cout << buffer << std::endl;
    return buffer;
}

std::string RPL_YOURHOST(User user)
{   
    std::string buffer = user.getPrefix() +  " 002 " + user.getNickname() + " :Your host is " + user.getHostname() + ", running version 1.0" + END;
    return buffer;
}

std::string RPL_CREATED(User user, std::string time)
{
    std::string buffer = user.getPrefix() +  " 003 " + user.getNickname() + " :This server was created " + time + END;
    return buffer;
}

std::string RPL_MYINFO(User user)
{
    std::string buffer = user.getPrefix() + "004" + user.getNickname() + " :";
    return buffer;
}

void send_reply(int fd, std::string rpl)
{
    if (send(fd, rpl.c_str(), rpl.length(), 0) == -1)
        std::cout << "error" << std::endl;
}

std::string reply(User user, Server server, int num)
{
    switch(num)
    {
        case 001:
            return(RPL_WELCOME(user));
        case 002:
            return(RPL_YOURHOST(user));
        case 003:
            return(RPL_CREATED(user, server.getCreationTime()));
        case 004:
            return(RPL_MYINFO(user));
        default:
            return NULL;
    }
}