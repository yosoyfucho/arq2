#include "StringService.h"
#include "ArithmeticService.h"
#include <Ice/Ice.h>

using namespace std;
using namespace UC3M;

/*Compilar
gcc -I. -c StringService.cpp ArithmeticService.cpp client.cpp
gcc -o client client.o StringService.o ArithmeticService.o -lIce -lIceUtil
*/

int
main (int argc, char* argv [])
{
	int status = 0;
	int a,b;
	a = 3;
	b = 8;

	Ice :: CommunicatorPtr ic;

	try
	{
		ic = Ice :: initialize(argc,argv);

		Ice :: ObjectPrx base1 = ic ->stringToProxy("StringService: default -h localhost -p 10000");

		// new for Arithmetic
		Ice :: ObjectPrx base2 = ic ->stringToProxy("ArithmeticService: default -h localhost -p 10001");

		StringServicePrx remoteService = StringServicePrx::checkedCast(base1);

		// new for Arithmetic
		ArithmeticServicePrx remoteService2 = ArithmeticServicePrx::checkedCast(base2);


		if((!remoteService)||(!remoteService2))
			throw "Invalid proxy";

		// your client code here!

		cout << "size of \"\" : "
			 << remoteService->stringSize("") << endl;
		cout << "size of \"a\" : "
			 << remoteService->stringSize("a") << endl;
		cout << "size of \"abcdef\" : "
			 << remoteService->stringSize("abcdef") << endl;
		cout << "toUpperCase of \"abcdef\" : "
			 << remoteService->toUpperCase("abcdef") << endl;

		// new for Arithmetic
		cout << "add of \"a + b\" : "
			 << remoteService2->addIntegers(a,b) << endl;
		cout << "subtract of \"b - a\" : "
			 << remoteService2->subtractIntegers(a,b) << endl;

	}
	catch (const Ice :: Exception& ex)
	{
		cerr << ex << endl;
		status = 1;
	}
	catch (const char* msg)
	{
		cerr << msg << endl;
		status = 1;
	}
	if (ic)
		ic -> destroy();

	return status;
}
