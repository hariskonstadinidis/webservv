/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hariskon <hariskonstadinidis@gmail.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/11/12 18:40:59 by hkonstan          #+#    #+#             */
/*   Updated: 2026/11/16 13:33:33 by hariskon         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "main.hpp"

int main()
{
    //CREATES SOCKET
    int server_fd = socket(AF_INET, SOCK_STREAM, 0);

    //CREATES ADDRESS
    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_port = htons(8080);
    addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);

    //BIND ADRESS TO THAT SOCKET
    if (bind(server_fd, reinterpret_cast<sockaddr*>(&addr), sizeof(addr)) == -1)
    {
        std::perror("bind");
        close(server_fd);
        return 1;
    }

    //PREPARE THE SOCKET TO RECEIVE REQUESTS
    if (listen(server_fd, 10) == -1)
    {
        std::perror("listen");
        close(server_fd);
        return 1;
    }

    //CREATE AN EVENT FOR THE SERVER
    epoll_event event{};
    event.events = EPOLLIN;
    event.data.fd = server_fd;
    
    //CREATE A POLL INSTANCE TO MONITOR THE REQUESTS
    int poll_fd = epoll_create1(0);
    //ADD SERVER SOCKET TO THE MONITORING INSTANCE
    epoll_ctl(poll_fd, EPOLL_CTL_ADD,server_fd, &event);

    epoll_event events[10]{};
    
    while (true)
    {  
        if (epoll_wait(poll_fd, events, 10, 1) == -1){
            
            std::perror("epoll failed");
            //clean fd buffers
            for(int i = 0;)
        }
        
        for(int i = 0; i < ready ; i++)
        {
            
            if (events[i].events & POLLIN)
            {
                if (events[i].data.fd == server_fd)
                {
                    int client_fd = accept(events[i].data.fd, nullptr, nullptr);
                    if (client_fd == -1)
                        std::perror("accept");
                    std::cout << "client connected\n";
                    
                    epoll_event client_event{};
                    client_event.events = POLLIN;
                    client_event.data.fd = client_fd;
                    
                    epoll_ctl(poll_fd, EPOLL_CTL_ADD, client_fd, &client_event);
                }
                else
                {
                    
                }
            }
    
            if (pfds.size() > 1 && (pfds[1].revents & POLLIN))
            {
                char buffer[100];
    
                ssize_t n = recv(pfds[1].fd, buffer, sizeof(buffer), 0);
    
                if (n > 0)
                {
                    std::ifstream file("index.html");
                    std::stringstream ss;
                    ss << file.rdbuf();
    
                    std::string html = ss.str();
    
                    std::string response =
                    "HTTP/1.1 200 OK\r\n"
                    "Content-Type: text/html\r\n"
                    "Content-Length: " + std::to_string(html.size()) + "\r\n"
                    "Connection: close\r\n"
                    "\r\n"
                    + html;
                    
                    send(pfds[1].fd, response.data(), response.size(), 0);
                }
                else
                {
                    close(pfds[1].fd);
                    pfds.erase(pfds.begin() + 1);
                }
            }
        }
    }

    for (size_t i = 0; i < pfds.size(); ++i)
        close(pfds[i].fd);

    return 0;
}