#pragma once
#define lwip_socket socket
#define lwip_setsockopt setsockopt
#define lwip_bind bind
#define lwip_listen listen
#define lwip_fcntl fcntl
#define lwip_accept accept
#define lwip_close close
#define lwip_send(f, b, n, fl) send(f, b, n, (fl) | MSG_DONTWAIT)
#define lwip_recv recv
#define MSG_DONTWAIT_ MSG_DONTWAIT
