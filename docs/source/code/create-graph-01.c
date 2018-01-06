/* 
 * Task: Read /var/log/dpkg.log and create a graph to visualize how
 * often packages are installed, upgraded and removed.
 * Tested with: gcc 7.2, hiredis 0.13 and redis 4.0.1
 */

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <hiredis.h>

#define RCMD(context, ...) {                  \
  freeReplyObject(reply);                     \
  reply = redisCommand(context, __VA_ARGS__); \
  assert(reply != NULL);                      \
  }

const char LOG_SEPARATOR = ' ';
const char CSV_SEPARATOR = ';';
const char *LOG_FILES[] = { "dpkg.log", };

redisContext *c;

/* This function reads log_file and puts the status into the database.*/
int read_log(const char *log_file, redisContext *c) {
  redisReply *reply=NULL;
  FILE *fp;
  char line[256];
  char *p, *date, *status;

  fp = fopen(log_file, "r");
  while (fgets(line, sizeof(line), fp) != NULL) {
    p = line;
    date = p++;
    while (p[0] != LOG_SEPARATOR) p++;
    p++;
    while (p[0] != LOG_SEPARATOR) p++;
    *(p-3) = '\0';
    status = ++p;
    while (p[0] != LOG_SEPARATOR) p++;
    *p = '\0';
    RCMD(c, "ZINCRBY %s 1 \"%s\"", status, date);
  }
  fclose(fp);
  return(0);
}

/* This function reads the database and writes the status CSV file. */
int write_csv(char *status, redisContext *c) {
  redisReply *reply=NULL;
  FILE *fp;
  int i,n;
  char filename[256];

  strcpy(filename, status);
  strcat(filename, ".csv");
  fp = fopen(filename, "w");
  RCMD(c, "ZRANGE %s 0 -1 WITHSCORES", status);
  n = reply->elements-1;
  for (i=0; i<n; i=i+2) {
    fprintf(fp, "%s %c %s\n", reply->element[i]->str, \
	    CSV_SEPARATOR, \
	    reply->element[i+1]->str);
  }
  fclose(fp);
  return(0);
}


int main(int argc, char **argv) {
  redisReply *reply=NULL;
  int i, n;
  char *status;
  
  c = redisConnect("localhost", 6379);
  if (c == NULL || c->err) {
    if (c) {
      printf("Connection error: %s\n", c->errstr);
      redisFree(c);
    } else {
      printf("Connection error: can't allocate redis context\n");
    }
    exit(1);
  }
	
  RCMD(c, "SELECT 0");
  RCMD(c, "FLUSHDB");

  n = sizeof(LOG_FILES)/sizeof(LOG_FILES[0]);
  for (i = 0; i < n; i++) {
    read_log(LOG_FILES[i], c);
  }

  RCMD(c, "SCAN 0 COUNT 10");
  n = reply->element[1]->elements;
  for (i=0; i<n; i++) {
    status = reply->element[1]->element[i]->str;
    write_csv(status, c);
  }

  redisFree(c);
  return(0);
}
