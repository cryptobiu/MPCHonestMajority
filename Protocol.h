#ifndef PROTOCOL_H_
#define PROTOCOL_H_

#include <stdlib.h>

#include "VDM.h"
#include "VDMTranspose.h"
#include "HIM.h"
#include "TGate.h"
#include "ArithmeticCircuit.h"
#include <vector>
#include <iostream>
#include <fstream>
#include <chrono>
#include "TemplateField.h"
#include "ProtocolTimer.h"
#include "MPCCommunication.h"
#include "../../include/infra/Common.hpp"
#include "../../include/primitives/Prg.hpp"
#include <thread>

#define flag_print false
#define flag_print_timings true
#define flag_print_output true


using namespace std;
using namespace std::chrono;

template <class FieldType>
class Protocol {
private:
    /**
     * N - number of parties
     * M - number of gates
     * T - number of malicious
     */
    ProtocolTimer* protocolTimer;
    int currentCirciutLayer = 0;
    int N, M, T, m_partyId;
    string s;
    int numOfInputGates, numOfOutputGates;
    string inputsFile, outputFile, ADDRESS;
    vector<FieldType> beta;
    HIM<FieldType> matrix_for_interpolate;
    HIM<FieldType> matrix_for_t;
    HIM<FieldType> matrix_for_2t;
    TemplateField<FieldType> *field;

    HIM<FieldType> matrix_him;
    VDM<FieldType> matrix_vand;
    VDMTranspose<FieldType> matrix_vand_transpose;

    HIM<FieldType> m;
    vector<FieldType> firstRowVandInverse;


    vector<FieldType> randomABShares;//a, b random shares
    vector<FieldType> c;//a vector of a*b shares

    vector<FieldType> randomTSharesOfflineMult;//a, b random shares
    vector<FieldType> cOfflineMult;//a vector of a*b shares


    vector<shared_ptr<ProtocolPartyData>>  parties;
    boost::asio::io_service io_service;
    ArithmeticCircuit circuit;
    vector<FieldType> gateValueArr; // the value of the gate (for my input and output gates)
    vector<FieldType> gateShareArr; // my share of the gate (for all gates)
    vector<FieldType> alpha; // N distinct non-zero field elements

    vector<FieldType> sharingBufTElements; // prepared T-sharings (my shares)
    vector<FieldType> sharingBuf2TElements; // prepared 2T-sharings (my shares)
    int shareIndex;


    vector<int> myInputs;

public:
    Protocol(int n, int id,TemplateField<FieldType> *field, string inputsFile, string outputFile, string circuitFile, string address,
             ProtocolTimer* protocolTimer);


    void roundFunctionSync(vector<vector<byte>> &sendBufs, vector<vector<byte>> &recBufs, int round);
    void exchangeData(vector<vector<byte>> &sendBufs,vector<vector<byte>> &recBufs, int first, int last);
    void roundFunctionSyncBroadcast(vector<byte> &message, vector<vector<byte>> &recBufs);
    void recData(vector<byte> &message, vector<vector<byte>> &recBufs, int first, int last);
    void roundFunctionSyncForP1(vector<byte> &myShare, vector<vector<byte>> &recBufs);
    void recDataToP1(vector<vector<byte>> &recBufs, int first, int last);

    void sendDataFromP1(vector<byte> &sendBuf, int first, int last);
    void sendFromP1(vector<byte> &sendBuf);


    /**
     * This method runs the protocol:
     * 1. Initialization Phase
     * 2. Preparation Phase
     * 3. Input Phase
     * 4. Computation Phase
     * 5. Verification Phase
     * 6. Output Phase
     */
    void run(int iteration);

    /**
     * This method reads text file and inits a vector of Inputs according to the file.
     */
    void readMyInputs();

    /**
     * We describe the protocol initialization.
     * In particular, some global variables are declared and initialized.
     */
    void initializationPhase();

    /**
     * Generates the first row of the inverse of the vandermonde matrix and saves it
     * in firstRowVandInverse for future use.
     */
    void initFirstRowInvVDM();

    /**
     * A random double-sharing is a pair of two sharings of the same random value, where the one sharing is
     * of degree t, and the other sharing is of degree 2t. Such random double-sharing are of big help in the
     * multiplication protocol.
     * We use hyper-invertible matrices to generate random double-sharings. The basic idea is as follows:
     * Every party generates one random double-sharing. These n double-sharings are processes through a
     * hyper-invertible matrix. From the resulting n double-sharings, t are checked to be valid (correct degree,
     * same secret), and t are then kept as “good” double-sharings. This is secure due to the diversion property
     * of hyper-invertible matrix: We know that n − t of the input double-sharings are good. So, if there are t
     * valid output double-sharings, then all double-sharings must be valid. Furthermore, the adversary knows
     * his own up to t input double-sharings, and learns t output double sharings. So, n − 2t output double
     * sharings are random and unknown to the adversary.
     * For the sake of efficiency, we do not publicly reconstruct t of the output double-sharings. Rather, we
     * reconstruct 2t output double sharings, each to one dedicated party only. At least t of these parties are
     * honest and correctly validate the reconstructed double-sharing.
     *
     * The goal of this phase is to generate “enough” double-sharings to evaluate the circuit. The double-
     * sharings are stored in a buffer SharingBuf , where alternating a degree-t and a degree-2t sharing (of the same secret)
     * is stored (more precisely, a share of each such corresponding sharings is stored).
     * The creation of double-sharings is:
     *
     * Protocol Generate-Double-Sharings:
     * 1. ∀i: Pi selects random value x-(i) and computes degree-t shares x1-(i) and degree-2t shares x2-(i).
     * 2. ∀i,j: Pi sends the shares x1,j and X2,j to party Pj.
     * 3. ∀j: Pj applies a hyper-invertible matrix M on the received shares, i.e:
     *      (y1,j,..., y1,j) = M(x1,j,...,x1,j)
     *      (y2,j,...,y2,j) = M (x2,j,...,x2,)
     * 4. ∀j, ∀k ≤ 2t: Pj sends y1,j and y2,j to Pk.
     * 5. ∀k ≤ 2t: Pk checks:
     *      • that the received shares (y1,1,...,y1,n) are t-consistent,
     *      • that the received shares (y2,1,...,y2,n) are 2t-consistent, and
     *      • that both sharings interpolate to the same secret.
     *
     * We use this algorithm, but extend it to capture an arbitrary number of double-sharings.
     * This is, as usual, achieved by processing multiple buckets in parallel.
     */
    bool preparationPhase();

    /**
     * The aim of this protocol is to produce random triples that will be later used in the main protocol.
     * The protocol takes as input an integer numOfTriples (the number of triples to output), and proceeds as follows.
     * 1. The parties call a protocol to generate random shares to obtain 2L random
     * 2. The parties run a semi-honest multiplication sub protocol to obtain a sharing of a*b
     */
    void generateBeaverTriples(int numOfTriples);

    /**
     * This protocol is secure only in the presence of a semi-honest adversary.
     */
    void GRRHonestMultiplication(FieldType *a, FieldType *b, vector<FieldType> &cToFill, int numOfTrupples);

    void offlineDNForMultiplication(int numOfTriples);

    /**
     * We do not need robust broadcast (which we require an involved and expensive sub-protocol).
     * As we allow abort, broadcast can be achieved quite easily.
     * Note that the trivial approach (distribute value, one round of echoing) requires quadratic communication,
     * which is too much.
     * Goal: One party Ps wants to broadcast n−t values x = (x1,...,xn−t).
     *
     * Protocol Broadcast:
     * 0. Ps holds input vector x = (x1,...,xn−t).
     * 1. ∀j: Ps sends x to Pj. Denote the received vector as x(j) (P j -th view on the sent vector).
     * 2. ∀j: Pj applies hyper-invertible matrix M : (y1(j),...,yn(j))= M*(x1(j),..,xn−t(j),0,...,0).
     * 3. ∀j,k: Pj sends yk to Pk.
     * 4. ∀k: Pk checks whether all received values {yk(j)}j are equal. If so, be happy, otherwise cry.
     */
    bool broadcast(int party_id, vector<byte> myMessage, vector<vector<byte>> &recBufsdiffBytes, HIM<FieldType> &mat);


    /**
     * The input phase proceeds in two steps:
     * First, for each input gate, the party owning the input creates shares for that input by choosing a random coefficients for the polynomial
     * Then, all the shares are sent to the relevant party
     */
    void inputPhase();

    void generateRandomShares(int numOfRnadoms, vector<FieldType>& randomElementsToFill);
    void generateRandom2TAndTShares(int numOfRandomPairs, vector<FieldType>& randomElementsToFill);



    /**
     * Check whether given points lie on polynomial of degree d.
     * This check is performed by interpolating x on the first d + 1 positions of α and check the remaining positions.
     */
    bool checkConsistency(vector<FieldType>& x, int d);

    FieldType reconstructShare(vector<FieldType>& x, int d);

    void openShare(int numOfRandomShares, vector<FieldType> &Shares, vector<FieldType> &secrets);

    /**
     * Process all additions which are ready.
     * Return number of processed gates.
     */
    int processAdditions();


    /**
     * Process all subtractions which are ready.
     * Return number of processed gates.
     */
    int processSubtractions();

    /**
     * Process all multiplications which are ready.
     * Return number of processed gates.
     */
    int processMultiplications(int lastMultGate);

    int processMultGRR();

    int processMultDN(int indexInRandomArray);

    /**
     * Process all random gates.
     */
    void processRandoms();

    int processSmul();

    int processNotMult();

    /**
     * Walk through the circuit and evaluate the gates. Always take as many gates at once as possible,
     * i.e., all gates whose inputs are ready.
     * We first process all random gates, then alternately process addition and multiplication gates.
     */
    void computationPhase(HIM<FieldType> &m);

    /**
     * The cheap way: Create a HIM from the αi’s onto ZERO (this is actually a row vector), and multiply
     * this HIM with the given x-vector (this is actually a scalar product).
     * The first (and only) element of the output vector is the secret.
     */
    FieldType interpolate(vector<FieldType> x);

    FieldType tinterpolate(vector<FieldType> x);


    /**
     * Walk through the circuit and verify the multiplication gates.
     * We first generate the random elements using a common AES key that was generated by the parties,
     * run the relevant verification algorithm and return accept/reject according to the output
     * of the verification algorithm.
     */
    void verificationPhase();


    vector<byte> generateCommonKey();
    void generatePseudoRandomElements(vector<byte> & aesKey, vector<FieldType> &randomElementsToFill, int numOfRandomElements);

    bool verificationOfBatchedTriples(FieldType *x, FieldType *y, FieldType *z,
                                      FieldType *a, FieldType *b, FieldType *c,
                                      vector<FieldType> randomElements, int numOfTriples);

    bool verificationOfSingleTriples(FieldType *x, FieldType *y, FieldType *z,
                                      FieldType *a, FieldType *b, FieldType *c,
                                      vector<FieldType> randomElements, int numOfTriples);


    /**
     * Walk through the circuit and reconstruct output gates.
     */
    void outputPhase();

    ~Protocol();


};


template <class FieldType>
Protocol<FieldType>::Protocol(int n, int id, TemplateField<FieldType> *field, string inputsFile, string outputFile, string circuitFile, string address,
                              ProtocolTimer* protocolTimer)
{
    this->protocolTimer = protocolTimer;
    this->field = field;
    ADDRESS = address;
    //comm = Communication::getInstance(n, id, address);
    N = n;
    T = n/3 - 1;
    this->inputsFile = inputsFile;
    this->outputFile = outputFile;
    if(n%3 > 0)
    {
        T++;
    }
    m_partyId = id;
    s = to_string(m_partyId);
    circuit.readCircuit(circuitFile.c_str());
    circuit.reArrangeCircuit();
    M = circuit.getNrOfGates();
    numOfInputGates = circuit.getNrOfInputGates();
    numOfOutputGates = circuit.getNrOfOutputGates();
    myInputs.resize(numOfInputGates);
    shareIndex = numOfInputGates;


    //comm->ConnectionToServer(s);

    //boost::asio::io_service io_service;

    parties = MPCCommunication::setCommunication(io_service, m_partyId-1, N, "Parties.txt");

    string tmp = "init times";
    //cout<<"before sending any data"<<endl;
    byte tmpBytes[20];
    for (int i=0; i<parties.size(); i++){
        if (parties[i]->getID() < id){
            parties[i]->getChannel()->write(tmp);
            parties[i]->getChannel()->read(tmpBytes, tmp.size());
        } else {
            parties[i]->getChannel()->read(tmpBytes, tmp.size());
            parties[i]->getChannel()->write(tmp);
        }
    }

    //cout<<"after sending any data"<<endl;


    readMyInputs();

    //cout<<"after read my inputs"<<endl;

    auto t1 = high_resolution_clock::now();
    initializationPhase(/*matrix_him, matrix_vand, m*/);

    auto t2 = high_resolution_clock::now();

    auto duration = duration_cast<milliseconds>(t2-t1).count();
    if(flag_print_timings) {
        cout << "time in milliseconds initializationPhase: " << duration << endl;
    }
}


/**
 * Protocol Broadcast:
 *  0. Ps holds input vector x = (X1,...,Xn−t).
 *  1. ∀j: Ps sends x to Pj . Denote the received vector as x (j) (P j -th view on the sent vector).
 *  2. ∀j: Pj applies hyper-invertible matrix M and calculate (y1,...,yn) = M(X1,...,Xn−t,0,...,0), padding t zeroes.
 *  3. ∀j,k: Pj sends yk to Pk .
 *  4. ∀k: Pk checks whether all received values are equal. If so, be happy, otherwise cry.
 *
 *  This protocol uses when Ps wants to broadcast exactly n−t values.
 *  if Ps wants more than n-t values we divide the values to buckes.
 *  Every bucket contains n-t values.
 *
 *  @param myMessage = vector of all the values which Ps wants to broadcast.
 *  @param recBufsdiff = the values which received from the protocol.
 */
template <class FieldType>
bool Protocol<FieldType>::broadcast(int party_id, vector<byte> myMessage, vector<vector<byte>> &recBufsdiffBytes, HIM<FieldType> &mat)
{
    int no_buckets;
    vector<vector<byte>> sendBufsBytes(N);
    vector<vector<FieldType>> sendBufsElements(N);
    vector<vector<FieldType>> recBufsElements(N);

    vector<vector<byte>> recBufs2Bytes(N);
    vector<vector<FieldType>> recBufs2Elements(N);


    // Ps sends his values to all parties and received there values.
    //comm->roundfunction2(myMessage, recBufsdiffBytes); // Values are in recBufsdiff
    roundFunctionSyncBroadcast(myMessage, recBufsdiffBytes);

    //turn the recbuf into recbuf of elements
    int fieldByteSize = field->getElementSizeInBytes();
    for(int i=0; i < N; i++)
    {
        recBufsElements[i].resize((recBufsdiffBytes[i].size()) / fieldByteSize);
        for(int j=0; j<recBufsElements[i].size();j++) {
            recBufsElements[i][j] = field->bytesToElement(recBufsdiffBytes[i].data() + ( j * fieldByteSize));
        }
    }


    if(flag_print) {
        cout << "recBufsdiff" << endl;
        for (int i = 0; i < N; i++) {
            //cout << i << "  " << recBufsdiff[i] << endl;
        }
    }

    vector<FieldType> X1(N);
    vector<FieldType> Y1(N);

    // calculate total number of values which received
    int count = 0;
    for(int i=0; i< N; i++)
    {
        count+=recBufsElements[i].size();
    }


    vector<FieldType> valBufs(count);
    int index = 0;

    // concatenate everything
    for(int l=0; l< N; l++)
    {
        for (int i = 0; i < recBufsElements[l].size() ; i++) {
            valBufs[index] = recBufsElements[l][i];
            index++;
        }
    }

    index = 0;

    if(flag_print) {
        cout << "valBufs " <<endl;
        for(int k = 0; k < count; k++)
        {
            cout << "valBufs " << k << " " << valBufs[k] << endl;
        }
    }

    // nr of buckets
    no_buckets = count / (N - T) + 1; // nr of buckets


    for(int i = 0; i < N; i++)
    {
        sendBufsElements[i].resize(no_buckets);
    }

    if(flag_print) {
        cout << " before the for " << '\n';}

    for(int k = 0; k < no_buckets; k++)
    {
        for(int i = 0; i < N; i++)
        {
            if((i < N-T) && (k*(N-T)+i < count))
            {
                //X1[i]= field->stringToElement(valBufs[index]);
                X1[i]= valBufs[index];
                index++;
            }
            else
            {
                // padding zero
                X1[i] = *(field->GetZero());
            }
        }

        if(flag_print) {
            for(int i = 0; i < N; i++)
            {
                cout << "X1[i]" << i << " " << field->elementToString(X1[i]) << endl;
            } }

        // x1 contains (up to) N-T values from ValBuf
        mat.MatrixMult(X1, Y1); // no cheating: all parties have same y1

        if(flag_print) {
            cout << "X1[i] after mult" << endl;}

        // ‘‘Reconstruct’’ values towards some party (‘‘reconstruct’’ with degree 0)
        if(flag_print) {
            for(int i = 0; i < N; i++)
            {
                cout << "X1[i]" << i << " " << field->elementToString(X1[i])<< endl;
            } }
        for(int i = 0; i < N; i++) {

           sendBufsElements[i][k] = Y1[i];
        }
        for(int i = 0; i < N; i++)
        {
            X1[i] = *(field->GetZero());
            Y1[i] = *(field->GetZero());
        }
    }

    if(flag_print) {
        cout << "index  2 time :" << index << '\n';

        cout  << "before roundfunction3 " << endl;
        for(int k=0; k< N; k++) {
           // cout << k << "  " << buffers[k] << endl;
        }}


    for(int i=0; i < N; i++)
    {
        sendBufsBytes[i].resize(no_buckets*fieldByteSize);
        recBufs2Bytes[i].resize(no_buckets*fieldByteSize);
        for(int j=0; j<no_buckets;j++) {
            field->elementToBytes(sendBufsBytes[i].data() + (j * fieldByteSize), sendBufsElements[i][j]);
        }
    }


    roundFunctionSync(sendBufsBytes, recBufs2Bytes,3);
    //comm->roundfunctionI(sendBufsBytes, recBufs2Bytes,3);

    for(int i=0; i < N; i++)
    {
        recBufs2Elements[i].resize((recBufs2Bytes[i].size()) / fieldByteSize);
        for(int j=0; j<recBufs2Elements[i].size();j++) {
            recBufs2Elements[i][j] = field->bytesToElement(recBufs2Bytes[i].data() + ( j * fieldByteSize));
        }
    }


    if(flag_print) {
        cout  << "after roundfunction3 " << endl;
        for(int k=0; k< N; k++) {
            //cout << k << "  " << recBufs2[k] << endl;
        }

        cout << "no_buckets  " << no_buckets << endl;}
    FieldType temp1;

    // no cheating: all parties have same y1
    // ‘‘Reconstruct’’ values towards some party (‘‘reconstruct’’ with degree 0)
    for(int k=0; k < no_buckets; k++) {
        if(flag_print) {
            cout << "fff  " << k<< endl;}
        if(recBufs2Elements[0].size() > 0) {
            temp1 = recBufs2Elements[0][k];
            //  arr.size()-1
            for (int i = 1; i < N; i++) {
                if(temp1 != recBufs2Elements[i][k])
                {
                    // cheating detected!!!
                    if(flag_print) {
                        cout << "                 cheating" << endl;}
                    return false;
                }
            }
        }
    }

    return true;
}

template <class FieldType>
void Protocol<FieldType>::readMyInputs()
{

    //cout<<"inputs file" << inputsFile<<endl;
    ifstream myfile;
    int input;
    int i =0;
    myfile.open(inputsFile);
    do {
        myfile >> input;
        myInputs[i] = input;
        i++;
    } while(!(myfile.eof()));
    myfile.close();
    //cout<<"after read inputs" <<endl;

}

template <class FieldType>
void Protocol<FieldType>::run(int iteration) {

    shareIndex = numOfInputGates;

    auto t1start = high_resolution_clock::now();
    auto t1 = high_resolution_clock::now();
    if(preparationPhase() == false) {
        if(flag_print) {
            cout << "cheating!!!" << '\n';}
        return;
    }
    else {
        if(flag_print) {
            cout << "no cheating!!!" << '\n' << "finish Preparation Phase" << '\n';}
    }

    auto t2 = high_resolution_clock::now();

    auto duration = duration_cast<milliseconds>(t2-t1).count();
    if(flag_print_timings) {
        cout << "time in milliseconds preparationPhase: " << duration << endl;
    }
    protocolTimer->preparationPhaseArr[iteration] =duration;





    t1 = high_resolution_clock::now();

    inputPhase();

    t2 = high_resolution_clock::now();

    duration = duration_cast<milliseconds>(t2-t1).count();
    protocolTimer->inputPreparationArr[iteration] = duration;
    if(flag_print_timings) {
        cout << "time in milliseconds inputPhase: " << duration << endl;
    }

    string sss = "";


     t1 = high_resolution_clock::now();

    computationPhase(m);

    t2 = high_resolution_clock::now();

    duration = duration_cast<milliseconds>(t2-t1).count();
    protocolTimer->computationPhaseArr[iteration] = duration;



    if(flag_print_timings) {
        cout << "time in milliseconds computationPhase: " << duration << endl;
    }

    t1 = high_resolution_clock::now();

    verificationPhase();

    t2 = high_resolution_clock::now();
    duration = duration_cast<milliseconds>(t2-t1).count();
    //protocolTimer->outputPhaseArr[iteration] = duration;

    if(flag_print_timings) {
        cout << "time in milliseconds verificationPhase: " << duration << endl;
    }

    t1 = high_resolution_clock::now();

    outputPhase();

    t2 = high_resolution_clock::now();

    duration = duration_cast<milliseconds>(t2-t1).count();
    protocolTimer->outputPhaseArr[iteration] = duration;

    if(flag_print_timings) {
        cout << "time in milliseconds outputPhase: " << duration << endl;
    }
    auto t2end = high_resolution_clock::now();

    duration = duration_cast<milliseconds>(t2end-t1start).count();
    protocolTimer->totalTimeArr[iteration] = duration;

    cout << "time in milliseconds for protocol: " << duration << endl;

}

template <class FieldType>
void Protocol<FieldType>::computationPhase(HIM<FieldType> &m) {
    int count = 0;
    int countNumMult = 0;
    int countNumMultForThisLayer = 0;
    //processRandoms();

    int numOfLayers = circuit.getLayers().size();
    for(int i=0; i<numOfLayers-1;i++){
//        count = processSmul();
//        count += processAdditions();
//        count += processSubtractions();
//        count += processMultiplications(m);

        currentCirciutLayer = i;
        count = processNotMult();

        countNumMultForThisLayer = processMultiplications(countNumMult);//send the index of the current mult gate
        countNumMult += countNumMultForThisLayer;;
        count+=countNumMultForThisLayer;

    }
}

/**
 * the function implements the second step of Input Phase:
 * the party broadcasts for each input gate the difference between
 * the random secret and the actual input value.
 * @param diff
 */
template <class FieldType>
void Protocol<FieldType>::inputPhase()
{


    int robin = 0;

    // the number of random double sharings we need altogether
    vector<FieldType> x1(N),y1(N);
    vector<vector<FieldType>> sendBufsElements(N);
    vector<vector<byte>> sendBufsBytes(N);
    vector<vector<byte>> recBufBytes(N);
    vector<vector<FieldType>> recBufElements(N);


    int input;
    int index = 0;
    vector<int> sizes(N);

    // prepare the shares for the input
    for (int k = 0; k < numOfInputGates; k++)
    {
        if(circuit.getGates()[k].gateType == INPUT) {
            //get the expected sized from the other parties
            sizes[circuit.getGates()[k].party - 1]++;

            if (circuit.getGates()[k].party == m_partyId) {
                auto input = myInputs[index];
                index++;
                if (flag_print) {
                    cout << "input  " << input << endl;
                }
                // the value of a_0 is the input of the party.
                x1[0] = field->GetElement(input);


                // generate random degree-T polynomial
                for(int i = 1; i < T+1; i++)
                {
                    // A random field element, uniform distribution
                    x1[i] = field->Random();

                }


                matrix_vand.MatrixMult(x1, y1, T+1); // eval poly at alpha-positions predefined to be alpha_i = i

                // prepare shares to be sent
                for(int i=0; i < N; i++)
                {
                    //cout << "y1[ " <<i<< "]" <<y1[i] << endl;
                    sendBufsElements[i].push_back(y1[i]);

                }
            }
        }
    }

    int fieldByteSize = field->getElementSizeInBytes();
    for(int i=0; i < N; i++)
    {
        sendBufsBytes[i].resize(sendBufsElements[i].size()*fieldByteSize);
        //cout<< "size of sendBufs1Elements["<<i<<" ].size() is " << sendBufs1Elements[i].size() <<"myID =" <<  m_partyId<<endl;
        recBufBytes[i].resize(sizes[i]*fieldByteSize);
        for(int j=0; j<sendBufsElements[i].size();j++) {
            field->elementToBytes(sendBufsBytes[i].data() + (j * fieldByteSize), sendBufsElements[i][j]);
        }
    }


    roundFunctionSync(sendBufsBytes, recBufBytes,10);


    //turn the bytes to elements
    for(int i=0; i < N; i++)
    {
        recBufElements[i].resize((recBufBytes[i].size()) / fieldByteSize);
        for(int j=0; j<recBufElements[i].size();j++) {
            recBufElements[i][j] = field->bytesToElement(recBufBytes[i].data() + ( j * fieldByteSize));
        }
    }



    vector<int> counters(N);

    for(int i=0; i<N; i++){
        counters[i] =0;
    }

    for (int k = 0; k < numOfInputGates; k++)
    {
        if(circuit.getGates()[k].gateType == INPUT)
        {
            auto share = recBufElements[circuit.getGates()[k].party - 1][counters[circuit.getGates()[k].party - 1]];
            counters[circuit.getGates()[k].party - 1] += 1;
            gateShareArr[circuit.getGates()[k].output] = share; // set the share sent from the party owning the input

        }
    }


}



template <class FieldType>
void Protocol<FieldType>::generateRandomShares(int numOfRandoms, vector<FieldType>& randomElementsToFill){


    int index = 0;
    vector<vector<byte>> recBufsBytes(N);
    int robin = 0;
    int no_random = numOfRandoms;

    vector<FieldType> x1(N),y1(N), x2(N),y2(N), t1(N), r1(N), t2(N), r2(N);;

    vector<vector<FieldType>> sendBufsElements(N);
    vector<vector<byte>> sendBufsBytes(N);

    // the number of buckets (each bucket requires one double-sharing
    // from each party and gives N-2T random double-sharings)
    int no_buckets = (no_random / (N-T))+1;

    //sharingBufTElements.resize(no_buckets*(N-2*T)); // my shares of the double-sharings
    //sharingBuf2TElements.resize(no_buckets*(N-2*T)); // my shares of the double-sharings

    //maybe add some elements if a partial bucket is needed
    randomElementsToFill.resize(no_buckets*(N-T));


    for(int i=0; i < N; i++)
    {
        sendBufsElements[i].resize(no_buckets);
        sendBufsBytes[i].resize(no_buckets*field->getElementSizeInBytes());
        recBufsBytes[i].resize(no_buckets*field->getElementSizeInBytes());
    }

    /**
     *  generate random sharings.
     *  first degree t.
     *
     */
    for(int k=0; k < no_buckets; k++)
    {
        // generate random degree-T polynomial
        for(int i = 0; i < T+1; i++)
        {
            // A random field element, uniform distribution, note that x1[0] is the secret which is also random
            x1[i] = field->Random();

        }

        matrix_vand.MatrixMult(x1, y1,T+1); // eval poly at alpha-positions

        // prepare shares to be sent
        for(int i=0; i < N; i++)
        {
            //cout << "y1[ " <<i<< "]" <<y1[i] << endl;
            sendBufsElements[i][k] = y1[i];

        }
    }

    if(flag_print) {
        for (int i = 0; i < N; i++) {
            for (int k = 0; k < sendBufsElements[0].size(); k++) {

                // cout << "before roundfunction4 send to " <<i <<" element: "<< k << " " << sendBufsElements[i][k] << endl;
            }
        }
        cout << "sendBufs" << endl;
        cout << "N" << N << endl;
        cout << "T" << T << endl;
    }

    int fieldByteSize = field->getElementSizeInBytes();
    for(int i=0; i < N; i++)
    {
        for(int j=0; j<sendBufsElements[i].size();j++) {
            field->elementToBytes(sendBufsBytes[i].data() + (j * fieldByteSize), sendBufsElements[i][j]);
        }
    }

    roundFunctionSync(sendBufsBytes, recBufsBytes,4);


    if(flag_print) {
        for (int i = 0; i < N; i++) {
            for (int k = 0; k < sendBufsBytes[0].size(); k++) {

                cout << "roundfunction4 send to " <<i <<" element: "<< k << " " << (int)sendBufsBytes[i][k] << endl;
            }
        }
        for (int i = 0; i < N; i++) {
            for (int k = 0; k < recBufsBytes[0].size(); k++) {
                cout << "roundfunction4 receive from " <<i <<" element: "<< k << " " << (int) recBufsBytes[i][k] << endl;
            }
        }
    }

    for(int k=0; k < no_buckets; k++) {
        for (int i = 0; i < N; i++) {
            t1[i] = field->bytesToElement(recBufsBytes[i].data() + (k * fieldByteSize));

        }
        matrix_vand_transpose.MatrixMult(t1, r1,N-T);

        //copy the resulting vector to the array of randoms
        for (int i = 0; i < N - T; i++) {

            randomElementsToFill[index] = r1[i];
            index++;

        }
    }

}


template <class FieldType>
void Protocol<FieldType>::generateRandom2TAndTShares(int numOfRandomPairs, vector<FieldType>& randomElementsToFill){


    int index = 0;
    vector<vector<byte>> recBufsBytes(N);
    int robin = 0;
    int no_random = numOfRandomPairs;

    vector<FieldType> x1(N),y1(N), x2(N),y2(N), t1(N), r1(N), t2(N), r2(N);;

    vector<vector<FieldType>> sendBufsElements(N);
    vector<vector<byte>> sendBufsBytes(N);

    // the number of buckets (each bucket requires one double-sharing
    // from each party and gives N-2T random double-sharings)
    int no_buckets = (no_random / (N-T))+1;

    //sharingBufTElements.resize(no_buckets*(N-2*T)); // my shares of the double-sharings
    //sharingBuf2TElements.resize(no_buckets*(N-2*T)); // my shares of the double-sharings

    //maybe add some elements if a partial bucket is needed
    randomElementsToFill.resize(no_buckets*(N-T)*2);


    for(int i=0; i < N; i++)
    {
        sendBufsElements[i].resize(no_buckets*2);
        sendBufsBytes[i].resize(no_buckets*field->getElementSizeInBytes()*2);
        recBufsBytes[i].resize(no_buckets*field->getElementSizeInBytes()*2);
    }

    /**
     *  generate random sharings.
     *  first degree t.
     *
     */
    for(int k=0; k < no_buckets; k++)
    {
        // generate random degree-T polynomial
        for(int i = 0; i < T+1; i++)
        {
            // A random field element, uniform distribution, note that x1[0] is the secret which is also random
            x1[i] = field->Random();

        }

        matrix_vand.MatrixMult(x1, y1,T+1); // eval poly at alpha-positions

        x2[0] = x1[0];
        // generate random degree-T polynomial
        for(int i = 1; i < 2*T+1; i++)
        {
            // A random field element, uniform distribution, note that x1[0] is the secret which is also random
            x2[i] = field->Random();

        }

        // prepare shares to be sent
        for(int i=0; i < N; i++)
        {
            //cout << "y1[ " <<i<< "]" <<y1[i] << endl;
            sendBufsElements[i][2*k] = y1[i];
            sendBufsElements[i][2*k + 1] = y2[i];

        }
    }

    if(flag_print) {
        for (int i = 0; i < N; i++) {
            for (int k = 0; k < sendBufsElements[0].size(); k++) {

                // cout << "before roundfunction4 send to " <<i <<" element: "<< k << " " << sendBufsElements[i][k] << endl;
            }
        }
        cout << "sendBufs" << endl;
        cout << "N" << N << endl;
        cout << "T" << T << endl;
    }

    int fieldByteSize = field->getElementSizeInBytes();
    for(int i=0; i < N; i++)
    {
        for(int j=0; j<sendBufsElements[i].size();j++) {
            field->elementToBytes(sendBufsBytes[i].data() + (j * fieldByteSize), sendBufsElements[i][j]);
        }
    }

    roundFunctionSync(sendBufsBytes, recBufsBytes,4);


    if(flag_print) {
        for (int i = 0; i < N; i++) {
            for (int k = 0; k < sendBufsBytes[0].size(); k++) {

                cout << "roundfunction4 send to " <<i <<" element: "<< k << " " << (int)sendBufsBytes[i][k] << endl;
            }
        }
        for (int i = 0; i < N; i++) {
            for (int k = 0; k < recBufsBytes[0].size(); k++) {
                cout << "roundfunction4 receive from " <<i <<" element: "<< k << " " << (int) recBufsBytes[i][k] << endl;
            }
        }
    }

    for(int k=0; k < no_buckets; k++) {
        for (int i = 0; i < N; i++) {
            t1[i] = field->bytesToElement(recBufsBytes[i].data() + (2*k * fieldByteSize));
            t2[i] = field->bytesToElement(recBufsBytes[i].data() + ((2*k +1) * fieldByteSize));

        }
        matrix_vand_transpose.MatrixMult(t1, r1,N-T);
        matrix_vand_transpose.MatrixMult(t2, r2,N-T);

        //copy the resulting vector to the array of randoms
        for (int i = 0; i < (N - T); i++) {

            randomElementsToFill[index*2] = r1[i];
            randomElementsToFill[index*2 +1] = r2[i];
            index++;

        }
    }

}

/**
 * some global variables are initialized
 * @param GateValueArr
 * @param GateShareArr
 * @param matrix_him
 * @param matrix_vand
 * @param alpha
 */
template <class FieldType>
void Protocol<FieldType>::initializationPhase()
{
    beta.resize(1);
    gateValueArr.resize(M);  // the value of the gate (for my input and output gates)
    gateShareArr.resize(M - circuit.getNrOfOutputGates()); // my share of the gate (for all gates)
    alpha.resize(N); // N distinct non-zero field elements
    vector<FieldType> alpha1(N-T);
    vector<FieldType> alpha2(T);

    beta[0] = field->GetElement(0); // zero of the field
    matrix_for_interpolate.allocate(1,N, field);


    matrix_him.allocate(N,N,field);
    matrix_vand.allocate(N,N,field);
    matrix_vand_transpose.allocate(N,N,field);
    m.allocate(T, N-T,field);

    // Compute Vandermonde matrix VDM[i,k] = alpha[i]^k
    matrix_vand.InitVDM();
    matrix_vand_transpose.InitVDMTranspose();

    // Prepare an N-by-N hyper-invertible matrix
    matrix_him.InitHIM();

    // N distinct non-zero field elements
    for(int i=0; i<N; i++)
    {
        alpha[i]=field->GetElement(i+1);
    }

    for(int i = 0; i < N-T; i++)
    {
        alpha1[i] = alpha[i];
    }
    for(int i = N-T; i < N; i++)
    {
        alpha2[i - (N-T)] = alpha[i];
    }

    m.InitHIMByVectors(alpha1, alpha2);

    matrix_for_interpolate.InitHIMByVectors(alpha, beta);

    vector<FieldType> alpha_until_t(T + 1);
    vector<FieldType> alpha_from_t(N - 1 - T);

    // Interpolate first d+1 positions of (alpha,x)
    matrix_for_t.allocate(N - 1 - T, T + 1, field); // slices, only positions from 0..d
    //matrix_for_t.InitHIMByVectors(alpha_until_t, alpha_from_t);
    matrix_for_t.InitHIMVectorAndsizes(alpha, T+1, N-T-1);

    vector<FieldType> alpha_until_2t(2*T + 1);
    vector<FieldType> alpha_from_2t(N - 1 - 2*T);

    // Interpolate first d+1 positions of (alpha,x)
    matrix_for_2t.allocate(N - 1 - 2*T, 2*T + 1, field); // slices, only positions from 0..d
    //matrix_for_2t.InitHIMByVectors(alpha_until_2t, alpha_from_2t);
    matrix_for_2t.InitHIMVectorAndsizes(alpha, 2*T + 1, N-(2*T +1));


    //create the first row of the inverse of the nxn vandemonde matrix firstRowVandInverse
    initFirstRowInvVDM();



    if(flag_print){
        cout<< "matrix_for_t : " <<endl;
        matrix_for_t.Print();

        cout<< "matrix_for_2t : " <<endl;
        matrix_for_2t.Print();

    }


}
template <class FieldType>
void Protocol<FieldType>::initFirstRowInvVDM(){
    firstRowVandInverse.resize(N);

    //first calc the multiplication of all the alpha's for the denominator and the diff for the
    FieldType accumMult = *field->GetOne();

    for(int i=0; i<N; i++){
        accumMult*= alpha[i];

    }

    FieldType accum = *field->GetOne();
    for(int j=0; j<N; j++){

        for(int m=0; m<N; m++){
            if(m!=j) {
                accum *= (alpha[m] - alpha[j]);
            }
        }

        firstRowVandInverse[j] = accumMult/alpha[j]/accum;
        accum = *field->GetOne();
    }
}

template <class FieldType>
bool Protocol<FieldType>::preparationPhase()
{
    generateBeaverTriples(circuit.getNrOfMultiplicationGates());

    //generate triples for the DN multiplication protocol
    offlineDNForMultiplication(circuit.getNrOfMultiplicationGates());

    return true;
}

template <class FieldType>
void Protocol<FieldType>::generateBeaverTriples(int numOfTriples){

    randomABShares.resize(numOfTriples*2);//a, b random shares
    c.resize(numOfTriples);//a vector of a*b shares

    //first generate 2*numOfTriples random shares
    generateRandomShares(numOfTriples*2,randomABShares);

    GRRHonestMultiplication(randomABShares.data(), randomABShares.data()+numOfTriples, c, numOfTriples);

}


/**
 * Check whether given points lie on polynomial of degree d. This check is performed by interpolating x on
 * the first d + 1 positions of α and check the remaining positions.
 */
template <class FieldType>
bool Protocol<FieldType>::checkConsistency(vector<FieldType>& x, int d)
{
    if(d == T)
    {
        vector<FieldType> y(N - 1 - d); // the result of multiplication
        vector<FieldType> x_until_t(T + 1);

        for (int i = 0; i < T + 1; i++) {
            x_until_t[i] = x[i];
        }


        matrix_for_t.MatrixMult(x_until_t, y);

        // compare that the result is equal to the according positions in x
        for (int i = 0; i < N - d - 1; i++)   // n-d-2 or n-d-1 ??
        {
            if ((y[i]) != (x[d + 1 + i])) {
                return false;
            }
        }
        return true;
    } else if (d == 2*T)
    {
        vector<FieldType> y(N - 1 - d); // the result of multiplication

        vector<FieldType> x_until_2t(2*T + 1);

        for (int i = 0; i < 2*T + 1; i++) {
            x_until_2t[i] = x[i];
        }

        matrix_for_2t.MatrixMult(x_until_2t, y);

        // compare that the result is equal to the according positions in x
        for (int i = 0; i < N - d - 1; i++)   // n-d-2 or n-d-1 ??
        {
            if ((y[i]) != (x[d + 1 + i])) {
                return false;
            }
        }
        return true;

    } else {
        vector<FieldType> alpha_until_d(d + 1);
        vector<FieldType> alpha_from_d(N - 1 - d);
        vector<FieldType> x_until_d(d + 1);
        vector<FieldType> y(N - 1 - d); // the result of multiplication

        for (int i = 0; i < d + 1; i++) {
            alpha_until_d[i] = alpha[i];
            x_until_d[i] = x[i];
        }
        for (int i = d + 1; i < N; i++) {
            alpha_from_d[i - (d + 1)] = alpha[i];
        }
        // Interpolate first d+1 positions of (alpha,x)
        HIM<FieldType> matrix(N - 1 - d, d + 1, field); // slices, only positions from 0..d
        matrix.InitHIMByVectors(alpha_until_d, alpha_from_d);
        matrix.MatrixMult(x_until_d, y);

        // compare that the result is equal to the according positions in x
        for (int i = 0; i < N - d - 1; i++)   // n-d-2 or n-d-1 ??
        {
            //if (field->elementToString(y[i]) != field->elementToString(x[d + 1 + i])) {
            if (y[i] != x[d + 1 + i]) {
                return false;
            }
        }
        return true;
    }
    return true;
}

// Interpolate polynomial at position Zero
template <class FieldType>
FieldType Protocol<FieldType>::interpolate(vector<FieldType> x)
{
    vector<FieldType> y(N); // result of interpolate
    matrix_for_interpolate.MatrixMult(x, y);
    return y[0];
}



template <class FieldType>
FieldType Protocol<FieldType>::reconstructShare(vector<FieldType>& x, int d){

    if (!checkConsistency(x, d))
    {
        // someone cheated!
        if(flag_print) {
            cout << "cheating!!!" << '\n';}
        exit(0);
    }
    else
        return interpolate(x);
}


template <class FieldType>
int Protocol<FieldType>::processNotMult(){
    int count=0;
    for(int k=circuit.getLayers()[currentCirciutLayer]; k < circuit.getLayers()[currentCirciutLayer+1]; k++)
    {


        // add gate
        if(circuit.getGates()[k].gateType == ADD)
        {
            gateShareArr[circuit.getGates()[k].output] = gateShareArr[circuit.getGates()[k].input1] + gateShareArr[circuit.getGates()[k].input2];
            count++;
        }

        else if(circuit.getGates()[k].gateType == SUB)//sub gate
        {
            gateShareArr[circuit.getGates()[k].output] = gateShareArr[circuit.getGates()[k].input1] - gateShareArr[circuit.getGates()[k].input2];
            count++;
        }
        else if(circuit.getGates()[k].gateType == SCALAR)
        {
            long scalar(circuit.getGates()[k].input2);
            FieldType e = field->GetElement(scalar);
            gateShareArr[circuit.getGates()[k].output] = gateShareArr[circuit.getGates()[k].input1] * e;

            count++;
        }
         

    }
    return count;

}

/**
 * the Function process all multiplications which are ready.
 * @return the number of processed gates.
 */
template <class FieldType>
int Protocol<FieldType>::processMultiplications(int lastMultGate)
{

    //return processMultGRR();
    return processMultDN(lastMultGate);

}
template <class FieldType>
int Protocol<FieldType>::processMultDN(int indexInRandomArray) {

    int index = 0;
    int numOfMultGates = circuit.getNrOfMultiplicationGates();
    int fieldByteSize = field->getElementSizeInBytes();
    int maxNumberOfLayerMult = circuit.getLayers()[currentCirciutLayer + 1] - circuit.getLayers()[currentCirciutLayer];
    vector<FieldType> xPlusAAndYPlusBShares(maxNumberOfLayerMult * 2);//hold both in the same vector to send in one batch
    vector<byte> xPlusAAndYPlusBSharesBytes(maxNumberOfLayerMult * 2 * fieldByteSize);//hold both in the same vector to send in one batch

    vector<FieldType> xPlusAAndYPlusB(maxNumberOfLayerMult * 2);//hold both in the same vector to send in one batch
    vector<byte> xPlusAAndYPlusBBytes(maxNumberOfLayerMult * 2 * fieldByteSize);

    vector<vector<byte>> recBufsBytes;


    //generate the shares for x+a and y+b. do it in the same array to send once
    for (int k = circuit.getLayers()[currentCirciutLayer];
         k < circuit.getLayers()[currentCirciutLayer + 1]; k++)//go over only the logit gates
    {
        auto gate = circuit.getGates()[k];

        if (gate.gateType == MULT) {

            //set the share of x+a
            xPlusAAndYPlusBShares[2 * index] = gateShareArr[gate.input1] + randomTSharesOfflineMult[indexInRandomArray];

            //set the share of y+b
            xPlusAAndYPlusBShares[2 * index + 1] = gateShareArr[gate.input2] + randomTSharesOfflineMult[indexInRandomArray + numOfMultGates];

            index++;
            indexInRandomArray++;

            //set the byte array as well
        }
    }

    //set the acctual number of mult gate proccessed in this layer
    int acctualNumOfMultGates = index;

    if (m_partyId == 1) {

        for(int j=0; j<xPlusAAndYPlusBShares.size();j++) {
            field->elementToBytes(xPlusAAndYPlusBSharesBytes.data() + (j * fieldByteSize), xPlusAAndYPlusBShares[j]);
        }

        //just party 1 needs the recbuf
        recBufsBytes.resize(N);

        for (int i = 0; i < N; i++) {
            recBufsBytes[i].resize(acctualNumOfMultGates*fieldByteSize*2);
        }

        //receive the shares from all the other parties
        roundFunctionSyncForP1(xPlusAAndYPlusBSharesBytes, recBufsBytes);

    }
    else {//since I am not party 1 parties[0]->getID()=0

        //send the shares to p1
        parties[0]->getChannel()->write(xPlusAAndYPlusBSharesBytes.data(), xPlusAAndYPlusBSharesBytes.size());

    }

    //reconstruct the shares recieved from the other parties
    if (m_partyId == 1) {

        vector<FieldType> xPlusA(N), yPlusB(N);

        for (int k = 0;k < acctualNumOfMultGates; k++)//go over only the logit gates
        {
            for (int i = 0; i < N; i++) {

                xPlusA[i] = field->bytesToElement(recBufsBytes[i].data() + ((2 * k) * fieldByteSize));
                yPlusB[i] = field->bytesToElement(recBufsBytes[i].data() + ((2 * k + 1) * fieldByteSize));
            }

            // reconstruct the shares by P0
            xPlusAAndYPlusB[2 * k] = interpolate(xPlusA);
            xPlusAAndYPlusB[2 * k + 1] = interpolate(yPlusB);

            //convert to bytes
            field->elementToBytes(xPlusAAndYPlusBBytes.data() + (2 * k * fieldByteSize), xPlusAAndYPlusB[2 * k]);
            field->elementToBytes(xPlusAAndYPlusBBytes.data() + ((2 * k + 1) * fieldByteSize), xPlusAAndYPlusB[2 * k + 1]);


        }

        //send the reconstructed vector to all the other parties
        sendFromP1(xPlusAAndYPlusBBytes);
    }

    else {//each party get the x+a and y+b reconstruced vector from party 1

        parties[0]->getChannel()->read(xPlusAAndYPlusBBytes.data(), xPlusAAndYPlusBBytes.size());
    }




    //fill the xPlusAAndYPlusB array for all the parties except for party 1 that already have this array filled
    if (m_partyId != 1) {

        for (int i = 0; i < acctualNumOfMultGates; i++) {

            xPlusAAndYPlusB[i] = field->bytesToElement(xPlusAAndYPlusBBytes.data() + (i * fieldByteSize));
        }
    }



    indexInRandomArray -= index;
    index = 0;

    //after the xPlusAAndYPlusB array is filled, we are ready to fill the output of the mult gates
    for (int k = circuit.getLayers()[currentCirciutLayer];
         k < circuit.getLayers()[currentCirciutLayer + 1]; k++)//go over only the logit gates
    {
        auto gate = circuit.getGates()[k];

        if (gate.gateType == MULT) {

            gateShareArr[gate.output] = cOfflineMult[indexInRandomArray] -
                                        xPlusAAndYPlusB[2 * index] *
                                        randomTSharesOfflineMult[indexInRandomArray + numOfMultGates] -
                                        xPlusAAndYPlusB[2 * index + 1] * randomTSharesOfflineMult[indexInRandomArray] +
                                        xPlusAAndYPlusB[2 * index] * xPlusAAndYPlusB[2 * index + 1];

            index++;
            indexInRandomArray++;

        }
    }

    return index;
}





template <class FieldType>
int Protocol<FieldType>::processMultGRR() {
    int index = 0;

    vector<FieldType> x1(N),y1(N);

    vector<vector<FieldType>> sendBufsElements(N);
    vector<vector<byte>> sendBufsBytes(N);
    vector<vector<byte>> recBufsBytes(N);
    vector<FieldType> valBuf(circuit.getLayers()[currentCirciutLayer + 1] - circuit.getLayers()[currentCirciutLayer]); // Buffers for differences
    FieldType d;

    for(int i=0; i < N; i++)
    {
        //sendBufs[i] = "";

        sendBufsElements[i].resize(
                circuit.getLayers()[currentCirciutLayer + 1] - circuit.getLayers()[currentCirciutLayer]);
        sendBufsBytes[i].resize((circuit.getLayers()[currentCirciutLayer + 1] - circuit.getLayers()[currentCirciutLayer]) *
                                field->getElementSizeInBytes());
        recBufsBytes[i].resize((circuit.getLayers()[currentCirciutLayer + 1] - circuit.getLayers()[currentCirciutLayer]) *
                               field->getElementSizeInBytes());
    }


    for(int k = circuit.getLayers()[currentCirciutLayer]; k < circuit.getLayers()[currentCirciutLayer + 1] ; k++)//go over only the logit gates
    {
        // its a multiplication which not yet processed and ready
        if(circuit.getGates()[k].gateType == MULT )
        {
            //set the secret of the polynomial to be the multiplication of the shares
            x1[0] = gateShareArr[circuit.getGates()[k].input1] * gateShareArr[circuit.getGates()[k].input2];

            // generate random degree-T polynomial
            for(int i = 1; i < T + 1; i++)
            {
                // A random field element, uniform distribution
                x1[i] = field->Random();

            }


            matrix_vand.MatrixMult(x1, y1, T + 1); // eval poly at alpha-positions

            // prepare shares to be sent
            for(int i=0; i < N; i++)
            {
                //cout << "y1[ " <<i<< "]" <<y1[i] << endl;
                sendBufsElements[i][index] = y1[i];
            }
            index++;

        }


    }

    //convert to bytes
    int fieldByteSize = field->getElementSizeInBytes();
    for(int i=0; i < N; i++)
    {
        for(int j=0; j<sendBufsElements[i].size();j++) {
            field->elementToBytes(sendBufsBytes[i].data() + (j * fieldByteSize), sendBufsElements[i][j]);
        }
    }

    roundFunctionSync(sendBufsBytes, recBufsBytes, 4);

    int fieldBytesSize = field->getElementSizeInBytes();
    index = 0;
    for(int k = circuit.getLayers()[currentCirciutLayer]; k < circuit.getLayers()[currentCirciutLayer + 1] ; k++) {

        if(circuit.getGates()[k].gateType == MULT) {
            // generate random degree-T polynomial
            for (int i = 0; i < N; i++) {
                x1[i] = field->bytesToElement(recBufsBytes[i].data() + (index * fieldBytesSize));


            }

            FieldType accum = *field->GetZero();
            for (int i = 0; i < N; i++) {

                accum += firstRowVandInverse[i] * x1[i];

            }

            gateShareArr[circuit.getGates()[k].output] = accum;

            index++;
        }

    }

    return index;
}

template <class FieldType>
void Protocol<FieldType>::GRRHonestMultiplication(FieldType *a, FieldType *b, vector<FieldType> &cToFill, int numOfTrupples)
{

    vector<FieldType> x1(N),y1(N);

    vector<vector<FieldType>> sendBufsElements(N);
    vector<vector<byte>> sendBufsBytes(N);
    vector<vector<byte>> recBufsBytes(N);

    FieldType d;

    vector<FieldType> ReconsBuf(numOfTrupples);


    for(int i=0; i < N; i++)
    {
        sendBufsElements[i].resize(numOfTrupples);
        sendBufsBytes[i].resize((numOfTrupples)*field->getElementSizeInBytes());
        recBufsBytes[i].resize((numOfTrupples)*field->getElementSizeInBytes());
    }


    for(int k = 0; k < numOfTrupples ; k++)//go over only the logit gates
    {

        //set the secret of the polynomial to be the multiplication of the shares
        x1[0] = a[k] * b[k];

        // generate random degree-T polynomial
        for(int i = 1; i < T+1; i++)
        {
            // A random field element, uniform distribution
            x1[i] = field->Random();

        }

        matrix_vand.MatrixMult(x1, y1, T+1); // eval poly at alpha-positions

        // prepare shares to be sent
        for(int i=0; i < N; i++)
        {
            //cout << "y1[ " <<i<< "]" <<y1[i] << endl;
            sendBufsElements[i][k] = y1[i];
        }

    }

    //convert to bytes
    int fieldByteSize = field->getElementSizeInBytes();
    for(int i=0; i < N; i++)
    {
        for(int j=0; j<sendBufsElements[i].size();j++) {
            field->elementToBytes(sendBufsBytes[i].data() + (j * fieldByteSize), sendBufsElements[i][j]);
        }
    }

    roundFunctionSync(sendBufsBytes, recBufsBytes,4);

    int fieldBytesSize = field->getElementSizeInBytes();

    for(int k = 0; k < numOfTrupples ; k++) {

       // generate random degree-T polynomial
        for (int i = 0; i < N; i++) {
            x1[i] = field->bytesToElement(recBufsBytes[i].data() + (k * fieldBytesSize));
        }

        FieldType accum = *field->GetZero();
        for (int i = 0; i < N; i++) {

            accum += firstRowVandInverse[i] * x1[i];

        }

        cToFill[k] = accum;
    }

}




/**
 * the Function process all multiplications which are ready.
 * @return the number of processed gates.
 */
template <class FieldType>
void Protocol<FieldType>::processRandoms()
{
    FieldType r1;
    //vector<string> arr = {};
    for(int k = (numOfInputGates - 1); k < (M - numOfOutputGates); k++)
    {
        if(circuit.getGates()[k].gateType == RANDOM)
        {

            r1 = sharingBufTElements[shareIndex];
            shareIndex++;

            gateShareArr[circuit.getGates()[k].output] = r1;
        }
    }
}
template <class FieldType>
void Protocol<FieldType>::offlineDNForMultiplication(int numOfTriples){

    int fieldByteSize = field->getElementSizeInBytes();
    //generate random shares for the multiplication gates. 2 shares for every multiplication gate
    randomTSharesOfflineMult.resize(numOfTriples*2);//a, b random shares
    cOfflineMult.resize(numOfTriples);//a vector of a*b shares

    vector<FieldType> abMinusRShares(numOfTriples);//a vector of a*b shares
    vector<FieldType> abMinusR(numOfTriples);//a vector of a*b . Note this holds the reconstructed vector and not the shares
    vector<byte> abMinusRBytes(numOfTriples * fieldByteSize);


    vector<byte> abMinusRSharesBytes(numOfTriples * fieldByteSize);
    vector<vector<byte>> recBufsBytes;


    vector<FieldType> randomTAnd2TShares(numOfTriples*2);//a, b random shares

    vector<FieldType> x(N);


    //first generate 2*numOfTriples random shares
    generateRandomShares(numOfTriples*2,randomTSharesOfflineMult);
    generateRandom2TAndTShares(numOfTriples*2,randomTAnd2TShares);

    //compute locally [ak][bk]-[rk]2t
    for(int i=0; i<numOfTriples; i++){

        //[a_k]*[b_k] - [r_k]_2t
        abMinusRShares[i] = randomTSharesOfflineMult[i]*randomTSharesOfflineMult[numOfTriples + i] - randomTAnd2TShares[2*i + 1];

        //convert to bytes
        field->elementToBytes(abMinusRSharesBytes.data() + (i * fieldByteSize), abMinusRShares[i]);
    }



    if(m_partyId==1){

        //just party 1 needs the recbuf
        recBufsBytes.resize(N);

        for(int i=0; i<N; i++){
            recBufsBytes[i].resize(numOfTriples*fieldByteSize);
        }

        //receive the shares from all the other parties
        roundFunctionSyncForP1(abMinusRSharesBytes, recBufsBytes);

    }
    else{//since I am not party 0 parties[0]->getID()=0

        //send the shares to p1
        parties[0]->getChannel()->write(abMinusRSharesBytes.data(), abMinusRSharesBytes.size());

    }

    //reconstruct the shares recieved from the other parties
    if(m_partyId==1) {

        for (int k = 0; k < numOfTriples; k++) {

            for (int i = 0; i < N; i++) {

                x[i] = field->bytesToElement(recBufsBytes[i].data() + (k * fieldByteSize));
            }


            // reconstruct the shares by P0
            abMinusR[k] = interpolate(x);
            //convert to bytes
            field->elementToBytes(abMinusRBytes.data() + (k * fieldByteSize), abMinusR[k]);

        }

        //send the reconstructed vector to all the other parties
        sendFromP1(abMinusRBytes);
    }
    else{//each party get the ab-r vector from party 1

        parties[0]->getChannel()->read(abMinusRBytes.data(), abMinusRBytes.size());
    }

    //fill the abMinusR array for all the parties except for party 0 that already have this array filled
    if(m_partyId!=1){

        for(int i=0; i < numOfTriples; i++) {

            abMinusR[i] = field->bytesToElement(abMinusRBytes.data() + (i*fieldByteSize));
        }
    }

    //after all parties have abMinusRBytes filled we are ready to compute the c part of the triples
    for(int i=0; i<numOfTriples; i++){

        cOfflineMult[i] = randomTAnd2TShares[2*i] + abMinusR[i];
    }
}

template <class FieldType>
void Protocol<FieldType>::verificationPhase(){

    //get the number of random elements to create
    int numOfRandomelements = circuit.getNrOfMultiplicationGates();
    //first generate the common aes key
    auto key = generateCommonKey();

    //print key
    //if(flag_print) {
        for (int i = 0; i < key.size(); i++) {
            cout << "key[" << i << "] for party :" << m_partyId << "is : " << (int) key[i] << endl;
        }
    //}
    vector<FieldType> randomElements(numOfRandomelements);
    generatePseudoRandomElements(key, randomElements, numOfRandomelements);

    //print key
    //if(flag_print) {
    for (int i = 0; i<3; i++) {
        cout << "randomElements[" << i << "] for party :" << m_partyId << "is : " << field->elementToString(randomElements[i]) << endl;
    }
    //}

    //preapre x,y,z for the verification sub protocol


    int numOfOutputGates = circuit.getNrOfOutputGates();
    int numOfInputGates = circuit.getNrOfInputGates();
    int numOfMultGates = circuit.getNrOfMultiplicationGates();

    vector<FieldType> x(numOfMultGates);
    vector<FieldType> y(numOfMultGates);
    vector<FieldType> z(numOfMultGates);

    int index = 0;
    for(int k=numOfInputGates-1; k<M-numOfOutputGates + 1;k++){

        auto gate = circuit.getGates()[k];

        if(gate.gateType == MULT){
            x[index] = gateShareArr[gate.input1];
            y[index] = gateShareArr[gate.input2];
            z[index] = gateShareArr[gate.output];
            index++;
        }


    }

    //call the verification sub protocol
/*    bool answer = verificationOfBatchedTriples(x.data(), y.data(), z.data(),
                                 randomABShares.data(), randomABShares.data() + numOfMultGates, c.data(),
                                 randomElements, numOfMultGates);

*/
      //call the verification sub protocol
      bool answer = verificationOfSingleTriples(x.data(), y.data(), z.data(),
                                                 randomABShares.data(), randomABShares.data() + numOfMultGates, c.data(),
                                                 randomElements, numOfMultGates);





      cout<<"Cheating is:" << answer<<endl;

  }




  template <class FieldType>
  vector<byte> Protocol<FieldType>::generateCommonKey(){

      int fieldByteSize = field->getElementSizeInBytes();

      //calc the number of elements needed for 128 bit AES key
      int numOfRandomShares = 16/field->getElementSizeInBytes() + 1;
      vector<FieldType> randomSharesArray(numOfRandomShares);
      vector<FieldType> aesArray(numOfRandomShares);
      vector<byte> aesKey(numOfRandomShares*fieldByteSize);


      //generate enough random shares for the AES key
      generateRandomShares(numOfRandomShares, randomSharesArray);

      openShare(numOfRandomShares, randomSharesArray, aesArray);


      //turn the aes array into bytes to get the common aes key.
      for(int i=0; i<numOfRandomShares;i++){

          for(int j=0; j<numOfRandomShares;j++) {
              field->elementToBytes(aesKey.data() + (j * fieldByteSize), aesArray[j]);
          }
      }

      //reduce the size of the key to 16 bytes
      aesKey.resize(16);

      return aesKey;

  }

  template <class FieldType>
  void Protocol<FieldType>::openShare(int numOfRandomShares, vector<FieldType> &Shares, vector<FieldType> &secrets){


      vector<vector<byte>> sendBufsBytes(N);
      vector<vector<byte>> recBufsBytes(N);

      vector<FieldType> x1(N);
      int fieldByteSize = field->getElementSizeInBytes();

      //calc the number of elements needed for 128 bit AES key

      //resize vectors
      for(int i=0; i < N; i++)
      {
          sendBufsBytes[i].resize(numOfRandomShares*fieldByteSize);
          recBufsBytes[i].resize(numOfRandomShares*fieldByteSize);
      }

      //set the first sending data buffer
      for(int j=0; j<numOfRandomShares;j++) {
          field->elementToBytes(sendBufsBytes[0].data() + (j * fieldByteSize), Shares[j]);
      }

      //copy the same data for all parties
      for(int i=1; i<N; i++){

          sendBufsBytes[i] = sendBufsBytes[0];
      }

      //call the round function to send the shares to all the users and get the other parties share
      roundFunctionSync(sendBufsBytes, recBufsBytes,12);

      //reconstruct each set of shares to get the secret

      for(int k=0; k<numOfRandomShares; k++){

          //get the set of shares for each element
          for(int i=0; i < N; i++) {

              x1[i] = field->bytesToElement(recBufsBytes[i].data() + (k*fieldByteSize));
          }


          secrets[k] = reconstructShare(x1, T);

      }

  }


  template <class FieldType>
  void Protocol<FieldType>::generatePseudoRandomElements(vector<byte> & aesKey, vector<FieldType> &randomElementsToFill, int numOfRandomElements){

      bool isLongRandoms;
      int size;
      if(field->getElementSizeInBytes()>4){
          isLongRandoms = true;
          size = 8;
      }
      else{

          isLongRandoms = false;
          size = 4;
      }

      cout<<"size is" << size << "for party : " << m_partyId;


      PrgFromOpenSSLAES prg((numOfRandomElements*size/*field->getElementSizeInBytes()*/) + 1);
    SecretKey sk(aesKey, "aes");
    prg.setKey(sk);

    for(int i=0; i<numOfRandomElements; i++){

        if(isLongRandoms)
            randomElementsToFill[i] = field->GetElement(prg.getRandom64());
        else
            randomElementsToFill[i] = field->GetElement(prg.getRandom32());
    }

}

template <class FieldType>
bool Protocol<FieldType>::verificationOfBatchedTriples(FieldType *x, FieldType *y, FieldType *z,
                                  FieldType *a, FieldType *b, FieldType *c,
                                  vector<FieldType> randomElements, int numOfTriples){



    vector<FieldType> r(numOfTriples);//vector holding the random shares generated
    vector<FieldType> rx(numOfTriples);//vector holding the multiplication of x and r
    vector<FieldType> firstMult(4*numOfTriples);//vector some computations
    vector<FieldType> secondMult(4*numOfTriples);//vector some computations
    vector<FieldType> outputMult(4*numOfTriples);//vector holding the 4*numOfTriples output of the multiplication
    vector<FieldType> v(numOfTriples);//vector holding the 4*numOfTriples output of the multiplication

    //first generate numOfTriples random shares
    generateRandomShares(numOfTriples, r);

    //run semi-honest multiplication on x and r
    GRRHonestMultiplication(x, r.data(),rx, numOfTriples);

    //prepare the 4k pairs for multiplication
    for(int k=0; k<numOfTriples; k++){

        firstMult[k*4] = rx[k] + a[k];//the row assignment (look at the paper)
        secondMult[k*4] = b[k];

        firstMult[k*4+1] = a[k];
        secondMult[k*4+1] = y[k] + b[k];

        firstMult[k*4+2] = firstMult[k*4];
        secondMult[k*4+2] = secondMult[k*4+1];

        firstMult[k*4+3] = r[k];
        secondMult[k*4+3] = z[k];


    }

    //run semi-honest multiplication on x and r
    GRRHonestMultiplication(firstMult.data(), secondMult.data(),outputMult, numOfTriples*4);

    //compute the output share to check
    FieldType vk;
    FieldType VShare;
    for(int k=0; k<numOfTriples; k++){
        vk = outputMult[4*k + 3] -c[k] + outputMult[4*k + 1] + outputMult[4*k] - outputMult[4*k + 2];
        VShare += vk*randomElements[k];

    }


    //open [V]
    vector<FieldType> shareArr(1);
    vector<FieldType> secretArr(1);
    shareArr[0] = VShare;

    openShare(1,shareArr,secretArr);

    //check that V=0
    if(secretArr[0] != *field->GetZero())
        return false;
    else
        return true;

}



template <class FieldType>
bool Protocol<FieldType>::verificationOfSingleTriples(FieldType *x, FieldType *y, FieldType *z,
                                                       FieldType *a, FieldType *b, FieldType *c,
                                                       vector<FieldType> randomElements, int numOfTriples){



    vector<FieldType> roAndSigma(numOfTriples*2);//vector holding the random shares generated
    vector<FieldType> secretArr(numOfTriples*2);
    vector<FieldType> secretArrResultsV(numOfTriples);

    vector<FieldType> v(numOfTriples);//vector holding the 4*numOfTriples output of the multiplication

    //prepare the 4k pairs for multiplication
    for(int k=0; k<numOfTriples; k++){

        roAndSigma[2*k] = randomElements[k]*x[k] + a[k];//ro
        roAndSigma[2*k + 1] = y[k] + b[k];//sigma
    }


    //open all the shares at once
    openShare(numOfTriples*2, roAndSigma, secretArr);

    vector<FieldType> shareArrV(numOfTriples);

    //compute the output share array
    for(int k=0; k<numOfTriples; k++){
        shareArrV[k] = randomElements[k]*z[k] - c[k] + secretArr[2*k+1]*a[k]+ secretArr[2*k]*b[k] - secretArr[2*k] * secretArr[2*k + 1];
    }

    openShare(numOfTriples,shareArrV,secretArrResultsV);

    //check that V=0 for each element
    for(int i=0; i< numOfTriples; i++){
        if(secretArrResultsV[i] != *field->GetZero())
            return false;
    }

    return true;


}



/**
 * the function Walk through the circuit and reconstruct output gates.
 * @param circuit
 * @param gateShareArr
 * @param alpha
 */
template <class FieldType>
void Protocol<FieldType>::outputPhase()
{
    int count=0;
    vector<FieldType> x1(N); // vector for the shares of my outputs
    vector<vector<FieldType>> sendBufsElements(N);
    vector<vector<byte>> sendBufsBytes(N);
    vector<vector<byte>> recBufBytes(N);

    FieldType num;
    ofstream myfile;
    myfile.open(outputFile);

    for(int k=M-numOfOutputGates; k < M; k++)
    {
        if(circuit.getGates()[k].gateType == OUTPUT)
        {
            // send to party (which need this gate) your share for this gate
            sendBufsElements[circuit.getGates()[k].party - 1].push_back(gateShareArr[circuit.getGates()[k].input1]);
        }
    }


    int fieldByteSize = field->getElementSizeInBytes();
    for(int i=0; i < N; i++)
    {
        sendBufsBytes[i].resize(sendBufsElements[i].size()*fieldByteSize);
        recBufBytes[i].resize(sendBufsElements[m_partyId - 1].size()*fieldByteSize);
        for(int j=0; j<sendBufsElements[i].size();j++) {
            field->elementToBytes(sendBufsBytes[i].data() + (j * fieldByteSize), sendBufsElements[i][j]);
        }
    }

    //comm->roundfunctionI(sendBufsBytes, recBufBytes,7);
    roundFunctionSync(sendBufsBytes, recBufBytes,7);



    int counter = 0;
    if(flag_print) {
        cout << "endnend" << endl;}
    for(int k=M-numOfOutputGates ; k < M; k++) {
        if(circuit.getGates()[k].gateType == OUTPUT && circuit.getGates()[k].party == m_partyId)
        {
            for(int i=0; i < N; i++) {

                x1[i] = field->bytesToElement(recBufBytes[i].data() + (counter*fieldByteSize));
            }


            // my output: reconstruct received shares
            if (!checkConsistency(x1, T))
            {
                // someone cheated!
                //if(flag_print) {
                    cout << "cheating!!!" << '\n';//}
                return;
            }
            if(flag_print_output)
                cout << "the result for "<< circuit.getGates()[k].input1 << " is : " << field->elementToString(interpolate(x1)) << '\n';
            //myfile << field->elementToString(interpolate(x1));

            counter++;
        }
    }

    // close output file
    myfile.close();
}


template <class FieldType>
void Protocol<FieldType>::roundFunctionSync(vector<vector<byte>> &sendBufs, vector<vector<byte>> &recBufs, int round) {

    //cout<<"in roundFunctionSync "<< round<< endl;

    int numThreads = 10;//parties.size();
    int numPartiesForEachThread;

    if (parties.size() <= numThreads){
        numThreads = parties.size();
        numPartiesForEachThread = 1;
    } else{
        numPartiesForEachThread = (parties.size() + numThreads - 1)/ numThreads;
    }


    recBufs[m_partyId-1] = sendBufs[m_partyId-1];
    //recieve the data using threads
    vector<thread> threads(numThreads);
    for (int t=0; t<numThreads; t++) {
        if ((t + 1) * numPartiesForEachThread <= parties.size()) {
            threads[t] = thread(&Protocol::exchangeData, this, ref(sendBufs), ref(recBufs),
                                t * numPartiesForEachThread, (t + 1) * numPartiesForEachThread);
        } else {
            threads[t] = thread(&Protocol::exchangeData, this, ref(sendBufs), ref(recBufs), t * numPartiesForEachThread, parties.size());
        }
    }
    for (int t=0; t<numThreads; t++){
        threads[t].join();
    }

}


template <class FieldType>
void Protocol<FieldType>::exchangeData(vector<vector<byte>> &sendBufs, vector<vector<byte>> &recBufs, int first, int last){


    //cout<<"in exchangeData";
    for (int i=first; i < last; i++) {

        if ((m_partyId-1) < parties[i]->getID()) {


            //send shares to my input bits
            parties[i]->getChannel()->write(sendBufs[parties[i]->getID()].data(), sendBufs[parties[i]->getID()].size());
            //cout<<"write the data:: my Id = " << m_partyId - 1<< "other ID = "<< parties[i]->getID() <<endl;


            //receive shares from the other party and set them in the shares array
            parties[i]->getChannel()->read(recBufs[parties[i]->getID()].data(), recBufs[parties[i]->getID()].size());
            //cout<<"read the data:: my Id = " << m_partyId-1<< "other ID = "<< parties[i]->getID()<<endl;

        } else{


            //receive shares from the other party and set them in the shares array
            parties[i]->getChannel()->read(recBufs[parties[i]->getID()].data(), recBufs[parties[i]->getID()].size());
            //cout<<"read the data:: my Id = " << m_partyId-1<< "other ID = "<< parties[i]->getID()<<endl;



            //send shares to my input bits
            parties[i]->getChannel()->write(sendBufs[parties[i]->getID()].data(), sendBufs[parties[i]->getID()].size());
            //cout<<"write the data:: my Id = " << m_partyId-1<< "other ID = "<< parties[i]->getID() <<endl;


        }

    }


}





template <class FieldType>
void Protocol<FieldType>::roundFunctionSyncBroadcast(vector<byte> &message, vector<vector<byte>> &recBufs) {

    //cout<<"in roundFunctionSyncBroadcast "<< endl;

    int numThreads = 10;//parties.size();
    int numPartiesForEachThread;

    if (parties.size() <= numThreads){
        numThreads = parties.size();
        numPartiesForEachThread = 1;
    } else{
        numPartiesForEachThread = (parties.size() + numThreads - 1)/ numThreads;
    }


    recBufs[m_partyId-1] = message;
    //recieve the data using threads
    vector<thread> threads(numThreads);
    for (int t=0; t<numThreads; t++) {
        if ((t + 1) * numPartiesForEachThread <= parties.size()) {
            threads[t] = thread(&Protocol::recData, this, ref(message), ref(recBufs),
                                t * numPartiesForEachThread, (t + 1) * numPartiesForEachThread);
        } else {
            threads[t] = thread(&Protocol::recData, this, ref(message),  ref(recBufs), t * numPartiesForEachThread, parties.size());
        }
    }
    for (int t=0; t<numThreads; t++){
        threads[t].join();
    }

}


template <class FieldType>
void Protocol<FieldType>::recData(vector<byte> &message, vector<vector<byte>> &recBufs, int first, int last){


    //cout<<"in exchangeData";
    for (int i=first; i < last; i++) {

        if ((m_partyId-1) < parties[i]->getID()) {


            //send shares to my input bits
            parties[i]->getChannel()->write(message.data(), message.size());
            //cout<<"write the data:: my Id = " << m_partyId - 1<< "other ID = "<< parties[i]->getID() <<endl;


            //receive shares from the other party and set them in the shares array
            parties[i]->getChannel()->read(recBufs[parties[i]->getID()].data(), recBufs[parties[i]->getID()].size());
            //cout<<"read the data:: my Id = " << m_partyId-1<< "other ID = "<< parties[i]->getID()<<endl;

        } else{


            //receive shares from the other party and set them in the shares array
            parties[i]->getChannel()->read(recBufs[parties[i]->getID()].data(), recBufs[parties[i]->getID()].size());
            //cout<<"read the data:: my Id = " << m_partyId-1<< "other ID = "<< parties[i]->getID()<<endl;



            //send shares to my input bits
            parties[i]->getChannel()->write(message.data(), message.size());
            //cout<<"write the data:: my Id = " << m_partyId-1<< "other ID = "<< parties[i]->getID() <<endl;


        }

    }


}



template <class FieldType>
void Protocol<FieldType>::roundFunctionSyncForP1(vector<byte> &myShare, vector<vector<byte>> &recBufs) {

    //cout<<"in roundFunctionSyncBroadcast "<< endl;

    int numThreads = parties.size();
    int numPartiesForEachThread;

    if (parties.size() <= numThreads){
        numThreads = parties.size();
        numPartiesForEachThread = 1;
    } else{
        numPartiesForEachThread = (parties.size() + numThreads - 1)/ numThreads;
    }


    recBufs[m_partyId-1] = myShare;
    //recieve the data using threads
    vector<thread> threads(numThreads);
    for (int t=0; t<numThreads; t++) {
        if ((t + 1) * numPartiesForEachThread <= parties.size()) {
            threads[t] = thread(&Protocol::recDataToP1, this,  ref(recBufs),
                                t * numPartiesForEachThread, (t + 1) * numPartiesForEachThread);
        } else {
            threads[t] = thread(&Protocol::recDataToP1, this, ref(recBufs), t * numPartiesForEachThread, parties.size());
        }
    }
    for (int t=0; t<numThreads; t++){
        threads[t].join();
    }

}


template <class FieldType>
void Protocol<FieldType>::recDataToP1(vector<vector<byte>> &recBufs, int first, int last){


    //cout<<"in exchangeData";
    for (int i=first; i < last; i++) {

        parties[i]->getChannel()->read(recBufs[parties[i]->getID()].data(), recBufs[parties[i]->getID()].size());
        //cout<<"read the data:: my Id = " << m_partyId-1<< "other ID = "<< parties[i]->getID()<<endl;
    }


}



template <class FieldType>
void Protocol<FieldType>::sendFromP1(vector<byte> &sendBuf) {

    //cout<<"in roundFunctionSyncBroadcast "<< endl;

    int numThreads = parties.size();
    int numPartiesForEachThread;

    if (parties.size() <= numThreads){
        numThreads = parties.size();
        numPartiesForEachThread = 1;
    } else{
        numPartiesForEachThread = (parties.size() + numThreads - 1)/ numThreads;
    }

    //recieve the data using threads
    vector<thread> threads(numThreads);
    for (int t=0; t<numThreads; t++) {
        if ((t + 1) * numPartiesForEachThread <= parties.size()) {
            threads[t] = thread(&Protocol::sendDataFromP1, this,  ref(sendBuf),
                                t * numPartiesForEachThread, (t + 1) * numPartiesForEachThread);
        } else {
            threads[t] = thread(&Protocol::sendDataFromP1, this, ref(sendBuf), t * numPartiesForEachThread, parties.size());
        }
    }
    for (int t=0; t<numThreads; t++){
        threads[t].join();
    }

}

template <class FieldType>
void Protocol<FieldType>::sendDataFromP1(vector<byte> &sendBuf, int first, int last){

    for(int i=first; i < last; i++) {

        parties[i]->getChannel()->write(sendBuf.data(), sendBuf.size());

    }


}




template <class FieldType>
Protocol<FieldType>::~Protocol()
{
    //delete comm;
}






#endif /* PROTOCOL_H_ */
