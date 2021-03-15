#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <errno.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <netdb.h>
#include <time.h>
#include <string.h>
#include <arpa/inet.h>
//#include <chrono>
#include <gtk/gtk.h>
extern int errno;
GtkBuilder *builder;
GtkWidget *window;
GtkWidget *text;
GtkWidget *buton;
GtkWidget *iesire;
GtkWidget *windo;
GtkWidget *jocnou;
GtkWidget *jocexisten, *existent, *iesin;
GtkWidget *w_cod;
GtkWidget *alegeri;
GtkWidget *trimitecod;
GtkWidget *incepejo;
GtkWidget *intrebari;
GtkWidget *labeli, *iesss, *iessss;
GtkWidget *label1;
GtkWidget *label3;
GtkWidget *label2;
GtkWidget *label4;
GtkWidget *label5;
GtkWidget *label6;
GtkWidget *label7;
GtkWidget *label8;
GtkWidget *label9;
GtkWidget *label10;
GtkWidget *label11;
GtkWidget *label12;
GtkWidget *label13;
GtkWidget *camera, *clasament;
GtkWidget *raspuns;
GtkWidget *codgresit;
GtkWidget *astept;
GtkWidget *asteptati;
GtkWidget *instructiuni;
GtkWidget *incepejoc;
GtkWidget *fixed1, *butonr, *butonr1, *butonr2;
GtkWidget *codexistent;
GtkWidget *trimitealege, *iesirecreator2, *iess;
GtkWidget *generala, *instru;
GtkWidget *istorie, *codd, *iesirejoc, *iesinst, *trimiterasp, *trimitemesaje;
GtkWidget *nrint, *iesjoc;
GtkWidget *fixed2, *fixed3, *fixed4, *fixed5, *fixed6, *fixed7, *fixed8, *fixed9, *fixed10, *fixed11, *fixed12, *fixed13, *fixed14;
int port;
int sd;
gint x, y;
int is = 1, ge = 1;
char s[1000], mesaj[1000], intrebare[1000];
int lungime;
int lungime2;
int start, finish;
time_t begin, end;
char ale[10];
char nr_int[1];
int venit = 0;
int f1 = 0, f2 = 0, f3 = 0, f4 = 0, f5 = 0; //astept, intrebari, camera
void schimba(GtkWidget *a, GtkWidget *b)
{

    gtk_window_get_position(GTK_WINDOW(a), &x, &y);
    gtk_widget_hide(GTK_WIDGET(a));
    gtk_widget_show(b);
    printf("\n x %d, y ", x);
    printf("%d", y);
    gtk_window_move(GTK_WINDOW(b), x, y);
    gtk_window_get_position(GTK_WINDOW(a), &x, &y);
    printf("\n x %d, y ", x);
    printf("%d", y);
    fflush(stdout);
}
void scrie(char mes[])
{
    lungime2 = strlen(mes) + 1;
    if (write(sd, &lungime2, sizeof(int)) <= 0)
    {
        perror("[client]Eroare la write() spre server.\n");
    }
    if (write(sd, mes, lungime2) <= 0)
    {
        perror("[client]Eroare la write() spre server.\n");
    }
    //printf("\nam scris: %s", mes);
    // printf("\n desc: %d ", sd);
    //printf("\n 1: %s ", mes);
}
void citeste()
{
    bzero(mesaj, 1000);
    fflush(stdout);
    if (read(sd, &lungime, sizeof(int)) <= 0)
    {
        perror("[client]Eroare la read() de la server.\n");
    }
    fflush(stdout);
    if (read(sd, mesaj, lungime + 1) <= 0)
    {
        perror("eroare la read de la server");
    }
    // printf("\n am citit: %s", mesaj);
    fflush(stdout);
}
void event()
{
    if (f1 == 1)
    {
        char f[100];
        bzero(f, 100);
        strcat(f, "maistau");
        scrie(f);
        citeste();
        if (strcmp(mesaj, "incepem") == 0)
        {
            f1 = 0, f3 = 1;
            schimba(astept, camera);
        }
        else{
            if(strcmp(mesaj, "proprietariesit")==0){
                f1=0;
                 gtk_label_set_text(GTK_LABEL(label1), " ");
                 gtk_label_set_text(GTK_LABEL(label10), "Proprietarul jocului a iesit, ne cerem scuze.");
            }
        }
    }
}
void event1()
{
    if (f3 == 1)
    {
        char f[100];
        bzero(f, 100);
        strcat(f, "maiai");
        scrie(f);
        citeste();
        if (strstr(mesaj, "Intrebare") != NULL)
        {
            //citeste();
            f3 = 0;
            f2 = 1;
            gtk_label_set_text(GTK_LABEL(labeli), mesaj);
            //printf("intrebare %s ", mesaj);
            fflush(stdout);
            schimba(camera, intrebari);
        }
        else if (strcmp(mesaj, "clasament") == 0)
        {
            f2=0;
            f3 = 0;
            citeste();
            gtk_label_set_text(GTK_LABEL(label8), mesaj);
            schimba(camera, clasament);
        }
    }
}
void event2()
{
    if (f2 == 1)
    {
        char f[100];
        bzero(f, 100);
        strcat(f, "maiamtimp");
        scrie(f);
        citeste();
        if (strcmp(mesaj, "nu") == 0)
        {
            f2 = 0;
            f3 = 1;
            schimba(intrebari, camera);
        }
    }
}
int main(int argc, char *argv[])
{
    struct sockaddr_in server;
    int nr = 0;
    for (int i = 0; i < 9; i++)
    {
        ale[i] = 'n';
    }
    if (argc != 3)
    {
        printf("Sintaxa: %s <adresa_server> <port>\n", argv[0]);
        return -1;
    }

    port = atoi(argv[2]);
    if ((sd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
    {
        perror("Eroare la socket().\n");
        return errno;
    }
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = inet_addr(argv[1]);
    server.sin_port = htons(port);

    if (connect(sd, (struct sockaddr *)&server, sizeof(struct sockaddr)) == -1)
    {
        perror("[client]Eroare la connect().\n");
        return errno;
    }
    char msgr[10000];
    char msg[1000];
    gtk_init(&argc, &argv);
    builder = gtk_builder_new();
    gtk_builder_add_from_file(builder, "part1.glade", NULL);
    window = GTK_WIDGET(gtk_builder_get_object(builder, "mmain"));
    alegeri = GTK_WIDGET(gtk_builder_get_object(builder, "alegeri"));
    windo = GTK_WIDGET(gtk_builder_get_object(builder, "joc"));
    fixed1 = GTK_WIDGET(gtk_builder_get_object(builder, "fixed1"));
    iesjoc = GTK_WIDGET(gtk_builder_get_object(builder, "iesjoc"));
    iesinst = GTK_WIDGET(gtk_builder_get_object(builder, "iesinst"));
    iesirecreator2 = GTK_WIDGET(gtk_builder_get_object(builder, "iesirecreator2"));
    iess = GTK_WIDGET(gtk_builder_get_object(builder, "iessss"));
    iessss = GTK_WIDGET(gtk_builder_get_object(builder, "iess"));
    fixed2 = GTK_WIDGET(gtk_builder_get_object(builder, "fixed2"));
    fixed14 = GTK_WIDGET(gtk_builder_get_object(builder, "fixed14"));
    clasament = GTK_WIDGET(gtk_builder_get_object(builder, "clasament"));
    codd = GTK_WIDGET(gtk_builder_get_object(builder, "codd"));
    instru = GTK_WIDGET(gtk_builder_get_object(builder, "instru"));
    fixed3 = GTK_WIDGET(gtk_builder_get_object(builder, "fixed3"));
    fixed4 = GTK_WIDGET(gtk_builder_get_object(builder, "fixed4"));
    fixed5 = GTK_WIDGET(gtk_builder_get_object(builder, "fixed5"));
    fixed6 = GTK_WIDGET(gtk_builder_get_object(builder, "fixed6"));
    fixed7 = GTK_WIDGET(gtk_builder_get_object(builder, "fixed7"));
    fixed8 = GTK_WIDGET(gtk_builder_get_object(builder, "fixed8"));
    fixed9 = GTK_WIDGET(gtk_builder_get_object(builder, "fixed9"));
    iesirejoc = GTK_WIDGET(gtk_builder_get_object(builder, "iesirejoc"));
    nrint = GTK_WIDGET(gtk_builder_get_object(builder, "nrint"));
    fixed10 = GTK_WIDGET(gtk_builder_get_object(builder, "fixed10"));
    fixed11 = GTK_WIDGET(gtk_builder_get_object(builder, "fixed11"));
    fixed12 = GTK_WIDGET(gtk_builder_get_object(builder, "fixed12"));
    fixed13 = GTK_WIDGET(gtk_builder_get_object(builder, "fixed13"));
    butonr = GTK_WIDGET(gtk_builder_get_object(builder, "butonr"));
    butonr1 = GTK_WIDGET(gtk_builder_get_object(builder, "butonr1"));
    butonr2 = GTK_WIDGET(gtk_builder_get_object(builder, "butonr2"));
    trimiterasp = GTK_WIDGET(gtk_builder_get_object(builder, "trimiterasp"));
    codexistent = GTK_WIDGET(gtk_builder_get_object(builder, "codexistent"));
    astept = GTK_WIDGET(gtk_builder_get_object(builder, "astept"));
    instructiuni = GTK_WIDGET(gtk_builder_get_object(builder, "instructiuni"));
    incepejo = GTK_WIDGET(gtk_builder_get_object(builder, "inceputjoc"));
    intrebari = GTK_WIDGET(gtk_builder_get_object(builder, "intrebari"));
    labeli = GTK_WIDGET(gtk_builder_get_object(builder, "intrebare"));
    label1 = GTK_WIDGET(gtk_builder_get_object(builder, "inc"));
    label3 = GTK_WIDGET(gtk_builder_get_object(builder, "label3"));
    generala = GTK_WIDGET(gtk_builder_get_object(builder, "generala"));
    istorie = GTK_WIDGET(gtk_builder_get_object(builder, "istorie"));
    iesin = GTK_WIDGET(gtk_builder_get_object(builder, "iesin"));
    label2 = GTK_WIDGET(gtk_builder_get_object(builder, "label2"));
    label4 = GTK_WIDGET(gtk_builder_get_object(builder, "label4"));
    label5 = GTK_WIDGET(gtk_builder_get_object(builder, "label5"));
    label6 = GTK_WIDGET(gtk_builder_get_object(builder, "label6"));
    label7 = GTK_WIDGET(gtk_builder_get_object(builder, "label7"));
    label8 = GTK_WIDGET(gtk_builder_get_object(builder, "label8"));
    label9 = GTK_WIDGET(gtk_builder_get_object(builder, "label9"));
    trimitealege = GTK_WIDGET(gtk_builder_get_object(builder, "trimitealege"));
    label10 = GTK_WIDGET(gtk_builder_get_object(builder, "label10"));
    label11 = GTK_WIDGET(gtk_builder_get_object(builder, "label11"));
    label12 = GTK_WIDGET(gtk_builder_get_object(builder, "label1"));
    label13 = GTK_WIDGET(gtk_builder_get_object(builder, "label13"));
    // label14 = GTK_WIDGET(gtk_builder_get_object(builder, "label14"));
    camera = GTK_WIDGET(gtk_builder_get_object(builder, "camera"));
    w_cod = GTK_WIDGET(gtk_builder_get_object(builder, "cod"));
    raspuns = GTK_WIDGET(gtk_builder_get_object(builder, "raspuns"));
    codgresit = GTK_WIDGET(gtk_builder_get_object(builder, "codgresit"));
    gtk_builder_connect_signals(builder, NULL);
    gtk_widget_hide(GTK_WIDGET(windo));
    gtk_widget_hide(GTK_WIDGET(clasament));
    gtk_widget_hide(GTK_WIDGET(w_cod));
    gtk_widget_hide(GTK_WIDGET(incepejo));
    gtk_widget_hide(GTK_WIDGET(intrebari));
    gtk_widget_hide(GTK_WIDGET(alegeri));
    gtk_widget_hide(GTK_WIDGET(astept));
    gtk_widget_hide(GTK_WIDGET(camera));
    gtk_widget_hide(GTK_WIDGET(instructiuni));
    buton = GTK_WIDGET(gtk_builder_get_object(builder, "buton1"));
    existent = GTK_WIDGET(gtk_builder_get_object(builder, "existent"));
    incepejoc = GTK_WIDGET(gtk_builder_get_object(builder, "incepejoc"));
    trimitecod = GTK_WIDGET(gtk_builder_get_object(builder, "trimitecod"));
    text = GTK_WIDGET(gtk_builder_get_object(builder, "text"));
    iesire = GTK_WIDGET(gtk_builder_get_object(builder, "iesire"));
    jocnou = GTK_WIDGET(gtk_builder_get_object(builder, "jocnou"));
    jocexisten = GTK_WIDGET(gtk_builder_get_object(builder, "jocexisten"));
    iesss = GTK_WIDGET(gtk_builder_get_object(builder, "iesss"));
    iessss = GTK_WIDGET(gtk_builder_get_object(builder, "iessss"));
    gtk_widget_hide(GTK_WIDGET(jocexisten));
    gtk_widget_hide(GTK_WIDGET(codgresit));
    g_object_unref(builder);
    gtk_label_set_text(GTK_LABEL(label1), "Pentru a putea intra in joc trebuie sa te autentifici.");
    gtk_label_set_text(GTK_LABEL(label3), "Alegeti din ce domeniu vor fi puse intrebarile");
    gtk_label_set_text(GTK_LABEL(label2), "Spuneti codul jocului");
    gtk_label_set_text(GTK_LABEL(label4), "Dati click cand vreti sa incepeti jocul");
    gtk_label_set_text(GTK_LABEL(label5), "Ne pare rau, ati introdus un cod gresit.");
    gtk_label_set_text(GTK_LABEL(label6), "Scrieti codul jocului.");
    gtk_label_set_text(GTK_LABEL(label7), "Pentru a putea continua trebuie sa intri intr-un joc sau sa creezi unul.");
    gtk_label_set_text(GTK_LABEL(label8), "");
     gtk_label_set_text(GTK_LABEL(label10), " ");
    gtk_label_set_text(GTK_LABEL(label9), "Aveti un timp de 20 se secunde pentru a raspunde la intrebare.");
    // gtk_label_set_text(GTK_LABEL(label10), "Asteptati sa va vina randul.");
    gtk_label_set_text(GTK_LABEL(label11), "Numarul de intrebari");
    gtk_label_set_text(GTK_LABEL(label12), "Asteptati sa inceapa jocul.");
    gtk_label_set_text(GTK_LABEL(label13), "Trebuie sa va vina randul.");
    g_signal_connect(astept, "event", G_CALLBACK(event), NULL);
    g_signal_connect(camera, "event", G_CALLBACK(event1), NULL);
    g_signal_connect(intrebari, "event", G_CALLBACK(event2), NULL);
    //   g_signal_connect(intrebari, "event", G_CALLBACK(event3), NULL);
    //GtkWindowGroup *gtk_window_group_new( )
    gtk_widget_show(window);
    gtk_main();
}
void on_text_changed(GtkEntry *e)
{
    bzero(s, 1000);
    strcat(s, gtk_entry_get_text(e));
}

void on_codd_changed(GtkEntry *e)
{
    bzero(s, 1000);
    strcat(s, gtk_entry_get_text(e));
}
void on_raspuns_changed(GtkEntry *e)
{
    bzero(s, 1000);
    strcat(s, gtk_entry_get_text(e));
}
void on_existent_changed(GtkEntry *e)
{
    bzero(s, 1000);
    strcat(s, gtk_entry_get_text(e));
}
void on_nrint_changed(GtkEntry *e)
{
    bzero(nr_int, 1);
    strcat(nr_int, gtk_entry_get_text(e));
}
void on_butonr_clicked()
{
    char f[100];
    bzero(f, 100);
    strcat(f, "maistau");
    scrie(f);
}
void on_buton1_clicked()
{
    char nume[100];
    bzero(nume, 100);
    strcpy(nume, "nume");
    scrie(nume);
    scrie(s);
    schimba(window, windo);
}
int on_iesire_clicked()
{
    bzero(s, 100);
    strcat(s, "exit");
    scrie(s);
    close(sd);
    gtk_widget_destroy(GTK_WIDGET(window));
    return EXIT_SUCCESS;
}
void on_jocexistent_clicked()
{
    bzero(s, 1000);
    strcpy(s, "jocexistent");
    scrie(s);
    schimba(windo, jocexisten);
}
void on_ies_clicked()
{
    bzero(s, 1000);
    strcat(s, "iesire");
    scrie(s);
    close(sd);
    gtk_widget_destroy(GTK_WIDGET(astept));
}
void on_iess_clicked()
{
    bzero(s, 1000);
    strcat(s, "iesire");
    scrie(s);
    close(sd);
    gtk_widget_destroy(GTK_WIDGET(astept));
}
void on_iesss_clicked()
{
    bzero(s, 1000);
    strcat(s, "iesire");
    scrie(s);
    close(sd);
    gtk_widget_destroy(GTK_WIDGET(jocexisten));
}
void on_iessss_clicked()
{
    bzero(s, 1000);
    strcat(s, "iesire");
    scrie(s);
    close(sd);
    gtk_widget_destroy(GTK_WIDGET(clasament));
}
void on_jocnou_clicked()
{
    bzero(s, 1000);
    strcat(s, "jocnou");
    fflush(stdout);
    scrie(s);
    schimba(windo, w_cod);
}
void on_codexistent_clicked()
{
    scrie(s);
    fflush(stdout);
    bzero(mesaj, 1000);
    citeste();
    fflush(stdout);
    if (strcmp(mesaj, "gresit") == 0)
    {
        schimba(jocexisten, codgresit);
    }
    else
    {
        fflush(stdout);
        if (strcmp(mesaj, "jocinceput") == 0)
        {
            int a;
        }
        else
        {
            f1 = 1;
            schimba(jocexisten, astept);
        }
    }
}
void on_trimitecod_clicked()
{
    lungime2 = strlen(s);
    scrie(s);
    schimba(w_cod, alegeri);
}
void on_trimiterasp_clicked()
{
    char ra[100];
    bzero(ra, 100);
    strcat(ra, "raspuns");
    scrie(ra);
    scrie(s);
    gtk_editable_delete_text(GTK_EDITABLE(raspuns), 0, -1);
}
void on_incepejoc_clicked()
{
    fflush(stdout);
    bzero(s, 1000);
    strcat(s, "incepjoc");
    scrie(s);
    fflush(stdout);
    bzero(intrebare, 1000);
    citeste(mesaj);
    printf("%s", mesaj);
    bzero(intrebare, 1000);
    strcat(intrebare, mesaj);
    fflush(stdout);
    f2 = 1;
    gtk_label_set_text(GTK_LABEL(labeli), mesaj);
    schimba(incepejo, intrebari);
}
int on_iesirejoc_clicked()
{
    bzero(s, 1000);
    strcat(s, "exit");
    scrie(s);
    close(sd);
    gtk_widget_destroy(GTK_WIDGET(codgresit));
    return EXIT_SUCCESS;
}
void on_iesirecreator_clicked()
{
    bzero(s, 1000);
    strcat(s, "iesire");
    scrie(s);
    close(sd);
    gtk_widget_destroy(GTK_WIDGET(alegeri));
}
void on_iesirecreator2_clicked()
{
    bzero(s, 1000);
    strcat(s, "iesire");
    scrie(s);
    close(sd);
    gtk_widget_destroy(GTK_WIDGET(incepejo));
}
void on_iesiredinjoc_clicked()
{
    bzero(s, 1000);
    strcat(s, "iesire");
    scrie(s);
    close(sd);
    gtk_widget_destroy(GTK_WIDGET(windo));
}
void on_inceput_clicked()
{
    schimba(clasament, windo);
}
void on_iesin_clicked()
{
    bzero(s, 1000);
    strcat(s, "iesire");
    scrie(s);
    close(sd);
    gtk_widget_destroy(GTK_WIDGET(intrebari));
}
void on_trimiterasp_unrealize()
{
    int a;
}
void on_instru_clicked()
{
    schimba(window, instructiuni);
}
void on_iesinst_clicked()
{
    schimba(instructiuni, window);
}
void on_generala_clicked()
{
    if (ge % 2 == 1)
    {
        ale[0] = 'd';
        ge++;
    }
    else
    {
        ale[0] = 'n';
        ge++;
    }
}
void on_istorie_clicked()
{
    if (is % 2 == 1)
    {
        ale[1] = 'd';
        is++;
    }
    else
    {
        ale[1] = 'n';
        is++;
    }
}
void on_trimitealege_clicked()
{
    char da[100];
    bzero(da, 100);
    strcat(da, "alegeri");
    scrie(da);
    scrie(ale);
    scrie(nr_int);
    f5 = 1;
    schimba(alegeri, incepejo);
}
void on_iesjoc_clicked()
{
    bzero(s, 1000);
    strcat(s, "iesire");
    scrie(s);
    close(sd);
    gtk_widget_destroy(GTK_WIDGET(camera));
}