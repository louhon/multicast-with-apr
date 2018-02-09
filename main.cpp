#include <apr.h>
#include <apr_pools.h>
#include <apr_network_io.h>
#include <apr_time.h>
#include <stdio.h>

int main(int argc, char *argv[])
{

    if (argc != 3) return -1;

    bool is_src = false;

    if (strcmp(argv[2], "s") == 0) {
        is_src = true;
    }

    apr_pool_t * pool = NULL;

    apr_socket_t *sock = NULL;

    apr_status_t ret = APR_SUCCESS;

    apr_initialize();

    apr_pool_create(&pool, NULL);
    
    apr_socket_create(&sock, APR_INET, SOCK_DGRAM, APR_PROTO_UDP, pool);

    if (sock) {
        printf("socket created at:%p\n", sock);
    }

    apr_sockaddr_t *local_addr = NULL;

    apr_sockaddr_info_get(&local_addr, argv[1], APR_INET, is_src ? 20000 : 30000, 0, pool);

    ret = apr_socket_bind(sock, local_addr);

    if (ret == APR_SUCCESS) {
        puts("bind addr ok!");
    }

    ret = apr_mcast_interface(sock, local_addr);

    if (ret == APR_SUCCESS) {
        puts("set mcast ok!");
    }

    apr_sockaddr_t * dst = NULL;

    ret = apr_sockaddr_info_get(&dst, "224.0.0.188", APR_INET, 30000, 0, pool);

    if (ret == APR_SUCCESS) {
        puts("dst addr ok!");
    }


    if (is_src) {

        int cnt = 0;
        char msg[20] = { 0 };

        while (1) {
            apr_sleep(1000 * 1000);
            cnt++;
            sprintf(msg, "%d\n", cnt);


            apr_size_t size = sizeof(msg);
            ret = apr_socket_sendto(sock, dst, 0, msg, &size);
            if (ret == APR_SUCCESS) {
                printf("sent ok! ");
                printf("msg:%s", msg);
            }
        }
    }
    else {

        ret = apr_mcast_join(sock, dst, local_addr, NULL);

        if (ret == APR_SUCCESS) {
            printf("join mcast ok!\n");
        }

        char msg[20] = { 0 };

        while (1) {
            apr_sleep(100 * 1000);
            apr_size_t size = sizeof(msg);
            memset(msg, 0, sizeof(msg));


            apr_sockaddr_t from;

#ifdef WIN32
#define MSG_DONTWAIT 0
#else

#endif

            apr_socket_recvfrom(&from, sock, MSG_DONTWAIT, msg, &size);

            if (ret == APR_SUCCESS && size > 0) {
                printf("recv ok! ");
                printf("msg:%s", msg);
            }
            
        }
    }




    return 0;
}
