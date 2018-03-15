#include <stdio.h>
#include <ctime>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <netdb.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <unistd.h>
#define PORT 8090
using namespace std;

const char* MONTHS[] =
{
    "January", "February", "March", "April", "May", "June",
    "July", "August", "September", "October", "November", "December"
};


char* date()
{
    static char da[10000]="";
    stringstream strs;
    stringstream strs2;
    stringstream strs3;
    time_t     rawtime;
    struct tm* timeinfo;

    time( &rawtime );
    timeinfo = localtime( &rawtime );

    /* cout << "Today's date is "
          << timeinfo->tm_mday << " "
          << MONTHS[ timeinfo->tm_mon ] << " "
          << (timeinfo->tm_year + 1900) << ".\n";
     //strcat(da,timeinfo->tm_mday);
     cout<<timeinfo->tm_mday<<endl;*/

    strs <<  timeinfo->tm_mday;
    strs3 << (timeinfo->tm_year + 1900);
    string tarikh = strs.str();
    string bochor = strs3.str();
    char* char_type = (char*) tarikh.c_str();
    char* char_type2 = (char*) bochor.c_str();
    strcat(da,char_type);
    strcat(da," ");
    strcat(da,MONTHS[ timeinfo->tm_mon ]);
    strcat(da," ");
    strcat(da,char_type2);
    strcat(da," ");
    //cout<<da<<endl;
    return da;
}
int get_file_size(std::string filename) // path to file
{
    FILE *p_file = NULL;
    p_file = fopen(filename.c_str(),"rb");
    fseek(p_file,0,SEEK_END);
    int size = ftell(p_file);
    fclose(p_file);
    return size;
}

void file_to_send(int sock,char const *ar)
{
    int length;
    char l[1024000]="";
    string line = "";
    bool isEmpty(true);
    char filelocation[1024]= "";
    strcat(filelocation,ar);
    // cout<<"File Location = "<<filelocation<<endl;
    string filename(filelocation);
    length = get_file_size(filename);
    ifstream myfile (filelocation);

    if (myfile.is_open())
    {
        //cout<<"length "<<length<<endl;
        if(length==0)
        {
            send(sock, ".", strlen("."), 0 );
            cout << "Client : "<<"." << '\n';

        }
        else
        {
            while ( getline (myfile,line) )
            {
                char *s = new char[line.length()+1];
                strcpy(s,line.c_str());
                //strcat(l,s);

                send(sock, s, strlen(s), 0 );
                cout << "Client : "<<s << '\n';
                if(strcmp(s,".")==0)
                {
                    break;
                }
                char buffer[1024];
                int valread = read( sock, buffer, 1024);
                //cout<<"--------------"<<buffer<<endl;

                //usleep(1000);

            }
        }


        myfile.close();
    }

    else cout << "Unable to open file";
    //cout<<l<<endl;
    //send(sock, l, strlen(l), 0 );
    //cout<<"Client : "<<l<<endl;


}

int main(int argc, char const *argv[])
{
    char add[1024];
    char from_add[1024];
    char sent_add[1024];
    char sent_header_add[1024];
    char hostname[1024];
    char *sent_user="shahin";
    char *sent_host="shuvo";

    hostname[1023] = '\0';

    struct sockaddr_in address;
    int sock = 0, valread;
    struct sockaddr_in serv_addr;
    char hello[1024] = "";
    char buffer[1024] = {0};
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        printf("\n Socket creation error \n");
        return -1;
    }

    memset(&serv_addr, '0', sizeof(serv_addr));

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);

    // Convert IPv4 and IPv6 addresses from text to binary form
    if(inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr)<=0)
    {
        printf("\nInvalid address/ Address not supported \n");
        return -1;
    }

    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    {
        printf("\nConnection Failed \n");
        return -1;
    }
    //send(sock , hello , strlen(hello) , 0 );
    //printf("Hello message sent\n");
    valread = read( sock, buffer, 1024);
    //printf("%s\n",buffer );
    if(buffer[0]!='2')
    {
        printf("Connection is not created successfully\n");
        close(sock);

    }
    else
    {
        printf("Connection established\n");
        cout<<"Server : "<<buffer;
        cin>>hello;
        send(sock, hello, strlen(hello), 0 );
        cout<<"Client : "<<hello<<endl;
        memset(buffer,0,sizeof(buffer));
        valread = read( sock, buffer, 1024);
        //printf("%s\n",buffer );
        if(buffer[0]!='2')
        {
            cout<<"Server : "<<buffer<<endl;
            close(sock);
            //printf("Hello Request isn't Received\n");
            //cout<<"Server : "<<buffer<<endl;

        }
        else
        {
            cout<<"Server : "<<buffer<<endl;
            //cout<<argv[0]<<" "<<argv[1]<<" "<<argv[2]<<endl;
            //cout<<" ";
            /*string mail_from;
            getline(std::cin,mail_from);
            cout<<mail_from<<endl;
            char *s = new char[mail_from.length()+1];*/
            /*char mail_from[100];
            scanf("%[^\n]s",mail_from);
            cout<<mail_from<<endl;*/
            char mail[100];
            fgets(mail,100,stdin);
            scanf("%[^\n]s",mail);
            char * user_name = getenv("USER");
            if (!user_name)
                user_name = getenv("USERNAME");
            //printf("Username: %s\n", user_name);



            //cin>>add;
            gethostname(hostname, 1023);

            strcat(add,mail);
            strcat(add," ");
            strcat(add,user_name);
            strcat(add,"@");
            strcat(add,hostname);
            // strcpy(from_add,add);
            strcat(add,": 8090");
            strcat(from_add,user_name);
            strcat(from_add,"@");
            strcat(from_add,hostname);
            send(sock, add, strlen(add), 0 );
            cout<<"Client : "<<add<<endl;
            memset(buffer,0,sizeof(buffer));
            valread = read( sock, buffer, 1024);
            //printf("%s\n",buffer );
            if(buffer[0]!='2')
            {
                cout<<"Server : "<<buffer<<endl;
                close(sock);
                //cout<<"Server : "<<buffer<<endl;
            }
            else
            {
                cout<<"Server : "<<buffer<<endl;
                //printf("From Request is Received Successfully\n");
                /*strcat(sent_add,"RCPT TO : ");
                strcat(sent_add,sent_user);
                strcat(sent_add,"@");
                strcat(sent_add,sent_host);
                strcat(sent_header_add,sent_user);
                strcat(sent_header_add,"@");
                strcat(sent_header_add,sent_host);*/
                            //char [100];
                fgets(sent_add,100,stdin);
                scanf("%[^\n]s",sent_add);
                //cin>>sent_add;
                strcat(sent_add," ");
                strcat(sent_add,argv[1]);
                send(sock, sent_add, strlen(sent_add), 0 );
                //send(sock , "RCPTshahin@shuvo" , strlen("shahin@shuvo") , 0 );
                cout<<"Client : "<<sent_add<<endl;

                memset(buffer,0,sizeof(buffer));
                valread = read( sock, buffer, 1024);
                //printf("%s\n",buffer );
                if(buffer[0]!='2')
                {
                    cout<<"Server : "<<buffer<<endl;
                    close(sock);
                    //printf("RCPT TO Request isn't Received\n");
                }
                else
                {
                    //  printf("RCPT TO Request is Received successfully\n");
                    cout<<"Server : "<<buffer<<endl;


                    char data[100];

                    fgets(data,100,stdin);
            scanf("%[^\n]s",data);
                    send(sock, data, strlen(data), 0);
                    cout<<"Client : "<<data<<" "<<endl;
                    memset(buffer,0,sizeof(buffer));
                    valread = read( sock, buffer, 1024);
                    if(buffer[0]!='3')
                    {
                        cout<<"Server : "<<buffer<<endl;
                        close(sock);
                        //printf("Data Request isn't received\n");
                        //cout<<"Server : "<<buffer<<endl;
                    }
                    else
                    {
                        //printf("Data Request is received successfully\n");
                        cout<<"Server : "<<buffer<<endl;
                        char* str_date = date();
                        char header[102400]="";
                        strcat(header,"FROM : ");
                        strcat(header,from_add);
                        strcat(header," ,");
                        strcat(header,"TO : ");
                        strcat(header,argv[1]);
                        strcat(header," ,");
                        strcat(header,"Subject : ");
                        strcat(header,argv[2]);
                        strcat(header," ,");
                        strcat(header,"Date : ");
                        strcat(header,str_date);
                        strcat(header," ");

                        send(sock, header, strlen(header), 0);
                        cout<<"Client : "<<header<<endl;
                        memset(buffer,0,sizeof(buffer));
                        valread = read( sock, buffer, 1024);
                        if(buffer[0]!='2')
                        {
                            cout<<"Server : "<<buffer<<endl;
                            close(sock);
                            //printf("header isn't received\n");
                        }
                        else
                        {
                            //printf("header is received successfully\n");
                            cout<<"Server : "<<buffer<<endl;
                            file_to_send(sock,argv[3]);
                            memset(buffer,0,sizeof(buffer));
                            valread = read( sock, buffer, 1024);
                            if(buffer[0]!='2')
                            {
                                cout<<"Server : "<<buffer<<endl;
                                close(sock);
                                //printf("Data isn't received\n");
                                    //cout<<"Server : "<<buffer<<endl;
                            }
                            else
                            {
                                cout<<"Server : "<<buffer<<endl;
                                //printf("Data is received successfully\n");
                                char quit[100];

                    fgets(quit,100,stdin);
                    scanf("%[^\n]s",quit);
                                //cin>>quit;
                                send(sock, quit, strlen(quit), 0);
                                cout<<"Client : "<<quit<<" "<<endl;
                                memset(buffer,0,sizeof(buffer));
                                valread = read( sock, buffer, 1024);
                                if(buffer[0]!='2')
                                {
                                    cout<<"Server : "<<buffer<<endl;
                                    close(sock);
                                    //printf("QUIT Request isn't received\n");

                                }
                                else
                                {
                                    cout<<"Server : "<<buffer<<endl;
                                    close(sock);
                                    //printf("QUIT Request is received successfully\n");

                                }
                            }

                        }



                    }


                }
            }

        }


        //printf("Hostname: %s\n", hostname);
    }

    return 0;
}
