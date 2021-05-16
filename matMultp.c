#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include <sys/time.h>
#include <ctype.h>

//Pointers to input and output file names
char *Mat1,*Mat2,*Output;
//values of arrays dimensions
int rows1,rows2,columns1,columns2;
//Pointers to 2D matrices
int **Matrix1,**Matrix2;
//Pointer to Result array which is used to print the output each time and it resets to 0 after each method
int **Result;
//Pointer to Output File
FILE *fop;
//For time measuring
struct timeval stop, start;

///This function is used to print the resulting array in the output file
void printResult(int numberOfThreads){
    for (int i = 0 ; i < rows1 ;i++){
        for (int j = 0 ; j < columns2 ;j++)
            fprintf(fop,"%d ",Result[i][j]);
        fprintf(fop,"\n");
    }
    fprintf(fop,"\nSeconds taken %lu\n", stop.tv_sec - start.tv_sec);
    fprintf(fop,"Microseconds taken: %lu\n", stop.tv_usec - start.tv_usec);
    fprintf(fop,"Number of threads used : %d" , numberOfThreads);
    fprintf(fop,"\n----------------------------\n");
}
///Arguments sent for the third method
struct threadData{
    int row,column;
};
///This method is used by thread to solve matrix multiplication by the third method
void *calculateMethod3(void * data) {
    struct threadData *myData;
    myData = (struct threadData *) data;
    int row = myData->row;
    int column = myData->column;
    for (int k = 0; k < columns1; k++)
        Result[row][column] += Matrix1[row][k] * Matrix2[k][column];
}
///The third method mentioned which uses one thread per element
void solveMethod3(){
    fprintf(fop,"Method #3\nThe Result Array :\n");
    int numOfThreads = rows1*columns2;
    pthread_t Threads[rows1][columns2];

    gettimeofday(&start, NULL); //start checking time

    for(int i = 0 ; i < rows1 ;i++){
        for(int j = 0 ; j < columns2 ; j++){
            struct threadData *data = malloc(sizeof (struct threadData));
            data->row = i;
            data->column = j;
            pthread_create(&Threads[i][j],NULL,calculateMethod3,(void *)data);
        }
    }
    for (int i = 0 ; i < rows1 ;i++)
        for (int j = 0 ; j < columns2 ; j++)
            pthread_join(Threads[i][j],NULL);

    gettimeofday(&stop, NULL); //end checking time
    printResult(numOfThreads);
}
///This method is used by thread to solve matrix multiplication by the second method
void *calculateMethod2(void * r){
    long row = (long) r;
    for (int j = 0 ; j < columns2 ;j++) {
        for (int k = 0; k < columns1; k++)
            Result[row][j] += Matrix1[row][k] * Matrix2[k][j];
    }
}
///The second method mentioned which uses one thread per row
void solveMethod2(){
    fprintf(fop,"Method #2\nThe Result Array :\n");
    int numOfThreads = rows1;
    pthread_t Threads[numOfThreads];

    gettimeofday(&start, NULL); //start checking time

    for(long i = 0 ; i < numOfThreads ;i++){
        pthread_create(&Threads[i],NULL,calculateMethod2,(void *)i);
    }
    for (int i = 0 ; i < numOfThreads ;i++){
        pthread_join(Threads[i],NULL);
    }

    gettimeofday(&stop, NULL); //end checking time
    printResult(numOfThreads);
}

///This method is used by thread to solve matrix multiplication by the first method
void *calculateMethod1(){
    for (int i = 0 ; i < rows1 ; i++){
        for (int j = 0 ; j < columns2 ;j++){
            for (int k = 0 ; k < columns1 ; k++){
                Result[i][j] += Matrix1[i][k] * Matrix2[k][j];
            }
        }
    }
}
///The first method mentioned which uses one thread only to solve the 2 matrices
void solveMethod1(){
    fprintf(fop,"Method #1\nThe Result Array :\n");
    pthread_t thread;

    gettimeofday(&start, NULL); //start checking time

    pthread_create(&thread,NULL,calculateMethod1,NULL);
    pthread_join(thread,NULL);

    gettimeofday(&stop, NULL); //end checking time
    printResult(1);
}
///This function is used to print out the error and close the program
void error(char *ERR,char*ERR2){
    FILE *fpe;
    fpe = fopen(Output,"w");
    fprintf(fpe,"%s %s",ERR,ERR2);
    fclose(fpe);
    exit(EXIT_FAILURE);
}
///This function resets the result array
void reset(){
    for (int i = 0 ; i < rows1 ;i++)
        for (int j = 0 ; j < columns2 ;j++)
            Result[i][j]=0;
}
///This function is used to initialize Matrix 1 and Matrix 2
int **initializeMatrix (char *Mat){
    FILE *fp;
    fp = fopen(Mat,"r");
    if (fp == NULL)
        error("ERROR : THE FOLLOWING FILE DOESN'T EXIST ",Mat);
    int rows,columns;
    if (Mat == Mat1){
        fscanf(fp,"row=%d col=%d",&rows1,&columns1);
        rows = rows1;
        columns = columns1;
    }else{
        fscanf(fp,"row=%d col=%d",&rows2,&columns2);
        rows = rows2;
        columns = columns2;
    }
    int **ResultMat;
    ResultMat = malloc(rows * columns * sizeof(int));
    for (int i=0; i<rows; i++)
        ResultMat[i] = (int *)malloc(columns * sizeof(int));
    for (int i = 0; i < rows; i++)
        for (int j = 0; j < columns; j++){
            char input[15];
            fscanf(fp,"%s",input);
            for (int k = 0 ; k < strlen(input); k++){
                if (!isdigit(input[k]))
                    error("ERROR : WRONG INPUT FORM MATRIX ",Mat);
            }
            ResultMat[i][j] = atoi(input);
        }
    return ResultMat;
}

int main(int argc, char* argv[]) {
    //This part manages arguments
    if (argc ==1){
        Mat1 = "a.txt";
        Mat2 = "b.txt";
        Output = "c.out";
    }else if (argc == 4){
        Mat1 = argv[1];
        Mat2 = argv[2];
        Output = argv[3];
    }else
        error("ERROR : ARGUMENTS ENTERED ARE WRONG",NULL);

    //Initialize matrix 1
    Matrix1 = initializeMatrix(Mat1);
    //Initialize matrix 2
    Matrix2 = initializeMatrix(Mat2);

    //Check if the 2 matrices can be multiplied together
    if (columns1 != rows2)
        error("ERROR : THE 2 MATRICES CAN'T BE MULTIPLIED TOGETHER",NULL);

    //Allocation of 2D result array(Global variable)
    Result = malloc(rows1 * columns2 * sizeof(int));
    for (int i=0; i<rows1; i++)
        Result[i] = (int *)malloc(columns2 * sizeof(int));

    //Resetting and Initializing the output File
    fop =fopen(Output,"w");

    reset();

    solveMethod1();

    reset();

    solveMethod2();

    reset();

    solveMethod3();

    //De-allocation of arrays and closing the output file
    for(int i = 0 ; i <rows1 ; i++){
        free(Result[i]);
        free(Matrix1[i]);
    }
    for(int i = 0 ; i < rows2 ; i++)
        free(Matrix2[i]);
    free(Result);
    free(Matrix1);
    free(Matrix2);
    fclose(fop);
    return 0;
}