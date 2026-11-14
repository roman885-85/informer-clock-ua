// Заглушка lwip/sockets.h (tools/sim/ntpsim): структури — з системи, самі виклики — модель мережі (world.cpp).
#pragma once
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <netinet/in.h>
#include <arpa/inet.h>
int lwip_socket(int domain, int type, int protocol);
int lwip_sendto(int s, const void *data, size_t size, int flags, const struct sockaddr *to, socklen_t tolen);
int lwip_select(int maxfdp1, fd_set *readset, fd_set *writeset, fd_set *exceptset, struct timeval *timeout);
int lwip_recvfrom(int s, void *mem, size_t len, int flags, struct sockaddr *from, socklen_t *fromlen);
int lwip_close_r(int s);
