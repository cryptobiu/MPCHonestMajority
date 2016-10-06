#include "VDM.h"

VDM::VDM(int n, int m) {
    this->m_m = m;
    this->m_n = n;
    this->m_matrix = new TFieldElement*[m_n];
    for (int i = 0; i < m_n; i++)
    {
        m_matrix[i] = new TFieldElement[m_m];
    }
}

void VDM::InitVDM() {
    vector<TFieldElement> alpha(m_n);
    for (int i = 0; i < m_n; i++) {
        alpha[i] = (TField::getInstance()->GetElement(i + 1));
    }

    for (int i = 0; i < m_n; i++) {
        m_matrix[i][0] = *(TField::getInstance()->GetOne());
        for (int k = 1; k < m_n; k++) {
            m_matrix[i][k] = m_matrix[i][k - 1] * (alpha[i]);
        }
    }
}

/**
 * the function print the matrix
 */
void VDM::Print()
{
    for (int i = 0; i < m_m; i++)
    {
        for(int j = 0; j < m_n; j++)
        {
            cout << (m_matrix[i][j]).getElement() << " ";

        }
        cout << " " << '\n';
    }

}


void VDM::MatrixMult(std::vector<TFieldElement> &vector, std::vector<TFieldElement> &answer)
{

    TFieldElement temp;
    TFieldElement temp1;

    for(int i = 0; i < vector.size(); i++) {
        if(vector[i].getElement() == NULL) {
            temp = TFieldElement("[]");
            vector[i] = temp;
        }
    }

    cout << "123  " <<  vector.size()<< '\n';

    for(int i = 0; i < m_n; i++)
    {
        temp = TFieldElement(GF2X::zero());
        for(int j=0; j < m_m; j++)
        {

            temp1 = m_matrix[i][j] * vector[j];
            temp = temp + temp1;
        }

        answer[i]=temp;
    }
}
//
VDM::~VDM() {
    for (int i = 0; i < m_n; i++) {
        delete[] m_matrix[i];
    }
    delete[] m_matrix;
}
