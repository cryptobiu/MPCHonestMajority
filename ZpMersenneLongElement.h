//
// Created by meital on 04/05/17.
//

#ifndef MPCHONESTMAJORITY_ZPMERSENNELONGELEMENT_H
#define MPCHONESTMAJORITY_ZPMERSENNELONGELEMENT_H

using namespace std;

class ZpMersenneLongElement {

//private:
public: //TODO return to private after tesing

    static const unsigned long p = 2305843009213693951;
    unsigned long elem;

public:

    ZpMersenneLongElement(){elem = 0;};
    ZpMersenneLongElement(unsigned long elem);

    ZpMersenneLongElement& operator=(const ZpMersenneLongElement& other);
    inline bool operator!=(const ZpMersenneLongElement& other){ return !(other.elem == elem); };

    ZpMersenneLongElement operator+(const ZpMersenneLongElement& f2);
    ZpMersenneLongElement operator-(const ZpMersenneLongElement& f2);
    ZpMersenneLongElement operator/(const ZpMersenneLongElement& f2);
    ZpMersenneLongElement operator*(const ZpMersenneLongElement& f2);

    inline ZpMersenneLongElement& operator+=(const ZpMersenneLongElement& f2){ elem = (f2.elem + elem) %p; return *this;};
    ZpMersenneLongElement& operator*=(const ZpMersenneLongElement& f2);





};

//inline ::ostream& operator<<(::ostream& s, const ZpMersenneLongElement& a){ return s << a.elem; };



#endif //MPCHONESTMAJORITY_ZPMERSENNELONGELEMENT_H
