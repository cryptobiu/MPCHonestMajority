//
// Created by meital on 18/04/17.
//

#ifndef MPCHONESTMAJORITY_HONESTMULTABSTRACT_H
#define MPCHONESTMAJORITY_HONESTMULTABSTRACT_H

#include <vector>
#include "TemplateField.h"

template <class FieldType>
class Protocol;

template <typename FieldType>
class HonestMultAbstract {

public:

    virtual void invokeOffline() = 0;

    virtual void mult(FieldType *a, FieldType *b, vector <FieldType> &cToFill, int numOfTrupples) = 0;
};

#endif //MPCHONESTMAJORITY_HONESTMULTABSTRACT_H