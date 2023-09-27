#include <stdio.h>
#include <unistd.h>
#include <pwd.h>


void print_user_prompt(void) {
  uid_t ruid = getuid();
  struct passwd * pw = getpwuid(ruid);
  printf("%s> ", pw->pw_name);
}

void print(char * output) {
  printf("%s\n", output);
}
