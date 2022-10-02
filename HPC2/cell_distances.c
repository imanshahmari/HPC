#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <stdint.h>
#include <math.h>
#include <time.h>

#define MAX_LINE_LENGTH 100
#define filename "cell_distances/test_data/cell_e4"

static inline long int findSize(char file_name[])
{
  // opening the file in read mode
  FILE *fp = fopen(file_name, "r");

  // checking if the file exist or not
  if (fp == NULL)
  {
    printf("File Not Found!\n");
    return -1;
  }

  fseek(fp, 0L, SEEK_END);

  // calculating the size of the file
  long int res = ftell(fp);

  // closing the file
  fclose(fp);

  return res;
}

static inline void  distance(float *d, float x1, float y1, float z1, float x2, float y2, float z2)
{
  *d =roundf((sqrtf((x2 - x1)*(x2 - x1) + (y2 - y1)*(y2 - y1) + (z2 - z1)*(z2 - z1)))*100)/100 ;
  return;
}

static inline int compare_function(const void *a, const void *b)
{
  float *x = (float *)a;
  float *y = (float *)b;
  // return *x - *y; // this is WRONG...
  if (*x < *y)
    return -1;
  else if (*x > *y)
    return 1;
  return 0;
}

int main(int argc, char argv[])
{ 
  // Parse begin
  double time_spent_parse = 0.0;
  clock_t begin_parse = clock();


  long int res = findSize(filename);
  int nrLines = res / (8 * 3); // 8bytes to store float and 3 of such
  printf("%d\n", nrLines);
  int currentLine = 0;
  char line[MAX_LINE_LENGTH + 1] = {0};

  printf("The size is %ld\n", res);

  float *elements = (float *)malloc(sizeof(float) * nrLines * 3); // here we use that each coordinate is represented by 8 charcters, matching sizeof(float)
  float **matrix = (float **)malloc(nrLines * sizeof(float *));

  for (size_t ix = 0, jx = 0; ix < nrLines; ++ix, jx += 3)
  {
    matrix[ix] = elements + jx;
  }

  FILE *file = fopen(filename, "r");
  if (file == NULL)
  {
    printf("error opening file\n");
    return -1;
  }

  while (fgets(line, MAX_LINE_LENGTH, file))
  {
    matrix[currentLine][0] = atof(line);
    matrix[currentLine][1] = atof(line + 8);
    matrix[currentLine][2] = atof(line + 16);
    currentLine++;
  }
  fclose(file);


  clock_t end_parse = clock();
  
  time_spent_parse += (double)(end_parse-begin_parse) /CLOCKS_PER_SEC;

  printf("Parsing: elapsed time is %f seconds\n", time_spent_parse);

  //Parse end


  // Calculate distance begin 

  double time_spent_calc = 0.0;
  clock_t begin_calc = clock();


  int combinations = (nrLines * (nrLines - 1)) / 2;
  printf("The number of combinations are: %d\n", combinations);

  float *distances = (float*)malloc(sizeof(float) * combinations);
  int *occurences =  (int*)malloc(sizeof(int) * combinations);


  int j = 0;
  for (int i = 0; i < nrLines - 1; i++)
  {
    for (int k = i + 1; k < nrLines; k++)
    {

      distance(&distances[j], matrix[i][0], matrix[i][1], matrix[i][2], matrix[k][0], matrix[k][1], matrix[k][2]);

      j = j + 1;
    }
  }

  clock_t end_calc = clock();
  
  time_spent_calc += (double)(end_calc-begin_calc) /CLOCKS_PER_SEC;

  printf("Calulate distances: elapsed time is %f seconds\n", time_spent_calc);
  // Calculate distance end

  // Sort

  double time_spent_sort = 0.0;
  clock_t begin_sort = clock();

  qsort(distances, combinations, sizeof(float), compare_function);

  clock_t end_sort = clock();
  
  time_spent_sort += (double)(end_sort-begin_sort) /CLOCKS_PER_SEC;

  printf("Sort: elapsed time is %f seconds\n", time_spent_sort);

  double time_spent_occ = 0.0;
  clock_t begin_occ = clock();


  for (int j = 0;  j < combinations; j++)
  {   

     occurences[j] = 1;
   
    if (distances[j] == distances[j + 1])
    {
      occurences[j] += 1.0;
    }
    

    //printf("%0.2f  ", distances[j]);
    //printf("%d\n", occurences[j]);
  }

  clock_t end_occ = clock();
  
  time_spent_occ += (double)(end_occ-begin_occ) /CLOCKS_PER_SEC;

  printf("Calculate occurences: elapsed time is %f seconds\n", time_spent_occ);

  printf("Total elapsed time is %f seconds \n", time_spent_parse+time_spent_calc+ time_spent_sort);


  free(matrix);
  free(elements);
  free(distances);
  free(occurences);

  return 0;
}