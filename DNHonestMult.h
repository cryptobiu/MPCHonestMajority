//
// Created by meital on 18/04/17.
//

#ifndef MPCHONESTMAJORITY_DNHONESTMULT_H
#define MPCHONESTMAJORITY_DNHONESTMULT_H

#include "HonestMultAbstract.h"

template <typename FieldType>
class DNHonestMult : public HonestMultAbstract<FieldType> {

private:
    vector<FieldType> randomTAnd2TShares;
    int offset = 0;
    int numOfRandoms;
    Protocol<FieldType> *protocol;

public:

    DNHonestMult(int numOfRandoms, Protocol<FieldType> *protocol): numOfRandoms(numOfRandoms), protocol(protocol){};

    void invokeOffline();

    void mult(FieldType *a, FieldType *b, vector <FieldType> &cToFill, int numOfTrupples);
};

template <class FieldType>
void DNHonestMult<FieldType>::invokeOffline(){

    protocol->generateRandom2TAndTShares(numOfRandoms*2,randomTAnd2TShares);
}

template <class FieldType>
void DNHonestMult<FieldType>::mult(FieldType *a, FieldType *b, vector<FieldType> &cToFill, int numOfTrupples)
{
    int fieldByteSize = protocol->field->getElementSizeInBytes();
    vector<FieldType> xyMinusRShares(numOfTrupples);//hold both in the same vector to send in one batch
    vector<byte> xyMinusRSharesBytes(numOfTrupples *fieldByteSize);//hold both in the same vector to send in one batch

    vector<FieldType> xyMinusR;//hold both in the same vector to send in one batch
    vector<byte> xyMinusRBytes;

    vector<vector<byte>> recBufsBytes;


    //generate the shares for x+a and y+b. do it in the same array to send once
    for (int k = 0; k < numOfTrupples; k++)//go over only the logit gates
    {
        //compute the share of xy-r
        xyMinusRShares[k] = a[k]*b[k] - randomTAnd2TShares[offset + 2*k+1];

    }

    //set the acctual number of mult gate proccessed in this layer
    xyMinusRSharesBytes.resize(numOfTrupples*fieldByteSize);
    xyMinusR.resize(numOfTrupples);
    xyMinusRBytes.resize(numOfTrupples*fieldByteSize);

    for(int j=0; j<xyMinusRShares.size();j++) {
        protocol->field->elementToBytes(xyMinusRSharesBytes.data() + (j * fieldByteSize), xyMinusRShares[j]);
    }

    if (protocol->m_partyId == 1) {

        //just party 1 needs the recbuf
        recBufsBytes.resize(protocol->N);

        for (int i = 0; i < protocol->N; i++) {
            recBufsBytes[i].resize(numOfTrupples*fieldByteSize);
        }

        //receive the shares from all the other parties
        protocol->roundFunctionSyncForP1(xyMinusRSharesBytes, recBufsBytes);

    }
    else {//since I am not party 1 parties[0]->getID()=1

        //send the shares to p1
        protocol->parties[0]->getChannel()->write(xyMinusRSharesBytes.data(), xyMinusRSharesBytes.size());

    }

    //reconstruct the shares recieved from the other parties
    if (protocol->m_partyId == 1) {

        vector<FieldType> xyMinurAllShares(protocol->N);

        for (int k = 0;k < numOfTrupples; k++)//go over only the logit gates
        {
            for (int i = 0; i < protocol->N; i++) {

                xyMinurAllShares[i] = protocol->field->bytesToElement(recBufsBytes[i].data() + (k * fieldByteSize));
            }

            // reconstruct the shares by P0
            xyMinusR[k] = protocol->interpolate(xyMinurAllShares);

            //convert to bytes
            protocol->field->elementToBytes(xyMinusRBytes.data() + (k * fieldByteSize), xyMinusR[k]);

        }

        //send the reconstructed vector to all the other parties
        protocol->sendFromP1(xyMinusRBytes);
    }

    else {//each party get the xy-r reconstruced vector from party 1

        protocol->parties[0]->getChannel()->read(xyMinusRBytes.data(), xyMinusRBytes.size());
    }




    //fill the xPlusAAndYPlusB array for all the parties except for party 1 that already have this array filled
    if (protocol->m_partyId != 1) {

        for (int i = 0; i < numOfTrupples; i++) {

            xyMinusR[i] = protocol->field->bytesToElement(xyMinusRBytes.data() + (i * fieldByteSize));
        }
    }

    //after the xPlusAAndYPlusB array is filled, we are ready to fill the output of the mult gates
    for (int k = 0; k < numOfTrupples; k++)//go over only the logit gates
    {
        cToFill[k] = randomTAnd2TShares[offset + 2*k] + xyMinusR[k];
    }

    offset+=numOfTrupples*2;

}


#endif //MPCHONESTMAJORITY_DNHONESTMULT_H
