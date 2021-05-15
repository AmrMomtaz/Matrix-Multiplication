#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include <sys/time.h>

//Pointers to input and output file names
char * Mat1,*Mat2,*Output;
//values of arrays dimensions
int rows1,rows2,columns1,columns2;
//Pointers to 2D matrices
int **Matrix1,**Matrix2;
//Pointer to Result array which is used to print the output each time and it resets to 0 after each method
int **Result;
//Pointer to Output File
FILE *fop;

///This function is used to print the resulting array in the output file
void printArr(){
    for (int i = 0 ; i < rows1 ;i++){
        for (int j = 0 ; j < columns2 ;j++)
            fprintf(fop,"%d ",Result[i][j]);
        fprintf(fop,"\n");
    }
}
///Point function for each thread in a row solving using second method
void *calculateMethod2(int row){
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

    struct timeval stop, start;
    gettimeofday(&start, NULL); //start checking time

    for(int i = 0 ; i < numOfThreads ;i++){
        pthread_create(&Threads[i],NULL,calculateMethod2,i);
    }
    for (int i = 0 ; i < numOfThreads ;i++){
        pthread_join(Threads[i],NULL);
    }
    gettimeofday(&stop, NULL); //end checking time
    printArr();
    fprintf(fop,"\nSeconds taken %lu\n", stop.tv_sec - start.tv_sec);
    fprintf(fop,"Microseconds taken: %lu\n", stop.tv_usec - start.tv_usec);
    fprintf(fop,"Number of threads used : %d" , numOfThreads);
    fprintf(fop,"\n----------------------------\n");
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

    struct timeval stop, start;
    gettimeofday(&start, NULL); //start checking time

    pthread_create(&thread,NULL,calculateMethod1,NULL);
    pthread_join(thread,NULL);

    gettimeofday(&stop, NULL); //end checking time
    printArr();
    fprintf(fop,"\nSeconds taken %lu\n", stop.tv_sec - start.tv_sec);
    fprintf(fop,"Microseconds taken: %lu\n", stop.tv_usec - start.tv_usec);
    fprintf(fop,"Number of threads used : 1");
    fprintf(fop,"\n----------------------------\n");
}

///This function resets the result array
void reset(){
    for (int i = 0 ; i < rows1 ;i++)
        for (int j = 0 ; j < columns2 ;j++)
            Result[i][j]=0;
}
///This function is used to initialize Matrix 1 and Matrix 2
int **initializeMatrix (FILE *fp, int rows , int columns){
    fscanf(fp,"row=%d col=%d",&rows,&columns);
    int **Result;
    Result = malloc(rows * columns * sizeof(int));
    for (int i=0; i<rows; i++)
        Result[i] = (int *)malloc(columns * sizeof(int));
    for (int i = 0; i < rows; i++)
        for (int j = 0; j < columns; j++)
            fscanf(fp,"%d",&Result[i][j]);
    return Result;
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
    }else{
        printf("ARGUMENTS ERROR");
    }

    //This part initialized the first Matrix
    FILE *fp1;
    fp1 = fopen(Mat1,"r");
    fscanf(fp1,"row=%d col=%d",&rows1,&columns1);
    Matrix1 = initializeMatrix(fp1,rows1,columns1);
    fclose(fp1);

    //This part initialized the second matrix
    FILE *fp2;
    fp2 = fopen(Mat2,"r");
    fscanf(fp2,"row=%d col=%d",&rows2,&columns2);
    Matrix2 = initializeMatrix(fp2,rows2,columns2);
    fclose(fp2);

    //Check if the 2 matrices can be multiplied together'
    if (rows2 != columns1)
        printf("ERROR .. THE 2 MATRICES CAN'T BE MULTIPLIED TOGETHER");

    //Allocation of 2D result array(Global variable)
    Result = malloc(rows1 * columns2 * sizeof(int));
    for (int i=0; i<rows1; i++)
        Result[i] = (int *)malloc(columns2 * sizeof(int));

    //Initializing the output File
    fop =fopen(Output,"a");

    reset();

    solveMethod1();

    reset();

    solveMethod2();

    reset();

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