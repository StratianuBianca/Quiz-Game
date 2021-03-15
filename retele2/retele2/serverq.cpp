#include <sys/types.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <stdlib.h>
#include <signal.h>
#include <iostream>
#include <pthread.h>
#include <chrono>
#include <sys/wait.h>
#include <sqlite3.h>

#define PORT 2020
using namespace std::chrono;
extern int errno;
sqlite3 *db;
int rc = sqlite3_open("intrebari.db", &db);
struct a
{
    char nume[1002];
    int joc, id, scor;
    int incepe_joc;
    int descriptor;
    int nrjuc, nrint, nr_cod;
    char cod[1002];
    time_t begin, end;
    char raspuns[100];
    char alegere[10];
    int intrebare, timp, acum;
    int jucatori[10000], in_timp_int;
    int nrint0[10], nrint1[10], nrint2[10], nrint3[10], nrint4[10];
    char raspbun[100];
} th[10000];

struct c
{
    char intreba[1000];
    int jucator, nr_jucatori, nr_intre, numar, nr_intpuse;
    int juca[10000];
    int jucatori[10000];
    char cod[1100];
} coduri[10000];
typedef struct thData
{
    int idThread;
    int cl;
} thData;
static void *treat(void *);
void autentificare(void *, int &);
void raspunde(void *);
int initializare2(int);
static int callback(void *data, int argc, char **argv, char **arg)
{
    int i;
    char *str = (char *)data;
    for (i = 1; i < argc - 1; i++)
    {
        strcat(str, argv[i]);
        strcat(str, "\n");
        //  printf("%s\n", argv[i]);
    }
    return 0;
}
static int callbackrasbun(void *data, int argc, char **argv, char **arg)
{
    int i;
    char *str = (char *)data;
    for (i = argc - 1; i < argc; i++)
    {
        strcpy(str, argv[i]);
    }
    return 0;
}
void scrie(int desc, char msg[1000])
{
    int lungime = 0;
    lungime = strlen(msg) + 1;
    if (write(desc, &lungime, sizeof(int)) <= 0)
    {
        perror("Eroare la write catre client");
    }
    if (write(desc, msg, lungime + 1) <= 0)
    {
        perror("Eroare la write catre client");
    }
    // printf("\n mesaj trimis: %s", msg);
}
void citire(int desc, char msgr[], int &lungime)
{
    lungime = 0;
    bzero(msgr, 1000);
    fflush(stdout);
    if (read(desc, &lungime, sizeof(int)) <= 0)
    {
        perror("Eroare la read de la client");
    }
    if (read(desc, msgr, lungime) <= 0)
    {
        perror("Eroare la read de la client");
    }
    //printf("\n raspuns client: %s ", msgr);
}

int joc(int, char s[]);
int intreb_joc(int, int);
int initializare(int);
int nr_jocuri;
int incepe(int);
int clasament(int);
int urmatorul(int);
int main()
{
    nr_jocuri = -1;
    struct sockaddr_in server;
    struct sockaddr_in from;
    int nr;
    int sd;
    int pid;
    pthread_t th[100];
    int i = 0;
    if ((sd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
    {
        perror("[server]Eroare la socket().\n");
        return errno;
    }
    int on = 1;
    setsockopt(sd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));
    bzero(&server, sizeof(server));
    bzero(&from, sizeof(from));
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = htonl(INADDR_ANY);
    server.sin_port = htons(PORT);
    if (bind(sd, (struct sockaddr *)&server, sizeof(struct sockaddr)) == -1)
    {
        perror("[server]Eroare la bind().\n");
        return errno;
    }
    if (listen(sd, 2) == -1)
    {
        perror("[server]Eroare la listen().\n");
        return errno;
    }
    while (1)
    {
        int client;
        thData *td;
        unsigned int length = sizeof(from);

        printf("[server]Asteptam la portul %d...\n", PORT);
        fflush(stdout);

        if ((client = accept(sd, (struct sockaddr *)&from, &length)) < 0)
        {
            perror("[server]Eroare la accept().\n");
            continue;
        }
        td = (struct thData *)malloc(sizeof(struct thData));
        td->idThread = i++;
        td->cl = client;
        pthread_create(&th[i], NULL, &treat, td);
    }
};
static void *treat(void *arg)
{
    struct thData tdL;
    tdL = *((struct thData *)arg);
    printf("[thread]- %d - Asteptam mesajul...\n", tdL.idThread);
    fflush(stdout);
    pthread_detach(pthread_self());
    raspunde((struct thData *)arg);
    close((intptr_t)arg);
    return (NULL);
};
void raspunde(void *arg)
{
    int rc = sqlite3_open("intrebari.db", &db);
    if (rc)
    {
        perror("Eroare");
    }
    int lungime2 = 0;
    char msg[10000];
    char msgr[10000];
    char mes[1000];
    bzero(msg, 10000);
    bzero(msgr, 10000);
    int lungime = 0;
    int numar = 0;
    int intreb = 0;
    int nr, i = 0;
    char intr[1000];
    bzero(intr, 1000);
    struct thData tdL;
    tdL = *((struct thData *)arg);
    th[tdL.idThread].descriptor = tdL.cl;
    int nrit = 0;
    while (1)
    {
        nrit++;
        fflush(stdout);
        bzero(msgr, 1000);
        lungime = 0;
        bzero(msgr, 1000);
        fflush(stdout);
        citire(tdL.cl, msgr, lungime2);
        if (strcmp(msgr, "exit") == 0)
        {
            close(tdL.cl);
            break;
        }
        if (strcmp(msgr, "nume") == 0)
        {
            citire(tdL.cl, msgr, lungime2);
            bzero(th[tdL.idThread].nume, 100);
            strcat(th[tdL.idThread].nume, msgr);
        }
        if (strcmp(msgr, "jocnou") == 0)
        {
            printf("\n jocnou");
            fflush(stdout);
            citire(tdL.cl, msgr, lungime2);
            printf("\ncod %s ", msgr);
            fflush(stdout);
            if (strcmp(msgr, "exit") == 0)
            {
                close(tdL.cl);
                break;
            }
            nr_jocuri++;
            th[tdL.idThread].nr_cod = nr_jocuri;
            strcpy(coduri[nr_jocuri].cod, msgr);
            th[tdL.idThread].joc = 1;
            strcpy(th[tdL.idThread].cod, msgr);
            th[tdL.idThread].nrjuc = 0;
            th[tdL.idThread].jucatori[0] = tdL.idThread;
        }
        if (strcmp(msgr, "jocexistent") == 0)
        {

            citire(tdL.cl, msgr, lungime2);
            if (strcmp(msgr, "exit") == 0)
            {
                close(tdL.cl);
                break;
            }
            int ok = 0;
            for (int i = 0; i < tdL.idThread; i++)
            {
                if (strcmp(th[i].cod, msgr) == 0)
                {
                    if (th[i].incepe_joc != 0)
                    {
                        bzero(mes, 1000);
                        strcat(mes, "jocinceput");
                        scrie(tdL.cl, mes);
                        break;
                    }
                    else
                    {
                        bzero(mes, 1000);
                        strcat(mes, "corect");
                        scrie(tdL.cl, mes);
                        strcat(th[tdL.idThread].cod, msgr);
                        th[tdL.idThread].joc = 0;
                        th[tdL.idThread].incepe_joc = 0;
                        th[i].nrjuc++;
                        th[i].jucatori[th[i].nrjuc] = tdL.idThread;
                        printf("\n la inceput %d", th[i].jucatori[th[i].nrjuc]);
                        fflush(stdout);
                        strcpy(th[tdL.idThread].alegere, th[i].alegere);
                        th[tdL.idThread].nr_cod = th[tdL.idThread].nr_cod;
                        th[tdL.idThread].nrint = th[tdL.idThread].nrint;
                        ok = 1;
                        break;
                    }
                }
            }
            if (ok == 0)
            {
                bzero(mes, 1000);
                strcat(mes, "gresit");
                scrie(tdL.cl, mes);
            }
        }
        if (strcmp(msgr, "iesire") == 0)
        {
            if (th[tdL.idThread].joc == 0)
            {
                for (int jj = 0; jj <= th[tdL.idThread].nrjuc; jj++)
                {
                    if (coduri[th[tdL.idThread].nr_cod].jucatori[jj] == tdL.idThread)
                    {
                        for (int kk = jj; kk < th[tdL.idThread].nrjuc; kk++)
                        {
                            coduri[th[tdL.idThread].nr_cod].jucatori[kk] = coduri[th[tdL.idThread].nr_cod].jucatori[kk + 1];
                        }
                    }
                }
                int nrr = 0;
                for (int j = 0; j < 10000; j++)
                {
                    if (strcmp(th[tdL.idThread].cod, th[j].cod) == 0 && j != tdL.idThread)
                    {
                        for (int jj = 0; jj <= th[tdL.idThread].nrjuc; jj++)
                        {
                            if (th[j].jucatori[jj] == tdL.idThread)
                            {
                                for (int kk = jj; kk < th[tdL.idThread].nrjuc; kk++)
                                {
                                    th[j].jucatori[kk] = th[j].jucatori[kk + 1];
                                }
                            }
                            if (coduri[th[tdL.idThread].nr_cod].jucator == th[j].jucatori[jj])
                            {
                                nrr = coduri[th[tdL.idThread].nr_cod].jucator;
                                printf("\n asas %d", coduri[th[tdL.idThread].nr_cod].jucator);
                            }
                        }
                        th[j].nrjuc--;
                    }
                }
                printf("\n nrr %d", nrr);
                int nr2 = coduri[th[tdL.idThread].nr_cod].jucatori[nrr + 1];
                if (nr2 == tdL.idThread && coduri[th[tdL.idThread].nr_cod].jucatori[coduri[th[tdL.idThread].nr_cod].nr_jucatori] == tdL.idThread)
                {
                    printf("\n aici intra");
                    coduri[th[tdL.idThread].nr_cod].numar = -1;
                    coduri[th[tdL.idThread].nr_cod].nr_intpuse++;
                }
                coduri[th[tdL.idThread].nr_cod].nr_jucatori--;
                if (nr < tdL.idThread)
                {
                    coduri[th[tdL.idThread].nr_cod].numar--;
                }
                if (coduri[th[tdL.idThread].nr_cod].jucator == tdL.idThread)
                {
                    coduri[th[tdL.idThread].nr_cod].numar--;
                    urmatorul(tdL.idThread);
                }
                printf("\n pritmul if %d", coduri[th[tdL.idThread].nr_cod].nr_jucatori);
                strcpy(th[tdL.idThread].cod, " ");

                close(tdL.cl);
            }

            else if (th[tdL.idThread].joc == 1 && th[tdL.idThread].incepe_joc == 0)
            {
                for (int j = 0; j < 10000; j++)
                {
                    if (strcmp(th[tdL.idThread].cod, th[j].cod) == 0 && tdL.idThread != j)
                    {
                        bzero(mes, 1000);
                        strcat(mes, "proprietariesit");
                        scrie(th[j].descriptor, mes);
                        strcpy(th[j].cod, " ");
                        close(th[j].descriptor);
                    }
                }
                th[tdL.idThread].joc = 0;
                th[tdL.idThread].scor = 0;
                close(tdL.cl);
                strcpy(th[tdL.idThread].cod, " ");
                break;
            }
            else if (th[tdL.idThread].joc == 1 && th[tdL.idThread].incepe_joc == 1)
            {
                th[tdL.idThread].joc = 0;
                for (int jj = 0; jj <= th[tdL.idThread].nrjuc; jj++)
                {
                    if (coduri[th[tdL.idThread].nr_cod].jucatori[jj] == tdL.idThread)
                    {
                        for (int kk = jj; kk < th[tdL.idThread].nrjuc; kk++)
                        {
                            coduri[th[tdL.idThread].nr_cod].jucatori[kk] = coduri[th[tdL.idThread].nr_cod].jucatori[kk + 1];
                        }
                    }
                }
                for (int jj = 0; jj <= th[tdL.idThread].nrjuc; jj++)
                {
                    if (th[tdL.idThread].jucatori[jj] == tdL.idThread)
                    {
                        for (int kk = jj; kk < th[tdL.idThread].nrjuc; kk++)
                        {
                            th[tdL.idThread].jucatori[kk] = th[tdL.idThread].jucatori[kk + 1];
                        }
                    }
                }
                coduri[th[tdL.idThread].nr_cod].nr_jucatori--;
                for (int j = 0; j < 10000; j++)
                {
                    if (strcmp(th[tdL.idThread].cod, th[j].cod) == 0 && tdL.idThread != j)
                    {
                        for (int jj = 0; jj <= th[tdL.idThread].nrjuc; jj++)
                        {
                            if (th[j].jucatori[jj] == tdL.idThread)
                            {
                                for (int kk = jj; kk < th[tdL.idThread].nrjuc; kk++)
                                {
                                    th[j].jucatori[kk] = th[j].jucatori[kk + 1];
                                }
                            }
                        }
                        th[j].nrjuc--;
                    }
                }
                close(tdL.cl);
                if (coduri[th[tdL.idThread].nr_cod].jucator == tdL.idThread)
                {
                    coduri[th[tdL.idThread].nr_cod].numar--;
                    urmatorul(tdL.idThread);
                }
                strcpy(th[tdL.idThread].cod, " ");
            }
            strcpy(th[tdL.idThread].cod, " ");
            break;
        }
        if (strcmp(msgr, "alegeri") == 0)
        {
            citire(tdL.cl, msgr, lungime2);
            strcpy(th[tdL.idThread].alegere, msgr);
            citire(tdL.cl, msgr, lungime2);
            int intreb = msgr[0] - '0';
            th[tdL.idThread].nrint = intreb;
        }
        if (strcmp(msgr, "incepjoc") == 0)
        {
            for (int k = 0; k < 10000; k++)
            {
                if (strcmp(th[tdL.idThread].cod, th[k].cod) == 0)
                {
                    th[k].nrjuc = th[tdL.idThread].nrjuc;
                    th[k].nr_cod = th[tdL.idThread].nr_cod;
                    for (int u = 0; u <= th[tdL.idThread].nrjuc; u++)
                    {
                        th[k].jucatori[u] = th[tdL.idThread].jucatori[u];
                        coduri[th[k].nr_cod].jucatori[u] = coduri[th[tdL.idThread].nr_cod].jucatori[u];
                    }
                    strcpy(th[k].alegere, th[tdL.idThread].alegere);
                    initializare(k);
                }
            }
            printf("\n nr juc: %d", th[tdL.idThread].nrjuc);
            fflush(stdout);
            coduri[th[tdL.idThread].nr_cod].nr_intpuse = 0;
            coduri[th[tdL.idThread].nr_cod].nr_intre = th[tdL.idThread].nrint;
            for (int jj = 0; jj <= th[tdL.idThread].nrjuc; jj++)
            {
                coduri[th[tdL.idThread].nr_cod].jucatori[jj] = th[tdL.idThread].jucatori[jj];
            }
            coduri[th[tdL.idThread].nr_cod].nr_jucatori = th[tdL.idThread].nrjuc;
            coduri[th[tdL.idThread].nr_cod].numar = 0;
            coduri[th[tdL.idThread].nr_cod].jucator = tdL.idThread;
            bzero(mes, 100);
            strcat(mes, "daaaaa");
            incepe(tdL.idThread);
            joc(tdL.idThread, intr);
            th[tdL.idThread].in_timp_int = 1;
            scrie(tdL.cl, intr);
        }
        if (strcmp(msgr, "maiai") == 0)
        {
            if (coduri[th[tdL.idThread].nr_cod].jucator != tdL.idThread && th[tdL.idThread].incepe_joc == 1)
            {
                bzero(mes, 1000);
                fflush(stdout);
                strcat(mes, "asteapta");
                scrie(tdL.cl, mes);
            }
            else
            {
                if (th[tdL.idThread].in_timp_int == 0 && coduri[th[tdL.idThread].nr_cod].jucator == tdL.idThread)
                {
                    printf("\n id thread %d:", tdL.idThread);
                    fflush(stdout);
                    bzero(intr, 1000);
                    joc(tdL.idThread, intr);
                    int lung = strlen(intr);
                    while (lung == 0)
                    {
                        joc(tdL.idThread, intr);
                        lung = strlen(intr);
                    }
                    printf("\n Intrebare %s ", intr);
                    fflush(stdout);
                    bzero(mes, 1000);
                    strcat(mes, "Intrebare ");
                    strcat(mes, intr);
                    fflush(stdout);
                    scrie(tdL.cl, mes);
                    th[tdL.idThread].in_timp_int = 1;
                }
            }
        }
        if (strcmp(msgr, "raspuns") == 0)
        {
            citire(tdL.cl, msgr, lungime2);
            strcpy(th[tdL.idThread].raspuns, msgr);
            printf("\nrasp bun %s", th[tdL.idThread].raspbun);
            if (strcmp(th[tdL.idThread].raspuns, th[tdL.idThread].raspbun) == 0)
            {
                th[tdL.idThread].scor++;
            }
            printf("\nraspuns %s  ", msgr);
            th[tdL.idThread].timp = 0;
        }
        if (strcmp(msgr, "maistau") == 0)
        {
            if (th[tdL.idThread].incepe_joc == 0)
            {
                bzero(mes, 1000);
                strcat(mes, "maistai");
                scrie(tdL.cl, mes);
            }
            else
            {
                bzero(mes, 1000);
                strcat(mes, "incepem");
                scrie(tdL.cl, mes);
            }
        }
        if (strcmp(msgr, "maiamtimp") == 0)
        {
            if (th[tdL.idThread].timp == 0)
            {
                bzero(mes, 1000);
                strcat(mes, "nu");
                scrie(tdL.cl, mes);
                th[tdL.idThread].in_timp_int = 0;
                urmatorul(tdL.idThread);
            }
            else
            {
                th[tdL.idThread].end = time(NULL);
                int scurs = (th[tdL.idThread].end - th[tdL.idThread].begin);
                fflush(stdout);
                if (scurs > 20)
                {
                    bzero(mes, 1000);
                    strcat(mes, "nu");
                    scrie(tdL.cl, mes);
                    bzero(th[tdL.idThread].raspuns, 100);
                    strcpy(th[tdL.idThread].raspuns, "g");
                    urmatorul(tdL.idThread);
                    th[tdL.idThread].in_timp_int = 0;
                }
                else
                {
                    bzero(mes, 1000);
                    strcat(mes, "da");
                    scrie(tdL.cl, mes);
                }
            }
        }
        if (strcmp(msgr, "amapasat") == 0)
        {
            if (th[tdL.idThread].incepe_joc == 1)
            {
                bzero(mes, 1000);
                strcat(mes, "da");
                scrie(tdL.cl, mes);
            }
            else
            {
                bzero(mes, 1000);
                strcat(mes, "nu");
                scrie(tdL.cl, mes);
            }
        }
    }
}
int incepe(int proprietar)
{
    coduri[th[proprietar].nr_cod].numar = 0;
    coduri[th[proprietar].nr_cod].jucator = proprietar;
}
int intreb_joc(int codd, int client)
{
    if (coduri[codd].jucator != client)
    {
        return 0;
    }
    else
    {
        return 1;
    }
}
int joc(int client, char s[1000])
{
    fflush(stdout);
    bzero(s, 1000);
    if (rc)
    {
        perror("Eroare");
    }
    int ok = 0;
    char sql[100];
    char intr[1000];
    char bun[10];
    int ras1;
    int ok2 = 0;
    bzero(intr, 1000);
    srand(time(0));
    int selectie, int_pusa;
    while (ok == 0 && ok2 == 0)
    {
        selectie = rand() % 2;
        fflush(stdout);
        if (th[client].alegere[selectie] == 'd')
        {
            ok2 = 1;
        }
        int_pusa = rand() % 5;
        if (selectie == 0)
            if (th[client].nrint0[int_pusa] == 0)
            {
                ok = 1;
                th[client].nrint0[int_pusa] = 1;
                sprintf(sql, "SELECT * FROM tab WHERE int = '%d';", int_pusa);
                rc = sqlite3_exec(db, sql, callback, intr, NULL);
                ras1 = sqlite3_exec(db, sql, callbackrasbun, bun, NULL);
            }

        if (selectie == 1)
        {
            if (th[client].nrint1[int_pusa] == 0)
            {
                ok = 1;
                th[client].nrint1[int_pusa] = 1;
                sprintf(sql, "SELECT * FROM isto WHERE int = '%d';", int_pusa);
                rc = sqlite3_exec(db, sql, callback, intr, NULL);
                ras1 = sqlite3_exec(db, sql, callbackrasbun, bun, NULL);
            }
        }
        if (ras1 != SQLITE_OK)
        {
            perror("eroare");
        }
        if (rc != SQLITE_OK)
        {
            perror("eroare");
        }
        if (selectie == 2)
        {
            if (th[client].nrint2[int_pusa] == 0)
            {
                ok = 1;
                th[client].nrint2[int_pusa] = 1;
            }
        }
        if (selectie == 3)
        {
            if (th[client].nrint3[int_pusa] == 0)
            {
                ok = 1;
                th[client].nrint3[int_pusa] = 1;
            }
        }
        if (selectie == 4)
        {
            if (th[client].nrint4[int_pusa] == 0)
            {
                ok = 1;
                th[client].nrint4[int_pusa] = 1;
            }
        }
        fflush(stdout);
    }
    fflush(stdout);
    strcat(s, intr);
    th[client].timp = 20;
    strcpy(th[client].raspbun, bun);
    th[client].begin = time(NULL);
}
int initializare(int jucator)
{
    for (int i = 0; i <= 10; i++)
    {
        th[jucator].nrint0[i] = 0;
        th[jucator].nrint1[i] = 0;
        th[jucator].nrint2[i] = 0;
        th[jucator].nrint3[i] = 0;
        th[jucator].nrint4[i] = 0;
    }
    th[jucator].timp = 20;
    th[jucator].scor = 0;
    th[jucator].incepe_joc = 1;
    th[jucator].in_timp_int = 0;
}
int urmatorul(int client)
{
    coduri[th[client].nr_cod].numar++;
    coduri[th[client].nr_cod].jucator = coduri[th[client].nr_cod].jucatori[coduri[th[client].nr_cod].numar];
    if (coduri[th[client].nr_cod].numar > coduri[th[client].nr_cod].nr_jucatori)
    {
        printf("\ndaaa");
        fflush(stdout);
        coduri[th[client].nr_cod].numar = 0;
        coduri[th[client].nr_cod].jucator = coduri[th[client].nr_cod].jucatori[0];
        coduri[th[client].nr_cod].nr_intpuse++;
    }
    if (coduri[th[client].nr_cod].nr_jucatori < 0)
    {
        printf("\n Am terminat");
        return 1;
    }
    if (coduri[th[client].nr_cod].nr_intpuse == coduri[th[client].nr_cod].nr_intre)
    {
        for (int ii = 0; ii <= th[client].nrjuc; ii++)
        {
            th[th[client].jucatori[ii]].incepe_joc = 0;
        }
        clasament(client);
        coduri[th[client].nr_cod].jucator = -1;
    }
}
int clasament(int client)
{
    int ii, jj;
    int clas[10000];
    int nr = 0;
    int maxi = 0;
    for (ii = 0; ii <= th[client].nrjuc; ii++)
    {
        printf("\nscor : %d", th[th[client].jucatori[ii]].scor);
        if (th[th[client].jucatori[ii]].scor > maxi)
        {
            maxi = th[th[client].jucatori[ii]].scor;
        }
    }
    ii = 0;
    printf(" \n nr juc %d", th[client].nrjuc);
    char clasament[10000];
    bzero(clasament, 10000);
    while (maxi != -1)
    {
        ii = 0;
        for (int ii = 0; ii <= th[client].nrjuc; ii++)
        {
            if (th[th[client].jucatori[ii]].scor == maxi)
            {
                strcat(clasament, th[th[client].jucatori[ii]].nume);
                printf("\n nume %s", th[th[client].jucatori[ii]].nume);
                strcat(clasament, " ");
                char s[1];
                s[0] = maxi + '0';
                strcat(clasament, s);
                if (th[client].nrjuc == 0)
                {
                    clasament[strlen(clasament) - 5] = '\0';
                }
                if(th[client].nrjuc>0){
                strcat(clasament, "\n");
                }
            }
        }
        maxi--;
    }
    char s[100];
    bzero(s, 100);
    strcat(s, "clasament");
    for (int jj = 0; jj <= th[client].nrjuc; jj++)
    {
        scrie(th[th[client].jucatori[jj]].descriptor, s);
        scrie(th[th[client].jucatori[jj]].descriptor, clasament);
    }
    initializare2(client);
}
int initializare2(int client)
{
    int nr = th[client].nrjuc;
    int nr2 = th[client].nr_cod;
    for (int ii = 0; ii <= nr; ii++)
    {
        th[coduri[th[client].nr_cod].jucatori[ii]].incepe_joc = -1;
        th[coduri[th[client].nr_cod].jucatori[ii]].nrint = 1000000;
        th[coduri[th[client].nr_cod].jucatori[ii]].scor = 0;
        th[coduri[th[client].nr_cod].jucatori[ii]].joc = -1;
        bzero(th[coduri[th[client].nr_cod].jucatori[ii]].alegere, 10);
        th[coduri[th[client].nr_cod].jucatori[ii]].nr_cod = -1;
    }
    for (int ii = 0; ii <= nr; ii++)
    {
        coduri[nr2].jucatori[ii] = -1;
    }
}