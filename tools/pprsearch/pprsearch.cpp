#include "pprtools.h"

#include <iostream>
#include <string>
#include <fstream>
#include <algorithm>

int main(int argc, char** argv)
{
    if (argc < 2) {
        return -1;
    }

    std::string cnf(argv[1]);
    std::istream* ppr = &std::cin;
    std::ifstream pprfile;

    if (argc == 3) {
        pprfile.open(argv[2]);
        ppr = &pprfile;
    }

    auto formula = SATFormula::fromDimacs(cnf);

    while (!ppr->eof()) {
        std::string proofLine;

        std::getline(*ppr, proofLine);

        if (!proofLine.empty() && (proofLine[0] != 'c')) {
            SATFormula::clause_t proofParsed = parseClause(proofLine);

            if (!proofParsed.empty() && (std::find(proofParsed.begin() + 1, proofParsed.end(), proofParsed.front()) != proofParsed.end())) {
                Minisat::Lit separator = proofParsed.front();
                SATFormula::clause_t prclause;
                SATFormula::clause_t prwitness;
                auto it = proofParsed.begin() + 1;

                prclause.push_back(proofParsed.front());

                for (; *it != proofParsed.front(); it++) {
                    prclause.push_back(*it);
                }

                prwitness.push_back(*it);

                for (it++; it != proofParsed.end(); it++) {
                    prwitness.push_back(*it);
                }

                auto permutation = search_permutation(formula, prclause, prwitness);

                assert(check_ppr(formula, prclause, prwitness, permutation));

                proofParsed.push_back(proofParsed.front());

                for (auto& i : permutation) {
                    if (!Minisat::sign(i.first) && (i.first != i.second)) {
                        proofParsed.push_back(i.first);
                        proofParsed.push_back(i.second);
                    }
                }

                std::cout << proofParsed;
                formula.addClause(prclause);
            } else {
                std::cout << proofParsed;
                formula.addClause(proofParsed);
            }
        }
    }

    return 0;
}
