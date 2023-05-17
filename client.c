#include <sys/types.h>
#include <sys/socket.h>
#include <stdio.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/sem.h>
#include <semaphore.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <pthread.h>

int fd;
sem_t sem_1;

int Socket(int domain, int type, int protocol) {
    int res = socket(domain, type, protocol);
    
    if (res == -1) {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }
    return res;
}

int Connect(int sockfd, const struct sockaddr *addr, socklen_t addrlen) {
    int res = connect(sockfd, addr, addrlen);
    
    if (res == -1) {
        perror("connect failed");
        exit(EXIT_FAILURE);
    }

    return res;
}

void Inet_pton(int af, const char *src, void *dst) {
    int res = inet_pton(af, src, dst);

    if (res == 0) {
        printf("inet_pton failed");
        exit(EXIT_FAILURE);
    }
    if (res == -1) {
        perror("inet_pton failed");
        exit(EXIT_FAILURE);
    }
}

int sendMSG(int msg1) {
    char msg[2];
    msg[0] = msg1 / 10;
    msg[1] = msg1 % 10;

    write(fd, msg, 2);

    sleep(1);

    char buf[2];
    int nread = read(fd, buf, 2);
    
    if (nread == -1) {
        perror("read failure");
        exit(EXIT_FAILURE);
    }
    if (nread == 0) {
        printf("EOF occured\n");
    }

    int ans = buf[0] * 10 + buf[1];
    sleep(2);
    return ans;
}

void *bee(void *args) {
    int flag = 0;
    int num = (int) args;
    num++;
    printf("Bee number %d woke up\n", num);

    int msg;

    while (flag != 1) {
        msg = 21;

        sem_wait(&sem_1);

        int buf = sendMSG(msg);

        sem_post(&sem_1);

        if (buf % 10 == 2) {
            flag = 1;
        }

        if (buf / 10 == 2) {
            printf("Bee number %d goes for the honey\n", num);
            int y = rand() % 30 + 15;
            sleep(y);

            msg = 22;

            sem_wait(&sem_1);

            sendMSG(msg);

            sem_post(&sem_1);

            printf("Bee number %d returns with the honey\n", num);

        }
        sleep(rand() % 20 + 10); 
    }
    printf("The bee is going to have a rest now\n");
}

void *Vinni(void* args) {
    int flag = 0;
    int bites = 0;

    while (flag != 1) {

        sleep(5);

        sem_wait(&sem_1);

        int msg;

        if (bites >= 3) {
            flag = 1;
            msg = 12;
        } else {
            msg = 11;
        }
        int buf = sendMSG(msg);

        sem_post(&sem_1);

        if (buf / 10 != 2) {
            printf("Not enough honey, Vinni is still sleeping\n");
        } else {
            printf("Vinni goes for the honey\n");
            if (buf % 10 == 2) {
                printf("Vinni takes the honey\n");
            } else {
                printf("bees bite Vinni and he runs away\n");
                bites++;
            }
        }

        sleep(20);
    }

    printf("Vinni decided to leave the hive alone\n");
}

int main(int argc, char **argv) {
    
    fd = Socket(AF_INET, SOCK_STREAM, 0);

    struct sockaddr_in adr ={0};
    adr.sin_family = AF_INET;
    adr.sin_port = htons(34543);

    Inet_pton(AF_INET, "127.0.0.1", &adr.sin_addr);
    Connect(fd, (struct sockaddr *) &adr, sizeof(adr));
    
    int n;
    n = atoi(argv[1]);

    sem_init(&sem_1, 0, 1);

    pthread_t threads[n];
    pthread_t vinni;

    int flag = n + 1;

    for (int i = 0; i < n; ++i) {
        pthread_create(&threads[i], NULL, bee, (void *)i);
    } 

    pthread_create(&vinni, NULL, Vinni, NULL);

    for (int i = 0; i < n; ++i) {
        pthread_join(threads[i], NULL);
    }
    pthread_join(vinni, NULL);

    while (flag != 0) {
        sleep(20);
    }

    close(fd);
    sem_destroy(&sem_1);
    return 0;
}


