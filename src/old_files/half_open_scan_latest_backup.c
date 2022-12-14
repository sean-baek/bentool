#include "protocol.h"
// #define TCP_PORT_MIN_SIZE 0
// #define TCP_PORT_MAX_SIZE 1023

void  strmac_to_buffer(const char *str, uint8_t *mac);

// enum {ARGV_CMD, ARGV_TARGET_IP, ARGV_START_PORT, ARGV_END_PORT};

void *tcp_thread_function(void *p);

struct param_data
{
    int sock;
    uint16_t start_port;
    uint16_t end_port;
    struct in_addr src_ip, dst_ip;

};

/*
int main(int argc, char **argv)
{
    puts("main() start\n");
    switch(argc)
    {
        case 2:
            half_open_scan(1, argv[1]);
            break;
        case 3:
            half_open_scan(2, argv[1], argv[2]);
            break;
        case 4:
            //printf("%s\n", argv[3]);
            half_open_scan(3, argv[1], argv[2], argv[3]);
            break;
        case 5:
            half_open_scan(4, argv[1], argv[2], argv[3], argv[4]);
            break;
        default:
            printf("usage()");
            break;
    }
}               
*/
int half_open_scan(int args, ...)
{
    pthread_t thread_id;
    uint16_t start_port, end_port, port;
    int on = 1;
    char* if_name;
    struct sockaddr_in addr;
    struct in_addr my_ip;
    struct nic_info nic_info;
    struct param_data param;
    struct tcp_packet packet;
   

    va_list ap;
    va_start(ap, args);

    if_name = va_arg(ap, char*);
    param.start_port= start_port = (uint16_t)atoi(va_arg(ap, char*));
    param.end_port = end_port = (uint16_t)atoi(va_arg(ap, char*));


    if((param.sock = socket(PF_INET, SOCK_RAW, IPPROTO_TCP)) < 0)
    {
        perror("socket ");
        return 1;
    }

    if(setsockopt(param.sock, IPPROTO_IP, IP_HDRINCL, &on, sizeof(on)) < 0)
    {
        perror("setsockopt ");
        return 1;
    }

    if(get_info(&nic_info, if_name)<0)
    {
        printf("Interface name error, input -if <interface_name> or don't use -if option\n");
        return -1;
    }

    memset(&addr, 0x00, sizeof(addr));
    addr.sin_family = AF_INET;
    memset(&packet, 0, sizeof(packet));

    switch(args)
    {
        case 3:
            {
                //struct in_addr start_ip, end_ip;
                uint32_t start_ip = 0, end_ip = 0, ip=0, mask=0;
                va_end(ap);

                mask = htonl(nic_info.maskaddr.s_addr);
                ip = htonl(nic_info.addr.s_addr);

                start_ip = (ip & mask) + 1;
                param.src_ip.s_addr = htonl(start_ip);
                
                //printf("start_ip : %u, %x\n", start_ip, start_ip);
                end_ip = (ip | ~mask) - 1;
                param.dst_ip.s_addr = htonl(end_ip);

                //printf("end_ip : %s\n", (char*)&end_ip);

                pthread_create(&thread_id, NULL, tcp_thread_function, &param);
                
                for(; start_ip <= end_ip; start_ip++)
                {
                    addr.sin_addr.s_addr = htonl(start_ip);
                    //printf("1\n");
                    printf("target ip : %s\n", inet_ntoa(addr.sin_addr));

                    for(port = start_port; port <= end_port; port++)
                    {
                        //printf("==============================\n");
                        //printf("target IP : %s : %d\n", inet_ntoa(addr.sin_addr), port);
                        //printf("==============================\n");
                        make_tcp_header(&packet.tcphdr, nic_info.addr, rand(), addr.sin_addr, port, rand(), 0, TH_SYN);
                        make_ip_header(&packet.iphdr, nic_info.addr, addr.sin_addr, sizeof(struct tcphdr));

                        if(sendto(param.sock, &packet, sizeof(packet), 0, (struct sockaddr *)&addr, sizeof(addr)) < 0)
                        {
                            perror("sendto ");
                            break;
                        }
                        if(port == end_port) break;
                    }   
                }
            } break;
        case 4:
            {
                // target_ip = va_arg(ap, char*);
                inet_aton(va_arg(ap, char*), &addr.sin_addr);
                va_end(ap); 
                printf("%s\n",inet_ntoa(addr.sin_addr));
                param.src_ip.s_addr = addr.sin_addr.s_addr;
                param.dst_ip.s_addr = addr.sin_addr.s_addr;
                pthread_create(&thread_id, NULL, tcp_thread_function, &param);
                for(port = start_port; port <= end_port; port++)
                {
                    make_tcp_header(&packet.tcphdr, nic_info.addr, rand(), addr.sin_addr, port, rand(), 0, TH_SYN);
                    make_ip_header(&packet.iphdr, nic_info.addr, addr.sin_addr, sizeof(struct tcphdr));

                    if(sendto(param.sock, &packet, sizeof(packet), 0, (struct sockaddr *)&addr, sizeof(addr)) < 0)
                    {
                        perror("sendto ");
                        break;
                    }
                    if(port == end_port) break;
                }
                
            } break;
        case 5:
            {    
                
                param.src_ip.s_addr = inet_addr(va_arg(ap, char*));
                param.dst_ip.s_addr = inet_addr(va_arg(ap, char*));
                va_end(ap);

                
                printf("param.src_ip : %s\n", inet_ntoa(param.src_ip));
                
                //printf("start_ip : %u, %x\n", start_ip, start_ip);
                
                printf("param.dst_ip : %s\n", inet_ntoa(param.dst_ip));

                //printf("end_ip : %s\n", (char*)&end_ip);

                pthread_create(&thread_id, NULL, tcp_thread_function, &param);
                uint32_t start_ip, end_ip;
                
                start_ip = ntohl(param.src_ip.s_addr);
                end_ip = ntohl(param.dst_ip.s_addr);

                for(; start_ip <= end_ip; start_ip++)
                {
                    addr.sin_addr.s_addr = htonl(start_ip);
                    //printf("1\n");
                    printf("target ip : %s\n", inet_ntoa(addr.sin_addr));

                    for(port = start_port; port <= end_port; port++)
                    {
                        //printf("==============================\n");
                        //printf("target IP : %s : %d\n", inet_ntoa(addr.sin_addr), port);
                        //printf("==============================\n");
                        make_tcp_header(&packet.tcphdr, nic_info.addr, rand(), addr.sin_addr, port, rand(), 0, TH_SYN);
                        make_ip_header(&packet.iphdr, nic_info.addr, addr.sin_addr, sizeof(struct tcphdr));

                        if(sendto(param.sock, &packet, sizeof(packet), 0, (struct sockaddr *)&addr, sizeof(addr)) < 0)
                        {
                            perror("sendto ");
                            break;
                        }
                        if(port == end_port) break;
                    }   
                }
                break;
            }
            break;
        // case 4:
        //     {
        //         inet_aton(va_arg(ap, char*), (struct in_addr*)&addr.sin_addr);
        //         param.start_port = start_port = atoi(va_arg(ap, char*));
        //         param.end_port = end_port = atoi(va_arg(ap, char*));
        //         va_end(ap);

        //         pthread_create(&thread_id, NULL, tcp_thread_function, &param);
                
        //         for(port = start_port; port <= end_port; port++)
        //         {
        //             make_tcp_header(&packet.tcphdr, nic_info.in_addr, rand(), addr.sin_addr, port, rand(), 0, TH_SYN);
        //             make_ip_header(&packet.iphdr, nic_info.in_addr, addr.sin_addr, sizeof(struct tcphdr));

        //             if(sendto(param.sock, &packet, sizeof(packet), 0, (struct sockaddr *)&addr, sizeof(addr)) < 0)
        //             {
        //                 perror("sendto ");
        //                 break;
        //             }
        //             if(port == end_port) break;
        //         }
        //     }
        //     break;
        default:
            break;
    }

    

    

    sleep(3);

    close(param.sock);

    return 0;
}

void *tcp_thread_function(void *p)
{
    int len;
    char buffer[PACKET_MAX_LEN];
    struct param_data *param_ptr = (struct param_data*)p;

    while((len = read(param_ptr->sock, buffer, PACKET_MAX_LEN)) > 0)
    {
        struct iphdr *iphdr = (struct iphdr *)buffer;

        if(iphdr->ip_p != IPPROTO_TCP) continue;

        struct tcphdr *tcphdr = (struct tcphdr *)(buffer + (iphdr->ip_hl << 2));
        if((ntohl(iphdr->src_ip.s_addr) >= ntohl(param_ptr->src_ip.s_addr)) && (ntohl(iphdr->src_ip.s_addr) <= ntohl(param_ptr->dst_ip.s_addr)))
        {
            if((ntohs(tcphdr->th_sport) >= param_ptr->start_port) && (ntohs(tcphdr->th_sport) <= param_ptr->end_port))
            {
                if(((tcphdr->th_flags & TH_SYN) == TH_SYN) && ((tcphdr->th_flags & TH_ACK) == TH_ACK))
                    printf("[opened] %s : %d\n", inet_ntoa(iphdr->src_ip), ntohs(tcphdr->th_sport));
            }
        }
    }

    return 0;
}

void strmac_to_buffer(const char *str, uint8_t *mac)
{
    int i;
    unsigned int tmac[6];

    sscanf(str, "%x:%x:%x:%x:%x:%x", &tmac[0], &tmac[1], &tmac[2], &tmac[3], &tmac[4], &tmac[5]);

    for(i = 0; i < 6; i += 1)
    {
        mac[i] = (unsigned char)tmac[i];
    }
}