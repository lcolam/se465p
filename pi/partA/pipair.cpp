#include <iostream>
#include <utility>
#include <string>
#include <map>
#include <vector>

using namespace std;

const int T_SUPPORT = 3;
const float T_CONFIDENCE = 0.65;

int main(int argc, char *argv[]) {
    string bcFile;
    if (argc >= 2) {
        bcFile = argv[1];
    }
    int suppThres = argc >= 3 ? atoi(argv[2]) : T_SUPPORT;
    float confThres = argc >= 4 ? atof(argv[3]) : T_CONFIDENCE;

    map<string, int> support;
    map<pair<string,string>, int> supportPair;

    map<string, vector<string> > scopes;
    map<pair<string,string>, vector<string> > scopesPair;

    string input;
    while (getline(cin, input)) {
        cout << input << endl;
    }

    cout << "END PROGRAM" << bcFile << "_" << suppThres << "_" << confThres << endl;
    return 0;
}

