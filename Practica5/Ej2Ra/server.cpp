#include "StringServiceI.h"
#include "ArithmeticServiceI.h"
#include <Ice/Ice.h>

/*
Compilar:
gcc -I. -c StringService.cpp ArithmeticService.cpp server.cpp
gcc -o server server.o StringService.o ArithmeticService.o -lIce -lIceUtil
*/

using namespace std;
using namespace UC3M;


::Ice::Int
UC3M::StringServiceI::stringSize(const ::std::string& s,
                                 const Ice::Current& current)
{
    return s.length();
}

::std::string
UC3M::StringServiceI::toUpperCase(const ::std::string& s,
                                  const Ice::Current& current)
{
    ::std::string ns(s);
    ::std::transform(ns.begin(), ns.end(), ns.begin(), ::toupper);
    return ns;
}

::Ice::Int
UC3M::ArithmeticServiceI::addIntegers(::Ice::Int a,
                                      ::Ice::Int b,
                                      const Ice::Current& current)
{
   int suma;
   suma = a + b;
   return suma;
}

::Ice::Int
UC3M::ArithmeticServiceI::subtractIntegers(::Ice::Int a,
                                           ::Ice::Int b,
                                           const Ice::Current& current)
{	
  int resta;
  resta = b-a;
  return resta;
}


int
main (int argc, char* argv[])
{
	int status = 0;
	Ice::CommunicatorPtr ic;

	try
	{
		ic = Ice ::initialize(argc,argv);
		Ice:: ObjectAdapterPtr adapter =
			ic->createObjectAdapterWithEndpoints("asii_adapter","default -p 10000");

		// new for Arithmetic
		Ice:: ObjectAdapterPtr adapter2 =
			ic->createObjectAdapterWithEndpoints("asii_adapter2","default -p 10001");

		Ice:: ObjectPtr object = new StringServiceI;

		// new for Arithmetic
		Ice:: ObjectPtr object2 = new ArithmeticServiceI;

		adapter->add(object, ic->stringToIdentity("StringService"));
		adapter->activate();

		// new for Arithmetic
		adapter2->add(object2, ic->stringToIdentity("ArithmeticService"));
		adapter2->activate();

		ic->waitForShutdown();
	}
	catch (const Ice::Exception& e)
	{
		cerr << e << endl;
		status = 1;
	}

	catch (const char* msg)
	{
		cerr << msg << endl;
		status = 1;
	}

	if (ic)
	{
		try
		{
			ic->destroy();
		}
		catch (const Ice::Exception& e)
		{
			cerr << e << endl;
			status = 1;
		}
	}

	return status;
}
