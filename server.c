//Server Code

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
//  Function name :  SendFileToClient
//  Description  :   Opens the requested file, reads its content, and sends it
//                   to the connected client over a TCP socket. First sends a
//                   header "OK <filesize>\n" on success, or "ERR\n" if the
//                   file cannot be opened. Then streams the raw file bytes.
//  Author       :   Mangesh Ashok Bedre
//  Date         :   8/02/2026
//
///////////////////////////////////////////////////////////////////////////////

void SendFileToClient(int ClientSocket, char *Filename)
{
    int fd = 0;
    int BytesRead = 0;

    char Buffer[1024];
    char Header[64] = {'\0'};

    struct stat sobj;

    printf("File name is : %s\n", Filename);

    fd = open(Filename, O_RDONLY);

    if(fd == -1)
    {
        perror("Unble to open");

        // Send error message to client
        write(ClientSocket, "ERR\n", 4);

        return;
    }

    stat(Filename, &sobj);

    // Header : "OK 1700"
    snprintf(Header, sizeof(Header), "OK %ld\n", (long)sobj.st_size);

    // Write Header to client
    write(ClientSocket, Header, strlen(Header));

    memset(Buffer, '\0', sizeof(Buffer));

    // Send actual file content
    while((BytesRead = read(fd, Buffer, sizeof(Buffer))) > 0)
    {
        write(ClientSocket, Buffer, BytesRead);
    }

    close(fd);
}

///////////////////////////////////////////////////////////////////////////////
//
//  Function name :  main
//  Description  :   Entry point of the server application. Creates a TCP
//                   socket, binds it to the specified port, and listens for
//                   incoming client connections in an infinite loop. For each
//                   accepted connection, a child process is forked to handle
//                   the file transfer independently, allowing the server to
//                   serve multiple clients concurrently.
//  Author       :   Mangesh Ashok Bedre
//  Date         :   8/02/2026
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  CommandLine Argument Application
//
//  1st Arguent : Port Number
//
//  ./server     9000
//   argv[0]    argv[1]
//
///////////////////////////////////////////////////////////////////////////////

int main(int argc, char *argv[])
{
    int Port = 0;
    int ServerSocket = 0;
    int ClientSocket = 0;
    int iRet = 0;

    pid_t pid = 0;
    char Filename[50] = {'\0'};

    struct sockaddr_in ServerAddr;
    struct sockaddr_in ClientAddr;

    socklen_t AddLen = sizeof(ClientAddr);

    if((argc < 2) || (argc > 2))
    {
        printf("Unble to processed as invalid number of argument\n");
        printf("Please provide : \n");
        printf("\nPort Number : \n");

        return -1;
    }

    // Port Number of server
    Port = atoi(argv[1]);

    ///////////////////////////////////////////////////////////////////////////
    //  step 1 : Create TCP socket
    ///////////////////////////////////////////////////////////////////////////

    ServerSocket = socket(AF_INET, SOCK_STREAM, 0);

    if(ServerSocket == -1)
    {
        perror("Unble to create server socket");
        return -1;
    }

    ///////////////////////////////////////////////////////////////////////////
    //  step 2 : Bind socket
    ///////////////////////////////////////////////////////////////////////////

    memset(&ServerAddr, 0, sizeof(ServerAddr));

    // Initialise structure
    ServerAddr.sin_family = AF_INET;
    ServerAddr.sin_port = htons(Port);
    ServerAddr.sin_addr.s_addr = INADDR_ANY;

    iRet = bind(ServerSocket, (struct sockaddr*) &ServerAddr, sizeof(ServerAddr));

    if(iRet == -1)
    {
        perror("Unble to bind");
        close(ServerSocket);
        return -1;
    }

    ///////////////////////////////////////////////////////////////////////////
    //  step 3 : Listen for client connection
    ///////////////////////////////////////////////////////////////////////////

    iRet = listen(ServerSocket, 11);

    if(iRet == -1)
    {
        perror("Server unble to listen request");
        close(ServerSocket);
        return -1;
    }

    printf("Server is running on port : %d\n", Port);

    ///////////////////////////////////////////////////////////////////////////
    //  Loop which accept client requests continiously
    ///////////////////////////////////////////////////////////////////////////

    while(1)
    {
        ///////////////////////////////////////////////////////////////////////////
        //  step 4 : Accept the client request
        ///////////////////////////////////////////////////////////////////////////

        memset(&ClientAddr, 0, sizeof(ClientAddr));

        printf("Server is waiting for client request \n");

        ClientSocket = accept(ServerSocket, (struct sockaddr*) &ClientAddr, &AddLen);

        if(ClientSocket == -1)
        {
            perror("Unble to accept client request");
            continue;
        }

        printf("Client gets connected : %s\n", inet_ntoa(ClientAddr.sin_addr));

        ///////////////////////////////////////////////////////////////////////////
        //  step 5 : Create new process to handle client request
        ///////////////////////////////////////////////////////////////////////////

        pid = fork();

        if(pid < 0)
        {
            perror("Unble to a new process for client request");
            close(ClientSocket);
            continue;
        }

        // New process gets created for client
        if(pid == 0)
        {
            printf("\nNew process is created for client request\n");

            close(ServerSocket);

            iRet = read(ClientSocket, Filename, sizeof(Filename));

            printf("Requested file by client : %s\n", Filename);

            Filename[strcspn(Filename, "\r\n")] = '\0';

            SendFileToClient(ClientSocket, Filename);

            close(ClientSocket);

            printf("File transfer done & client disconnected\n\n");

            exit(0);

        }//End of if(fork)
        else    // Parent process (Server)
        {
            close(ClientSocket);
        }//End of else

    }//End of while

    return 0;

}//End of main
