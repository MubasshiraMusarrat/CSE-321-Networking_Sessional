#include <iostream>
#include <string>
#include <vector>
#include <bitset>
#include <windows.h>

using namespace std;

vector<bool> operator%(vector<bool> dividend, vector<bool> divisor)
{
    vector<bool> remainder;
    int dividendSize = dividend.size();
    int divisorSize = divisor.size();

    for (int i = 0; i <= dividendSize - divisorSize; i++)
    {
        if (dividend[i])
        {
            for (int j = 0; j < divisorSize; j++)
            {
                dividend[i + j] = dividend[i + j] ^ divisor[j];
            }
        }
    }

    for (int i = dividendSize - divisorSize + 1; i < dividendSize; i++)
    {
        remainder.push_back(dividend[i]);
    }

    return remainder;
}

vector<bool> operator>>(vector<bool> temp, int shift)
{
    temp.erase(temp.end() - shift, temp.end());
    return temp;
}

vector<bool> addParity(int r, vector<bool> temp)
{
    for (int j = 0; j < r; j++)
    {
        int parityPosition = 1 << j;
        int parity = 0;
        for (int k = parityPosition; k <= temp.size(); k++)
        {
            if (k & parityPosition)
            {
                parity ^= temp[k - 1];
            }
        }
        temp[parityPosition - 1] = parity;
    }
    return temp;
}

int main()
{
    srand(1);
    string S_x;
    int m;
    double prob;
    vector<bool> G_x;
    cout << "Enter data string: ";
    getline(cin, S_x);
    cout << "Enter number of data bytes in a row (m): ";
    cin >> m;
    cout << "Enter probability of error (p): ";
    cin >> prob;
    cin.ignore();
    cout << "Enter generator polynomial: ";
    string g;
    getline(cin, g);
    for (char c : g)
    {
        G_x.push_back(c != '0');
    }

    // padding input string to multiple of m
    while (S_x.length() % m != 0)
    {
        S_x += "~";
    }
    cout << endl
         << "Data string after padding: " << S_x << endl;

    // Creating data block
    vector<vector<bool>> dataBlock(S_x.length() / m);
    int index = 0;
    cout << endl
         << "Data block (ascii code of m characters per row): " << endl;
    for (int i = 0; i < S_x.length(); i += m)
    {
        for (int j = 0; j < m; j++)
        {
            char c = S_x[i + j];
            bitset<8> binary(c);
            for (int k = 0; k < 8; k++)
            {
                dataBlock[index].push_back(binary[7 - k]);
                cout << dataBlock[index][j * 8 + k];
            }
        }
        cout << endl;
        index++;
    }

    // calculating number of parity bits
    int msgBitsPerRow = m * 8;
    int r = 0;
    while (msgBitsPerRow + r + 1 > (1 << r))
    {
        r++;
    }
    // cout<<"Number of parity bits per row: "<<r<<endl;

    int totalBitsPerRow = msgBitsPerRow + r;

    // adding parity bits
    cout << endl
         << "Data block after adding check bits: " << endl;
    for (int i = 0; i < dataBlock.size(); i++)
    {
        vector<bool> temp;
        int dataIndex = 0;
        for (int j = 0; j < totalBitsPerRow; j++)
        {
            if (j == (1 << dataIndex) - 1)
            {
                temp.push_back(0);
                dataIndex++;
            }
            else
            {
                temp.push_back(dataBlock[i][j - dataIndex]);
            }
        }

        dataBlock[i] = addParity(r, temp);

        dataIndex = 0;
        for (int j = 0; j < dataBlock[i].size(); j++)
        {
            if (j == (1 << dataIndex) - 1)
            {
                SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 10);
                dataIndex++;
            }
            else
            {
                SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15);
            }
            cout << dataBlock[i][j];
        }
        cout << endl;
    }

    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15);

    // serializing data in column-major order
    vector<bool> dataStream;
    cout << endl
         << "Data bits after columnwise serialization: " << endl;
    for (int j = 0; j < totalBitsPerRow; j++)
    {
        for (int i = 0; i < dataBlock.size(); i++)
        {
            dataStream.push_back(dataBlock[i][j]);
            cout << dataStream[j * dataBlock.size() + i];
        }
    }
    cout << endl;

    // calculate generator polymonials size
    u_int heighestPower = 0; // if there are leading 0's in the generator polynomial
    for (int i = 0; i < G_x.size(); i++)
    {
        if (G_x[i])
        {
            heighestPower = G_x.size() - i;
            break;
        }
    }
    // cout<<"Heighest power: "<<heighestPower<<endl;

    // append data stream with g-1 0's. g = generator polynomial size
    vector<bool> tempStream = dataStream;
    for (int i = 1; i < heighestPower; i++)
    {
        tempStream.push_back(0);
    }

    // calculate reminder of data string modulo2devision generator polynomial
    vector<bool> remainder = tempStream % G_x;

    // add crc checksum to data stream
    cout << endl
         << "Data bits after appending CRC checksum (sent frame): " << endl;
    dataStream.insert(dataStream.end(), remainder.begin(), remainder.end());
    for (int i = 0; i < dataStream.size(); i++)
    {
        if (i == (dataStream.size() - remainder.size()))
        {
            SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 11);
        }
        cout << dataStream[i];
    }
    cout << endl;
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15);

    // toggle data bits with probability p
    vector<bool> receivedStream = dataStream;
    for (int i = 0; i < dataStream.size(); i++)
    {
        //double random = (rand() % 10000) * 0.0001;
        double random = double(rand()) / double(RAND_MAX);
        if (random < prob)
        {
            receivedStream[i] = !receivedStream[i];
        }
    }

    // print received frame
    cout << endl
         << "Received frame: " << endl;
    for (int i = 0; i < receivedStream.size(); i++)
    {
        if (receivedStream[i] != dataStream[i])
        {
            SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 12);
        }
        cout << receivedStream[i];
        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15);
    }
    cout << endl;
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15);

    // calculate reminder of received frame modulo2devision generator polynomial
    remainder = receivedStream % G_x;

    // detect error if remider != 0
    bool errorDetected = false;
    for (int i = 0; i < remainder.size(); i++)
    {
        if (remainder[i])
        {
            errorDetected = true;
            break;
        }
    }
    cout << endl
         << "Result of CRC checksum matching: ";
    if (errorDetected)
    {
        cout << "Error detected" << endl;
    }
    else
    {
        cout << "No error detected" << endl;
    }

    // erase crc checksum bits from received frame
    receivedStream = receivedStream >> remainder.size();

    // deserialize the frame in column-major order
    vector<vector<bool>> receivedDataBlock(receivedStream.size() / totalBitsPerRow, vector<bool>(totalBitsPerRow));
    for (int j = 0; j < totalBitsPerRow; j++)
    {
        for (int i = 0; i < receivedDataBlock.size(); i++)
        {
            receivedDataBlock[i][j] = receivedStream[j * receivedDataBlock.size() + i];
        }
    }

    // print data block after removing crc checksum bits
    cout << endl
         << "Data block after removing CRC checksum bits: " << endl;
    for (int i = 0; i < receivedDataBlock.size(); i++)
    {
        for (int j = 0; j < receivedDataBlock[i].size(); j++)
        {
            if (receivedDataBlock[i][j] != dataBlock[i][j])
            {
                SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 12);
            }
            cout << receivedDataBlock[i][j];
            SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15);
        }
        cout << endl;
    }
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15);

    // count number of pairty bits in the reciver side
    int parityCount = receivedDataBlock[0].size() - ((receivedDataBlock[0].size() / 8) * 8);
    // cout<<"Parity count: "<<parityCount<<endl;

    // correct error
    for (int i = 0; i < receivedDataBlock.size(); i++)
    {
        int errorPosition = 0;
        receivedDataBlock[i] = addParity(parityCount, receivedDataBlock[i]);
        int dataIndex = 0;
        for (int j = 0; j < totalBitsPerRow; j++)
        {
            if (j == (1 << dataIndex) - 1)
            {
                if (receivedDataBlock[i][j] != 0)
                {
                    if ((errorPosition + j + 1) > totalBitsPerRow)
                    {
                        break;
                        //cout << "Too many errors" << endl;
                    }
                    errorPosition += (j + 1);
                }
                dataIndex++;
            }
        }

        if (errorPosition > 0)
        {
            receivedDataBlock[i][errorPosition - 1] = !receivedDataBlock[i][errorPosition - 1];
        }
    }

    // remove check bits
    cout << endl
         << "Data block after removing check bits: " << endl;
    for (int i = 0; i < receivedDataBlock.size(); i++)
    {
        int dataIndex = 0;
        vector<bool> temp;
        for (int j = 0; j < totalBitsPerRow; j++)
        {
            if (j == (1 << dataIndex) - 1)
            {
                dataIndex++;
                continue;
            }
            temp.push_back(receivedDataBlock[i][j]);
        }
        receivedDataBlock[i] = temp;
        for (int j = 0; j < receivedDataBlock[i].size(); j++)
        {
            cout << receivedDataBlock[i][j];
        }
        cout << endl;
    }

    // transform the bits into characters
    string S_y;
    for (int i = 0; i < receivedDataBlock.size(); i++)
    {
        for (int j = 0; j < receivedDataBlock[i].size(); j += 8)
        {
            bitset<8> binary;
            for (int k = 0; k < 8; k++)
            {
                binary[7 - k] = receivedDataBlock[i][j + k];
            }
            S_y += char(binary.to_ulong());
        }
    }

    cout << endl
         << "Output frame: " << S_y << endl;

    return 0;
}