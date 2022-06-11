#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <unistd.h>
using namespace std;

int alphabetSize;

vector<string> readFile(string input){
    ifstream f(input);
    string line;
    vector<string> vecRead;
    if (!f){
        cout << "Error!" << endl;
    }
    else{
        while (getline(f, line)){
            vecRead.push_back(line);
        }
    }
    f.close();
    return vecRead;
}

long get_mem_usage(){
    struct rusage myusage;
    getrusage(RUSAGE_SELF, &myusage);
    return myusage.ru_maxrss;
}

vector<int> encoding(string S){
    vector<int> encS(S.length());
    for (int i=0; i<encS.size(); i++){
        if (S[i] == 'A') encS[i] = 1;
        else if (S[i] == 'C') encS[i] = 2;
        else if (S[i] == 'G') encS[i] = 3;
        else if (S[i] == 'T') encS[i] = 4;
    }
    return encS;
}

vector<int> getMod12(int n){
    int s12Len;
    if (n % 3 == 0) s12Len = n / 3 * 2;
    else if (n % 3 == 1) s12Len = n / 3 * 2 + 1;
    else if (n % 3 == 2) s12Len = n / 3 * 2 + 2;
    vector<int> s12(s12Len);
    int i;
    int j=0;
    for (i=0; i<n+1; i++){
        if (i % 3 != 0){
            s12[j] = i;
            j++;
        }
    }
    return s12;
}

vector<int> countingSort(vector<int> A){
    int i, j;
    int m = A.size();
    vector<int> B(m);
    vector<int> C(alphabetSize);
    for (i=0; i<alphabetSize; i++){
        C[i] = 0;
    }
    for (j=0; j<m; j++){
        C[A[j]] = C[A[j]] + 1;
    }
    for (i=1; i<alphabetSize+1; i++){
        C[i] = C[i] + C[i - 1];
    }
    for (j=m-1; j>=0; j--){
        B[C[A[j]] - 1] = j;
        C[A[j]] = C[A[j]] - 1;
    }
    return B;
}

void sortingTriple(vector<int> &S, vector<int> &s12Old, vector<vector<int> > &W){
    int i;
    int m = s12Old.size();
    vector<int> A(m);
    vector<int> B(m);
    vector<vector<int> > tempW(m, vector<int>(3, 0));
    tempW = W;
    vector<int> s12New;
    s12New = s12Old;
    for (int j=2; j>=0; j--){
        for (i=0; i<m; i++){
            A[i] = W[i][j];
        }
        B = countingSort(A);
        for (i=0; i<m; i++){
            for (int k=0; k<3; k++){
                tempW[i][k] = W[B[i]][k];
            }
            s12New[i] = s12Old[B[i]];
        }
        W = tempW;
        s12Old = s12New;
    }
}

void getPartitionName(vector<vector<int> > &triple, vector<int> &s12, vector<vector<int> > &W, vector<int> &s12W, vector<int> &position){
    int m = s12.size();
    vector<int> orderW(m);
    int i;
    int j = 1;
    orderW[0] = 1;
    for (i=1; i<m; i++){
        if ((W[i][0] == W[i - 1][0]) && (W[i][1] == W[i - 1][1]) && (W[i][2] == W[i - 1][2])){
            orderW[i] = orderW[i - 1];
        }
        else{
            j++;
            orderW[i] = j;
        }
    }
    vector<int> oriPos = position;
    for (i=0; i<s12.size(); i++){
        if (s12W[i] % 3 == 1) s12[(s12W[i] - 1)/ 3 * 2] = orderW[i];
        else s12[(s12W[i] - 2)/ 3 * 2 + 1] = orderW[i];
    }
    vector<vector<int> > triplePartition(s12.size(), vector<int>(3, 0));
    vector<int> s12Partition(s12.size());
    j = 0;
    for (i=0; i<s12.size(); i++){
        if (oriPos[i] % 3 == 1) {
            triplePartition[j][0] = triple[i][0];
            triplePartition[j][1] = triple[i][1];
            triplePartition[j][2] = triple[i][2];
            s12Partition[j] = s12[i];
            position[j] = oriPos[i];
            j++;
        }
    }
    for (i=1; i<s12.size(); i++){
        if (oriPos[i] % 3 == 2) {
            triplePartition[j][0] = triple[i][0];
            triplePartition[j][1] = triple[i][1];
            triplePartition[j][2] = triple[i][2];
            s12Partition[j] = s12[i];
            position[j] = oriPos[i];
            j++;
        }
    }
    triple = triplePartition;
    s12 = s12Partition;
}

vector<int> assignSA12(vector<int> s12, vector<int> position, int n){
    vector<int> SkewAlgorithm(vector<int> S, int n);
    int m = s12.size();
    int i;
    int n0 = (n + 2) / 3;;
    bool unique = true;
    int maxValue = s12[0];
    for (i=1; i<m; i++){
        if (s12[i] > maxValue) maxValue = s12[i];
    }
    if (maxValue < m) unique = false;
    vector<int> SA12(m);
    if (unique){
        for (i=0; i<m; i++){
            if (position[i] % 3 == 1) SA12[s12[i] - 1] = (position[i] - 1) / 3;
            else SA12[s12[i] - 1] = (position[i] - 2) / 3 + n0;
        }
    }
    else{
        s12.push_back(0);
        s12.push_back(0);
        s12.push_back(0);
        SA12 = SkewAlgorithm(s12, m);
    }
    return SA12;
}

vector<int> assignS0(vector<int> S, int n, vector<int> SA12){
    int m = SA12.size();
    int n0 = n + 1 - m;
    vector<int> positionMod0(n0);
    vector<int> letterMod0(n0);
    int i,j = 0;
    for (i=0; i<m; i++){
        if (SA12[i] < n0){
            positionMod0[j] = 3 * SA12[i];
            letterMod0[j] = S[positionMod0[j]];
            j++;
        }
    }
    vector<int> orderMod0(n0);
    orderMod0 = countingSort(letterMod0);
    vector<int> SA0(n0);
    for (i=0; i<n0; i++){
        SA0[i] = positionMod0[orderMod0[i]];
    }
    return SA0;
}

char comparePair(int pair0Char, int pair12Char, int pair0Num, int pair12Num){
    if (pair0Char < pair12Char) return '<'; //'<': fill in SA0; '>': fill in SA12
    else if (pair0Char > pair12Char) return '>';
    else{
        if (pair0Num < pair12Num) return '<';
        else return '>';
    }
}

char compareTriple(int triple0Char1, int triple12Char1, int triple0Char2, int triple12Char2, int triple0Num, int triple12Num){
    if (triple0Char1 < triple12Char1) return '<'; //'<': fill in SA0; '>': fill in SA12
    else if (triple0Char1 > triple12Char1) return '>';
    else{
        if (triple0Char2 < triple12Char2) return '<';
        else if (triple0Char2 > triple12Char2) return '>';
        else{
            if (triple0Num < triple12Num) return '<';
            else return '>';
        }
    }
}

vector<int> merge(vector<int> S, int n, vector<int> s12, vector<int> SA12, vector<int> SA0){
    vector<int> SA(n);
    int p = 0, t = 0, i, j;
    if (n % 3 == 0) p = 1;
    else t = 1;
    int n0 = (n + 2) / 3;
    int pair0Char, pair12Char, triple0Char1, triple12Char1, triple0Char2, triple12Char2;
    int pair0Num, pair12Num, triple0Num,triple12Num;
    char flag;
    for (int k=0; k<n; k++){
        j = SA0[p];
        if (SA12[t] < n0) i = SA12[t] * 3 + 1;
        else i = (SA12[t] - n0) * 3 + 2;
        if (p >= SA0.size()){
            SA[k] = i;
            t++;
        }
        else if (t >= SA12.size()){
            SA[k] = j;
            p++;
        }
        else{
            if (SA12[t] < n0){
                pair0Char = S[j];
                pair12Char = S[i];
                pair0Num = s12[j / 3];
                pair12Num = s12[SA12[t] + n0];
                flag = comparePair(pair0Char, pair12Char, pair0Num, pair12Num);
                if (flag == '<') {
                    SA[k] = j;
                    p++;
                }
                else{
                    SA[k] = i;
                    t++;
                }
            }
            else {
                triple0Char1 = S[j];
                triple12Char1 = S[i];
                triple0Char2 = S[j + 1];
                triple12Char2 = S[i + 1];
                triple0Num = s12[j / 3 + n0];
                triple12Num = s12[SA12[t] - n0 + 1];
                flag = compareTriple(triple0Char1, triple12Char1, triple0Char2, triple12Char2, triple0Num, triple12Num);
                if (flag == '<') {
                    SA[k] = j;
                    p++;
                }
                else{
                    SA[k] = i;
                    t++;
                }
            }
        }
    }
    return SA;
}

vector<int> SkewAlgorithm(vector<int> S, int n){
    vector<int> s12 = getMod12(n);
    int m = s12.size();
    vector<int> position = s12;
    vector<vector<int> > triple(m, vector<int>(3, 0));
    int i = 0;
    for (i=0; i<m; i++){
        triple[i][0] = S[s12[i]];
        triple[i][1] = S[s12[i] + 1];
        triple[i][2] = S[s12[i] + 2];
    }
    vector<vector<int> > W = triple;
    vector<int> s12W = s12;
    sortingTriple(S, s12W, W);
    getPartitionName(triple, s12, W, s12W, position);
    vector<int> SA12(m);
    SA12 = assignSA12(s12, position, n);
    for (i=0; i<m; i++){
        s12[SA12[i]] = i + 1;
    }
    vector<int> SA0(n + 1 - m);
    SA0 = assignS0(S, n, SA12);
    vector<int> SA(n);
    SA = merge(S, n, s12, SA12, SA0);
    return SA;
}

vector<int> calSA(vector<int> S){
    int n = S.size();
    S.push_back(0);
    S.push_back(0);
    S.push_back(0);
    vector<int> tmpSA(n);
    tmpSA = SkewAlgorithm(S, n);
    vector<int> SA(n + 1);
    SA[0] = n;
    for (int i=1; i<n+1; i++){
        SA[i] = tmpSA[i - 1];
    }
    return SA;
}

void BWT(vector<int> &S, vector<int> &L, int &I){
    int n = L.size();
    vector<int> SA(n);
    SA = calSA(S);
    S.push_back(0);
    for (int i=n-1; i>=0; i--){
        if (SA[i] == 0){
            L[i] = 0;
            I = i;
        }
        else L[i] = S[SA[i] - 1];
    }
}

void calC (vector<int> &L, vector<int> &C){
    int n = L.size();
    int i;
    for (i=0; i<n; i++){
        if (L[i] == 0) C[0]++;
        else if (L[i] == 1) C[1]++;
        else if (L[i] == 2) C[2]++;
        else if (L[i] == 3) C[3]++;
        else if (L[i] == 4) C[4]++;
    }
    for (i=1; i<5; i++){
        C[i] = C[i-1] + C[i];
    }
    for (i=5; i>0; i--){
        C[i] = C[i-1];
    }
    C[0] = 0;
}

void calF (vector<int> &L, vector<vector<int> > &F){
    int n = L.size();
    int i, j;
    for (i=0; i<n+1; i++){
        if (L[i] == 0) F[i][0]++;
        else if (L[i] == 1) F[i][1]++;
        else if (L[i] == 2) F[i][2]++;
        else if (L[i] == 3) F[i][3]++;
        else if (L[i] == 4) F[i][4]++;
    }
    for (i=1; i<n+1; i++){
        for (j=0; j<5; j++){
            F[i][j] = F[i-1][j] + F[i][j];
        }
    }
    for (i=n; i>0; i--){
        for (j=0; j<5; j++){
            F[i][j] = F[i-1][j];
        }
    }
    for (j=0; j<5; j++){
        F[i][j] = 0;
    }
}

int BWTQuery(vector<int> L, int m, vector<int> C, vector<vector<int> > F, vector<int> P) {
    int n = P.size();
    int l = 0;
    int h = m;
    int i = n - 1;
    while (i >= 0 & l < h){
        l = C[P[i]] + F[l][P[i]];
        h = C[P[i]] + F[h][P[i]];
        i = i - 1;
    }
    return h - l;
}

int main(int argc, const char * const argv[]){
//int main(){
    clock_t start, finish;
    start = clock();
    long baseline = get_mem_usage();
    const string inputFile(argv[1]);
//    string inputFile = "/Users/yinsaiyo/Documents/C++/BWT/dataset/test2/4.txt";
    vector<string> vecRead = readFile(inputFile); //read file from txt
    string strT = vecRead[0];
    string strP = vecRead[1];
    vector<int> T = encoding(strT);
    vector<int> P = encoding(strP);
    alphabetSize = T.size();
    if (T.size() >= 221) alphabetSize = T.size();
    else alphabetSize = 221; // to avoid memory leak when T.size() is small
    int m = T.size() + 1;
    vector<int> L(m);
    int I;
    BWT(T, L, I);
    vector<int> C = {0, 0, 0, 0, 0}; //Original alphabet: DNA nucleotides
    calC(L, C);
    vector<vector<int> > F(m+1, vector<int>(5, 0));
    calF(L, F);
    int BWTResult = 1;
    BWTResult = BWTQuery(L, m, C, F, P);
    cout << "Match count of BWT query:\t" << BWTResult << endl;
    cout << "Memory used:\t" << get_mem_usage() - baseline << endl;
    finish = clock();
    double duration = (double) (finish - start) / CLOCKS_PER_SEC;
    cout << "Running time:\t" << duration << endl;
    return 0;
}

