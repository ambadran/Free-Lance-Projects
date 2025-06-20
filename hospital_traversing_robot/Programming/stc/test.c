#include <stdio.h>


int main() {

  int target = 180;
  int current = -90;

  int diff = target - current;
  diff = (diff + 180) % 360;
  /* printf("%d\n", diff); */

  if (diff < 0) diff += 360;
  diff -= 180;

  printf("%d\n", diff);
}
