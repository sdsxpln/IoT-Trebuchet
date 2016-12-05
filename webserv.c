#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/stat.h>

const char *basic_response = "HTTP-Version: HTTP/1.0 200 OK\nContent-Length: 41\nContent-Type: text/html\n\n<html><head><title>It wokrs</head></html>";

const char *response_template = "HTTP-Version: HTTP/1.0 %s\nContent-Length: %ld\nContent-Type: %s\n\n%s";
const char *request_template = "%s %s HTTP/%s\n%s";
const char *html_template = "<html><title>webserv</title><body>%s</body></html>";

const char *content_plain = "text/plain";
const char *content_html = "text/html";
const char *content_gif = "image/gif";
const char *content_jpeg = "image/jpeg";

char *HTTP_OK = "200 OK";
char *HTTP_NOTFOUND = "404 Not Found";
char *HTTP_NOTIMPLEMENTED = "501 Not Implemented";

//should add a signal handler for exiting child processes.

char *format_response(int response_code, char *response_msg, char *content_type, void *content, int content_length)
{

}

char *basic_html_response(char *status, char *input_string)
{
  write(2, "in basic html resposne\n", 20);

  char *response_output = malloc(8000);
  char html_output[8000];
  sprintf(html_output, html_template, input_string);
  sprintf(response_output, response_template, status, strlen(input_string), content_html, html_output);
  return response_output;

}
//currently just copied from the given tcp source files
int main (int argc, const char *argv[])
{
  int sd, new_sd;
  if (argv[1] == NULL)
  {
    printf("%s\n", "webserv requires a port number, correct usage:\n./webserv port-number");
    exit(1);
  }
  int port = atoi(argv[1]);
  if (port < 5000 || port > 65536)
  {
    printf("%s\n", "provided port number is too high, must be in range 5000-65536");
    exit(1);
  }
  struct sockaddr_in name, cli_name;
  int sock_opt_val = 1;
  socklen_t cli_len;

  //should dynamically allocate this or better manage reading multiple times
  char data[8000]; 
  
  if ((sd = socket (AF_INET, SOCK_STREAM, 0)) < 0) 
  {
    perror("(servConn): socket() error");
    exit (-1);
  }

  if (setsockopt (sd, SOL_SOCKET, SO_REUSEADDR, (char *) &sock_opt_val,
    sizeof(sock_opt_val)) < 0) 
  {
    perror ("(servConn): Failed to set SO_REUSEADDR on INET socket");
    exit (-1);
  }

  name.sin_family = AF_INET;
  name.sin_port = htons (port);
  name.sin_addr.s_addr = htonl(INADDR_ANY);

  if (bind (sd, (struct sockaddr *)&name, sizeof(name)) < 0) 
  {
    perror ("(servConn): bind() error");
    exit (-1);
  }

  listen (sd, 5);

  for (;;) 
  {
    cli_len = sizeof (cli_name);
    new_sd = accept (sd, (struct sockaddr *) &cli_name, &cli_len);
    printf ("Assigning new socket descriptor:  %d\n", new_sd);

    if (new_sd < 0) 
    {
      perror ("(servConn): accept() error");
      exit (-1);
    }

    if (fork () == 0) 
    {   

      //set root directory to the cwd
      char dir[1000];
      getcwd(dir, 1000);
      chroot(dir);


      char request[8000];
      char version[8000];
      char method[100];
      //char remaining_data[8000];

      close (sd);

      read (new_sd, &data, 8000); 

      sscanf(data, request_template, method, request, version);
      if (strcmp(method, "GET") != 0)
      {
          fprintf(stderr,"wasn't a get");

          char *response = basic_html_response(HTTP_NOTIMPLEMENTED, "Error - Method not supported");
          write(new_sd, response, strlen(response));
          close(new_sd);
          exit(0);
      }
      fprintf(stderr, "request is: %s\nversion is: %s\n\n\n",
        request, version);
      fprintf(stderr, "wtfffff");

      struct stat statbuf;
      if (stat(request, &statbuf) < 0)
      {
        write(2, "after stat\n", 20);

        if (errno == ENOENT)
        {
          write(2, "errnolol\n", 3);

          //char *response = basic_html_response(HTTP_NOTFOUND, "404 resource not found");
          write(2, "after response made", 20);

          write(new_sd, basic_response, strlen(basic_response));
          close(new_sd);
          exit (0);


        }

      }
      //if (write (new_sd, basic_response, strlen(basic_response)) < 0)
      //  perror("error writing response to socket");





      //should probably check if there's more data to read, and if there is, read it
      printf ("Received string = %s\n", data);
      close(new_sd);
      write(2, "hehhfdasjflj", 5);

      exit (0);

    }
    close(new_sd);
  }
}