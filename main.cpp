/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hariskon <hariskonstadinidis@gmail.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/11/12 18:40:59 by hkonstan          #+#    #+#             */
/*   Updated: 2026/11/16 15:36:00 by hariskon         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "main.hpp"

int main()
{
    //CREATES SOCKET
    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd == -1)
    {
        std::perror("server socket creation failed");
        return 1;
    }
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
    if (poll_fd == -1)
    {
        std::perror("epoll instance failed");
        close(server_fd);
        return 1;
    }
    //ADD SERVER SOCKET TO THE MONITORING INSTANCE
    epoll_ctl(poll_fd, EPOLL_CTL_ADD,server_fd, &event);

    //CREATE THE EVENT ARRAY TO STORE THE EVENTS EPOLL RETURNS
    epoll_event events[10]{};
    std::unordered_map<int, Client> clients;
    
    while (true)
    {  
        int ready = epoll_wait(poll_fd, events, 10, 1);
        if (ready == -1)
        {
            std::perror("epoll failed");
            //clean fd buffers
        }
        
        for(int i = 0; i < ready ; i++)
        { 
            int fd = events[i].data.fd;
            uint32_t flags = events[i].events;
            
            if (fd == server_fd)
            {
                // if (flags & (EPOLLERR | EPOLLHUP))
                // {
                //     //handle error
                // }
                
                if (flags & EPOLLIN)
                {
                    int client_fd = accept(events[i].data.fd, nullptr, nullptr);
                    if (client_fd == -1)
                    {
                        std::perror("accept failed");
                        break;
                    }
                    std::cout << "client connected\n";
                    
                    epoll_event client_event{};
                    client_event.events = POLLIN;
                    client_event.data.fd = client_fd;
                    
                    if (epoll_ctl(poll_fd, EPOLL_CTL_ADD, client_fd, &client_event))
                    {
                        std::perror("epoll_ctl failed");
                        break;
                    }
            
                    Client new_client;
                    clients.emplace(client_fd, new_client);
                }
            }
            else
            {
                if (flags & EPOLLIN)
                {
                    char buffer[100];
        
                    ssize_t n = recv(fd, buffer, sizeof(buffer), 0);
        
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
                        
                        send(fd, response.data(), response.size(), 0);
                    }
                }
            }
        }
    }

    for (auto it = clients.begin(); it == clients.end();)
    {
        close(it->first);
        it++;
        clients.erase(it);
    }
    close(server_fd);
    return 0;
}