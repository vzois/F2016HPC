#include "time/Time.h"
#include "tools/Utils.h"
#include "tools/ArgParser.h"

void initMatrix(double *&A, double *&B, unsigned int N){
	Utils<double> u;
	for(unsigned int i = 0 ; i < N*N;i++){
		A[i] = i+1;
		B[i] = i + 1;
	}
}

void zeros(double *&C, unsigned int N){
	for(unsigned int i = 0 ; i < N*N;i++){
		C[i] = 0;
	}
}

void dgemm0(const double *A, const double *B, double *&C, unsigned int N){
	for(unsigned int i = 0; i < N ;i++){
		for(unsigned int j = 0; j < N ;j++){
			for(unsigned int k = 0; k < N ;k++){
				C[i * N + j] += A[ i * N + k ] * B[ k * N + j ];
			}
		}
	}

}

void dgemm1(const double *A, const double *B, double *&C, unsigned int N){
	for(unsigned int i = 0; i < N ;i++){
		for(unsigned int j = 0; j < N ;j++){
			register double rC = 0;
			for(unsigned int k = 0; k < N ;k++){
				rC += A[ i * N + k ] * B[ k * N + j ];
				//std::cout << A[ i * N + k ] <<","<< B[ k * N + j ]<< std::endl;
			}
			//std::cout<<std::endl;
			C[ i * N + j ] = rC;
		}
	}
}

void dgemm2(const double *A, const double *B, double *&C, unsigned int N){

	for(unsigned int i = 0; i < N ;i+=2){
		for(unsigned int j = 0; j < N ;j+=2){
			register int iC = i * N + j; register int iiC = iC + N;
			register double rC00 = 0.0f;
			register double rC01 = 0.0f;
			register double rC10 = 0.0f;
			register double rC11 = 0.0f;

			for(unsigned int k = 0; k < N ;k+=2){
				register int iA = i * N + k; register int iiA = iA + N;
				register int iB = k * N + j; register int iiB = iB + N;

				register double rA00 = A[ iA ]; register double rA01 = A[ iA + 1 ];
				register double rA10 = A[ iiA ]; register double rA11 = A[ iiA + 1 ];
				register double rB00 = B[ iB ]; register double rB01 = B[ iB + 1 ];
				register double rB10 = B[ iiB ]; register double rB11 = B[ iiB + 1 ];

				rC00 += rA00 * rB00 + rA01 * rB10;
				rC01 += rA00 * rB01 + rA01 * rB11;
				rC10 += rA10 * rB00 + rA11 * rB10;
				rC11 += rA10 * rB01 + rA11 * rB11;
			}
			C[ iC ] = rC00;
			C[ iC + 1 ] = rC01;
			C[ iiC ] = rC10;
			C[ iiC + 1 ] = rC11;
		}
	}
}

void dgemm3(const double *A, const double *B, double *&C, unsigned int N){
	for(unsigned int i = 0; i < N ;i+=2){
		for(unsigned int j = 0; j < N ;j+=4){
			register int iC = i * N + j; register int iiC = iC + N;

			register double rC00 = 0.0f;
			register double rC01 = 0.0f;
			register double rC10 = 0.0f;
			register double rC11 = 0.0f;
			register double rC02 = 0.0f;
			register double rC12 = 0.0f;
			register double rC03 = 0.0f;
			register double rC13 = 0.0f;



			for(unsigned int k = 0; k < N ;k+=2){
				register int iA = i * N + k; register int iiA = iA + N;
				register int iB = k * N + j; register int iiB = iB + N;

				register double rA0 = A[iA]; register double rA1 = A[iiA];
				register double rB0 = B[iB]; register double rB1 = B[iB + 1]; register double rB2 = B[iB + 2]; register double rB2 = B[iB + 3];

				rC00 += rA0 * rB0;
				rC01 += rA0 * rB1;
				rC02 += rA0 * rB2;
				rC03 += rA0 * rB3;

				rC10 += rA1 * rB0;
				rC11 += rA1 * rB1;
				rC12 += rA1 * rB2;
				rC13 += rA1 * rB3;

				rA0 = A[iA+1]; rA1 = A[iiA+1];
				rB0 = B[iiB]; rB1 = B[iiB + 1];

				rC00 += rA0 * rB0; rC01 += rA0 * rB1; rC10 += rA1 * rB0; rC11 += rA1 * rB1;
			}

			C[ iC ] = rC00;
			C[ iC + 1 ] = rC01;
			C[ iC + 2 ] = rC02;
			C[ iC + 3 ] = rC03;

			C[ iiC ] = rC10;
			C[ iiC + 1 ] = rC11;
			C[ iiC + 2 ] = rC12;
			C[ iiC + 3 ] = rC13;
		}
	}
}

int cmpResults(double *C, double *D, unsigned int N){
	for(unsigned int i = 0; i <N; i++){
		if (D[i] != C[i]){
			std::cout<< "different results error: " <<i << "," << C[i] << "," << D[i] << std::endl;
			return 1;
		}
	}
	return 0;
}

int main(int argc, char **argv){
	ArgParser ap;
	ap.parseArgs(argc,argv);

	double *A,*B,*C, *D;
	int N = 0 ;


	if ( ap.count() == 0 ){
		std::cout<< "Please provide matrix size argument: dgemm -n=64" << std::endl;
		return 1;
	}else{
		if( !ap.exists("-n") ){
			std::cout<< "Execute the program using -n for the matrix size: ./dgemm -n=64" << std::endl;
			return 1;
		}
		N = ap.getInt("-n");
	}

	std::cout << "Matrix dimensions (" << N << "x" << N << ")" << std::endl;

	A = new double[N * N];
	B = new double[N * N];
	C = new double[N * N];
	D = new double[N * N];

	initMatrix(A,B,N);
	zeros(C,N);
	zeros(D,N);
	Time<secs> t;

	t.start();
	dgemm0(A,B,C,N);
	double d0 = t.lap("Elapsed time for dgemm0 in secs");

	t.start();
	dgemm1(A,B,D,N);
	double d1 = t.lap("Elapsed time for dgemm1 in secs");
	if(cmpResults(C,D,N) != 0 ) return 1;

	zeros(C,N);
	t.start();
	dgemm2(A,B,C,N);
	double d2 = t.lap("Elapsed time for dgemm2 in secs");
	if(cmpResults(C,D,N) != 0 ) return 1;


	uint64_t fop = N;
	fop *=fop*fop*2;
	std::cout << "Double precision floating point operations: " << fop << std::endl;
	d0 = (fop / d0)/1000000000;
	std::cout << "Dgemm0 GFLOPS: " << d0 << std::endl;
	d1 = (fop / d1)/1000000000;
	std::cout << "Dgemm1 GFLOPS: " << d1 << std::endl;
	d2 = (fop / d2)/1000000000;
	std::cout << "Dgemm2 GFLOPS: " << d2 << std::endl;

	delete A;
	delete B;
	delete C;
	delete D;


	return 0;

}
