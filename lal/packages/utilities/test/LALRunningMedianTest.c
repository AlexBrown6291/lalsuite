/******************************** <lalVerbatim file="LALRunningMedianTestCV">
Author: B. Machenschalk
$Id$
********************************* </lalVerbatim> */

/********************************************************** <lalLaTeX>
\subsection{Program \texttt{LALRunningMedianTest.c}}
\label{ss:LALRunningMedianTest.c}

Program to test running median function

\subsubsection*{Usage}
\begin{verbatim}
LALRunningMedianTest [length blocksize [lalDebugLevel]]
\end{verbatim}

\subsubsection*{Description}

This program test the LALRunningMedian functions
First the proper function of the input checks is tested.
Then it reads an array size and a block size from the command
line, fills an array of the given size with random numbers,
computes medians of all blocks with blocksize using the
LALRunningMedian functions and compares the results against
inividually calculated medians. The test is repeated with
blocksize - 1 (to check for even/odd errors).
The default values for array length and window
width are 1024 and 512.

\subsubsection*{Exit codes}
\input{LALRunningMedianTestCE}

\subsubsection*{Uses}
\begin{verbatim}
lalDebugLevel
LALPrintError()
LALSCreateVector()
LALSDestroyVector()
LALDCreateVector()
LALDDestroyVector()
LALSRunningMedian()
LALDRunningMedian()
LALCheckMemoryLeaks()
\end{verbatim}

\subsubsection*{Notes}

\vfill{\footnotesize \input{LALRunningMedianTestCV}}
******************************************************* </lalLaTeX> */

#include <stdlib.h>
#include <lal/LALStdlib.h>
#include <lal/LALConstants.h>
#include <lal/LALMalloc.h>
#include <lal/SeqFactories.h>
#include <lal/LALRunningMedian.h>

NRCSID( LALRUNNINGMEDIANTESTC, "$Id$" );

/***************************** <lalErrTable file="LALRunningMedianTestCE"> */
#define LALRUNNINGMEDIANTESTC_ENOM 0
#define LALRUNNINGMEDIANTESTC_EARG 1
#define LALRUNNINGMEDIANTESTC_ESUB 2
#define LALRUNNINGMEDIANTESTC_EALOC 3
#define LALRUNNINGMEDIANTESTC_EFALSE 4
#define LALRUNNINGMEDIANTESTC_EERR 5
#define LALRUNNINGMEDIANTESTC_MSGENOM "Nominal exit"
#define LALRUNNINGMEDIANTESTC_MSGEARG "Error parsing command-line arguments"
#define LALRUNNINGMEDIANTESTC_MSGESUB "Subroutine returned error"
#define LALRUNNINGMEDIANTESTC_MSGEALOC "Could not allocate data space"
#define LALRUNNINGMEDIANTESTC_MSGEFALSE "Medians mismatch"
#define LALRUNNINGMEDIANTESTC_MSGEERR "Subroutine returned wrong or no error"
/***************************** </lalErrTable> */

/* Declare and set the default lalDebugLevel */
int lalDebugLevel = 0; /* LALMEMINFO */

/* global program name */
char*argv0;

/* A local macro for printing error messages */
#define EXIT( code, program, message )                                \
  if ( 1 ) {                                                          \
    if (( lalDebugLevel & LALERROR ) && (code))                       \
      LALPrintError( "Error[0] %d: program %s, file %s, line %d, %s\n"\
                     "        %s\n", (code), (program), __FILE__,     \
                     __LINE__, LALRUNNINGMEDIANTESTC, (message) );    \
    else if ( lalDebugLevel & LALINFO )                               \
      LALPrintError( "Info[0]: program %s, file %s, line %d, %s\n"    \
                     "        %s\n", (program), __FILE__, __LINE__,   \
                     LALRUNNINGMEDIANTESTC, (message) );              \
    return (code);                                                    \
  } else (void)(0)


#define TOLERANCE ( 10.0 * LAL_REAL8_EPS )
#define compare_double( x, y ) \
( ( y == 0 ? ( x == 0 ? 0 : fabs((x-y)/x) ) : fabs((x-y)/y) ) > TOLERANCE )

struct rngmed_val_index {
  REAL8 data;
  UINT8 index;
};

static int rngmed_sortindex(const void *elem1, const void *elem2){
  /*Used in running qsort*/
  
  struct rngmed_val_index *A, *B;
  double data1, data2;
  
  A=(struct rngmed_val_index *)elem1;
  B=(struct rngmed_val_index *)elem2;
  
  data1=A->data;
  data2=B->data;
  if (data1 < data2)
    return -1;
  else if (data1==data2)
    return 0;
  else
    return 1;
  
}


int testDRunningMedian(LALStatus *stat, REAL8Sequence *input, UINT4 length, LALRunningMedianPar param) {
/* Test the LALDRunningMedian (REAL8Sequence) function by
   comparing the reults to individaully calculated medians */

  REAL8 ratio, median;
  REAL8Sequence *medians=NULL;
  struct rngmed_val_index *index_block;
  UINT4 i,k;

  /* call running median */
  LALDRunningMedian( stat, &medians, input, param );
  if ( stat->statusCode ) {
    printf("ERROR: LALRunningMedian returned status %d\n",stat->statusCode);
    EXIT( LALRUNNINGMEDIANTESTC_ESUB, argv0, LALRUNNINGMEDIANTESTC_MSGESUB );
  }

  /* allocate memory for calculating single medians */
  index_block = (struct rngmed_val_index *)LALCalloc(param.blocksize, sizeof(struct rngmed_val_index));
  if(!index_block)
      EXIT( LALRUNNINGMEDIANTESTC_EALOC, argv0, LALRUNNINGMEDIANTESTC_MSGEALOC );

  /* compare all medians */
  for(i=0;i<length-param.blocksize+1;i++) {

    /* prepare array for sort */
    for(k=0;k<param.blocksize;k++){
      index_block[k].data=input->data[k+i];
      index_block[k].index=k;
    }

    /* sort */
    qsort(index_block, param.blocksize, sizeof(struct rngmed_val_index),rngmed_sortindex);

    /* find median */
    if(param.blocksize%2==1)
      median = index_block[(param.blocksize-1)/2].data;
    else
      median = (index_block[param.blocksize/2-1].data+index_block[param.blocksize/2].data)/2;

    /* compare results */
    if(compare_double(median,medians->data[i])) {
      printf("ERROR: index:%d median:% 22.15e running median:% 22.15e mismatch:% 22.15e\n", 
             i, median, medians->data[i], median - medians->data[i]);
      LALFree(index_block);
      LALFree(medians);
      LALFree(input);
      EXIT( LALRUNNINGMEDIANTESTC_EFALSE, argv0, LALRUNNINGMEDIANTESTC_MSGEFALSE );
    }
  }

  LALFree(index_block);
  LALDDestroyVector(stat,&medians);
  if ( stat->statusCode ) {
    printf("ERROR: LALDestroyVector returned status %d\n",stat->statusCode);
    EXIT( LALRUNNINGMEDIANTESTC_ESUB, argv0, LALRUNNINGMEDIANTESTC_MSGESUB );
  }
  return(0);
}




int testSRunningMedian(LALStatus *stat, REAL4Sequence *input, UINT4 length, LALRunningMedianPar param) {
/* Test the LALSRunningMedian (REAL4Sequence) function by
   comparing the reults to individaully calculated medians */

  REAL4 ratio, median;
  REAL4Sequence *medians=NULL;
  struct rngmed_val_index *index_block;
  UINT4 i,k;

  /* call running median */
  LALSRunningMedian( stat, &medians, input, param );
  if ( stat->statusCode ) {
    printf("ERROR: LALRunningMedian returned status %d\n",stat->statusCode);
    EXIT( LALRUNNINGMEDIANTESTC_ESUB, argv0, LALRUNNINGMEDIANTESTC_MSGESUB );
  }

  /* allocate memory for calculating single medians */
  index_block = (struct rngmed_val_index *)LALCalloc(param.blocksize, sizeof(struct rngmed_val_index));
  if(!index_block)
      EXIT( LALRUNNINGMEDIANTESTC_EALOC, argv0, LALRUNNINGMEDIANTESTC_MSGEALOC );

  /* compare all medians */
  for(i=0;i<length-param.blocksize+1;i++) {

    /* prepare array for sort */
    for(k=0;k<param.blocksize;k++){
      index_block[k].data=input->data[k+i];
      index_block[k].index=k;
    }

    /* sort */
    qsort(index_block, param.blocksize, sizeof(struct rngmed_val_index),rngmed_sortindex);

    /* find median */
    if(param.blocksize%2==1)
      median = index_block[(param.blocksize-1)/2].data;
    else
      median = (index_block[param.blocksize/2-1].data+index_block[param.blocksize/2].data)/2;

    /* compare results */
    if(compare_double(median,medians->data[i])) {
      printf("ERROR: index:%d median:%f running median:%f mismatch\n", i, median, medians->data[i]);
      LALFree(index_block);
      LALFree(medians);
      LALFree(input);
      EXIT( LALRUNNINGMEDIANTESTC_EFALSE, argv0, LALRUNNINGMEDIANTESTC_MSGEFALSE );
    }
  }

  LALFree(index_block);
  LALSDestroyVector(stat,&medians);
  if ( stat->statusCode ) {
    printf("ERROR: LALDestroyVector returned status %d\n",stat->statusCode);
    EXIT( LALRUNNINGMEDIANTESTC_ESUB, argv0, LALRUNNINGMEDIANTESTC_MSGESUB );
  }
  return(0);
}





/**************
 **** MAIN ****
 **************/


int main( int argc, char **argv )
{
  LALStatus stat;
  UINT4 blocksize = 512, length = 1024;
  REAL4Sequence *input4=NULL, *medians4=NULL;
  REAL8Sequence *input8=NULL, *medians8=NULL;
  LALRunningMedianPar param;
  UINT4 i;

  /* set global program name */
  argv0 = argv[0];

  /* init status pointer */
  memset(&stat, 0, sizeof(LALStatus));

  /* init param structure */
  memset(&param, 0, sizeof(param));


  /* Parse input line. */
  if ( argc >= 3 ) {
    length = atoi(argv[1]);
    blocksize = atoi(argv[2]);
  }
  if ( argc == 4 )
    lalDebugLevel = atoi( argv[3] );

  if (blocksize <= 3){
    fprintf(stderr,"blocksize must be >3\n");
    EXIT( LALRUNNINGMEDIANTESTC_EARG, argv0, LALRUNNINGMEDIANTESTC_MSGEARG );
  }
  if (blocksize > length){
    fprintf(stderr,"blocksize must be <= length\n");
    EXIT( LALRUNNINGMEDIANTESTC_EARG, argv0, LALRUNNINGMEDIANTESTC_MSGEARG );
  }

  /* create test input */
  LALSCreateVector( &stat, &input4, length );
  if( stat.statusCode )
      EXIT( LALRUNNINGMEDIANTESTC_EALOC, argv0, LALRUNNINGMEDIANTESTC_MSGEALOC );
  LALDCreateVector( &stat, &input8, length );
  if( stat.statusCode )
      EXIT( LALRUNNINGMEDIANTESTC_EALOC, argv0, LALRUNNINGMEDIANTESTC_MSGEALOC );
  for(i=0;i<length;i++)
    input4->data[i] = (input8->data[i] = (double)rand()/(double)RAND_MAX);

  /* test error conditions */

#ifndef LAL_NDEBUG
    if ( ! lalNoDebug )
        {

  /* blocksize = 0 checks */

  memset(&stat, 0, sizeof(LALStatus));
  LALSRunningMedian(&stat,&medians4,input4,param);
  if(stat.statusCode == LALRUNNINGMEDIANH_EZERO)
    printf("  PASS: LALSRunningMedian blocksize =0 results in error\n");
  else
    EXIT( LALRUNNINGMEDIANTESTC_EERR, argv0, LALRUNNINGMEDIANTESTC_MSGEERR );

  memset(&stat, 0, sizeof(LALStatus));
  LALDRunningMedian(&stat,&medians8,input8,param);
  if(stat.statusCode == LALRUNNINGMEDIANH_EZERO)
    printf("  PASS: LALDRunningMedian blocksize =0 results in error\n");
  else
    EXIT( LALRUNNINGMEDIANTESTC_EERR, argv0, LALRUNNINGMEDIANTESTC_MSGEERR );


  /* blocksize = 2 checks */

  param.blocksize = 2;

  memset(&stat, 0, sizeof(LALStatus));
  LALSRunningMedian(&stat,&medians4,input4,param);
  if(stat.statusCode == LALRUNNINGMEDIANH_EZERO)
    printf("  PASS: LALSRunningMedian blocksize =2 results in error\n");
  else
    EXIT( LALRUNNINGMEDIANTESTC_EERR, argv0, LALRUNNINGMEDIANTESTC_MSGEERR );

  memset(&stat, 0, sizeof(LALStatus));
  LALDRunningMedian(&stat,&medians8,input8,param);
  if(stat.statusCode == LALRUNNINGMEDIANH_EZERO)
    printf("  PASS: LALDRunningMedian blocksize =2 results in error\n");
  else
    EXIT( LALRUNNINGMEDIANTESTC_EERR, argv0, LALRUNNINGMEDIANTESTC_MSGEERR );


  /* blocksize too large checks */

  param.blocksize = length+1;

  memset(&stat, 0, sizeof(LALStatus));
  LALSRunningMedian(&stat,&medians4,input4,param);
  if(stat.statusCode == LALRUNNINGMEDIANH_ELARGE)
    printf("  PASS: LALSRunningMedian blocksize > input length results in error\n");
  else
    EXIT( LALRUNNINGMEDIANTESTC_EERR, argv0, LALRUNNINGMEDIANTESTC_MSGEERR );

  memset(&stat, 0, sizeof(LALStatus));
  LALDRunningMedian(&stat,&medians8,input8,param);
  if(stat.statusCode == LALRUNNINGMEDIANH_ELARGE)
    printf("  PASS: LALDRunningMedian blocksize > input length results in error\n");
  else
    EXIT( LALRUNNINGMEDIANTESTC_EERR, argv0, LALRUNNINGMEDIANTESTC_MSGEERR );

        } /* if ( ! lalNoDebug ) */
#endif /* LAL_NDEBUG */


  /* now set the blocksize corretly for the rest of the program */
  param.blocksize = blocksize;

#ifndef LAL_NDEBUG
    if ( ! lalNoDebug )
        {

  /* NULL pointer input check */

  memset(&stat, 0, sizeof(LALStatus));
  LALSRunningMedian(&stat,&medians4,NULL,param);
  if(stat.statusCode == LALRUNNINGMEDIANH_ENULL)
    printf("  PASS: LALSRunningMedian NULL input results in error\n");
  else
    EXIT( LALRUNNINGMEDIANTESTC_EERR, argv0, LALRUNNINGMEDIANTESTC_MSGEERR );
  
  memset(&stat, 0, sizeof(LALStatus));
  LALDRunningMedian(&stat,&medians8,NULL,param);
  if(stat.statusCode == LALRUNNINGMEDIANH_ENULL)
    printf("  PASS: LALDRunningMedian NULL input results in error\n");
  else
    EXIT( LALRUNNINGMEDIANTESTC_EERR, argv0, LALRUNNINGMEDIANTESTC_MSGEERR );

  /* NULL pointer median check */

  memset(&stat, 0, sizeof(LALStatus));
  LALSRunningMedian(&stat,NULL,input4,param);
  if(stat.statusCode == LALRUNNINGMEDIANH_EIMED)
    printf("  PASS: LALSRunningMedian median != &NULL results in error\n");
  else
    EXIT( LALRUNNINGMEDIANTESTC_EERR, argv0, LALRUNNINGMEDIANTESTC_MSGEERR );

  memset(&stat, 0, sizeof(LALStatus));
  LALDRunningMedian(&stat,NULL,input8,param);
  if(stat.statusCode == LALRUNNINGMEDIANH_EIMED)
    printf("  PASS: LALDRunningMedian median != &NULL results in error\n");
  else
    EXIT( LALRUNNINGMEDIANTESTC_EERR, argv0, LALRUNNINGMEDIANTESTC_MSGEERR );

        } /* if ( ! lalNoDebug ) */
#endif /* LAL_NDEBUG */

  /* finaly restore status after checking error conditions */
  memset(&stat, 0, sizeof(LALStatus));


  /* test normal operation */

  if(testDRunningMedian(&stat,input8,length,param))
    EXIT( LALRUNNINGMEDIANTESTC_EFALSE, argv0, LALRUNNINGMEDIANTESTC_MSGEFALSE );
  else
    printf("  PASS: LALDRunningMedian(%d,%d)\n",length,param.blocksize);

  if(testSRunningMedian(&stat,input4,length,param))
    EXIT( LALRUNNINGMEDIANTESTC_EFALSE, argv0, LALRUNNINGMEDIANTESTC_MSGEFALSE );
  else
    printf("  PASS: LALSRunningMedian(%d,%d)\n",length,param.blocksize);

  /* decrement the blocksize for the next two test to check for even/odd errors */
  param.blocksize--;

  if(testDRunningMedian(&stat,input8,length,param))
    EXIT( LALRUNNINGMEDIANTESTC_EFALSE, argv0, LALRUNNINGMEDIANTESTC_MSGEFALSE );
  else
    printf("  PASS: LALDRunningMedian(%d,%d)\n",length,param.blocksize);

  if(testSRunningMedian(&stat,input4,length,param))
    EXIT( LALRUNNINGMEDIANTESTC_EFALSE, argv0, LALRUNNINGMEDIANTESTC_MSGEFALSE );
  else
    printf("  PASS: LALSRunningMedian(%d,%d)\n",length,param.blocksize);


  /* free dummy input memory */
  LALDDestroyVector(&stat,&input8);
  LALSDestroyVector(&stat,&input4);

  /* check for memory leaks */
  LALCheckMemoryLeaks();

  /* report status if wanted */
  if(lalDebugLevel)
    REPORTSTATUS(&stat);
  
  /* nominal exit */
  EXIT( LALRUNNINGMEDIANTESTC_ENOM, argv0, LALRUNNINGMEDIANTESTC_MSGENOM );
}
