/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bben-yaa <bben-yaa@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/11/24 13:05:12 by ddecourt          #+#    #+#             */
/*   Updated: 2023/01/07 15:38:57 by bben-yaa         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "main.hpp"
#include <cctype>

Server::Server()
{
    last_ping = 0;
    heartbeat = time(0);
    creation = time(0);
    std::cout << GREEN << "==> Create new server" << DEFAULT << std::endl;
}

Server::Server(const Server &lhs)
{
    last_ping = lhs.last_ping;
    heartbeat = lhs.heartbeat;
}

void Server::setLastping(time_t time)
{
    last_ping = time;
    return;
}

Server::~Server()
{
    std::cout << "Server deleted" << std::endl;
}

//https://www.geeksforgeeks.org/socket-programming-cc/
void Server::init(int port)
{
    int opt = 1;
    //int PORT = 1054;

    if (( sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        perror("socket creation failed");
        // exit(EXIT_FAILURE);
    }
    else
        std::cout << GREEN << "==> Socket succesfully created" << DEFAULT << std::endl;

    if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt)))
    {
        perror("setsockopt");
        // exit(EXIT_FAILURE);
    }
    
    if (fcntl(sockfd, F_SETFL, O_NONBLOCK) < 0)
    {
        perror("fcntl failed");
        // exit(EXIT_FAILURE);
    }

    struct sockaddr_in address;
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(port);
    //The htons() function converts the unsigned short integer 
    //hostshort from host byte order to network byte order.

    if (bind(sockfd, (struct sockaddr*)&address, sizeof(address)) < 0)
    {
        perror("bind failed");
        // exit(EXIT_FAILURE);
    }

    if (listen(sockfd, address.sin_port) < 0) 
    {
        perror("listen");
        // exit(EXIT_FAILURE);
    }

    //memset(&this->fds[0], 0 , sizeof(fds));
    //SEGV if we don't push_back an element to "malloc" the space for a pollfd 
    fds.push_back(pollfd());
    fds.back().fd = sockfd;
    fds.back().events = POLLIN;
}

void Server::execute()
{
    //https://www.ibm.com/docs/en/i/7.2?topic=designs-using-poll-instead-select
    
    int ping = 2 * 1000;
    int timeout = (15 * 1000);
    int now = time(0);


    if (now - last_ping >= (timeout / 1000)) 
    {
        // std::cout << "NOW time = " << now << std::endl;
        // std::cout << "result = " << (now - last_ping >= ping) << std::endl;
        // std::cout << "Ping time = " << ping << std::endl;
        // std::cout << "Last Ping time = " << last_ping << std::endl;
        heartbeat_management(timeout, now);
    }
    
    //std::cout << "Start executing" << std::endl;
    
    // fds -> Use ref ptr &myvar[0]
    // nfds -> Nbr of fd open myvar.size()
    // timeout -> heartbeat timeout ping delay
    
    //int poll(struct pollfd *fds, nfds_t nfds, int timeout);
    int rc = -1;
    if ((rc = poll(&this->fds[0], fds.size(), ping)) < 0)
    {
        std::cerr << "  poll() failed." << std::endl;
        return;
    }
    
    // if (rc == 0)
    // {
    //     std::cerr << "  poll() timed out.  End program." << std::endl;
    //     return;
    // }
    
    if(this->fds[0].revents == POLLIN)
    {
        accept_new_user();
        std::cout << BLUE << "==> New user try to connect" << DEFAULT << std::endl;   
    }
    else
    {
        //How do I access each element?  
        //iterer sur les fd 
        //for
            //if(this->fds[it].revents == POLLIN)
            //    receive (UDF)
        std::vector<pollfd>::iterator it; 
         for (it = fds.begin() ; it != fds.end(); it++)
        {
            if((*it).revents == POLLIN)
            {
                User *user;
                user = get_user_by_fd((*it).fd);
                Message message(user, this);
                message.receive_msg();
            }
        }
        //std::cout << "need to find back the users of POLLIN" << std::endl;
    }
    std::map<int, User>::iterator it;
    std::vector<Channel> *channel = get_all_channels();
    for (it = users.begin(); it != users.end(); it++)
    {
        if ((*it).second.isOnline() == false)
        {
            //remove user from channel list.
            std::vector<Channel>::iterator it2; 
            for(it2 = channel->begin(); it2 != channel->end(); it2++)
                (*it2).RemoveUserFromChan((*it).second);
            delete ((*it).second)._cmd;
            remove_user(&(*it).second);
        }
    }
}

void Server::accept_new_user()
{
    //std::cout << "need to accept a user in the channel" << std::endl;
    sockaddr_in address;
    socklen_t len = sizeof(address);
    //std::string infos;

    //int accept(int sockfd, struct sockaddr *addr, socklen_t *addrlen);
    int fd = accept(sockfd, (struct sockaddr*)&address, &len);
    if ( fd < 0)
        return;
    User newuser(fd, address);
    //std::cout << "new user -> is register = " << newuser.isRegistered() << std::endl; 
    std::map<int, User>::iterator it;
    it = users.begin();
    users.insert(it, std::make_pair(fd, newuser));
    
    fds.push_back(pollfd());
    fds.back().fd = fd;
    fds.back().events = POLLIN;
}

void Server::heartbeat_management(int timeout, int now)
{
    std::map<int, User>::iterator it;
    for (it = users.begin(); it != users.end(); it++)
    {
        //std::cout << "CURRENT USER " << (*it).second.getNickname() << "   " << (now - (*it).second.getLastPing()) << std::endl;
        if (now - (*it).second.getLastPing() >= timeout)
        {
            std::cout << "need to delete user, user timeout" << std::endl;
        }
        else if (now - (*it).second.getLastPing() >= (timeout / 1000))
            send_reply((*it).second.getFd(), "PING :" + (*it).second.getNickname() + END);
        else {
            return;
        }
    }
    last_ping = time(0);
}

std::vector<std::string> Server::get_all_nicknames()
{
    std::vector<std::string> ret;
    std::map<int, User>::iterator it;
    for (it = users.begin(); it != users.end(); it++)
        ret.push_back((*it).second.getNickname());
    return ret;
}

std::vector<Channel> *Server::get_all_channels()
{
    return (&channels);
}

std::vector<std::string> Server::get_all_channels_names()
{
    std::vector<std::string> ret;
    std::vector<Channel>::iterator it;
    for (it = channels.begin(); it != channels.end(); it++)
        ret.push_back((*it).getName());
    return ret;
}


User *Server::get_user_by_fd(int user_fd)
{
    std::map<int, User>::iterator it;
    for (it = users.begin(); it != users.end(); it++)
    {
        if((*it).first == user_fd) 
        {    
            return (&(it)->second);
        }
    }
    return NULL;
}

User *Server::get_user_by_nickname(std::string nickname)
{
    std::map<int, User>::iterator it;
    for (it = users.begin(); it != users.end(); it++)
    {
        if((*it).second.getNickname() == nickname)
        {
            return (&(it)->second);
        }
    }
    return NULL;
}

Channel *Server::get_channel_by_name(std::string str)
{
    std::vector<Channel>::iterator it;
    for (it = channels.begin(); it != channels.end(); it++)
    {
        if ((*it).getName() == str)
        {
            return (&(*it));
        }
    }
    return NULL;
}

void Server::addChannel(Channel *channel)
{
    channels.push_back(*channel);
    return;
}

std::string Server::getCreationTime()
{
    std::string ts = ctime(&creation);
    ts = ts.substr(0, ts.length() - 1);
    return ts;
};

void Server::remove_user(User *user)
{
    std::map<int, User>::iterator it;
    it = users.find(user->getFd());
    if (it != users.end())
    {
        close(user->getFd());
        users.erase(it);
    }
}

void Server::remove_pollfd(User *user)
{
    int fd = user->getFd();
    std::vector<pollfd>::iterator it;
    for (it = fds.begin(); it != fds.end(); it++)
    {
        if ((*it).fd == fd)
            fds.erase((it));
    } 
}