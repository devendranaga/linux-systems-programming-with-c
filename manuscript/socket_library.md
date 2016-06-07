# socket library

We are going to write a socket library as an exercise to what we have learnt in the socket programming.

The socket library has the following API.
```

The below are the error numbers to return by the APIs that we are about to write.

typedef enum {
    TCP_SOCK_OPEN_SUCCESS = 0,
    TCP_SOCK_OPEN_ERROR_INVAL_PROTO,
    TCP_SOCK_BIND_ERROR_SERVER_ALREADY_BOUND,
    TCP_SOCK_BIND_ERROR_INVAL_AF,
    TCP_SOCK_CONN_ERROR_INVAL_CONN,
    TCP_SOCK_ACCEPT_ERROR_INVAL_CLIENT,
    TCP_SOCK_SEND_ERROR_SOCK_CLOSED,
    TCP_SOCK_SEND_ERROR_SOCK_INVAL,
    UDP_SOCK_OPEN_ERROR_INVAL_PROTO,
    UDP_SOCK_BIND_ERROR_SERVER_ALREADY_BOUND,
    UDP_SOCK_BIND_ERROR_INVAL_AF,
    UDP_SOCK_CONN_ERROR_INVAL_CONN,
    UDP_SOCK_SEND_ERROR_SOCK_CLOSED,
    UDP_SOCK_SEND_ERROR_SOCK_INVAL,
} socketlib_ret_t;

// default connections to the listen function
#define SOCKET_LIB_MAX_CONN 100

1. tcp_socket_create_client(char *ip_port);
2. tcp_socket_create_server(char *ip_port, tcp_socket_accept_conn_cb *);
3. tcp_socket_destroy(int sock);
4. tcp_socket_accept_conn_cb(int sock, struct sockaddr *remote, socklen_t *remote_len);
5. tcp_socket_send(int sock, void *buf, int len, int opts);
6. tcp_socket_recv(int sock, void *buf, int len, int opts);
7. udp_socket_create_client(char *ip_port, char *to_send_ip_port);
8. udp_socket_create_server(char *ip_port, udp_socket_accept_conn_cb *);
9. udp_socket_destroy(int sock);
10. udp_socket_send(int sock, void *buf, int len, int opts);
11. udp_socket_recv(int sock, void *buf, int len, int opts);
12. udp_socket_sendto(int sock, void *buf, int len, int opts, struct sockaddr *addr, socklen_t len);
13. udp_socket_recvfrom(int sock, void *buf, int len, int opts, struct sockaddr *addr, socklen_t *len);
14. socklib_select(int sock, fd_set *rdfds, struct timeval *timeout);
```
