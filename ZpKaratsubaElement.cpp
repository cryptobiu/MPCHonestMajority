//
// Created by hila on 05/05/17.
//

#include "ZpKaratsubaElement.h"
#include "gmp.h"


unsigned long p = 1071482619497;



ZpKaratsubaElement::ZpKaratsubaElement()
{
    elem = 0;
}

ZpKaratsubaElement::ZpKaratsubaElement(long elem)
{
    this->elem = elem % p;

    if(this->elem<0){
        this->elem = elem + p;
    }
}

ZpKaratsubaElement ZpKaratsubaElement::operator+(const ZpKaratsubaElement& f2)
{
    ZpKaratsubaElement answer;

    answer.elem = (elem + f2.elem);

    if(answer.elem>=p)
        answer.elem-=p;

    return answer;
}

ZpKaratsubaElement ZpKaratsubaElement::operator-(const ZpKaratsubaElement& f2)
{
    ZpKaratsubaElement answer;

    long temp =  (long)elem - (long)f2.elem;

    if(temp<0){
        answer.elem = temp + p;
    }
    else{
        answer.elem = temp;
    }

    return answer;
}

ZpKaratsubaElement ZpKaratsubaElement::operator*(const ZpKaratsubaElement& f2)
{
    ZpKaratsubaElement answer;

    unsigned long x1 = elem >> 8; // the top 56 bit (should be only 32 bit)
    unsigned long y1 = f2.elem >> 8;

    unsigned long x0 = elem & 0xFF;
    unsigned long y0 = f2.elem & 0xFF;

    unsigned long u0 = x1 * y1;

    // mod p
    u0 = u0 % p;

    //unsigned long u1 = (u0 << 8) + (u0 << 16);

  //  unsigned long v0 = (x1 - x0) * (y1 - y0);

    // mod p
   // v0 = v0 % p;

    //unsigned long v1 = v0 << 8;

    unsigned long w0 = x0 * y0;

   // unsigned long w1 = (w0 << 8) + w0;

   // unsigned long result = ((u0 << 8) + (u0 << 16)) - (v0 << 8) + ((w0 << 8) + w0);

    // mod p
    answer.elem = (((u0 << 8) + (u0 << 16)) - ((((x1 - x0) * (y1 - y0)) % p) << 8) + ((w0 << 8) + w0)) % p;

    return answer;
}

ZpKaratsubaElement& ZpKaratsubaElement::operator=(const ZpKaratsubaElement& other) // copy assignment
{
    if (this != &other) { // self-assignment check expected
        elem = other.elem;
    }
    return *this;
}

/**
 * Euclid's extended algorithm:
 * ax + by = gcd(a,b)
 */
void gcd (unsigned long a, unsigned long b, unsigned long& gcd, unsigned long& x, unsigned long& y) {

    x = 0, y = 1;
    unsigned long u = 1, v = 0, m, n, q, r;
    gcd = b;
    while (a != 0) {
        q = gcd / a;
        r = gcd % a;
        m = x - u * q;
        n = y - v * q;
        gcd = a;
        a = r;
        x = u;
        y = v;
        u = m;
        v = n;
    }
}

/**
 * Modular division:
 * find z such that: z * B mod m == A.
 * If there is more than one (i.e. when gcd(B,m)>1) - returns the smallest such integer
 */
ZpKaratsubaElement ZpKaratsubaElement::operator/(const ZpKaratsubaElement& f2)
{

    ZpKaratsubaElement answer;
    mpz_t d;
    mpz_t result;
    mpz_t mpz_elem;
    mpz_t mpz_me;
    mpz_init_set_str (d, "1071482619497", 10);
    mpz_init(mpz_elem);
    mpz_init(mpz_me);


    //mpz_init_set(mpz_elem, f2.elem);
    //mpz_init_set(mpz_me, elem);

    mpz_set_ui(mpz_elem, f2.elem);
    mpz_set_ui(mpz_me, elem);

    mpz_init(result);

    mpz_invert ( result, mpz_elem, d );

    mpz_mul (result, result, mpz_me);
    mpz_mod (result, result, d);


    answer.elem = mpz_get_ui(result);

    cout << "result of div is : " << result << endl;
    cout << "result of modulo is : " << d << endl;

//    ZpKaratsubaElement answer;
//
//    unsigned long r_gcd, x, y;
//    gcd (f2.elem, p, r_gcd, x, y);  // x B + y m = gcd(B,m)
//    if (elem % r_gcd == 0) {
//        unsigned long q = elem / r_gcd;       // x q B + y q m = m gcd = A
//        answer.elem = ((x + p) * q) % (p / r_gcd);   // Return the smallest result possible
//        return answer;
//    }



    return answer;



}