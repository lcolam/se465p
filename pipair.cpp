#include <iostream>
#include <utility>
#include <string>
#include <map>
#include <set>
#include <iomanip>

using namespace std;

const int T_SUPPORT = 3;
const float T_CONFIDENCE = 0.65;

map<string, int> support;
map<pair<string, string>, int> supportPair;
map<string, set<string> > scopes;
map<pair<string, string>, set<string> > scopesPair;

string findFunctionName(const string& input) {
    size_t foundStart = input.find("\'");
    size_t foundEnd = input.find("\'", foundStart+1);
    return input.substr(foundStart+1, foundEnd-foundStart-1);
}

void processFunctionsFromScope(const string& scope, const set<string>& functions) {
    for (set<string>::const_iterator it1 = functions.begin(); it1 != functions.end(); ++it1) {
        map<string, int>::iterator supportIndex = support.find(*it1);
        if (supportIndex == support.end()) {
            support.insert(pair<string, int>(*it1, 0));
        }
        support[*it1]++;

        map<string, set<string> >::iterator scopeIndex = scopes.find(*it1);
        if (scopeIndex == scopes.end()) {
            scopes.insert(pair<string, set<string> >(*it1, set<string>()));
        }
        scopes[*it1].insert(scope);

        for (set<string>::const_iterator it2 = functions.begin(); it2 != functions.end(); ++it2) {
            if (*it1 != *it2) {
                pair<string, string> funcPair(*it1, *it2);
                map<pair<string, string>, int>::iterator supportPairIndex = supportPair.find(funcPair);
                if (supportPairIndex == supportPair.end()) {
                    supportPair.insert(pair<pair<string, string>, int>(funcPair, 0));
                }
                supportPair[funcPair]++;

                map<pair<string, string>, set<string> >::iterator scopesPairIndex = scopesPair.find(funcPair);
                if (scopesPairIndex == scopesPair.end()) {
                    scopesPair.insert(pair<pair<string, string>, set<string> >(funcPair, set<string>()));
                }
                scopesPair[funcPair].insert(scope);
            }
        }
    }
}

void printErrors(const int& suppThres, const float& confThres) {
    for (map<pair<string, string>, int>::const_iterator suppIt = supportPair.begin(); suppIt != supportPair.end(); ++suppIt) {
        pair<string, string> funcPair = suppIt->first;
        int supp = support[funcPair.first];
        float conf = suppIt->second * 1.00000 / supp;
        if (supp > suppThres && conf >= confThres) {
            set<string> scopesWith = scopesPair[funcPair];
            set<string> allScopes = scopes[funcPair.first];
            for (set<string>::iterator it1 = allScopes.begin(); it1 != allScopes.end(); ++it1) {
                set<string>::iterator it2 = scopesWith.find(*it1);

                if (it2 == scopesWith.end()) {
                    string first = funcPair.first > funcPair.second ? funcPair.second : funcPair.first;
                    string second = funcPair.first < funcPair.second ? funcPair.second : funcPair.first;

                    cout << "bug: " << funcPair.first << " in " << *it1 << ", pair: (" << first << ", " << second << "), ";
                    cout << "support: " << suppIt->second << ", confidence: " << setprecision(2) << fixed << conf * 100 << "\%" << endl;
                }
            }
        }
    }
}

void readFromCallGraph() {
    string input;
    bool inScope = false;
    string scope;
    set<string> functionsCalled;

    while (getline(cin, input)) {
        if (!inScope) {
            size_t found = input.find("Call graph node for function: \'");
            if (found != string::npos) {
                inScope = true;
                scope = findFunctionName(input);
                functionsCalled.clear();
            }
        } else {
            if (input.empty()) {
                inScope = false;
                processFunctionsFromScope(scope, functionsCalled);
            } else {
                size_t found = input.find("calls function ");
                if (found != string::npos) {
                    string function = findFunctionName(input);
                    functionsCalled.insert(function);
                }
            }
        }
    }
    if (inScope) {
        processFunctionsFromScope(scope, functionsCalled);
    }
}

int main(int argc, char *argv[]) {
    string bcFile;
    if (argc >= 2) {
        bcFile = argv[1];
    }
    int suppThres = argc >= 3 ? atoi(argv[2]) : T_SUPPORT;
    float confThres = argc >= 4 ? atoi(argv[3]) / 100.00000 : T_CONFIDENCE;

    readFromCallGraph();        
    printErrors(suppThres, confThres);

    return 0;
}

