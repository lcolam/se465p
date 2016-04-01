#include <iostream>
#include <utility>
#include <string>
#include <map>
#include <set>
#include <iomanip>

using namespace std;

const int T_SUPPORT = 3;
const float T_CONFIDENCE = 0.65;

map<string, set<string> > scopeFunctions, origScopeFunctions;
map<string, int> support;
map<pair<string, string>, int> supportPair;
map<string, set<string> > scopes;
map<pair<string, string>, set<string> > scopesPair;

// Helper funcion to parse out function name from line of input
string findFunctionName(const string& input) {
    size_t foundStart = input.find("\'");
    size_t foundEnd = input.find("\'", foundStart+1);
    return input.substr(foundStart+1, foundEnd-foundStart-1);
}

// This function reads the piped input from cin and parses it into a map mapping from procedure scope to functions utilized
void readFromCallGraph() {
    string input;
    bool inScope = false;
    string scope;

    // Continue until no more input from cin
    while (getline(cin, input)) {
        // If currently not in scope portion of input, then continue reading until scope name found from call graph
        if (!inScope) {
            size_t found = input.find("Call graph node for function: \'");
            if (found != string::npos) {
                scope = findFunctionName(input);

                // Insert an empty set into the map for the key of the procedure scope read
                origScopeFunctions.insert(pair<string, set<string> >(scope, set<string>()));
                inScope = true;
            }
        } else {
            // If empty line, then end of procedure scope on call graph
            if (input.empty()) {
                inScope = false;
            } else {
                // If calls another function, add it into the set for the scope
                size_t found = input.find("calls function ");
                if (found != string::npos) {
                    string function = findFunctionName(input);
                    origScopeFunctions[scope].insert(function);
                }
            }
        }
    }
}

// This function resolves interprocedural scopes, one run of this resolves one level
void resolveInterProc() {
    // Iterate through all scopes contained in the built map
    for (map<string, set<string> >::iterator it = scopeFunctions.begin(); it != scopeFunctions.end(); ++it) {
        set<string> functionsToAdd;
        // Iterate through all functions utilized in the scope
        for (set<string>::iterator funcUsedIt = it->second.begin(); funcUsedIt != it->second.end(); ++funcUsedIt) {
            // For each function used, check if it is contained in the scope functions map
            map<string, set<string> >::iterator scopeIt = scopeFunctions.find(*funcUsedIt);
            // If found, means that it is interprocedural
            if (scopeIt != scopeFunctions.end()) {
                // Buffer all functions from the found scope to add into set
                for (set<string>::const_iterator funcIt = scopeIt->second.begin(); funcIt != scopeIt->second.end(); ++funcIt) {
                    functionsToAdd.insert(*funcIt);
                }
            }
        }
        // Add all buffered functions
        for (set<string>::const_iterator funcAddIt = functionsToAdd.begin(); funcAddIt != functionsToAdd.end(); ++funcAddIt) {
            it->second.insert(*funcAddIt);
        }
    }
}

void processFunctionsInScope(const string& scope, const set<string>& functions) {
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

// This function processes every scope contained inside the scope map
void processScopes() {
    for (map<string, set<string> >::const_iterator it = scopeFunctions.begin(); it != scopeFunctions.end(); ++it) {
        processFunctionsInScope(it->first, it->second);
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
                    // Check if function is inside original unmodified scope, if it isn't, then can skip
                    set<string>::iterator found = origScopeFunctions[*it1].find(funcPair.first);
                    if (found == origScopeFunctions[*it1].end()) {
                        continue;
                    }

                    string first = funcPair.first > funcPair.second ? funcPair.second : funcPair.first;
                    string second = funcPair.first < funcPair.second ? funcPair.second : funcPair.first;

                    cout << "bug: " << funcPair.first << " in " << *it1 << ", pair: (" << first << ", " << second << "), ";
                    cout << "support: " << suppIt->second << ", confidence: " << setprecision(2) << fixed << conf * 100 << "\%" << endl;
                }
            }
        }
    }
}

int main(int argc, char *argv[]) {
    string bcFile;
    if (argc >= 2) {
        bcFile = argv[1];
    }
    int suppThres = argc >= 3 ? atoi(argv[2]) : T_SUPPORT;
    float confThres = argc >= 4 ? atoi(argv[3]) / 100.00000 : T_CONFIDENCE;
    int resIPTimes = argc >= 5 ? atoi(argv[4]) : 0;

    readFromCallGraph();
    scopeFunctions = origScopeFunctions;
    for (int i = 0; i < resIPTimes; ++i) {
        resolveInterProc();
    }
    processScopes();
    printErrors(suppThres, confThres);

    return 0;
}

