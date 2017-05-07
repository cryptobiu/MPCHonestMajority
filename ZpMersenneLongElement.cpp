//
// Created by meital on 04/05/17.
//

#include "ZpMersenneLongElement.h"
#include <iostream>
#include <cmath>
#include <emmintrin.h>


long shiftNumLong = (((long)1 << 61) - 1);

using namespace std;


ZpMersenneLongElement& ZpMersenneLongElement::operator=(const ZpMersenneLongElement& other) // copy assignment
{
    if (this != &other) { // self-assignment check expected
        elem = other.elem;
    }
    return *this;
}


ZpMersenneLongElement::ZpMersenneLongElement(unsigned long elem) {

    this->elem = elem %p;

    if(this->elem<0){
        this->elem = elem + p;
    }

}
ZpMersenneLongElement ZpMersenneLongElement::operator-(const ZpMersenneLongElement& f2)
{
    ZpMersenneLongElement answer;

    int temp =  (long)elem - (long)f2.elem;

    if(temp<0){
        answer.elem = temp + p;
    }
    else{
        answer.elem = temp;
    }



    return answer;
}

ZpMersenneLongElement ZpMersenneLongElement::operator+(const ZpMersenneLongElement& f2)
{
    ZpMersenneLongElement answer;

    answer.elem = (elem + f2.elem);

    if(answer.elem>=p)
        answer.elem-=p;

    return answer;
}

ZpMersenneLongElement ZpMersenneLongElement::operator*(const ZpMersenneLongElement& f2)
{


    ZpMersenneLongElement answer;


    //implement elem * f2.elem using ints
    int *ab = (int *)&elem;
    int *cd = (int *)&f2.elem;

    long bd = long(ab[1]) * long(cd[1]);
    long ac = long(ab[0]) * long(cd[0]);
    long adPlusBc = long(ab[1]) * long(cd[0]) + long(ab[0]) * long(cd[1]);

    //put the results into the __m128i



//    __m128i left, right;
//
//    left =  _mm_set_epi64((__m64)0L, (__m64)elem);
//    right =  _mm_set_epi64((__m64)0L, (__m64)f2.elem);


   // __m128i mult128 = (long)elem * (long) f2.elem;

//    //get the bottom 31 bit
//    unsigned int bottom = multLong & p;
//
//    //get the top 31 bits
//    unsigned int top = (multLong>>31);
//
//    answer.elem = bottom + top;
//
//    //maximim the value of 2p-2
//    if(answer.elem>=p)
//        answer.elem-=p;
//
//    //answer.elem = ((long)elem * (long) f2.elem) %p;
//
//
//
//    //return ZpMersenneIntElement((bottom + top) %p);
    return answer;

}

ZpMersenneLongElement& ZpMersenneLongElement::operator*=(const ZpMersenneLongElement& f2){

    long multLong = (long)elem * (long) f2.elem;

    //get the bottom 31 bit
    unsigned int bottom = multLong & p;

    //get the top 31 bits
    unsigned int top = (multLong>>31) ;

    elem = bottom + top;

    //maximim the value of 2p-2
    if(elem>=p)
        elem-=p;

    return *this;

}

ZpMersenneLongElement ZpMersenneLongElement::operator/(const ZpMersenneLongElement& f2)
{

//    //code taken from NTL for the function XGCD
//    long a = f2.elem;
//    long b = p;
//    __m128i s;
//
//    long  u, v, q, r;
//    __m128i u0, v0, u1, v1, u2, v2;
//
//    long aneg = 0, bneg = 0;
//
//    if (a < 0) {
//        a = -a;
//        aneg = 1;
//    }
//
//    if (b < 0) {
//        b = -b;
//        bneg = 1;
//    }
//
//    v1=_mm_setzero_si128();
//    u1=v1 + 1;
//    u2=_mm_setzero_si128();
//    v2=u1;
//    u = a; v = b;
//
//    while (v != 0) {
//        q = u / v;
//        r = u % v;
//        u = v;
//        v = r;
//        u0 = u2;
//        v0 = v2;
//        u2 =  u1 - q*u2;
//        v2 = v1- q*v2;
//        u1 = u0;
//        v1 = v0;
//    }
//
//    if (aneg)
//        u1 = -u1;
//
//
//    s = u1;
//
////    if (s < 0)
//        s =  s + p;
//
////    ZpMersenneLongElement inverse(s);
//
//
//
//    return inverse* (*this);


}
