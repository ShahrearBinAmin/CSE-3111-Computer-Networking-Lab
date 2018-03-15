// Server side C/C++ program to demonstrate Socket programming
#include <bits/stdc++.h>
#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <netdb.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <stdbool.h>
#include <regex>
#include <dirent.h>
#include <unistd.h>
#define GetCurrentDir getcwd
#define PORT 8090
using namespace std;
static std::smatch match_obj;

bool StartsWith(const char *a, const char *b)
{
    if(strncmp(a, b, strlen(b)) == 0) return 1;
    return 0;
}
bool bad_sqnc_check(char *a)
{
    if(StartsWith(a,"RCPT TO") || StartsWith(a,"MAIL FROM")|| StartsWith(a,"DATA")||StartsWith(a,"QUIT")|| StartsWith(a,"HELO"))
    {
        return 1;
    }
    return 0;
}
bool EmailValidator(char *email)
{
    std::regex email_regex(".*([[:w:]]+)@([[:w:]]+): [[:d:]]{1,4}");
    bool match=regex_match(email,email_regex);
    return match;
}
bool EmailValExact(char *email)
{
    //cout<<email<<endl;
    std::string rcpt_email(email);
    //cout<<rcpt_email<<endl;
    std::regex email_regex("([[:w:]]+)@([[:w:]]+)");
    std::smatch mo;
    bool match=regex_search(rcpt_email,mo,email_regex);
    match_obj=mo;
    return match;
}
void checkHostName(int hostname)
{
    if (hostname == -1)
    {
        perror("gethostname");
        exit(1);
    }
}

string workingDir()
{
    char cCurrentPath[FILENAME_MAX];

    if (!GetCurrentDir(cCurrentPath, sizeof(cCurrentPath)))
    {
        //return errno;
        cout<<"Error"<<endl;
        return NULL;
    }

    cCurrentPath[sizeof(cCurrentPath) - 1] = '\0'; /* not really required */
    //cout<<cCurrentPath<<endl;
    //printf ("The current working directory is %s", cCurrentPath);
    return cCurrentPath;
}

bool checkDir(char *filename)
{
    DIR *dir;
    char * S = new char[workingDir().length() + 1];
    std::strcpy(S,workingDir().c_str());
    struct dirent *ent;
    if ((dir = opendir (S)) != NULL)
    {
        /* print all the files and directories within directory */
        while ((ent = readdir (dir)) != NULL)
        {
            //printf ("%s\n", ent->d_name);
            if(strcmp(ent->d_name,filename)==0)
            {
                //cout<<"exist"<<endl;
                return true;
            }
        }
        closedir (dir);
    }
    else
    {
        /* could not open directory */
        perror ("");
        //return EXIT_FAILURE;
        return false;
    }
}
void writeFile(char *file_name,char *file_contents)
{
    ofstream myfile;
    myfile.open (file_name,ios::app);
    myfile <<file_contents;
    myfile.close();
}
int main(int argc, char const *argv[])
{
    int server_fd, new_socket, valread;
    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);
    char buffer[1024] = {0};


    ///printed
    printf("Server Opened at port number : %d\n\n",PORT);
    // Creating socket file descriptor
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0)
    {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    // Forcefully attaching socket to the port 8080
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT,
                   &opt, sizeof(opt)))
    {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }
    address.sin_family = AF_INET;
    //address.sin_addr.s_addr = INADDR_ANY;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons( PORT );

    // Forcefully attaching socket to the port 8080
    if (bind(server_fd, (struct sockaddr *)&address,
             sizeof(address))<0)
    {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }
    if (listen(server_fd, 10) < 0)
    {
        perror("listen");
        exit(EXIT_FAILURE);
    }

    //valread = read( new_socket , buffer, 1024);
    //printf("%s\n",buffer );

    //get host name
    int client_number=0;
    while(true)
    {
        cout<<"-----------------------------------------------"<<endl;

        if ((new_socket = accept(server_fd, (struct sockaddr *)&address,
                                 (socklen_t*)&addrlen))<0)
        {
            perror("accept");
            exit(EXIT_FAILURE);
        }
        else
        {
            cout<<"Connection established with client "<<++client_number<<endl;
            //cout<<
        }

        int hostname;
        char hostbuffer[256];
        char welc_status[]="220";
        char welc_status_cpy[]="250";
        char error_status[]="304";
        char hello[]="HELO";
        char from[]="MAIL FROM";
        char welcome[1024] = "  welcome from  ";


        hostname = gethostname(hostbuffer, sizeof(hostbuffer));
        checkHostName(hostname);

        strcat(welcome,hostbuffer);
        strcat(welc_status,welcome);
        strcat(welc_status,"\n");

        send(new_socket, welc_status, strlen(welc_status), 0 );
        printf("Server: %s",welc_status);

        memset(buffer, 0, sizeof buffer);
        //memset(welc_status,0,sizeof welc_status);
        valread = read( new_socket, buffer, 1024);
        cout<<"Client: "<<buffer<<endl;
        if(StartsWith(buffer,hello))
        {
            //printf("%s",welc_status_cpy);
            char to_be_sent[1024]="250 HELO,pleased to meet you *_*";
            send(new_socket,to_be_sent,sizeof(to_be_sent), 0 );
            //printf("Client: HELO received\n");
            cout<<"Server: "<<to_be_sent<<endl;
        }
        else if(bad_sqnc_check(buffer))
        {
            char error_mail[1024]="503 Bad sequence of commands";
            send(new_socket, error_mail, strlen(error_mail), 0 );
            //printf("Client: MAIL FROM not received\n");
            cout<<"Server: "<<error_mail<<endl;
            continue;
        }
        else
        {
            char error_mail_rcpt[1024]="500 Syntax error,command unrecognized";
            send(new_socket, error_mail_rcpt, strlen(error_mail_rcpt), 0 );
            cout<<"Server: "<<error_mail_rcpt<<endl;
        }

        memset(buffer, 0, sizeof buffer);
        valread = read( new_socket, buffer, 1024);
        // printf("Client:%s\n",buffer);

        cout<<"Client: "<<buffer<<endl;
        if(StartsWith(buffer,from))
        {
            //printf("%s",welc_status_cpy);
            if(EmailValidator(buffer))
            {
                char to_be_sent[1024]="250 ";
                strcat(to_be_sent," ");
                strcat(to_be_sent," ... Sender ok");
                send(new_socket,to_be_sent,strlen(to_be_sent), 0 );
                //printf("Client: MAIL FROM received\n");
                cout<<"Server: "<<to_be_sent<<endl;

                //recpt to
                memset(buffer, 0, sizeof buffer);
                valread = read( new_socket, buffer, 1024);
                cout<<"Client: "<<buffer<<endl;
                //cout<<"test : "<<buffer<<endl;
                if(StartsWith(buffer,"RCPT TO"))
                {
                    if(EmailValExact(buffer))
                    {

                        //cout<<match_obj[0]<<endl;
                        //cout<<match_obj[1]<<endl;
                        //cout<<match_obj[2]<<endl;

                        char *l=new char[match_obj[1].str().length()+1];
                        strcpy(l,match_obj[1].str().c_str());

                        if(checkDir(l))
                        {
                            char to_be_sent[1024]="250 ";
                            strcat(to_be_sent," ");
                            strcat(to_be_sent," ... Recipient ok");
                            send(new_socket,to_be_sent,strlen(to_be_sent),0);
                            cout<<"Server: "<<to_be_sent<<endl;

                            //cout<<"exits"<<endl;
                            memset(buffer, 0, sizeof buffer);
                            valread = read( new_socket, buffer, 1024);
                            cout<<"Client: "<<buffer<<endl;
                            if(StartsWith(buffer,"DATA"))
                            {
                                char to_be_sent[1024]="354 Enter mail, end with “.” on a line by itself";
                                send(new_socket,to_be_sent, sizeof(to_be_sent), 0 );
                                //printf("Client: DATA request received\n");
                                cout<<"Server: "<<to_be_sent<<endl;


                                memset(buffer, 0, sizeof buffer);
                                valread = read( new_socket, buffer, 10240);
                                cout<<"Client: "<<buffer<<endl;
                                strcat(buffer,"\n");
                                writeFile(l,buffer);
                                /*if(valread){
                                    writeFile(l,buffer);
                                }else{
                                    cout<<"Header problem"<<endl;
                                }*/
                                if(1)
                                {

                                    //cout<<"header received"<<endl;
                                    char to_be_sent[1024]="250 , header Ok";
                                    send(new_socket,to_be_sent,strlen(to_be_sent), 0 );
                                    //printf("Client: Header request received\n");
                                    cout<<"Server: "<<to_be_sent<<endl;
                                    char buffer[10240];
                                    /*valread = read( new_socket, buffer, 1024000);
                                    //cout<<buffer<<endl;
                                    cout<<"Client:      "<<buffer<<endl;
                                    while(strcmp(buffer,".")!=0){
                                        memset(buffer,0,sizeof buffer);
                                        valread = read( new_socket, buffer, 1024000);
                                        cout<<"cline ------->"<<buffer<<endl;
                                    }
                                    cout<<buffer<<endl;*/
                                    while(1)
                                    {
                                        //cout<<"loop"<<endl;

                                        memset(buffer,0,sizeof buffer);
                                        valread = read( new_socket, buffer, 102400);
                                        //cout<<"Client ->"<<buffer<<endl;
                                        if(strcmp(buffer,".")==0)
                                        {
                                            cout<<"ClientMsg-->."<<endl;
                                            break;
                                        }
                                        else
                                        {
                                            cout<<"ClientMsg-->"<<buffer<<endl;
                                            strcat(buffer,"\n");
                                            writeFile(l,buffer);
                                        }

                                        send(new_socket,"240",3, 0 );


                                    }
                                    char temp[]="\n";
                                    writeFile(l,temp);

                                    char to_be_sent2[1024]="250 , message Ok";
                                    send(new_socket,to_be_sent2,strlen(to_be_sent2), 0 );

                                    memset(buffer, 0, sizeof buffer);
                                    valread = read( new_socket, buffer, 1024);
                                    cout<<"Client: "<<buffer<<endl;
                                    if(StartsWith(buffer,"QUIT"))
                                    {
                                        char to_be_sent[1024]="221 QUIT ... Ok";
                                        send(new_socket,to_be_sent,strlen(to_be_sent), 0 );
                                        //printf("Client: QUIT request received\n");
                                        cout<<"Server: "<<to_be_sent<<endl;

                                    }
                                    else if(bad_sqnc_check(buffer))
                                    {
                                        char error_mail[1024]="503 Bad sequence of commands";
                                        send(new_socket, error_mail, strlen(error_mail), 0 );
                                        //printf("Client: MAIL FROM not received\n");
                                        cout<<"Server: "<<error_mail<<endl;
                                        continue;
                                    }
                                    else
                                    {
                                        //send(new_socket, error_status, strlen(error_status), 0 );
                                        //printf("Client: QUIT request not received\n");

                                        char error_mail_quit[1024]="421 QUIT request not received";
                                        send(new_socket, error_mail_quit, strlen(error_mail_quit), 0 );
                                        cout<<"Server: "<<error_mail_quit<<endl;
                                        continue;
                                    }
                                }
                                else
                                {
                                    //send(new_socket, error_status, strlen(error_status), 0 );
                                    //printf("Server: Header request not received\n");

                                    char error_mail_header[1024]="556 Header request not received";
                                    send(new_socket, error_mail_header, strlen(error_mail_header), 0 );
                                    cout<<"Server: "<<error_mail_header<<endl;
                                    continue;
                                }




                            }
                            else if(bad_sqnc_check(buffer))
                            {
                                char error_mail[1024]="503 Bad sequence of commands";
                                send(new_socket, error_mail, strlen(error_mail), 0 );
                                //printf("Client: MAIL FROM not received\n");
                                cout<<"Server: "<<error_mail<<endl;
                                continue;
                            }
                            else
                            {
                                // send(new_socket, error_status, strlen(error_status), 0 );
                                //printf("Client: DATA request not received\n");
                                //cout<<"Server :"<<error_status<<" DATA request not received"<<endl;

                                char error_mail_data[1024]="500 Syntax error, command unrecognized";
                                send(new_socket, error_mail_data, strlen(error_mail_data), 0 );
                                cout<<"Server: "<<error_mail_data<<endl;
                                continue;
                            }

                        }
                        else
                        {


                            //send(new_socket,error_status,strlen(error_status),0);
                            //cout<<"Server: "<<error_status<<" RCPT TO file name not exits"<<endl;
                            char error_mail_noexist[1024]="550 Requested action not taken: mailbox unavailable";
                            send(new_socket, error_mail_noexist, strlen(error_mail_noexist), 0 );
                            cout<<"Server: "<<error_mail_noexist<<endl;
                            continue;

                        }

                    }
                    else
                    {
                        //send(new_socket,error_status,strlen(error_status),0);
                        //printf("Client:RCPT To Invalid email\n");
                        // cout<<"Server: "<<error_status<<" RCPT To Invalid email"<<endl;

                        char error_mail_rcpt_inva[1024]="501 Syntax error in parameters or arguments";
                        send(new_socket, error_mail_rcpt_inva, strlen(error_mail_rcpt_inva), 0 );
                        cout<<"Server: "<<error_mail_rcpt_inva<<endl;
                        continue;
                    }

                }
                else if(bad_sqnc_check(buffer))
                {
                    char error_mail[1024]="503 Bad sequence of commands";
                    send(new_socket, error_mail, strlen(error_mail), 0 );
                    //printf("Client: MAIL FROM not received\n");
                    cout<<"Server: "<<error_mail<<endl;
                    continue;
                }
                else
                {
                    //send(new_socket,error_status,strlen(error_status),0);
                    //printf("Client: RCPT To error\n");
                    //cout<<"Server: "<<error_status<<endl;

                    char error_mail_rcpt[1024]="555 MAIL FROM/RCPT TO parameters not recognized or not implemented";
                    send(new_socket, error_mail_rcpt, strlen(error_mail_rcpt), 0 );
                    cout<<"Server: "<<error_mail_rcpt<<endl;
                    continue;


                }
            }
            else
            {
                //send(new_socket,error_status,strlen(error_status),0);
                //printf("Client:Mail FROM Invalid email format\n");
                //cout<<"Server: "<<error_status<<endl;

                char error_mail_inva[1024]="450 Requested mail action not taken: mailbox unavailable";
                send(new_socket, error_mail_inva, strlen(error_mail_inva), 0 );
                cout<<"Server: "<<error_mail_inva<<endl;
                continue;
            }

        }
        else if(bad_sqnc_check(buffer))
        {
            char error_mail[1024]="503 Bad sequence of commands";
            send(new_socket, error_mail, strlen(error_mail), 0 );
            //printf("Client: MAIL FROM not received\n");
            cout<<"Server: "<<error_mail<<endl;
            continue;
        }
        else
        {
            char error_mail[1024]="555 MAIL FROM/RCPT TO parameters not recognized or not implemented";
            send(new_socket, error_mail, strlen(error_mail), 0 );
            //printf("Client: MAIL FROM not received\n");
            cout<<"Server: "<<error_mail<<endl;
            continue;
        }
    }

    close(new_socket);
    return 0;
}
