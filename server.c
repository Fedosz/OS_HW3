#include <sys/types.h>
#include <sys/socket.h>
#include <stdio.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

int Socket(int domain, int type, int protocol) {
    int res = socket(domain, type, protocol);
    
    if (res == -1) {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }
    return res;
}

void Bind(int sockfd, const struct sockaddr *addr, socklen_t addrlen) {
    int res = bind(sockfd, addr, addrlen);

    if (res == -1) {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }
}

void Listen(int sockfd, int backlog) {
    int res = listen(sockfd, backlog);
    if (res == -1) {
        perror("listen failed");
        exit(EXIT_FAILURE);
    }
}

int Accept(int sockfd, const struct sockaddr *addr, socklen_t *addrlen) {
    int res = accept(sockfd, addr, addrlen);

    if (res == -1) {
        perror("accept failed");
        exit(EXIT_FAILURE);
    }

    return res;
}

int main(int argc, char **argv) {
    int n = atoi(argv[1]);
    int serverfd = Socket(AF_INET, SOCK_STREAM, 0);

    struct sockaddr_in adr = {0};
    adr.sin_family = AF_INET;
    adr.sin_port = htons(34543);

    Bind(serverfd,(struct sockaddr *) &adr, sizeof(adr));

    Listen(serverfd, 1);

    socklen_t addrlen = sizeof(adr);

    int fd = Accept(serverfd, (struct sockaddr *) &adr, &addrlen);

    char data[5];
    data[0] = 0; // honey / 10
    data[1] = 0; // honey % 10
    data[2] = n; // bees in hive
    data[3] = 0; // bites
    data[4] = 1;

    int flag = n + 1;

    while (flag != 0) {
        
        ssize_t nread;
        char buf[2];
        nread = read(fd, buf, 2);

        printf("msg accepted\n");

        if (nread == -1) {
            perror("read failed");
            exit(EXIT_FAILURE);
        }
        if (nread == 0) {
            printf("END OF FILE occured\n");
        }

        if (buf[0] == 1) { //is Vinni
            if (buf[1] == 2) { //3 bites
                data[4] = 0;
            } else {
                printf("Vinni arrived\n");
                int honey = data[0] * 10 + data[1];

                printf("%d honey in the hive\n", honey);

                char answer[2];

                answer[0] = 1;
                answer[1] = 1;

                if (honey >= 15) {
                    answer[0] = 2; // enough honey
                    if (data[2] < 3) {
                        answer[1] = 2;
                    }
                } 

                write(fd, answer, 2);
            }
        } else { // is Bee
            if (buf[1] == 2) { //arrives
                printf("Bee arrives with the honey\n");
                data[2]++;
                int honey = data[0] * 10 + data[1];
                honey++;
                data[0] = honey / 10;
                data[1] = honey % 10;
                char answer[2];
                answer[0] = 1;
                answer[1] = 1;
                write(fd, answer, 2);
            } else { //wants to fly for honey

                printf("Bee wants to fly away\n");
                char answer[2];
                answer[0] = 1;
                answer[1] = 1;

                if (data[2] > 1 && data[0] * 10 + data[1] + n - data[2] < 30) {
                    answer[0] = 2; //can fly
                    data[2]--;
                }

                if (data[4] = 0) {
                    answer[1] = 2;
                }

                write(fd, answer, 2); // answer if bee can fly away
            }
        }

        if (data[4] = 0) {
            flag--;
        }

    }

    close(serverfd);

    printf("Hive is going to sleep");

    return 0;
} 
