#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>

#define IMAGE "nearffxx/ubuntu:18.04"

void sigh(int signum) {
    system("docker rm -f $(echo $USER$SSH_TTY | tr / _)");
}

char *cmd(char *cmd_str) {

  FILE *fp;
  char id[1024];

  fp = popen(cmd_str, "r");
  if (fp == NULL) {
    printf("Err\n" );
    exit(1);
  }

  fgets(id, sizeof(id) - 1, fp);

  pclose(fp);

  return strdup(id);
}

void dockerit(char *ssh_cmd) {
    char *HOMEHOME = cmd("echo -n $HOME:$HOME");
    char *HOME = getenv("HOME");
    char *UIDGID = cmd("echo -n $( id -u $USER ):$( id -g $USER )");
    char *NAME = cmd("echo -n $USER");
    char *SSH_AUTH_SOCK = getenv("SSH_AUTH_SOCK");
    char *SSH_AUTH_SOCKv = cmd("echo -n $SSH_AUTH_SOCK:$SSH_AUTH_SOCK");
    char *RUNNING = cmd("docker inspect -f {{.State.Running}} $USER");

    if (ssh_cmd && !strncmp(ssh_cmd, "scp ", 4))
        execl("/usr/bin/docker", "docker", "run", "--rm", "-i", "--cpus=1", "--memory=8g", "--net=host", "--cap-add=SYS_PTRACE", "-v", "/etc/group:/etc/group:ro", "-v", "/etc/passwd:/etc/passwd:ro", "-v", HOMEHOME, "--workdir", HOME, "--hostname", "docker", "-u", UIDGID, "--name", NAME, IMAGE, "bash", "-c", ssh_cmd, NULL);
    else if(!strncmp(RUNNING, "true", 4))
    	execl("/usr/bin/docker", "docker", "attach", NAME, NULL);
    else if(SSH_AUTH_SOCK)
        execl("/usr/bin/docker", "docker", "run", "--rm", "-it", "--cpus=1", "--memory=8g", "--net=host", "--cap-add=SYS_PTRACE", "-e", "SSH_AUTH_SOCK", "-v", SSH_AUTH_SOCKv, "-v", "/etc/group:/etc/group:ro", "-v", "/etc/passwd:/etc/passwd:ro", "-v", HOMEHOME, "--workdir", HOME, "--hostname", "docker", "-u", UIDGID, "--name", NAME, IMAGE, "tmux", "new", "-t", "0", NULL);
    else
        execl("/usr/bin/docker", "docker", "run", "--rm", "-it", "--cpus=1", "--memory=8g", "--net=host", "--cap-add=SYS_PTRACE", "-v", "/etc/group:/etc/group:ro", "-v", "/etc/passwd:/etc/passwd:ro", "-v", HOMEHOME, "--workdir", HOME, "--hostname", "docker", "-u", UIDGID, "--name", NAME, IMAGE, "tmux", "new", "-t", "0", NULL);

    free(HOMEHOME);
    free(UIDGID);
    free(NAME);
    free(RUNNING);
}

int main(int argc, char *argv[])
{
    int wstatus;
    char *ssh_cmd;

    //freopen( "/dev/null", "w", stderr);

    setuid(0);

    //signal(SIGHUP, sigh);
    signal(SIGTERM, sigh);
    signal(SIGKILL, sigh);

    ssh_cmd = getenv("SSH_ORIGINAL_COMMAND");
    if (!fork())
    	dockerit(ssh_cmd);

    wait(&wstatus);

    return 0;
}
