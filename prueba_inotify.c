// Prueba de captura de notificaciones de Linux
// Adaptado de http://www.thegeekstuff.com/2010/04/inotify-c-program-example/
// por Alberto Lafuente, Fac. Informática UPV/EHU

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <linux/inotify.h>

#define EVENT_SIZE (sizeof(struct inotify_event))
//#define EVENT_SIZE  (3*sizeof(uint32_t)+sizeof(int))
#define EVENT_BUF_LEN (1024 * (EVENT_SIZE + 16))
#define TESTIGO_NAME "inotify.example.executing"

int main(int argc, char *argv[])
{
  int length, i = 0;
  int fd;
  int wd;
  int wd_cd;
  char buffer[EVENT_BUF_LEN];
  char testigo[1024];
  char evento[1024];

  if (argc != 4)
  {
    fprintf(stdout, "Uso: %s Directorio_a_monitorizar user pass\n", argv[0]);
    exit(1);
  }

  //  fprintf(stdout, "---Prueba de inotify sobre %s\n", argv[1]);
  //  fprintf(stdout, "---Notifica crear/borrar ficheros/directorios sobre %s\n", argv[1]);
  //  fprintf(stdout, "---%s debe exixtir!\n", argv[1]);

  /*creating the INOTIFY instance*/
  fd = inotify_init();

  /*checking for error*/
  if (fd < 0)
  {
    perror("inotify_init");
  }

  /*adding the /tmp directory into watch list. Here, the suggestion is to validate the existence of the directory before adding into monitoring list.*/
  //  wd = inotify_add_watch( fd, "/tmp", IN_CREATE | IN_DELETE );

  /* Monitorizamos el directorio indicado como argumento. Debe estar creado. */
  //  mkdir(argv[1]);
  wd_cd = inotify_add_watch(fd, argv[1], IN_ALL_EVENTS);

  /* Testigo para finalizar cuando lo borremos: */

  strcpy(testigo, argv[1]);
  strcat(testigo, "/");
  strcat(testigo, TESTIGO_NAME);
  mkdir(testigo);

  /*read to determine the event change happens on the directory. Actually this read blocks until the change event occurs*/
  struct inotify_event event_st, *event;
  int k = 0;
  int exiting = 0;

  sleep(1);

  printf("%s\n", argv[2]);
  fflush(stdout);
  printf("%s\n", argv[3]);
  fflush(stdout);

  while (!exiting)
  {
    length = read(fd, buffer, EVENT_BUF_LEN);
    /*checking for error*/
    if (length < 0)
    {
      perror("read");
      break;
    }
    //    struct inotify_event *event = ( struct inotify_event * ) &buffer[ i ];
    while ((i < length) && !exiting)
    {
      //    event= &event_st;
      event = (struct inotify_event *)&buffer[i];
      //    memcpy(event, buffer, length);
      if (event->len)
      {
        fflush(stdout);
        //      memcpy(event+EVENT_SIZE, buffer+EVENT_SIZE, length);
        if (event->mask & IN_CREATE || event->mask & IN_MOVED_TO)
        {
          if (event->mask & IN_ISDIR)
          { // event: directory created
            fprintf(stdout, "3\n");
            fprintf(stdout, "%s/%s\n", argv[1], event->name);
          }
          else
          { // event: fie created
            fprintf(stdout, "3\n");
            fprintf(stdout, "%s/%s\n", argv[1], event->name);
          }
        }
        else if (event->mask & IN_DELETE || event->mask & IN_MOVED_FROM)
        {
          if (event->mask & IN_ISDIR)
          { // event: directory removed
            if (!strcmp(event->name, TESTIGO_NAME))
            {
              exiting = 1;
              //              break;
            }
            fprintf(stdout, "4\n");
            fprintf(stdout, "%s/%s\n", argv[1], event->name);
          }
          else
          { // event: file removed
            fprintf(stdout, "4\n");
            fprintf(stdout, "%s/%s\n", argv[1], event->name);
          }
        }
        else if (event->mask & IN_MODIFY)
        {
          if (event->mask & IN_ISDIR)
          { // event: directory removed
            if (!strcmp(event->name, TESTIGO_NAME))
            {
              exiting = 1;
              //              break;
            }
            fprintf(stdout, "4\n");
            fprintf(stdout, "%s/%s\n", argv[1], event->name);
            fprintf(stdout, "3\n");
            fprintf(stdout, "%s/%s\n", argv[1], event->name);
          }
          else
          { // event: file removed
            fprintf(stdout, "4\n");
            fprintf(stdout, "%s/%s\n", argv[1], event->name);
            fprintf(stdout, "3\n");
            fprintf(stdout, "%s/%s\n", argv[1], event->name);
          }
        }
      }
      i += EVENT_SIZE + event->len;
      fflush(stdout);
      //    fprintf(stdout, "---example.event count: %i\n", i);
    }
    i = 0;
  }

  //  printf("5\n");
  /*removing the directory from the watch list.*/
  inotify_rm_watch(fd, wd);
  inotify_rm_watch(fd, wd_cd);
  //  rmdir(argv[1]);

  /*closing the INOTIFY instance*/
  close(fd);
}
