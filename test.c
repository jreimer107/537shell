#include <stdio.h>

int main(int argc, char *argv[]) {
  printf("%s\n", "Success");
  for (int i = 0; i < argc; i++) {
    printf("Child's args: %s\n", argv[i]);
  }
}
