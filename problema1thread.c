#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#define SIZE 9
#define NUM_THREADS 9
#define DEBUG 1

FILE *file;
int result[SIZE*3];
struct ARG
{
  int num;
  char *argv;
  void (*check)();
};

void *multipleRunner(void *param);
void checkLine(int lineNum, char *argv);
void checkColumn(int columnNum, char *argv);
void checkSubgrid(int subgridNum, char *argv);
void checkSudoku(int array[SIZE], int num, int multiple);
void checkSolution();

int main(int argc, char *argv[]) {

  if(argc != 2)
  {
    fprintf(stderr, "Usage: problema1.out <file name>\n");
    return -1;
  }

  if (file = fopen(argv[1], "r"))
  {
    fclose(file);
    void (*check[3]) (int num, char *argv) = {checkLine, checkColumn, checkSubgrid};
    int i, j;
    for(i = 0; i < 3; i++)
    {
      pthread_t workers[NUM_THREADS];
      pthread_attr_t attrs[NUM_THREADS];
      struct ARG args[NUM_THREADS];
      for(j = 0; j < NUM_THREADS; j++)
      {
        args[j].num = j;
        args[j].argv = argv[1];
        args[j].check = check[i];
        pthread_attr_init(&attrs[j]);
        pthread_create(&workers[j], &attrs[j], multipleRunner, &args[j]);
      }
      for(j = 0; j < NUM_THREADS; j++)
        pthread_join(workers[j], NULL);
    }
    checkSolution();
    //
    if (DEBUG)
    {
      printf("\nSolutions:");
      for (i = 0; i < SIZE*3; i++)
      {
        if (!(i % 9))
          printf("\n");
        printf("%d", result[i]);
      }
      printf("\n\n");
    }
    //
  }
  else
  {
    fprintf(stderr, "File %s not found!\n", argv[1]);
    return -1;
  }

  return 0;
}

void *multipleRunner(void *param)
{
  struct ARG *arg = (struct ARG *)param;
  arg->check(arg->num, arg->argv);
  pthread_exit(0);
}

void checkLine(int lineNum, char *argv)
{
  //size of line is 19
  int i, line[SIZE], position = (lineNum * 19);
  FILE *file = fopen(argv, "r");
  fseek(file, position, SEEK_SET);
  for (i = 0; i < SIZE; i++)
    fscanf(file, "%d", &line[i]);
  fclose(file);
  //
  if (DEBUG)
  {
    printf("line %d: ", lineNum + 1);
    for (i = 0; i < SIZE; i++)
      printf("%d ", line[i]);
    printf("\n");
  }
  //
  checkSudoku(line, lineNum, 0);
}

void checkColumn(int columnNum, char *argv)
{
  int column[SIZE], i;
  FILE *file = fopen(argv, "r");
  for (i = 0; i < SIZE; i++)
  {
    //columnNum * 2 because of empty spaces
    int position = (i * 19) + (columnNum * 2);
    fseek(file, position, SEEK_SET);
    fscanf(file, "%d", &column[i]);
  }
  fclose(file);
  //
  if (DEBUG)
  {
    printf("column %d: ", columnNum + 1);
    for (i = 0; i < SIZE; i++)
      printf("%d ", column[i]);
    printf("\n");
  }
  //
  checkSudoku(column, columnNum, 1);
}

void checkSubgrid(int subgridNum, char *argv)
{
  int line, column, position, subgrid[SIZE];
  FILE *file = fopen(argv, "r");
  // ((subgridNum / 3) * 57) for first line of subgrid, ((subgridNum % 3) * 6) for first column of subgrid
  int initialPosition = ((subgridNum / 3) * 57) + ((subgridNum % 3) * 6);
  for (line = 0; line < 3; line++)
    for (column = 0; column < 3; column++)
    {
      // (line * 19) for next line inside subgrid, (column * 2) for next column inside subgrid
      position = initialPosition + (line * 19) + (column * 2);
      fseek(file, position, SEEK_SET);
      fscanf(file, "%d", &subgrid[(3 * line) + column]);
    }
  fclose(file);
  //
  if (DEBUG)
  {
    int i;
    printf("subgrid %d: ", subgridNum + 1);
    for (i = 0; i < SIZE; i++)
      printf("%d ", subgrid[i]);
    printf("\n");
  }
  //
  checkSudoku(subgrid, subgridNum, 2);
}

void checkSudoku(int array[SIZE], int num, int multiple)
{
  int i, count[SIZE] = {0}, checker = 1, position = num + (multiple * 9);
  for(i = 0; i < SIZE; i++)
  {
    if (count[array[i]] != 1)
      count[array[i]]++;
    else
      checker = 0;
  }
  if (checker)
    result[position] = 1;
  else
    result[position] = 0;
}

void checkSolution() {
  int i;
  char *solution = "Valid solution!";
  for (i = 0; i < SIZE*3; i++)
  {
    if (!result[i])
      solution = "Invalid solution!";
  }
  printf("%s\n", solution);
}