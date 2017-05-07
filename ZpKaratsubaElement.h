//
// Created by hila on 05/05/17.
//

#ifndef ZPKARATSUBA_ZPKARATSUBAELEMENT_H
#define ZPKARATSUBA_ZPKARATSUBAELEMENT_H

#include "stdint.h"
#include <iostream>



using namespace std;

class ZpKaratsubaElement {

public:
    unsigned long elem;

    ZpKaratsubaElement();
    ZpKaratsubaElement(long elem);

    ZpKaratsubaElement operator*(const ZpKaratsubaElement& f2);
    ZpKaratsubaElement operator+(const ZpKaratsubaElement& f2);
    ZpKaratsubaElement operator-(const ZpKaratsubaElement& f2);
    ZpKaratsubaElement operator/(const ZpKaratsubaElement& f2);

    ZpKaratsubaElement& operator=(const ZpKaratsubaElement& other);

};

#endif //ZPKARATSUBA_ZPKARATSUBAELEMENT_H
