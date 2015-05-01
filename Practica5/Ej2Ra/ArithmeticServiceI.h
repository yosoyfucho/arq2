#ifndef __ArithmeticServiceI_h__
#define __ArithmeticServiceI_h__

#include <ArithmeticService.h>

namespace UC3M
{

class ArithmeticServiceI : virtual public ArithmeticService
{
public:

    virtual ::Ice::Int addIntegers(::Ice::Int,
                                   ::Ice::Int,
                                   const Ice::Current&);

    virtual ::Ice::Int subtractIntegers(::Ice::Int,
                                        ::Ice::Int,
                                        const Ice::Current&);
};

}

#endif
