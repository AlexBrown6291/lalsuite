#include <limits.h>
#include <stdlib.h>
#include <stdio.h>
#include <lal/Date.h>
#include <lal/XLALError.h>

NRCSID(STRINGCONVERTTESTC, "$Id$");


struct TESTCASE {
	const char *string;
	int sec, ns;
	const char *remainder;
	int xlal_errno;
};

int lalDebugLevel = 0;


static int runtest(const struct TESTCASE *testcase)
{
	int retval;
	LIGOTimeGPS gps;
	LIGOTimeGPS gpsCorrect;
	char *endptr;
	int failure = 0;

	XLALGPSSet(&gpsCorrect, testcase->sec, testcase->ns);

	XLALClearErrno();
	retval = XLALStrToGPS(&gps, testcase->string, &endptr);

	fprintf(stdout, "Input = \"%s\"\n\tOutput =\t%lld ns with \"%s\" remainder, errno %d\n\tCorrect =\t%lld ns with \"%s\" remainder, errno %d\n\t\t===>", testcase->string, XLALGPSToINT8NS(&gps), endptr, XLALGetBaseErrno(), XLALGPSToINT8NS(&gpsCorrect), testcase->remainder, testcase->xlal_errno);

	if(retval == 0 && testcase->xlal_errno == 0) {
		if(XLALGPSCmp(&gps, &gpsCorrect) || strcmp(endptr, testcase->remainder))
			failure = 1;
	} else if(XLALGetBaseErrno() != testcase->xlal_errno)
		failure = 1;

	if(failure)
		fprintf(stdout, "*** FAIL ***\n");
	else
		fprintf(stdout, "Pass\n");

	return(failure);
}


int main(void)
{
	/* Most of these test were shamelessly stolen from Peter's original
	 * code for testing LALStringToGPS() */
	struct TESTCASE general_testcases[] = {
		{"1234.5", 1234, 500000000, "", 0},
		{"712345678", 712345678, 0, "", 0},
		{"00000000712346678", 712346678, 0, "", 0},
		{"000000000000000000000000000000000712347678", 712347678, 0, "", 0},
		{"000000000000000000712348678.00000000000000", 712348678, 0, "", 0},
		{"000000000000000000712349678.00000000000001", 712349678, 0, "", 0},
		{"722345678.", 722345678, 0, "", 0},
		{"1722346678.", 1722346678, 0, "", 0},
		{"01722347678.", 1722347678, 0, "", 0},
		{"001722348678.", 1722348678, 0, "", 0},
		{"732345678.0", 732345678, 0, "", 0},
		{"742345678.7", 742345678, 700000000, "", 0},
		{"752345678.000861", 752345678, 861000, "", 0},
		{"762345678.000862547", 762345678, 862547, "", 0},
		{"772345678.0008635474", 772345678, 863547, "", 0},
		/*{"782345678.0008645475", 782345678, 864548, "", 0},*/
		{"792345678.000865547687287", 792345678, 865548, "", 0},
		{"702345678.9999999994", 702345678, 999999999, "", 0},
		/*{"712345678.9999999995", 712345679, 0, "", 0},*/
		{"722345678.9999999996", 722345679, 0, "", 0},
		{"2000000000", 2000000000, 0, "", 0},
		{"7323456785", LONG_MAX, 0, "", XLAL_ERANGE},
		{"7423456785234", LONG_MAX, 0, "", XLAL_ERANGE},
		{"752345678e0", 752345678, 0, "", 0},
		{"762345678e+0", 762345678, 0, "", 0},
		{"772345678e-0", 772345678, 0, "", 0},
		{"782345678e00", 782345678, 0, "", 0},
		{"792345678e+00", 792345678, 0, "", 0},
		{"702345678e-00", 702345678, 0, "", 0},
		{"712345678.e0", 712345678, 0, "", 0},
		{"722345678.e+0", 722345678, 0, "", 0},
		{"732345678.e-0", 732345678, 0, "", 0},
		{"742345678.00e0", 742345678, 0, "", 0},
		{"752345678.00e+0", 752345678, 0, "", 0},
		{"762345678.00e-0", 762345678, 0, "", 0},
		{"772345678.06e0", 772345678, 60000000, "", 0},
		{"782345678.06e+0", 782345678, 60000000, "", 0},
		{"792345678.06e-0", 792345678, 60000000, "", 0},
		{"7023.45678e5", 702345678, 0, "", 0},
		{"7123.457785255e+05", 712345778, 525500000, "", 0},
		{"7223458785255e-4", 722345878, 525500000, "", 0},
		{"43d", 43, 0, "d", 0},
		{"44.3873qr", 44, 387300000, "qr", 0},
		{"45.3973 qr", 45, 397300000, " qr", 0},
		{"46.3073 e2", 46, 307300000, " e2", 0},
		{"47.3173e2", 4731, 730000000, "", 0},
		{"6.85e7", 68500000, 0, "", 0},
		{"6.9512345678901e7", 69512345, 678901000, "", 0},
		{"6.05e7dkjf", 60500000, 0, "dkjf", 0},
		{"6.15ex0", 6, 150000000, "ex0", 0},
		{"6.25E7", 62500000, 0, "", 0},
		{"6.35E7dkjf", 63500000, 0, "dkjf", 0},
		{"6.45Ex0", 6, 450000000, "Ex0", 0},
		{"752345678.5433e258", LONG_MAX, 0, "", XLAL_ERANGE},
		{"762345678.5533e258r574", LONG_MAX, 0, "r574", XLAL_ERANGE},
		{"772345678.5633e.258", 772345678, 563300000, "e.258", 0},
		{"782345678.5733.258", 782345678, 573300000, ".258", 0},
		{"792345678.5833+258", 792345678, 583300000, "+258", 0},
		{"702345678.5933-258", 702345678, 593300000, "-258", 0},
		{"712345678.5033.258E02", 712345678, 503300000, ".258E02", 0},
		{"-722345678.5133", -722345679, 486700000, "", 0},
		{"-73234567800.5233", LONG_MIN, 0, "", XLAL_ERANGE},
		{"-742345678.000000625", -742345679, 999999375, "", 0},
		{"-743345678.9999999994", -743345679, 1, "", 0},
		/*{"-744345678.9999999995", -744345678, -999999999, "", 0},*/
		{"-752345678.9999999996", -752345679, 0, "", 0},
		{"5e-2", 0, 50000000, "", 0},
		{"7e-7", 0, 700, "", 0},
		{"6e-10", 0, 1, "", 0},
		{"8e-11", 0, 0, "", 0},
		{"-7e-12", 0, 0, "", 0},
		{"-4e-6", -1, 999996000, "", 0},
		{"-4.2e-2", -1, 958000000, "", 0},
		{".5244", 0, 524400000, "", 0},
		{"-.5244", -1, 475600000, "", 0},
		{"0", 0, 0, "", 0},
		{"+", 0, 0, "+", 0},
		{"-", 0, 0, "-", 0},
		{"e", 0, 0, "e", 0},
		{"e3", 0, 0, "e3", 0},
		{"x", 0, 0, "x", 0},
		{"0x", 0, 0, "x", 0},
		{"0.0000000000000000000000000000000000000001e40", 1, 0, "", 0},
		{"10000000000000000000000000000000000000000e-40", 1, 0, "", 0},
		{NULL, 0, 0, NULL, 0}
	};
	struct TESTCASE glibc_testcases[] = {
		{"0x0", 0, 0, "", 0},
		{"0x00", 0, 0, "", 0},
		{"00x0", 0, 0, "x0", 0},
		{"00x00", 0, 0, "x00", 0},
		{"0x2CD7E24E.8", 752345678, 500000000, "", 0},
		{"0x10P-6", 0, 250000000, "", 0},
		{NULL, 0, 0, NULL, 0}
	};
	struct TESTCASE *testcase;
	int failures = 0;

	/* run tests that all platforms must pass */
	for(testcase = general_testcases; testcase->string; testcase++)
		failures += runtest(testcase);

	/* if C library is smart enough to handle hex floats, do more tests */
	if(strtod("0x.8", NULL) == 0.5)
		for(testcase = glibc_testcases; testcase->string; testcase++)
			failures += runtest(testcase);

	fprintf(stdout, "Summary of GPS string conversion tests: ");
	if(failures) {
		fprintf(stdout, "%d FAILURES\n", failures);
		exit(9);
	} else
		fprintf(stdout, "all succeeded\n");

	exit(0);
}
