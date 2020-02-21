#include <apr.h>
#include <apr_pools.h>
#include <apr_network_io.h>
#include <apr_time.h>
#include <stdio.h>
#include <stdlib.h>

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
    
    apr_socket_create(&sock, APR_INET6, SOCK_DGRAM, APR_PROTO_UDP, pool);

    if (sock) {
        printf("socket created at:%p\n", sock);
    }

    apr_sockaddr_t *local_addr = NULL;

    apr_sockaddr_info_get(&local_addr, argv[1], APR_INET6, is_src ? 20000 : 30000, 0, pool);

    ret = apr_socket_bind(sock, local_addr);

    if (ret == APR_SUCCESS) {
        puts("bind addr ok!");
    }

    ret = apr_mcast_interface(sock, local_addr);

    if (ret == APR_SUCCESS) {
        puts("set mcast ok!");
    }

    apr_sockaddr_t * dst = NULL;

    ret = apr_sockaddr_info_get(&dst, "ff08::8", APR_INET6, 30001, 0, pool);

    if (ret == APR_SUCCESS) {
        puts("dst addr ok!");
    }else{
        puts("invalid dst addr?");
        exit(-1);
    }


    if (is_src) {

        FILE * frtp = fopen("pcmu.rtp", "rb");
        char *fbuf = (char *)malloc(1024*1024);
        int cnt = 0;

        if(frtp == NULL){
            puts("rtp file not available");
            exit(-1);
        }

        fseek(frtp, 0, SEEK_END);

        apr_size_t all_size = ftell(frtp);

        fseek(frtp, 0, SEEK_SET);

        printf("file size:%ld\n", all_size);

        apr_size_t pos = 0;

        fread(fbuf, 1, all_size, frtp);

        apr_time_t t_start = apr_time_now();

        while (pos <= all_size - 172) {
            
            cnt++;

            apr_size_t size = 172;

            ret = apr_socket_sendto(sock, dst, 0, fbuf + pos, &size);

            apr_time_t t_now = apr_time_now();

            apr_time_t t_diff = t_now - t_start;

            apr_time_t t_sleep = 0;

            if(t_diff < cnt * 20 * 1000){
               t_sleep = cnt * 20 * 1000 - t_diff;
               apr_sleep(t_sleep);

            }

            apr_time_t t_send = apr_time_now();

            pos += 172;

            if (ret == APR_SUCCESS) {
                printf("[%d] sent ok! sleep:%ld, \t delay:%ld\n", cnt, t_sleep, t_send - t_now);
            }
        }
        fclose(frtp);
        free(fbuf);
    }
    else {

        ret = apr_mcast_join(sock, dst, local_addr, NULL);

        if (ret == APR_SUCCESS) {
            printf("join mcast ok!\n");
        }

        char msg[2000] = { 0 };

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
                printf("recv ok! \n");
            }
            
        }
    }




    return 0;
}
