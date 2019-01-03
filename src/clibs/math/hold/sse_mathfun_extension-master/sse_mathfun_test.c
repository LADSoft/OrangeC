/*
Written by Tolga Mizrak 2016
Modification of sse_mathfun_test.c, written by Julien Pommier
*/

/*
Results on a 3.30 GHz AMD FX-6100 Six-Core, compiled with Visual C++ Enterprise 2015 Update 3 (x64)
command line: cl.exe /W4 /DUSE_SSE2 /EHsc- /MD /GS- /Gy /fp:fast /Ox /Oy- /GL /Oi /O2 sse_mathfun_test.c

checking sines on [0*Pi, 1*Pi]
max deviation from sinf(x): 5.96046e-08 at 0.193304238313*Pi, max deviation from cephes_sin(x): 0
max deviation from cosf(x): 5.96046e-08 at 0.303994872157*Pi, max deviation from cephes_cos(x): 0
deviation of sin(x)^2+cos(x)^2-1: 1.78814e-07 (ref deviation is 1.19209e-07)
   ->> precision OK for the sin_ps / cos_ps / sincos_ps <<-

checking sines on [-1000*Pi, 1000*Pi]
max deviation from sinf(x): 5.96046e-08 at  338.694424873*Pi, max deviation from cephes_sin(x): 0
max deviation from cosf(x): 5.96046e-08 at  338.694424873*Pi, max deviation from cephes_cos(x): 0
deviation of sin(x)^2+cos(x)^2-1: 1.78814e-07 (ref deviation is 1.19209e-07)
   ->> precision OK for the sin_ps / cos_ps / sincos_ps <<-

checking exp/log [-60, 60]
max (relative) deviation from expf(x): 1.18944e-07 at -56.8358421326, max deviation from cephes_expf(x): 0
max (absolute) deviation from logf(x): 1.19209e-07 at -1.67546617985, max deviation from cephes_logf(x): 0
deviation of x - log(exp(x)): 1.19209e-07 (ref deviation is 5.96046e-08)
   ->> precision OK for the exp_ps / log_ps <<-

checking tan on [-0.25*Pi, 0.25*Pi]
max deviation from tanf(x): 1.19209e-07 at 0.250000006957*Pi, max deviation from cephes_tan(x): 5.96046e-08
   ->> precision OK for the tan_ps <<-

checking tan on [-0.49*Pi, 0.49*Pi]
max deviation from tanf(x): 3.8147e-06 at -0.490000009841*Pi, max deviation from cephes_tan(x): 9.53674e-07
   ->> precision OK for the tan_ps <<-

checking cot on [0.2*Pi, 0.7*Pi]
max deviation from cotf(x): 1.19209e-07 at 0.204303119606*Pi, max deviation from cephes_cot(x): 1.19209e-07
   ->> precision OK for the cot_ps <<-

checking cot on [0.01*Pi, 0.99*Pi]
max deviation from cotf(x): 3.8147e-06 at 0.987876517942*Pi, max deviation from cephes_cot(x): 9.53674e-07
   ->> precision OK for the cot_ps <<-

checking atan on [-10*Pi, 10*Pi]
max deviation from atanf(x): 1.19209e-07 at -9.39207109497*Pi, max deviation from cephes_atan(x): 1.19209e-07
   ->> precision OK for the atan_ps <<-

checking atan on [-10000*Pi, 10000*Pi]
max deviation from atanf(x): 1.19209e-07 at  -7350.3826719*Pi, max deviation from cephes_atan(x): 1.19209e-07
   ->> precision OK for the atan_ps <<-

checking atan2 on [-1*Pi, 1*Pi]
max deviation from atan2f(x): 2.38419e-07 at (0.797784384786*Pi, -0.913876806545*Pi), max deviation from cephes_atan2(x): 2.38419e-07
   ->> precision OK for the atan2_ps <<-

checking atan2 on [-10000*Pi, 10000*Pi]
max deviation from atan2f(x): 2.38419e-07 at ( 658.284195009*Pi, -2685.93394561*Pi), max deviation from cephes_atan2(x): 2.38419e-07
   ->> precision OK for the atan2_ps <<-

exp([        -1000,          -100,           100,          1000]) = [            0,             0, 2.4061436e+38, 2.4061436e+38]
exp([    -nan(ind),           inf,          -inf,           nan]) = [2.4061436e+38, 2.4061436e+38,             0, 2.4061436e+38]
log([            0,           -10,         1e+30, 1.0005271e-42]) = [         -nan,          -nan,     69.077553,    -87.336548]
log([    -nan(ind),           inf,          -inf,           nan]) = [   -87.336548,     88.722839,          -nan,    -87.336548]
sin([    -nan(ind),           inf,          -inf,           nan]) = [    -nan(ind),     -nan(ind),           nan,           nan]
cos([    -nan(ind),           inf,          -inf,           nan]) = [          nan,     -nan(ind),     -nan(ind),           nan]
sin([       -1e+30,       -100000,         1e+30,        100000]) = [          inf,  -0.035749275,          -inf,   0.035749275]
cos([       -1e+30,       -100000,         1e+30,        100000]) = [    -nan(ind),    -0.9993608,     -nan(ind),    -0.9993608]
benching                 sinf .. ->   16.3 millions of vector evaluations/second ->  40 cycles/value on a 2600MHz computer
benching                 cosf .. ->   15.8 millions of vector evaluations/second ->  41 cycles/value on a 2600MHz computer
benching                 expf .. ->   18.8 millions of vector evaluations/second ->  35 cycles/value on a 2600MHz computer
benching                 logf .. ->   17.8 millions of vector evaluations/second ->  36 cycles/value on a 2600MHz computer
benching                 tanf .. ->   13.8 millions of vector evaluations/second ->  47 cycles/value on a 2600MHz computer
benching                 cotf .. ->   12.2 millions of vector evaluations/second ->  53 cycles/value on a 2600MHz computer
benching                atanf .. ->   10.4 millions of vector evaluations/second ->  62 cycles/value on a 2600MHz computer
benching               atan2f .. ->    5.3 millions of vector evaluations/second -> 121 cycles/value on a 2600MHz computer
benching            atan2_ref .. ->   11.7 millions of vector evaluations/second ->  56 cycles/value on a 2600MHz computer
benching                sqrtf .. ->   69.5 millions of vector evaluations/second ->   9 cycles/value on a 2600MHz computer
benching               rsqrtf .. ->   70.2 millions of vector evaluations/second ->   9 cycles/value on a 2600MHz computer
benching          cephes_sinf .. ->   15.2 millions of vector evaluations/second ->  43 cycles/value on a 2600MHz computer
benching          cephes_cosf .. ->   16.6 millions of vector evaluations/second ->  39 cycles/value on a 2600MHz computer
benching          cephes_expf .. ->    2.9 millions of vector evaluations/second -> 220 cycles/value on a 2600MHz computer
benching          cephes_logf .. ->    3.4 millions of vector evaluations/second -> 186 cycles/value on a 2600MHz computer
benching               sin_ps .. ->   30.6 millions of vector evaluations/second ->  21 cycles/value on a 2600MHz computer
benching               cos_ps .. ->   31.1 millions of vector evaluations/second ->  21 cycles/value on a 2600MHz computer
benching            sincos_ps .. ->   30.9 millions of vector evaluations/second ->  21 cycles/value on a 2600MHz computer
benching               exp_ps .. ->   27.3 millions of vector evaluations/second ->  24 cycles/value on a 2600MHz computer
benching               log_ps .. ->   23.5 millions of vector evaluations/second ->  28 cycles/value on a 2600MHz computer
benching               tan_ps .. ->   22.2 millions of vector evaluations/second ->  29 cycles/value on a 2600MHz computer
benching               cot_ps .. ->   22.0 millions of vector evaluations/second ->  29 cycles/value on a 2600MHz computer
benching              atan_ps .. ->   31.1 millions of vector evaluations/second ->  21 cycles/value on a 2600MHz computer
benching             atan2_ps .. ->   24.1 millions of vector evaluations/second ->  27 cycles/value on a 2600MHz computer
benching              sqrt_ps .. ->   63.9 millions of vector evaluations/second ->  10 cycles/value on a 2600MHz computer
benching             rsqrt_ps .. ->   64.1 millions of vector evaluations/second ->  10 cycles/value on a 2600MHz computer
*/

#ifdef _MSC_VER
	// warning C4305: 'initializing': truncation from 'double' to 'const float'
	#pragma warning( disable : 4305 )
	// warning C4244: 'return': conversion from 'double' to 'int', possible loss of data
	#pragma warning( disable : 4244 )
#endif

#include <stdio.h>
#include <xmmintrin.h>

/* useful when debuggin.. */
void print4( __m128 v )
{
	float *p = (float *)&v;
#ifndef USE_SSE2
	_mm_empty();
#endif
	printf( "[%13.8g, %13.8g, %13.8g, %13.8g]", p[0], p[1], p[2], p[3] );
}

void print2i( __m64 v )
{
	unsigned *p = (unsigned *)&v;
	printf( "[%08x %08x]", p[0], p[1] );
}

#ifdef USE_SSE2
#include <emmintrin.h>
void print4i( __m128i v )
{
	unsigned *p = (unsigned *)&v;
	printf( "[%08x %08x %08x %08x]", p[0], p[1], p[2], p[3] );
}
#endif

#include "sse_mathfun_extension.h"
#include <math.h>
#include <stdlib.h>
#include <time.h>
#include <assert.h>

#ifdef HAVE_SYS_TIMES
#include <sys/times.h>
#include <unistd.h>
#endif

#ifdef HAVE_VECLIB
#include <vecLib/vfp.h>
#endif
#ifdef HAVE_ACML
#include <acml_mv_m128.h>
#endif

typedef ALIGN16_BEG union {
	float f[4];
	int i[4];
	v4sf v;
} ALIGN16_END V4SF;

#define MAX( a, b ) ( ( ( a ) > ( b ) ) ? ( a ) : ( b ) )

double frand() { return rand() / (double)RAND_MAX; }

#if defined( HAVE_SYS_TIMES )
inline double uclock_sec( void )
{
	static double ttclk = 0.;
	if( ttclk == 0. ) ttclk = sysconf( _SC_CLK_TCK );
	struct tms t;
	return ( (double)times( &t ) ) / ttclk;
}
#else

#if 0
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

double freq;
void initTimer()
{
  LARGE_INTEGER li;
  QueryPerformanceFrequency( &li );
  freq = (double)( li.QuadPart );
}
double getTime()
{
  LARGE_INTEGER li;
  QueryPerformanceCounter( &li );
  return (double)( li.QuadPart ) / freq;
}

  inline double uclock_sec(void)
{ return getTime(); }
#else
inline double uclock_sec( void ) { return (double)clock() / (double)CLOCKS_PER_SEC; }
#endif
#endif

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

#ifndef M_LN2
#define M_LN2 0.69314718055994530942
#endif

int bitdiff( float a, float b )
{
	if( a == b )
		return 24;
	else if( a == 0 ) {
		int j = -log( fabs( b ) ) / M_LN2;
		if( j > 24 ) j = 24;
		return j;
	} else
		return log( fabs( a ) ) / M_LN2 - log( fabs( b - a ) ) / M_LN2;
}

/* they are defined at the bottom of the file */
float cephes_sinf( float );
float cephes_cosf( float );
float cephes_logf( float );
float cephes_expf( float );

float cephes_tanf( float );
float cephes_cotf( float );
float cephes_atanf( float );
float cephes_atan2f( float, float );

int check_sincos_precision( float xmin, float xmax )
{
	unsigned nb_trials = 100000;
	printf( "checking sines on [%g*Pi, %g*Pi]\n", xmin, xmax );

	float max_err_sin_ref = 0, max_err_sin_cep = 0, max_err_sin_x = 0;
	float max_err_cos_ref = 0, max_err_cos_cep = 0, max_err_cos_x = 0;
	float max_err_sum_sqr_test = 0;
	float max_err_sum_sqr_ref = 0;
	xmin *= M_PI;
	xmax *= M_PI;
	unsigned i;
	for( i = 0; i < nb_trials; ++i ) {
		V4SF vx, sin4, cos4, sin4_2, cos4_2;
		vx.f[0] = i * ( xmax - xmin ) / ( nb_trials - 1 ) + xmin;
		vx.f[1] = ( i + .5 ) * ( xmax - xmin ) / ( nb_trials - 1 ) + xmin;
		vx.f[2] = frand() * ( xmax - xmin );
		vx.f[3] = ( i / 32 ) * M_PI / ( ( i % 32 ) + 1 );
		if( vx.f[3] < xmin || vx.f[3] > xmax ) vx.f[3] = frand() * ( xmax - xmin );

		/*
		vx.f[0] = M_PI/2;
		vx.f[1] = M_PI;
		vx.f[2] = M_PI/3;
		vx.f[3] = M_PI/4;
		*/
		sin4.v = sin_ps( vx.v );
		cos4.v = cos_ps( vx.v );
		sincos_ps( vx.v, &sin4_2.v, &cos4_2.v );
		unsigned j;
		for( j = 0; j < 4; ++j ) {
			float x = vx.f[j];
			float sin_test = sin4.f[j];
			float cos_test = cos4.f[j];
			if( sin_test != sin4_2.f[j] ) {
				printf( "sin / sincos mismatch at x=%g\n", x );
				exit( 1 );
			}
			if( cos_test != cos4_2.f[j] ) {
				printf( "cos / sincos mismatch at x=%g\n", x );
				return 1;
			}
			float sin_ref = sinf( x );
			float sin_cep = cephes_sinf( x );
			float err_sin_ref = fabs( sin_ref - sin_test );
			float err_sin_cep = fabs( sin_cep - sin_test );
			if( err_sin_ref > max_err_sin_ref ) {
				max_err_sin_ref = err_sin_ref;
				max_err_sin_x = x;
			}
			max_err_sin_cep = MAX( max_err_sin_cep, err_sin_cep );
			float cos_ref = cosf( x );
			float cos_cep = cephes_cosf( x );
			float err_cos_ref = fabs( cos_ref - cos_test );
			float err_cos_cep = fabs( cos_cep - cos_test );
			if( err_cos_ref > max_err_cos_ref ) {
				max_err_cos_ref = err_cos_ref;
				max_err_cos_x = x;
			}
			max_err_cos_cep = MAX( max_err_cos_cep, err_cos_cep );
			float err_sum_sqr_test = fabs( 1 - cos_test * cos_test - sin_test * sin_test );
			float err_sum_sqr_ref = fabs( 1 - cos_ref * cos_ref - sin_ref * sin_ref );
			max_err_sum_sqr_ref = MAX( max_err_sum_sqr_ref, err_sum_sqr_ref );
			max_err_sum_sqr_test = MAX( max_err_sum_sqr_test, err_sum_sqr_test );
			// printf("sin(%g) = %g %g err=%g\n", x, sin_ref, sin_test, err_sin_ref);
		}
	}
	printf( "max deviation from sinf(x): %g at %14.12g*Pi, max deviation from cephes_sin(x): %g\n",
			max_err_sin_ref, max_err_sin_x / M_PI, max_err_sin_cep );
	printf( "max deviation from cosf(x): %g at %14.12g*Pi, max deviation from cephes_cos(x): %g\n",
			max_err_cos_ref, max_err_cos_x / M_PI, max_err_cos_cep );

	printf( "deviation of sin(x)^2+cos(x)^2-1: %g (ref deviation is %g)\n", max_err_sum_sqr_test,
			max_err_sum_sqr_ref );

	if( max_err_sum_sqr_ref < 2e-7 && max_err_sin_ref < 2e-7 && max_err_cos_ref < 2e-7 ) {
		printf( "   ->> precision OK for the sin_ps / cos_ps / sincos_ps <<-\n\n" );
		return 0;
	} else {
		printf( "\n   WRONG PRECISION !! there is a problem\n\n" );
		return 1;
	}
}

int check_precision( float xmin, float xmax, const char *name, v4sf ( *fn )( v4sf ),
					 float ( *ref_clib )( float ), float ( *ref_cephes )( float ),
					 int laxPrecision )
{
	unsigned nb_trials = 100000;
	printf( "checking %s on [%g*Pi, %g*Pi]\n", name, xmin, xmax );

	float max_err_ref = 0, max_err_cep = 0, max_err_x = 0;
	xmin *= M_PI;
	xmax *= M_PI;
	unsigned i;
	for( i = 0; i < nb_trials; ++i ) {
		V4SF vx, result;
		vx.f[0] = frand() * ( xmax - xmin ) + xmin;
		vx.f[1] = frand() * ( xmax - xmin ) + xmin;
		vx.f[2] = frand() * ( xmax - xmin ) + xmin;
		vx.f[3] = frand() * ( xmax - xmin ) + xmin;

		result.v = fn( vx.v );
		unsigned j;
		for( j = 0; j < 4; ++j ) {
			float x = vx.f[j];
			float result_test = result.f[j];

			float ref = ref_clib( x );
			float ref_cep = ref_cephes( x );
			float err_ref = fabs( ref - result_test );
			float err_ref_cep = fabs( ref_cep - result_test );
			if( err_ref > max_err_ref ) {
				max_err_ref = err_ref;
				max_err_x = x;
			}
			max_err_ref = MAX( max_err_ref, err_ref );
			max_err_cep = MAX( max_err_cep, err_ref_cep );
		}
	}
	printf( "max deviation from %sf(x): %g at %14.12g*Pi, max deviation from cephes_%s(x): %g\n",
			name, max_err_ref, max_err_x / M_PI, name, max_err_cep );

	if( ( !laxPrecision && max_err_ref < 2e-7 ) || ( laxPrecision && max_err_ref < 4e-6 ) ) {
		printf( "   ->> precision OK for the %s_ps <<-\n\n", name );
		return 0;
	} else {
		printf( "\n   WRONG PRECISION !! there is a problem\n\n" );
		return 1;
	}
}
int check_precision2( float xmin, float xmax, const char *name, v4sf ( *fn )( v4sf, v4sf ),
					  float ( *ref_clib )( float, float ), float ( *ref_cephes )( float, float ),
					  int laxPrecision )
{
	unsigned nb_trials = 100000;
	printf( "checking %s on [%g*Pi, %g*Pi]\n", name, xmin, xmax );

	float max_err_ref = 0, max_err_cep = 0, max_err_x = 0, max_err_y = 0;
	xmin *= M_PI;
	xmax *= M_PI;
	unsigned i;
	for( i = 0; i < nb_trials; ++i ) {
		V4SF vx, vy, result;
		vx.f[0] = frand() * ( xmax - xmin ) + xmin;
		vx.f[1] = frand() * ( xmax - xmin ) + xmin;
		vx.f[2] = frand() * ( xmax - xmin ) + xmin;
		vx.f[3] = frand() * ( xmax - xmin ) + xmin;

		vy.f[0] = frand() * ( xmax - xmin ) + xmin;
		vy.f[1] = frand() * ( xmax - xmin ) + xmin;
		vy.f[2] = frand() * ( xmax - xmin ) + xmin;
		vy.f[3] = frand() * ( xmax - xmin ) + xmin;

		result.v = fn( vx.v, vy.v );
		unsigned j;
		for( j = 0; j < 4; ++j ) {
			float x = vx.f[j];
			float y = vy.f[j];
			float result_test = result.f[j];

			float ref = ref_clib( x, y );
			float ref_cep = ref_cephes( x, y );
			float err_ref = fabs( ref - result_test );
			float err_ref_cep = fabs( ref_cep - result_test );
			if( err_ref > max_err_ref ) {
				max_err_ref = err_ref;
				max_err_x = x;
				max_err_y = y;
			}
			max_err_ref = MAX( max_err_ref, err_ref );
			max_err_cep = MAX( max_err_cep, err_ref_cep );
		}
	}
	printf(
		"max deviation from %sf(x): %g at (%14.12g*Pi, %14.12g*Pi), max deviation "
		"from cephes_%s(x): %g\n",
		name, max_err_ref, max_err_x / M_PI, max_err_y / M_PI, name, max_err_cep );

	if( ( !laxPrecision && max_err_ref < 3e-7 ) || ( laxPrecision && max_err_ref < 4e-6 ) ) {
		printf( "   ->> precision OK for the %s_ps <<-\n\n", name );
		return 0;
	} else {
		printf( "\n   WRONG PRECISION !! there is a problem\n\n" );
		return 1;
	}
}

union float_int_union {
	unsigned int i;
	float f;
} QNAN = {0xFFC00000}, QNAN2 = {0x7FC00000}, PINF = {0x7F800000}, MINF = {0xFF800000};

int check_explog_precision( float xmin, float xmax )
{
	unsigned nb_trials = 100000;
	printf( "checking exp/log [%g, %g]\n", xmin, xmax );

	float max_err_exp_ref = 0, max_err_exp_cep = 0, max_err_exp_x = 0;
	float max_err_log_ref = 0, max_err_log_cep = 0, max_err_log_x = 0;
	float max_err_logexp_test = 0;
	float max_err_logexp_ref = 0;
	unsigned i;
	for( i = 0; i < nb_trials; ++i ) {
		V4SF vx, exp4, log4;
		vx.f[0] = frand() * ( xmax - xmin ) + xmin;
		vx.f[1] = frand() * ( xmax - xmin ) + xmin;
		vx.f[2] = frand() * ( xmax - xmin ) + xmin;
		vx.f[3] = frand() * ( xmax - xmin ) + xmin;
		exp4.v = exp_ps( vx.v );
		log4.v = log_ps( exp4.v );
		unsigned j;
		for( j = 0; j < 4; ++j ) {
			float x = vx.f[j];
			float exp_test = exp4.f[j];
			float log_test = log4.f[j];
			float exp_ref = expf( x );
			float exp_cep = cephes_expf( x );
			float err_exp_ref = fabs( exp_ref - exp_test ) / exp_ref;
			float err_exp_cep = fabs( exp_cep - exp_test ) / exp_ref;
			if( err_exp_ref > max_err_exp_ref ) {
				max_err_exp_ref = err_exp_ref;
				max_err_exp_x = x;
			}
			max_err_exp_cep = MAX( max_err_exp_cep, err_exp_cep );

			float log_ref = logf( exp_test );
			float log_cep = cephes_logf( exp_test );
			float err_log_ref = fabs( log_ref - log_test );
			float err_log_cep = fabs( log_cep - log_test );
			if( err_log_ref > max_err_log_ref ) {
				max_err_log_ref = err_log_ref;
				max_err_log_x = x;
			}
			max_err_log_cep = MAX( max_err_log_cep, err_log_cep );
			float err_logexp_test = fabs( x - log_test );
			float err_logexp_ref = fabs( x - logf( expf( x ) ) );
			max_err_logexp_ref = MAX( max_err_logexp_ref, err_logexp_ref );
			max_err_logexp_test = MAX( max_err_logexp_test, err_logexp_test );
		}
	}
	printf(
		"max (relative) deviation from expf(x): %g at %14.12g, max deviation from cephes_expf(x): "
		"%g\n",
		max_err_exp_ref, max_err_exp_x, max_err_exp_cep );
	printf(
		"max (absolute) deviation from logf(x): %g at %14.12g, max deviation from cephes_logf(x): "
		"%g\n",
		max_err_log_ref, max_err_log_x, max_err_log_cep );

	printf( "deviation of x - log(exp(x)): %g (ref deviation is %g)\n", max_err_logexp_test,
			max_err_logexp_ref );

	if( max_err_logexp_test < 2e-7 && max_err_exp_ref < 2e-7 && max_err_log_ref < 2e-7 ) {
		printf( "   ->> precision OK for the exp_ps / log_ps <<-\n\n" );
		return 0;
	} else {
		printf( "\n   WRONG PRECISION !! there is a problem\n\n" );
		return 1;
	}
}

void dumb()
{
	V4SF x = {{0.0903333798051, 0.0903333798051, 0.0903333798051, 0.0903333798051}};
	V4SF w;
	w.v = log_ps( x.v );
	float z = cephes_logf( x.f[0] );
	printf( "log_ps returned " );
	print4( w.v );
	printf( "\ncephes returned: %14.12g and logf(%g)=%14.12g\n", z, x.f[0], logf( x.f[0] ) );

	print4( _mm_cmpeq_ps( x.v, x.v ) );
	printf( "\n" );
	exit( 1 );
}

void check_special_values()
{
	V4SF vx;
	vx.f[0] = -1000;
	vx.f[1] = -100;
	vx.f[2] = 100;
	vx.f[3] = 1000;
	printf( "exp(" );
	print4( vx.v );
	printf( ") = " );
	print4( exp_ps( vx.v ) );
	printf( "\n" );
	vx.f[0] = QNAN.f;
	vx.f[1] = PINF.f;
	vx.f[2] = MINF.f;
	vx.f[3] = QNAN2.f;
	printf( "exp(" );
	print4( vx.v );
	printf( ") = " );
	print4( exp_ps( vx.v ) );
	printf( "\n" );
	vx.f[0] = 0;
	vx.f[1] = -10;
	vx.f[2] = 1e30f;
	vx.f[3] = 1e-42f;
	printf( "log(" );
	print4( vx.v );
	printf( ") = " );
	print4( log_ps( vx.v ) );
	printf( "\n" );
	vx.f[0] = QNAN.f;
	vx.f[1] = PINF.f;
	vx.f[2] = MINF.f;
	vx.f[3] = QNAN2.f;
	printf( "log(" );
	print4( vx.v );
	printf( ") = " );
	print4( log_ps( vx.v ) );
	printf( "\n" );
	printf( "sin(" );
	print4( vx.v );
	printf( ") = " );
	print4( sin_ps( vx.v ) );
	printf( "\n" );
	printf( "cos(" );
	print4( vx.v );
	printf( ") = " );
	print4( cos_ps( vx.v ) );
	printf( "\n" );
	vx.f[0] = -1e30;
	vx.f[1] = -100000;
	vx.f[2] = 1e30;
	vx.f[3] = 100000;
	printf( "sin(" );
	print4( vx.v );
	printf( ") = " );
	print4( sin_ps( vx.v ) );
	printf( "\n" );
	printf( "cos(" );
	print4( vx.v );
	printf( ") = " );
	print4( cos_ps( vx.v ) );
	printf( "\n" );
}

#define DECL_SCALAR_FN_BENCH( fn )            \
	int bench_##fn()                          \
	{                                         \
		int niter = 10000, i, j;              \
		float x = 0.5f, y = 0;                \
		for( i = 0; i < niter; ++i ) {        \
			for( j = 0; j < 4; ++j ) {        \
				x += 1e-6f;                   \
				y += fn( x + 5 * ( j & 1 ) ); \
			}                                 \
		}                                     \
		if( y == 2.32132323232f ) niter--;    \
		return niter;                         \
	}

#define DECL_VECTOR_FN_BENCH( fn )                                 \
	int bench_##fn()                                               \
	{                                                              \
		int niter = 10000, i;                                      \
		v4sf bmin = _mm_set_ps1( 0.5 ), bmax = _mm_set_ps1( 1.0 ); \
		v4sf x = _mm_set_ps1( 0.75 );                              \
		for( i = 0; i < niter; ++i ) {                             \
			x = fn( x );                                           \
			x = _mm_min_ps( x, bmax );                             \
			x = _mm_max_ps( x, bmin );                             \
		}                                                          \
		if( ( (float *)&x )[0] == 2.32132323232f ) niter--;        \
		return niter;                                              \
	}

#ifdef __GNUC__
#define HAVE_SINCOS_X86_FPU
void sincos_x86_fpu( double t, double *st, double *ct )
{
	asm( "fsincos;" : "=t"( *ct ), "=u"( *st ) : "0"( t ) );
	//*st = sin(t); *ct = cos(t);
}
#elif defined( _MSC_VER ) && !defined( _WIN64 )
#define HAVE_SINCOS_X86_FPU
void sincos_x86_fpu( double t, double *st_, double *ct_ )
{
	_asm
	{
		fld QWORD PTR[t] fsincos mov ebx, [ct_] fstp QWORD PTR[ebx] mov ebx,
			[st_] fstp QWORD PTR[ebx]
	}
}
#endif

#ifdef HAVE_SINCOS_X86_FPU
float stupid_sincos_x86_fpu( float x )
{
	double s, c;
	sincos_x86_fpu( x, &s, &c );
	return s + c;
}
#endif

v4sf stupid_sincos_ps( v4sf x )
{
	v4sf s, c;
	sincos_ps( x, &s, &c );
	return s;
}

float stupid_atan2f( float x ) { return atan2f( x, x + 0.1f ); }
float stupid_atan2_ref( float x ) { return atan2_ref( x, x + 0.1f ); }
v4sf stupid_atan2_ps( v4sf x )
{
	v4sf y = _mm_add_ps( x, x );
	return atan2_ps( y, x );
}

float rsqrtf( float x ) { return 1.0f / sqrtf( x ); }

float cotf( float x ) { return 1.0f / tanf( x ); }

DECL_SCALAR_FN_BENCH( sinf );
DECL_SCALAR_FN_BENCH( cosf );
#ifdef HAVE_SINCOS_X86_FPU
DECL_SCALAR_FN_BENCH( stupid_sincos_x86_fpu );
#endif
DECL_SCALAR_FN_BENCH( logf );
DECL_SCALAR_FN_BENCH( expf );
DECL_SCALAR_FN_BENCH( cephes_sinf );
DECL_SCALAR_FN_BENCH( cephes_cosf );
DECL_SCALAR_FN_BENCH( cephes_expf );
DECL_SCALAR_FN_BENCH( cephes_logf );
DECL_SCALAR_FN_BENCH( tanf );
DECL_SCALAR_FN_BENCH( cotf );
DECL_SCALAR_FN_BENCH( atanf );
DECL_VECTOR_FN_BENCH( sin_ps );
DECL_VECTOR_FN_BENCH( cos_ps );
DECL_VECTOR_FN_BENCH( stupid_sincos_ps );
DECL_VECTOR_FN_BENCH( exp_ps );
DECL_VECTOR_FN_BENCH( log_ps );
DECL_VECTOR_FN_BENCH( tan_ps );
DECL_VECTOR_FN_BENCH( cot_ps );
DECL_VECTOR_FN_BENCH( atan_ps );
#ifdef HAVE_VECLIB
DECL_VECTOR_FN_BENCH( vsinf );
DECL_VECTOR_FN_BENCH( vcosf );
DECL_VECTOR_FN_BENCH( vlogf );
DECL_VECTOR_FN_BENCH( vexpf );
#endif
#ifdef HAVE_ACML
DECL_VECTOR_FN_BENCH( __vrs4_sinf );
DECL_VECTOR_FN_BENCH( __vrs4_cosf );
DECL_VECTOR_FN_BENCH( __vrs4_expf );
DECL_VECTOR_FN_BENCH( __vrs4_logf );
#endif

DECL_SCALAR_FN_BENCH( sqrtf );
DECL_SCALAR_FN_BENCH( sqrt_ps );
DECL_SCALAR_FN_BENCH( rsqrtf );
DECL_SCALAR_FN_BENCH( rsqrt_ps );
DECL_SCALAR_FN_BENCH( stupid_atan2f );
DECL_SCALAR_FN_BENCH( stupid_atan2_ref );
DECL_VECTOR_FN_BENCH( stupid_atan2_ps );

void run_bench( const char *s, int ( *fn )() )
{
	printf( "benching %20s ..", s );
	fflush( stdout );
	double t0 = uclock_sec(), t1, tmax = 1.0;
	double niter = 0;
	do {
		niter += fn();
		t1 = uclock_sec();
	} while( t1 - t0 < tmax );
#define REF_FREQ_MHZ 2600.0
	printf(
		" -> %6.1f millions of vector evaluations/second -> %3.0f cycles/value on a %gMHz "
		"computer\n",
		floor( niter / ( t1 - t0 ) / 1e5 ) / 10, ( t1 - t0 ) * REF_FREQ_MHZ * 1e6 / niter / 4,
		REF_FREQ_MHZ );
}

void sanity_check()
{
	printf( "doing some sanity checks...\n" );
#ifndef USE_SSE2
	V4SF v = {{1, 2, 3, 4}}, z = {{5, 6, 7, 8}};
	v2si mm0, mm1;
	COPY_XMM_TO_MM( v.v, mm0, mm1 );
	COPY_MM_TO_XMM( mm0, mm1, v.v );
	printf( "truncation to int: " );
	print2i( mm0 );
	print2i( mm1 );
	printf( "\n" );
	_mm_empty();
	printf( "float vector: " );
	print4( v.v );
	printf( "\n" );
	fflush( stdout );
	assert( v.f[0] == 1 );
	assert( v.f[1] == 2 );
	assert( v.f[2] == 3 );
	assert( v.f[3] == 4 );

	V4SF w;
	w.v = v.v;
	// printf("float vector: "); print4(w); printf("\n");
	mm0 = _mm_cvttps_pi32( w.v );
	w.v = _mm_movehl_ps( w.v, w.v );
	mm1 = _mm_cvttps_pi32( w.v );
	/*_mm_empty();
	printf("truncation to int: "); print2i(mm0); print2i(mm1); printf("\n");
	*/
	w.v = _mm_cvtpi32x2_ps( mm0, mm1 );
	print2i( ( (v2si *)&w.v )[0] );
	print2i( ( (v2si *)&w.v )[1] );
	_mm_empty();
	v.v = w.v;
	assert( ( ( (long long)&v.v ) & 0xf ) == 0 );
	printf( "converted back to float: " );
	print4( v.v );
	printf( "\n" );
	fflush( stdout );
	assert( v.f[0] == 1 );
	assert( v.f[1] == 2 );
	assert( v.f[2] == 3 );
	assert( v.f[3] == 4 );

	w.v = _mm_movehl_ps( z.v, w.v );
	printf( "test for _mm_movehl_ps bug..\n" );
	print4( w.v );
	printf( "\n" );
	fflush( stdout );
	if( w.f[0] != 3 || w.f[1] != 4 || w.f[2] != 7 || w.f[3] != 8 ) {
		printf( "your compiler has the nasty bug on _mm_movehl_ps: IT IS BROKEN\n" );
		exit( 1 );
	}

	printf( "test for _mm_cmpxx_ps bug..\n" );
	V4SF r;
	r.v = _mm_cmplt_ps( w.v, w.v );
	if( r.i[0] != 0 || r.i[1] != 0 || r.i[2] != 0 || r.i[3] != 0 ) {
		printf( "your compiler has the nasty bug on all _mm_cmp*_ps functions: IT IS BROKEN\n" );
		exit( 1 );
	}
	r.v = _mm_cmpeq_ps( w.v, w.v );
	if( r.i[0] != -1 || r.i[1] != -1 || r.i[2] != -1 || r.i[3] != -1 ) {
		printf( "your compiler has the nasty bug on all _mm_cmp*_ps functions: IT IS BROKEN\n" );
		exit( 1 );
	}
#endif  // USE_SSE2
}

int main()
{
#ifndef HAVE_SYS_TIMES
// initTimer();
#endif
	// dumb();
	// sanity_check();
	int err = 0;
	err += check_sincos_precision( 0., 1.0 );
	err += check_sincos_precision( -1000, 1000 );
	err += check_explog_precision( -60, 60 );
	err += check_precision( -0.25f, 0.25f, "tan", tan_ps, tanf, cephes_tanf, 0 );
	err += check_precision( -0.49f, 0.49f, "tan", tan_ps, tanf, cephes_tanf, 1 );
	err += check_precision( 0.2, 0.7, "cot", cot_ps, cotf, cephes_cotf, 0 );
	err += check_precision( 0.01, 0.99, "cot", cot_ps, cotf, cephes_cotf, 1 );
	err += check_precision( -10, 10, "atan", atan_ps, atanf, cephes_atanf, 0 );
	err += check_precision( -10000, 10000, "atan", atan_ps, atanf, cephes_atanf, 0 );
	err += check_precision2( -1, 1, "atan2", atan2_ps, atan2f, cephes_atan2f, 0 );
	err += check_precision2( -10000, 10000, "atan2", atan2_ps, atan2f, cephes_atan2f, 0 );

	if( err ) {
		printf( "some precision tests have failed\n" );
	}

	check_special_values();
	run_bench( "sinf", bench_sinf );
	run_bench( "cosf", bench_cosf );
#ifdef HAVE_SINCOS_X86_FPU
	run_bench( "sincos (x87)", bench_stupid_sincos_x86_fpu );
#endif
	run_bench( "expf", bench_expf );
	run_bench( "logf", bench_logf );
	run_bench( "tanf", bench_tanf );
	run_bench( "cotf", bench_cotf );
	run_bench( "atanf", bench_atanf );
	run_bench( "atan2f", bench_stupid_atan2f );
	run_bench( "atan2_ref", bench_stupid_atan2_ref );
	run_bench( "sqrtf", bench_sqrtf );
	run_bench( "rsqrtf", bench_sqrtf );

	run_bench( "cephes_sinf", bench_cephes_sinf );
	run_bench( "cephes_cosf", bench_cephes_cosf );
	run_bench( "cephes_expf", bench_cephes_expf );
	run_bench( "cephes_logf", bench_cephes_logf );

	run_bench( "sin_ps", bench_sin_ps );
	run_bench( "cos_ps", bench_cos_ps );
	run_bench( "sincos_ps", bench_stupid_sincos_ps );
	run_bench( "exp_ps", bench_exp_ps );
	run_bench( "log_ps", bench_log_ps );

	run_bench( "tan_ps", bench_tan_ps );
	run_bench( "cot_ps", bench_cot_ps );
	run_bench( "atan_ps", bench_atan_ps );
	run_bench( "atan2_ps", bench_stupid_atan2_ps );

	run_bench( "sqrt_ps", bench_sqrt_ps );
	run_bench( "rsqrt_ps", bench_sqrt_ps );

#ifdef HAVE_VECLIB
	run_bench( "vsinf", bench_vsinf );
	run_bench( "vcosf", bench_vcosf );
	run_bench( "vexpf", bench_vexpf );
	run_bench( "vlogf", bench_vlogf );
#endif
#ifdef HAVE_ACML
	run_bench( "acml vrs4_sinf", bench___vrs4_sinf );
	run_bench( "acml vrs4_cosf", bench___vrs4_cosf );
	run_bench( "acml vrs4_expf", bench___vrs4_expf );
	run_bench( "acml vrs4_logf", bench___vrs4_logf );
#endif
	return err;
}

/* cephes functions, copied here to serve as a reference */

/*              sinf.c
 *
 *  Circular sine
 *
 *
 *
 * SYNOPSIS:
 *
 * float x, y, sinf();
 *
 * y = sinf( x );
 *
 *
 *
 * DESCRIPTION:
 *
 * Range reduction is into intervals of pi/4.  The reduction
 * error is nearly eliminated by contriving an extended precision
 * modular arithmetic.
 *
 * Two polynomial approximating functions are employed.
 * Between 0 and pi/4 the sine is approximated by
 *      x  +  x**3 P(x**2).
 * Between pi/4 and pi/2 the cosine is represented as
 *      1  -  x**2 Q(x**2).
 *
 *
 * ACCURACY:
 *
 *                      Relative error:
 * arithmetic   domain      # trials      peak       rms
 *    IEEE    -4096,+4096   100,000      1.2e-7     3.0e-8
 *    IEEE    -8192,+8192   100,000      3.0e-7     3.0e-8
 *
 * ERROR MESSAGES:
 *
 *   message           condition        value returned
 * sin total loss      x > 2^24              0.0
 *
 * Partial loss of accuracy begins to occur at x = 2^13
 * = 8192. Results may be meaningless for x >= 2^24
 * The routine as implemented flags a TLOSS error
 * for x >= 2^24 and returns 0.0.
 */

/*              cosf.c
 *
 *  Circular cosine
 *
 *
 *
 * SYNOPSIS:
 *
 * float x, y, cosf();
 *
 * y = cosf( x );
 *
 *
 *
 * DESCRIPTION:
 *
 * Range reduction is into intervals of pi/4.  The reduction
 * error is nearly eliminated by contriving an extended precision
 * modular arithmetic.
 *
 * Two polynomial approximating functions are employed.
 * Between 0 and pi/4 the cosine is approximated by
 *      1  -  x**2 Q(x**2).
 * Between pi/4 and pi/2 the sine is represented as
 *      x  +  x**3 P(x**2).
 *
 *
 * ACCURACY:
 *
 *                      Relative error:
 * arithmetic   domain      # trials      peak         rms
 *    IEEE    -8192,+8192   100,000      3.0e-7     3.0e-8
 */

/*
Cephes Math Library Release 2.2:  June, 1992
Copyright 1985, 1987, 1988, 1992 by Stephen L. Moshier
Direct inquiries to 30 Frost Street, Cambridge, MA 02140
*/

/* Single precision circular sine
 * test interval: [-pi/4, +pi/4]
 * trials: 10000
 * peak relative error: 6.8e-8
 * rms relative error: 2.6e-8
 */

static float FOPI = 1.27323954473516;
static float PIO4F = 0.7853981633974483096;
/* Note, these constants are for a 32-bit significand: */
/*
  static float DP1 =  0.7853851318359375;
  static float DP2 =  1.30315311253070831298828125e-5;
  static float DP3 =  3.03855025325309630e-11;
  static float lossth = 65536.;
*/

/* These are for a 24-bit significand: */
static float DP1 = 0.78515625;
static float DP2 = 2.4187564849853515625e-4;
static float DP3 = 3.77489497744594108e-8;
static float lossth = 8192.;
static float T24M1 = 16777215.;

static float sincof[] = {-1.9515295891E-4, 8.3321608736E-3, -1.6666654611E-1};
static float coscof[] = {2.443315711809948E-005, -1.388731625493765E-003, 4.166664568298827E-002};

float cephes_sinf( float xx )
{
	float *p;
	float x, y, z;
	register unsigned long j;
	register int sign;

	sign = 1;
	x = xx;
	if( xx < 0 ) {
		sign = -1;
		x = -xx;
	}
	if( x > T24M1 ) {
		// mtherr( "sinf", TLOSS );
		return ( 0.0 );
	}
	j = FOPI * x; /* integer part of x/(PI/4) */
	y = j;
	/* map zeros to origin */
	if( j & 1 ) {
		j += 1;
		y += 1.0;
	}
	j &= 7; /* octant modulo 360 degrees */
	/* reflect in x axis */
	if( j > 3 ) {
		sign = -sign;
		j -= 4;
	}
	if( x > lossth ) {
		// mtherr( "sinf", PLOSS );
		x = x - y * PIO4F;
	} else {
		/* Extended precision modular arithmetic */
		x = ( ( x - y * DP1 ) - y * DP2 ) - y * DP3;
	}
	/*einits();*/
	z = x * x;
	// printf("my_sinf: corrected oldx, x, y = %14.10g, %14.10g, %14.10g\n", oldx, x, y);
	if( ( j == 1 ) || ( j == 2 ) ) {
		/* measured relative error in +/- pi/4 is 7.8e-8 */
		/*
		  y = ((  2.443315711809948E-005 * z
		  - 1.388731625493765E-003) * z
		  + 4.166664568298827E-002) * z * z;
		*/
		p = coscof;
		y = *p++;
		y = y * z + *p++;
		y = y * z + *p++;
		y *= z;
		y *= z;
		y -= 0.5 * z;
		y += 1.0;
	} else {
		/* Theoretical relative error = 3.8e-9 in [-pi/4, +pi/4] */
		/*
		  y = ((-1.9515295891E-4 * z
		  + 8.3321608736E-3) * z
		  - 1.6666654611E-1) * z * x;
		  y += x;
		*/
		p = sincof;
		y = *p++;
		y = y * z + *p++;
		y = y * z + *p++;
		y *= z;
		y *= x;
		y += x;
	}
	/*einitd();*/
	// printf("my_sinf: j=%d result = %14.10g * %d\n", j, y, sign);
	if( sign < 0 ) y = -y;
	return ( y );
}

/* Single precision circular cosine
 * test interval: [-pi/4, +pi/4]
 * trials: 10000
 * peak relative error: 8.3e-8
 * rms relative error: 2.2e-8
 */

float cephes_cosf( float xx )
{
	float x, y, z;
	int j, sign;

	/* make argument positive */
	sign = 1;
	x = xx;
	if( x < 0 ) x = -x;

	if( x > T24M1 ) {
		// mtherr( "cosf", TLOSS );
		return ( 0.0 );
	}

	j = FOPI * x; /* integer part of x/PIO4 */
	y = j;
	/* integer and fractional part modulo one octant */
	if( j & 1 ) /* map zeros to origin */
	{
		j += 1;
		y += 1.0;
	}
	j &= 7;
	if( j > 3 ) {
		j -= 4;
		sign = -sign;
	}

	if( j > 1 ) sign = -sign;

	if( x > lossth ) {
		// mtherr( "cosf", PLOSS );
		x = x - y * PIO4F;
	} else
		/* Extended precision modular arithmetic */
		x = ( ( x - y * DP1 ) - y * DP2 ) - y * DP3;

	// printf("xx = %g -> x corrected = %g sign=%d j=%d y=%g\n", xx, x, sign, j, y);

	z = x * x;

	if( ( j == 1 ) || ( j == 2 ) ) {
		y = ( ( ( -1.9515295891E-4f * z + 8.3321608736E-3f ) * z - 1.6666654611E-1f ) * z * x ) + x;
	} else {
		y = ( ( 2.443315711809948E-005f * z - 1.388731625493765E-003f ) * z
			  + 4.166664568298827E-002f ) * z * z;
		y -= 0.5 * z;
		y += 1.0;
	}
	if( sign < 0 ) y = -y;
	return ( y );
}

/*              expf.c
 *
 *  Exponential function
 *
 *
 *
 * SYNOPSIS:
 *
 * float x, y, expf();
 *
 * y = expf( x );
 *
 *
 *
 * DESCRIPTION:
 *
 * Returns e (2.71828...) raised to the x power.
 *
 * Range reduction is accomplished by separating the argument
 * into an integer k and fraction f such that
 *
 *     x    k  f
 *    e  = 2  e.
 *
 * A polynomial is used to approximate exp(f)
 * in the basic range [-0.5, 0.5].
 *
 *
 * ACCURACY:
 *
 *                      Relative error:
 * arithmetic   domain     # trials      peak         rms
 *    IEEE      +- MAXLOG   100000      1.7e-7      2.8e-8
 *
 *
 * Error amplification in the exponential function can be
 * a serious matter.  The error propagation involves
 * exp( X(1+delta) ) = exp(X) ( 1 + X*delta + ... ),
 * which shows that a 1 lsb error in representing X produces
 * a relative error of X times 1 lsb in the function.
 * While the routine gives an accurate result for arguments
 * that are exactly represented by a double precision
 * computer number, the result contains amplified roundoff
 * error for large arguments not exactly represented.
 *
 *
 * ERROR MESSAGES:
 *
 *   message         condition      value returned
 * expf underflow    x < MINLOGF         0.0
 * expf overflow     x > MAXLOGF         MAXNUMF
 *
 */

/*
Cephes Math Library Release 2.2:  June, 1992
Copyright 1984, 1987, 1989 by Stephen L. Moshier
Direct inquiries to 30 Frost Street, Cambridge, MA 02140
*/

/* Single precision exponential function.
 * test interval: [-0.5, +0.5]
 * trials: 80000
 * peak relative error: 7.6e-8
 * rms relative error: 2.8e-8
 */

static float MAXNUMF = 3.4028234663852885981170418348451692544e38;
static float MAXLOGF = 88.72283905206835;
static float MINLOGF = -103.278929903431851103; /* log(2^-149) */

static float LOG2EF = 1.44269504088896341;

static float C1 = 0.693359375;
static float C2 = -2.12194440e-4;

float cephes_expf( float xx )
{
	float x, z;
	int n;

	x = xx;

	if( x > MAXLOGF ) {
		// mtherr( "expf", OVERFLOW );
		return ( MAXNUMF );
	}

	if( x < MINLOGF ) {
		// mtherr( "expf", UNDERFLOW );
		return ( 0.0 );
	}

	/* Express e**x = e**g 2**n
	 *   = e**g e**( n loge(2) )
	 *   = e**( g + n loge(2) )
	 */
	z = floorf( LOG2EF * x + 0.5 ); /* floor() truncates toward -infinity. */

	x -= z * C1;
	x -= z * C2;
	n = z;

	z = x * x;
	/* Theoretical peak relative error in [-0.5, +0.5] is 4.2e-9. */
	z = ( ( ( ( ( 1.9875691500E-4f * x + 1.3981999507E-3f ) * x + 8.3334519073E-3f ) * x
			  + 4.1665795894E-2f ) * x
			+ 1.6666665459E-1f ) * x
		  + 5.0000001201E-1f ) * z
		+ x + 1.0;

	/* multiply by power of 2 */
	x = ldexpf( z, n );

	return ( x );
}

/*              logf.c
 *
 *  Natural logarithm
 *
 *
 *
 * SYNOPSIS:
 *
 * float x, y, logf();
 *
 * y = logf( x );
 *
 *
 *
 * DESCRIPTION:
 *
 * Returns the base e (2.718...) logarithm of x.
 *
 * The argument is separated into its exponent and fractional
 * parts.  If the exponent is between -1 and +1, the logarithm
 * of the fraction is approximated by
 *
 *     log(1+x) = x - 0.5 x**2 + x**3 P(x)
 *
 *
 *
 * ACCURACY:
 *
 *                      Relative error:
 * arithmetic   domain     # trials      peak         rms
 *    IEEE      0.5, 2.0    100000       7.6e-8     2.7e-8
 *    IEEE      1, MAXNUMF  100000                  2.6e-8
 *
 * In the tests over the interval [1, MAXNUM], the logarithms
 * of the random arguments were uniformly distributed over
 * [0, MAXLOGF].
 *
 * ERROR MESSAGES:
 *
 * logf singularity:  x = 0; returns MINLOG
 * logf domain:       x < 0; returns MINLOG
 */

/*
Cephes Math Library Release 2.2:  June, 1992
Copyright 1984, 1987, 1988, 1992 by Stephen L. Moshier
Direct inquiries to 30 Frost Street, Cambridge, MA 02140
*/

/* Single precision natural logarithm
 * test interval: [sqrt(2)/2, sqrt(2)]
 * trials: 10000
 * peak relative error: 7.1e-8
 * rms relative error: 2.7e-8
 */
float LOGE2F = 0.693147180559945309;
float SQRTHF = 0.707106781186547524;
float MACHEPF = 5.9604644775390625E-8;

float cephes_logf( float xx )
{
	register float y;
	float x, z, fe;
	int e;

	x = xx;
	fe = 0.0;
	/* Test for domain */
	if( x <= 0.0 ) {
		// ERROR
		return ( MINLOGF );
	}

	x = frexpf( x, &e );
	// printf("\nmy_logf: frexp -> e = %d x = %g\n", e, x);
	if( x < SQRTHF ) {
		e -= 1;
		x = x + x - 1.0; /*  2x - 1  */
	} else {
		x = x - 1.0;
	}
	z = x * x;
	/* 3.4e-9 */
	/*
	p = logfcof;
	y = *p++ * x;
	for( i=0; i<8; i++ )
	  {
	  y += *p++;
	  y *= x;
	  }
	y *= z;
	*/

	y = ( ( ( ( ( ( ( ( 7.0376836292E-2f * x - 1.1514610310E-1f ) * x + 1.1676998740E-1f ) * x
					- 1.2420140846E-1f ) * x
				  + 1.4249322787E-1f ) * x
				- 1.6668057665E-1f ) * x
			  + 2.0000714765E-1f ) * x
			- 2.4999993993E-1f ) * x
		  + 3.3333331174E-1f ) * x * z;

	// printf("my_logf: poly = %g\n", y);

	if( e ) {
		fe = e;
		y += -2.12194440e-4f * fe;
	}
	y += -0.5 * z; /* y - 0.5 x^2 */

	// printf("my_logf: x = %g y = %g\n", x, y);
	z = x + y; /* ... + x  */

	if( e ) z += 0.693359375f * fe;

	return ( z );
}

static float tancotf( float xx, int cotflg )
{
	float x, y, z, zz;
	long j;
	int sign;

	/* make argument positive but save the sign */
	if( xx < 0.0 ) {
		x = -xx;
		sign = -1;
	} else {
		x = xx;
		sign = 1;
	}

	if( x > lossth ) {
		return ( 0.0 );
	}

	/* compute x mod PIO4 */
	j = FOPI * x; /* integer part of x/(PI/4) */
	y = j;

	/* map zeros and singularities to origin */
	if( j & 1 ) {
		j += 1;
		y += 1.0;
	}

	z = ( ( x - y * DP1 ) - y * DP2 ) - y * DP3;

	zz = z * z;

	if( x > 1.0e-4 ) {
		/* 1.7e-8 relative error in [-pi/4, +pi/4] */
		y = ( ( ( ( ( 9.38540185543E-3 * zz + 3.11992232697E-3 ) * zz + 2.44301354525E-2 ) * zz
				  + 5.34112807005E-2 ) * zz
				+ 1.33387994085E-1 ) * zz
			  + 3.33331568548E-1 ) * zz * z
			+ z;
	} else {
		y = z;
	}

	if( j & 2 ) {
		if( cotflg )
			y = -y;
		else
			y = -1.0 / y;
	} else {
		if( cotflg ) y = 1.0 / y;
	}

	if( sign < 0 ) y = -y;

	return ( y );
}

float cephes_tanf( float x ) { return ( tancotf( x, 0 ) ); }
float cephes_cotf( float x ) { return ( tancotf( x, 1 ) ); }

float cephes_atanf( float xx )
{
	float x, y, z;
	int sign;

	x = xx;

	/* make argument positive and save the sign */
	if( xx < 0.0 ) {
		sign = -1;
		x = -xx;
	} else {
		sign = 1;
		x = xx;
	}
	/* range reduction */
	if( x > 2.414213562373095 ) /* tan 3pi/8 */
	{
		y = PIO2F;
		x = -( 1.0 / x );
	}

	else if( x > 0.4142135623730950 ) /* tan pi/8 */
	{
		y = PIO4F;
		x = ( x - 1.0 ) / ( x + 1.0 );
	} else
		y = 0.0;

	z = x * x;
	y += ( ( ( 8.05374449538e-2 * z - 1.38776856032E-1 ) * z + 1.99777106478E-1 ) * z
		   - 3.33329491539E-1 ) * z * x
		 + x;

	if( sign < 0 ) y = -y;

	return ( y );
}

float cephes_atan2f( float y, float x )
{
	float z, w;
	int code;

	code = 0;

	if( x < 0.0 ) code = 2;
	if( y < 0.0 ) code |= 1;

	if( x == 0.0 ) {
		if( code & 1 ) {
			return ( -PIO2F );
		}
		if( y == 0.0 ) return ( 0.0 );
		return ( PIO2F );
	}

	if( y == 0.0 ) {
		if( code & 2 ) return ( PIF );
		return ( 0.0 );
	}

	switch( code ) {
		default:
		case 0:
		case 1:
			w = 0.0;
			break;
		case 2:
			w = PIF;
			break;
		case 3:
			w = -PIF;
			break;
	}

	z = cephes_atanf( y / x );

	return ( w + z );
}