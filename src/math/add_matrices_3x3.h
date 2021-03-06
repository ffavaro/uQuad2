////////////////////////////////////////////////////////////////////////////////
// File: add_matrices_3x3.h                                                   //
// Routine(s):                                                                //
//    Add_Matrices_3x3                                                        //
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//  void Add_Matrices_3x3(double *C, double *A, double *B)                    //
//                                                                            //
//  Description:                                                              //
//     Add the 3x3 matrices A and B to form the 3x3 matrix C, i.e. C = A + B. //
//     All matrices should be declared as "double X[3][3]" in the calling     //
//     routine where X = A, B, C.                                             //
//                                                                            //
//  Arguments:                                                                //
//     double *C    Address of the first element of the matrix C.             //
//     double *A    Address of the first element of the matrix A.             //
//     double *B    Address of the first element of the matrix B.             //
//                                                                            //
//  Return Values:                                                            //
//     void                                                                   //
//                                                                            //
//  Example:                                                                  //
//     double A[3][3],  B[3][3], C[3][3];                                     //
//                                                                            //
//     (your code to initialize the matrices A and B)                         //
//                                                                            //
//     Add_Matrices_3x3((double *) C, &A[0][0], &B[0][0]);                    //
//     printf("The matrix C = A + B is \n"); ...                              //
////////////////////////////////////////////////////////////////////////////////

#define Add_Matrices_3x3(C,A,B) {double*pC=(double*)C; double*pA=(double*)A;\
double*pB=(double*)B; pC[0]=pA[0]+pB[0]; pC[1]=pA[1]+pB[1]; pC[2]=pA[2]+pB[2];\
pC[3]=pA[3]+pB[3]; pC[4]=pA[4]+pB[4]; pC[5]=pA[5]+pB[5]; pC[6]=pA[6]+pB[6];\
pC[7]=pA[7]+pB[7]; pC[8]=pA[8]+pB[8];}
