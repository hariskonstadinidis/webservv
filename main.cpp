/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hkonstan <hkonstan@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/11/12 18:40:59 by hkonstan          #+#    #+#             */
/*   Updated: 2026/11/13 17:53:41 by hkonstan         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "main.hpp"

int main()
{
    int server_fd = socket(AF_INET, SOCK_STREAM, 0);

    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_port = htons(8080);
    addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);

    int b = bind(server_fd, reinterpret_cast<sockaddr*>(&addr), sizeof(addr));
    if (b == -1)
    {
        perror("bind");
        return 1;
    }

    int l = listen(server_fd, 10);
    if (l == -1)
    {
        perror("listen");
        return 1;
    }

    epoll_event event{};
    event.events = EPOLLIN;
    event.data.fd = server_fd;
    
    int poll_fd = epoll_create1(0);
    epoll_ctl(poll_fd, EPOLL_CTL_ADD,server_fd, &event);

    epoll_event events[10]{};
    
    int ready = epoll_wait(poll_fd, events, 10, -1) ;
    if (ready == -1)
    {
        std::cerr << "epoll fail\n";
    }
    while (true)
    {  
        for(int i = 0; i < ready ; i++)
        {
            
            if (events[i].events & POLLIN)
            {
                int client_fd = accept(events[i].data.fd, nullptr, nullptr);
                if (client_fd == -1)
                    std::perror("accept");
                std::cout << "client connecte\n";
                
                epoll_event client_event{};
                client_event.events = POLLIN;
                client_event.data.fd = client_fd;
                
                epoll_ctl(poll_fd, EPOLL_CTL_ADD, client_fd, &client_event);
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