#include "protocol.h"

#define optN 6
enum {i, sA, sP, sH, pA, aS};

int bentool_main(int argc, char **argv)
{
    char c;
    int i_flag;
    char *if_name = IF_NAME;
    int start_arg=0;
    int opt_argc=1;
    int flag[optN]={0,};
    
    while((c=getopt(argc,argv,"i::s::p::a::"))!=-1)
    {
        switch(c)
        {
            case 'i':
                if(!optarg)
                {   
                    flag[i]=1;
                    for(start_arg=optind ; optind<argc ;optind++)
                        if(argv[optind][0] == 0 || argv[optind][0]=='-')
                            break;
                    if(start_arg!=optind)
                        opt_argc += optind -start_arg;
                    break;
                }
                else if(optarg[0]=='f')
                {
                    if(optarg[1])
                    {
                        printf("Option error\n");
                        return -1;
                    }
                    int if_c;
                    for(if_c=optind ; if_c<argc ;if_c++)
                        if(argv[if_c][0] == 0 || argv[if_c][0]=='-')
                            break;
                    if(if_c == optind)
                    {
                       printf("input interface name\n");
                       return -1;
                    }
                    else if((if_c - optind) > 1)
                    {
                        printf("Too many arguments\n");
                        return -1;
                    }
                    else
                        if_name = argv[optind];
                }
                else
                    printf("No option\n");
                break;
            case 's':
                if(!optarg || optarg[1])
                {   
                    printf("Option error\n");
                    return -1;
                }
                char s = optarg[0];
                switch(s)
                {
                    case 'A':
                        flag[sA]=1;
                        for(start_arg=optind ; optind<argc ;optind++)
                            if(argv[optind][0] == 0 || argv[optind][0]=='-')
                                break;
                        if(start_arg!=optind)
                            opt_argc += optind -start_arg;
                        break;
                    case 'P':
                        flag[sP]=1;
                        for(start_arg=optind ; optind<argc ;optind++)
                            if(argv[optind][0] == 0 || argv[optind][0]=='-')
                                break;
                        if(start_arg!=optind)
                            opt_argc += optind -start_arg;
                        break;
                    case 'H':
                        flag[sH]=1;
                        for(start_arg=optind ; optind<argc ;optind++)
                            if(argv[optind][0] == 0 || argv[optind][0]=='-')
                                break;
                        if(start_arg!=optind)
                            opt_argc += optind -start_arg;
                        break;
                    default:
                        printf("No option\n");
                        return -1;
                }
                break;
            case 'p':
                if(!optarg || optarg[1])
                {   
                    printf("Option error\n");
                    return -1;
                }
                char p = optarg[0];
                switch(p)
                {
                    case 'A':
                        flag[pA]=1;
                        for(start_arg=optind ; optind<argc ;optind++)
                            if(argv[optind][0] == 0 || argv[optind][0]=='-')
                                break;
                        if(start_arg!=optind)
                            opt_argc += optind -start_arg;
                        break;
                    default:
                        printf("No option\n");
                        return -1;
                }
                break;
            case 'a':
                if(!optarg || optarg[1])
                {   
                    printf("Option error\n");
                    return -1;
                }
                char a = optarg[0];
                switch(a)
                {
                    case 'S':
                        flag[aS]=1;
                        for(start_arg=optind ; optind<argc ;optind++)
                            if(argv[optind][0] == 0 || argv[optind][0]=='-')
                                break;
                        if(start_arg!=optind)
                            opt_argc += optind -start_arg;
                        break;
                    default:
                        printf("No option\n");
                        return -1;
                }
                break;
            case '?':
                return -1;
        }
    }
    int sum = 0;
    for(int i =0; i<optN;i++)
        sum += flag[i];
        
    if(sum==0)
    {
        printf("use option\n");
        return -1;
    }
    else if(sum>1)
    {
        printf("too many option\n");
        return -1;
    }

    if(flag[i])
    {
        switch(opt_argc)
        {
            case 1:
                get_interface_devices(NULL);
                break;
            case 2:
                get_interface_devices(argv[start_arg]);
                break;
            default:
                printf("Too many arguments\n");
        }
    }
    else if(flag[sA])
    {
        switch(opt_argc)
        {
            case 1:
                arp_scan(opt_argc, if_name);
                break;
            case 2:
                arp_scan(opt_argc, if_name,argv[start_arg]);
                break;
            case 3:
                arp_scan(opt_argc, if_name,argv[start_arg], argv[start_arg+1]);
                break;
            default:
                printf("Too many arguments\n");
        }
    }
    else if(flag[sP])
    {
        switch(opt_argc)
        {
            case 1:
                ping_scan(opt_argc, if_name);
                break;
            case 2:
                ping_scan(opt_argc, if_name,argv[start_arg]);
                break;
            case 3:
                ping_scan(opt_argc, if_name,argv[start_arg], argv[start_arg+1]);
                break;
            default:
                printf("Incorrect use\n");
        }

    }
    else if(flag[sH])
    {   
        opt_argc--;
        switch(opt_argc)
        {
            case 1:
                half_open_scan(opt_argc, if_name);
                break;
            case 2:
                half_open_scan(opt_argc, if_name, argv[start_arg]);
                break;
            case 3:
                half_open_scan(opt_argc, if_name, argv[start_arg], argv[start_arg+1]);
                break;
            case 4:
                half_open_scan(opt_argc, if_name, argv[start_arg], argv[start_arg+1], argv[start_arg+2]);
                break;
            default:
                printf("Incorrect use\n");
        }

    }
    else if(flag[pA])
    { 
        switch(opt_argc)
        {
            case 3:
                arp_spoof(if_name, argv[start_arg], argv[start_arg+1]);
                break;

            default:
                printf("Incorrect use\n");
        }
    }
    else if(flag[aS])
    {
        opt_argc--;
        switch(opt_argc)
        {
            case 1:
                syn_flood(opt_argc, argv[start_arg]);
                break;
            case 2:
                syn_flood(opt_argc, argv[start_arg], argv[start_arg+1]);
                break;

            default:
                printf("Incorrect use\n");
        }
    }
    
    return 0;
}