#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>

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
    char *NAME = cmd("echo -n $USER$SSH_TTY | tr / _");

    if (ssh_cmd && !strncmp(ssh_cmd, "scp ", 4))
        execl("/usr/bin/docker", "docker", "run", "--rm", "-i", "--cpus=4", "--memory=8g", "--net=host", "--cap-add=SYS_PTRACE", "-v", "/etc/group:/etc/group:ro", "-v", "/etc/passwd:/etc/passwd:ro", "-v", HOMEHOME, "--workdir", HOME, "--hostname", "docker", "-u", UIDGID, "--name", NAME, "secbin", "bash", "-c", ssh_cmd, NULL);
    else
        execl("/usr/bin/docker", "docker", "run", "--rm", "-it", "--cpus=4", "--memory=8g", "--net=host", "--cap-add=SYS_PTRACE", "-v", "/etc/group:/etc/group:ro", "-v", "/etc/passwd:/etc/passwd:ro", "-v", HOMEHOME, "--workdir", HOME, "--hostname", "docker", "-u", UIDGID, "--name", NAME, "secbin", "bash", "-c", "bash", NULL);

    free(HOMEHOME);
    free(UIDGID);
    free(NAME);
}

int main(int argc, char *argv[])
{
    int wstatus;
    char *ssh_cmd;

    freopen( "/dev/null", "w", stderr);

    setuid(0);

    signal(SIGHUP, sigh);
    signal(SIGTERM, sigh);
    signal(SIGKILL, sigh);

    ssh_cmd = getenv("SSH_ORIGINAL_COMMAND");
    if (!fork())
    	dockerit(ssh_cmd);

    wait(&wstatus);

    return 0;
}
