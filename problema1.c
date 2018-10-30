#include <stdio.h>
#include <stdlib.h>

#define SIZE 9
#define DEBUG 1

FILE *file;
int result[SIZE*3];

void checkLine(int lineNum);
void checkColumn(int columnNum);
void checkSubgrid(int subgridNum);
void checkSudoku(int array[SIZE], int num, int multiple);

int main(int argc, char const *argv[]) {

  if(argc != 2)
  {
    fprintf(stderr, "Usage: problema1.out <file name>\n");
    return -1;
  }

  if (file = fopen(argv[1], "r"))
  {
    int i;
    for (i = 0; i < SIZE; i++)
    {
      //
      if (DEBUG)
        printf("%d\n", i+1);
      //
      checkLine(i);
      checkColumn(i);
      checkSubgrid(i);
    }
    fclose(file);
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

  int i;
  for (i = 0; i < SIZE*3; i++)
  {
    if (!result[i]){
      printf("Invalid solution!\n");
      return 0;
    }
  }
  printf("Valid solution!\n");

  return 0;
}

void checkLine(int lineNum)
{
  //size of line is 19
  int i, line[SIZE], position = (lineNum * 19);
  fseek(file, position, SEEK_SET);
  for (i = 0; i < SIZE; i++)
    fscanf(file, "%d", &line[i]);
  //
  if (DEBUG)
  {
    printf("line: ");
    for (i = 0; i < SIZE; i++)
      printf("%d ", line[i]);
    printf("\n");
  }
  //
  checkSudoku(line, lineNum, 0);
}

void checkColumn(int columnNum)
{
  int column[SIZE], i;
  for (i = 0; i < SIZE; i++)
  {
    //columnNum * 2 because of empty spaces
    int position = (i * 19) + (columnNum * 2);
    fseek(file, position, SEEK_SET);
    fscanf(file, "%d", &column[i]);
  }
  //
  if (DEBUG)
  {
    printf("column: ");
    for (i = 0; i < SIZE; i++)
      printf("%d ", column[i]);
    printf("\n");
  }
  //
  checkSudoku(column, columnNum, 1);
}

void checkSubgrid(int subgridNum)
{
  int line, column, position, subgrid[SIZE];
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
  //
  if (DEBUG)
  {
    int i;
    printf("subgrid: ");
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
