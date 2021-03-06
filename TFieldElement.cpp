/*
 * TFieldElement.cpp
 *
 *  Created on: Aug 9, 2016
 *      Author: hila
 */

#include "TFieldElement.h"
class GF2XModulus;

using namespace NTL;

TFieldElement::TFieldElement() : m_poly(8) {
	 irreduciblePolynomial = BuildSparseIrred_GF2X(8);
}

TFieldElement::TFieldElement(NTL::GF2X poly) : m_poly(8) {
	irreduciblePolynomial = BuildSparseIrred_GF2X(8);
	this->m_poly = poly;
}

TFieldElement::TFieldElement(string str) : m_poly(8) {

	irreduciblePolynomial = BuildSparseIrred_GF2X(8);
	istringstream iss(str);
	iss >> this->m_poly;
}


NTL::GF2X& TFieldElement::getElement() {
	return m_poly;
}

TFieldElement TFieldElement::operator-(TFieldElement& f2)
{
	TFieldElement answer;
	NTL::GF2X f;
	f = this->getElement() - f2.getElement();
	answer.setPoly(f);
	return answer;
}

TFieldElement TFieldElement::operator+(TFieldElement& f2)
{
	TFieldElement answer;
	NTL::GF2X f;
	f = this->getElement() + f2.getElement();
	answer.setPoly(f);
	return answer;
}

void TFieldElement::operator+=(TFieldElement& f2)
{
	this->getElement() += f2.getElement();
}

void TFieldElement::operator*=(TFieldElement& f2)
{
	MulMod(this->getElement(), this->getElement(),  f2.getElement(),  this->irreduciblePolynomial);
}

TFieldElement TFieldElement::operator/(TFieldElement& f2)
{
	TFieldElement answer;
	NTL::GF2X f;
	NTL::GF2X rf;
	// x = a^{-1} % f, error is a is not invertible
    InvMod(f,f2.getElement() ,this->irreduciblePolynomial);
    MulMod(rf, f,  this->getElement(), this->irreduciblePolynomial);
	answer.setPoly(rf);
	return answer;
}
NTL::GF2X TFieldElement::getIrreducible()
{
    this->irreduciblePolynomial;
}

TFieldElement TFieldElement::operator*(TFieldElement& f2)
{
	TFieldElement answer;
	NTL::GF2X f;
	MulMod(f, this->getElement(),  f2.getElement(),  this->irreduciblePolynomial);
	answer.setPoly(f);
	return answer;
}

void TFieldElement::operator=(const TFieldElement& f2)
{
	this->setPoly(f2.m_poly);
}

int TFieldElement::setPoly(NTL::GF2X poly) {
	this->m_poly = poly;
	return 0;
}

string TFieldElement::toString()
{
    ostringstream stream;
    stream << m_poly;
    string str =  stream.str();
    return str;
}

TFieldElement::~TFieldElement() {}

