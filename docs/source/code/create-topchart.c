/* 
 * Task: Read text from a file and list 10 most frequently used words
 * in it.
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

const int SIZEOFLINE = 1023;
const char* DELIMETERS = " ;,.-!?";
const char* filename = "redis.txt";

int main(int argc, char **argv) {
  redisContext *c;
  redisReply *reply=NULL;
  FILE *fp;
  char line[SIZEOFLINE+1];
  char* word;
  int i, n;
  
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

  fp = fopen(filename, "r");
  while (fgets(line, SIZEOFLINE, fp) != NULL) {
    word = strtok(line, DELIMETERS);
    while (word != NULL) {
      if (strlen(word) > 1) {
	RCMD(c, "ZINCRBY \"topchart\" 1 \"%s\"", word);
      }
      word = strtok (NULL, DELIMETERS);
    }
  }
  fclose(fp);

  RCMD(c, "ZRANGE \"topchart\" -10 -1 WITHSCORES");
  n = reply->elements-1;
  for (i=0; i<n; i=i+2) {
    printf("%s %s\n", reply->element[i+1]->str, reply->element[i]->str);
  }
  
  redisFree(c);
  return(0);
}
