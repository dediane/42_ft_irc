/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.hpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ddecourt <ddecourt@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/11/24 13:03:02 by ddecourt          #+#    #+#             */
/*   Updated: 2023/01/02 16:51:02 by ddecourt         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef MAIN_HPP
#define MAIN_HPP

#define END "\r\n"
#define BUFFER_SIZE 4086
#define PASSWORD "123pass"
#include <vector>
#include <map>
#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <poll.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <time.h>
#include <netdb.h>
#include <sstream>

#include "server.hpp"
#include "user/user.hpp"
#include "channel/channel.hpp"
#include "reply/reply.hpp"
#include "command/command.hpp"
#include "reply/error.hpp"
#include "message/message.hpp"
#include "colors.hpp"

#include <algorithm>



#endif