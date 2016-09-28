/*
 * TField.cpp
 *
 *  Created on: Aug 9, 2016
 *      Author: hila
 */

#include "TField.h"



bool TField::m_instanceFlag = false;
TField* TField::m_single = NULL;
/**
 * the function create a field by:
 * generate the irreducible polynomial x^8 + x^4 + x^3 + x + 1 to work with
 * init the field with the newly generated polynomial
 */
TField::TField() {

	GF2X irreduciblePolynomial = BuildSparseIrred_GF2X(8);
	cout << "Leader irreducible Polynomial is " << irreduciblePolynomial << endl;
	GF2E::init(irreduciblePolynomial);

//	GF2X p;
//	SetCoeff(p, 8, 1);
//	SetCoeff(p, 4, 1);
//	SetCoeff(p, 3, 1);
//	SetCoeff(p, 1, 1);
//	SetCoeff(p, 0, 1);
//
//	cout << p<<endl;
//
//	GF2E::init(p);
	//m_ZERO = new TFieldElement(GF2X::zero());
    GF2X pone0;
    SetCoeff(pone0, 0, 0);
    m_ZERO = new TFieldElement(GF2X(0));
	GF2X pone;
	SetCoeff(pone, 0, 1);
	m_ONE = new TFieldElement(GF2X(1));
}

// Read field description from a config file
/*TField TField::ReadField ()
{
	// how to read

}*/

/*
 * The i-th field element. The ordering is arbitrary, *except* that
 * the 0-th field element must be the neutral w.r.t. addition, and the
 * 1-st field element must be the neutral w.r.t. multiplication.
 */
TFieldElement* TField::GetElement(uint8_t b) {

	if(b == 1)
	{
		return this->GetOne();
	}
	if(b == 0)
	{
		return this->GetZero();
	}
	TFieldElement* element = new TFieldElement();
	bitset<8> bits(b);

	for(int i=0; i < 8; i++) {
			// set the coefficient of x^i to 1
			SetCoeff(element->getElement(),i,bits[i]);
	}
	return element;
}


/*
 * The i-th field element. The ordering is arbitrary, *except* that
 * the 0-th field element must be the neutral w.r.t. addition, and the
 * 1-st field element must be the neutral w.r.t. multiplication.
 */
TFieldElement TField::GetElementByValue(uint8_t b) {

	if(b == 1)
	{
		return *this->GetOne();
	}
	if(b == 0)
	{
		return *this->GetZero();
	}
	TFieldElement element;
	bitset<8> bits(b);

	for(int i=0; i < 8; i++) {
		// set the coefficient of x^i to 1
		SetCoeff(element.getElement(),i,bits[i]);
	}
	return element;
}

/**
 * return the field
 */
TField* TField::getInstance()
{
    if(!m_instanceFlag)
    {
        m_single = new TField();
        m_instanceFlag = true;
        return m_single;
    }
    else
    {
        return m_single;
    }
}


/**
 * A random random field element, uniform distribution
 */
TFieldElement* TField::Random() {
	TFieldElement* randomElement;
	PRG & prg = PRG::instance();
	uint8_t b = prg.getRandom();
	randomElement = this->GetElement(b);
	return randomElement;
}
//
//void TField::InitOne()
//{
//	TFieldElement* randomElement;
//	uint8_t b;
//	randomElement = this->GetElement(b);
//
//	for (int i=0; i<256; i++)
//	{
//		b=i;
//		randomElement = this->GetElement(b);
//
//		if(NTL::IsOne(randomElement->getElement()) == 1)
//		{
//			m_ONE = randomElement;
//			cout << randomElement->getElement();
//			return;
//		}
//	}
//}

TFieldElement* TField::GetZero()
{
	return m_ZERO;
}

TFieldElement* TField::GetOne()
{
//	InitOne();
	return m_ONE;
}

TField::~TField() {
	m_instanceFlag = false;
}


