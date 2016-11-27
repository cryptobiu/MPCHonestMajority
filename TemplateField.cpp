//
// Created by meital on 15/11/16.
//

#include "TemplateField.h"


using namespace NTL;

template <>
TemplateField<ZZ_p>::TemplateField(long fieldParam) {

    this->fieldParam = fieldParam;
    this->elementSizeInBytes = (fieldParam + 255)/256;//round up to the next byte

    ZZ_p::init(ZZ(fieldParam));

    m_ZERO = new ZZ_p(0);
    m_ONE = new ZZ_p(1);
}
/*
 * The i-th field element. The ordering is arbitrary, *except* that
 * the 0-th field element must be the neutral w.r.t. addition, and the
 * 1-st field element must be the neutral w.r.t. multiplication.
 */
template <>
GF2E TemplateField<GF2E>::GetElement(long b) {

    if(b == 1)
    {
        return *GetOne();
    }
    if(b == 0)
    {
        return *GetZero();
    }
    GF2X element;

    for(int i=0; i < fieldParam; i++) {
        // set the coefficient of x^i to 1
        SetCoeff(element,i,(b >> i) & 1);
    }

    return to_GF2E(element);
}


template <>
ZZ_p TemplateField<ZZ_p>::GetElement(long b) {

    ZZ_p element(b);
    if(b == 1)
    {
        return *m_ONE;
    }
    if(b == 0)
    {
        return *m_ZERO;
    }
    else{
        return element;
    }
}


/**
 * the function create a field by:
 * generate the irreducible polynomial x^8 + x^4 + x^3 + x + 1 to work with
 * init the field with the newly generated polynomial
 */
template <>
TemplateField<GF2E>::TemplateField(long fieldParam) {

    this->fieldParam = fieldParam;
    this->elementSizeInBytes = fieldParam/8;
    GF2X irreduciblePolynomial = BuildSparseIrred_GF2X(fieldParam);
    GF2E::init(irreduciblePolynomial);

    m_ZERO = new GF2E(0);
    m_ONE = new GF2E(1);
}

template <>
void TemplateField<GF2E>::elementToBytes(unsigned char* elemenetInBytes, GF2E& element){

    BytesFromGF2X(elemenetInBytes,rep(element),fieldParam/8);
}


template <>
GF2E TemplateField<GF2E>::bytesToElement(unsigned char* elemenetInBytes){

   // GF2E outputElement;
    //first create a GF2X
    GF2X polynomialElement;

    //translate the bytes into a GF2X element
    GF2XFromBytes(polynomialElement, elemenetInBytes, fieldParam/8);


    //convert the GF2X to GF2E
    //outputElement = to_GF2E(polynomialElement);

    return to_GF2E(polynomialElement);
}




