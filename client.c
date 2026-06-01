// Client application

#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<stdbool.h>

#include<unistd.h>
#include<fcntl.h>
#include<sys/stat.h>

#include<sys/socket.h>
#include<netinet/in.h>
#include<arpa/inet.h>

///////////////////////////////////////////////////////////////////////////////
//
//  Function name :  ReadLine
//  Description  :   Reads characters one-by-one from a socket until a newline
//                   character '\n' is encountered, the buffer limit is reached,
//                   or the connection is closed. The result is stored as a
//                   null-terminated string in the provided buffer. Used to
//                   parse the server's response header line.
//  Author       :   Mangesh Ashok Bedre
//  Date         :   8/02/2026
//
///////////////////////////////////////////////////////////////////////////////

int ReadLine(int Sock, char *line, int max)
{
    int i = 0;
    char ch = '\0';
    int n = 0;

    while(i < (max - 1))
    {
        n = read(Sock, &ch, 1);

        if(n <= 0)
        {
            break;
        }

        line[i++] = ch;

        if(ch == '\n')
        {
            break;
        }
    }//End of while

    line[i] = '\0';

    return i;
}

///////////////////////////////////////////////////////////////////////////////
//
//  Function name :  main
//  Description  :   Entry point of the client application. Parses command-line
//                   arguments, creates a TCP socket, connects to the specified
//                   server, sends the requested filename, reads the response
//                   header to get the file size, and then receives the file
//                   content in chunks, writing it to a local output file.
//                   Prints download status upon completion or failure.
//  Author       :   Mangesh Ashok Bedre
//  Date         :   8/02/2026
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  CommandLine Argument Application
//
//  1st Arguent : IP Address
//  2nd Arguent : Port Number
//  3rd Arguent : Target File name
//  4th Arguent : New file name
//
//  ./client    127.0.0.1     9000       Demo.txt      A.txt
//   argv[0]    argv[1]       argv[2]    argv[3]       argv[4]
//
//  agrc = 5
//
///////////////////////////////////////////////////////////////////////////////

int main(int argc, char *argv[])
{
    int Port = 0;                   // argv[2]
    int Sock = 0;
    int iRet = 0;
    long recived = 0;
    long remainng = 0;
    int n = 0;
    int toRead = 0;

    char *Filename = NULL;          // argv[3]
    char *OutFilename = NULL;       // argv[4]
    char *IP = NULL;                // argv[1]
    char Header[64] = {'\0'};
    char Buffer[1024] = {'\0'};

    struct sockaddr_in ServerAddr;

    if((argc < 5) || (argc > 5))
    {
        printf("Unble to processed as invalid number argument\n");

        printf("Please provide below arguments\n");

        printf("1st Arguent : IP Address\n");
        printf("2nd Arguent : Port Number\n");
        printf("3rd Arguent : Target File name\n");
        printf("4th Arguent : New file name\n");

        return -1;
    }

    // Store command line argument
    IP = argv[1];
    Port = atoi(argv[2]);
    Filename = argv[3];
    OutFilename = argv[4];

    ///////////////////////////////////////////////////////////////////////////
    //  step 1 : Create TCP socket
    ///////////////////////////////////////////////////////////////////////////

    Sock = socket(AF_INET, SOCK_STREAM, 0);

    if(Sock < 0)
    {
        perror("Unble to create client socket");
        return -1;
    }

    ///////////////////////////////////////////////////////////////////////////
    //  step 2 : Connect with server
    ///////////////////////////////////////////////////////////////////////////

    memset(&ServerAddr, 0, sizeof(ServerAddr));

    ServerAddr.sin_family = AF_INET;
    ServerAddr.sin_port = htons(Port);

    // Convert the IP address into binary format
    inet_pton(AF_INET, IP, &ServerAddr.sin_addr);

    iRet = connect(Sock, (struct sockaddr*)&ServerAddr, sizeof(ServerAddr));

    if(iRet == -1)
    {
        perror("Unble to connect with server");
        close(Sock);
        return -1;
    }

    ///////////////////////////////////////////////////////////////////////////
    //  step 3 : Send file name
    ///////////////////////////////////////////////////////////////////////////

    write(Sock, Filename, strlen(Filename));
    write(Sock, "\n", 1);

    ///////////////////////////////////////////////////////////////////////////
    //  step 4 : Read the header
    ///////////////////////////////////////////////////////////////////////////

    iRet = ReadLine(Sock, Header, sizeof(Header));

    if(iRet <= 0)
    {
        perror("Server gets disconnected abnormally");
        close(Sock);
        return -1;
    }

    long Filesize = 0;

    sscanf(Header, "OK %ld", &Filesize);

    printf("File Size is : %ld\n", Filesize);

    ///////////////////////////////////////////////////////////////////////////
    //  step 5 : Create new file
    ///////////////////////////////////////////////////////////////////////////

    int outfd = 0;

    outfd = open(OutFilename, O_CREAT | O_WRONLY | O_TRUNC, 0777);

    if(outfd < 0)
    {
        perror("Unble to create downloaded file");
    }

    while(recived < Filesize)
    {
        remainng = Filesize - recived;

        if(remainng > 1024)
        {
            toRead = 1024;
        }
        else
        {
            toRead = remainng;
        }

        n = read(Sock, Buffer, toRead);

        write(outfd, Buffer, n);

        recived += n;

    }//End of while

    close(outfd);
    close(Sock);

    if(recived == Filesize)
    {
        printf("Download complete...\n");
        return 0;
    }
    else
    {
        perror("Download failed");
        return -1;
    }

    return 0;

}//End of main
