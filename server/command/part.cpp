/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   part.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ddecourt <ddecourt@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/12/08 20:30:10 by ddecourt          #+#    #+#             */
/*   Updated: 2023/01/18 13:47:39 by ddecourt         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

//to leave a channel

#include "main.hpp"

void Command::part(Message *msg, std::vector<std::string> message)
{
    User *user = msg->getuser();
    Channel *channel;
    Server *server = msg->getserver();
    std::vector<User> all_usr;

    if (message.size() == 1)
    {
        send_reply(user->getFd(), ERR_NEEDMOREPARAMS("PART"));
        return ;
    }
    if ((channel = server->get_channel_by_name(message[1])) != NULL)
    {
        if (channel->isUserinChannel(*user))
        {
		    if (channel->is_usermode( user->getFd() , 'o'))
            {
                all_usr = channel->getUsers();
                std::vector<User>::iterator it;
                if (all_usr.size() > 1) {
                    for (it = all_usr.begin(); it != all_usr.end(); it++)
                    {
                        if ((*it).getNickname() != user->getNickname() && channel->getUserMode((*it).getFd()).find("o") != std::string::npos) {
                            break ;
                        }
                    }
                    if (it == all_usr.end()) {
                        it = all_usr.begin();
                        if ((*it).getNickname() != user->getNickname()) {   
                            channel->ChangeMode((*it).getFd(), "+o");
                        }
                        else {
                            it++;
                            channel->ChangeMode((*it).getFd(), "+o");
                        }
                    }
                }
            }
            channel->broadcast_msg(user->getPrefix() + " PART " + channel->getName() + END, user);
            channel->deleteUser(*user);
            send_reply(user->getFd(), user->getPrefix() + " PART " + channel->getName() + END);
            std::cout << RED << "==> [PART] " << CYAN << "User " << user->getNickname() << " part from the channel " << DEFAULT << std::endl;   
            return ;
        }
        return (send_reply(user->getFd(), user->getPrefix() + " 442 " + ERR_NOTONCHANNEL(channel->getName())));
    }
    return (send_reply(user->getFd(), user->getPrefix() + " 403 " + ERR_NOSUCHCHANNEL(message[1])));
}