/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   channel.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: parallels <parallels@student.42.fr>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/11/30 21:55:27 by ddecourt          #+#    #+#             */
/*   Updated: 2023/01/13 15:47:04 by parallels        ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "channel.hpp"


Channel::Channel()
{
    return;
}

Channel::Channel(std::string name, User user)
{
    _name = name;
    //user.setNickname("@" + user.getNickname());
    _users.push_back(user);
    _mode = "+n";
    _users_mode.insert(std::pair<int, std::string>(user.getFd(), "+o"));
}

Channel::Channel(User user)
{
    _users.push_back(user);
    _mode = "+n";
    _users_mode.insert(std::pair<int, std::string>(user.getFd(), "+o"));
}


Channel::Channel(const Channel &lhs)
{
    _name = lhs._name;
    _topic = lhs._topic;
    _mode = lhs._mode;
    _users = lhs._users;
    _key = lhs._key;
    _users_mode = lhs._users_mode;
    return;
}

Channel &Channel::operator=(const Channel &lhs)
{
    this->_mode = lhs._mode;
    this->_name = lhs._name;
    this->_topic = lhs._topic;
    this->_users = lhs._users;
    this->_key = lhs._key;
    this->_users_mode = lhs._users_mode;
    return (*this);
}

Channel::~Channel()
{
    return;
}

bool Channel::isUserinChannel(User user)
{
    std::vector<User>::iterator it;
    for (it = _users.begin(); it != _users.end(); it++)
    {
        if (user.getNickname() == (*it).getNickname())
            return true;
    }
    for (it = _users.begin(); it != _users.end(); it++)
    {
        if ("@" + user.getNickname() == (*it).getNickname())
            return true;
    }
    
    
    return false;
}

bool Channel::isUserinvited(User user)
{
    std::vector<User>::iterator it;
    for (it = _users_invited.begin(); it != _users_invited.end(); it++)
    {
        if (user.getNickname() == (*it).getNickname())
            return true;
    }
    return false;
}

void Channel::addUser(User user)
{
    _users.push_back(user);
    return;
}

void Channel::addUserInvited(User user)
{
    _users_invited.push_back(user);
    return;
}

void Channel::deleteUser(User user)
{
    std::vector<User>::iterator it;
    for (it = _users.begin(); it != _users.end(); it++)
    {
        if ((*it).getNickname() == user.getNickname())
            break;
    }
    _users.erase(it);
    return;
}

void Channel::addUserMode(int fd, std::string mode)
{
    _users_mode.insert(std::pair<int, std::string>(fd, mode));
}

void Channel::removeUserMode(int fd)
{
    std::map<int, std::string>::iterator it;
    it = _users_mode.find(fd);
    if (it != _users_mode.end())
        _users_mode.erase(it);
}

User *Channel::get_user(User *user)
{
    std::vector<User>::iterator it;
    for (it = _users.begin(); it != _users.end(); it++)
    {
        if((*it).getFd() == user->getFd()) 
        {    
            return &(*(it));
        }
    }
    return NULL;
}

 void Channel::broadcast(std::string rpl)
 {
    std::vector<User>::iterator it;
    for (it = _users.begin(); it != _users.end(); it++)
    {
        if (send((*it).getFd(), rpl.c_str(), rpl.length(), 0) == -1)
        {   
            std::cout << PURPLE << "Error: send function failed" << DEFAULT << std::endl;
            return ;
        }
    }
 }

  void Channel::broadcast_msg(std::string rpl, User *user)
 {    
    std::vector<User>::iterator it;
    for (it = _users.begin(); it != _users.end(); it++)
    {
        if (((*it).getFd() != user->getFd()) && ((*it).getFd() > 0))
        {
            if (send((*it).getFd(), rpl.c_str(), rpl.length(), 0) == -1)
            {
                std::cout << PURPLE << "Error: send function failed" << DEFAULT << std::endl;      
                return ;
            }
        }
    }
 }

void Channel::print_users()
{
    std::vector<User>::iterator it;
    for(it = _users.begin(); it != _users.end(); it++)
    {
        std::cout << RED << (*it).getNickname() << DEFAULT << std::endl;
    }
}

std::string Channel::getName() {return _name;}
std::string Channel::getTopic() {return _topic;}
std::string Channel::getMode() {return _mode;}
std::string Channel::getKey() {return _key;}
std::vector<User> Channel::getUsers() {return _users;}
std::string Channel::getUserMode(int fd) {return  (_users_mode.find(fd))->second;}
int Channel::usr_size(){return (_users.size());};


bool Channel::is_mode(char x)
{
    if (this->getMode().find(x) != std::string::npos)
        return true;
    return false;
}

bool Channel::is_usermode(int fd, char x)
{
    if (this->getUserMode(fd).find(x) != std::string::npos)
        return true;
    return false;
}

void Channel::setName(std::string name)
{
    _name = name;
    return;
}
void Channel::setTopic(std::string topic) {_topic = topic; return;}
void Channel::setMode(std::string mode) {_mode = mode; return;}
void Channel::setKey(std::string key) {_key = key; return;}
void Channel::setUserMode(std::string mode, User user)
{
    _users_mode.insert(std::pair<int, std::string>(user.getFd(), mode));
}


void    Channel::RemoveUserFromChan(User user)
{
    if (isUserinChannel(user) == true)
        deleteUser(user);
}