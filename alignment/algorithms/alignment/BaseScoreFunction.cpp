#include <alignment/algorithms/alignment/BaseScoreFunction.hpp>

BaseScoreFunction::BaseScoreFunction(int insP, int delP, int subPriorP, int delPriorP,
                                     int affineExtensionP, int affineOpenP)
{
    ins = insP;
    del = delP;
    substitutionPrior = subPriorP;
    globalDeletionPrior = delPriorP;
    affineExtend = affineExtensionP;
    affineOpen = affineOpenP;
}
