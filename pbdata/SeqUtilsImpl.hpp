#include <cstdint>
#include <iostream>
#include <vector>

#include <pbdata/DNASequence.hpp>
#include <pbdata/NucConversion.hpp>
#include <pbdata/SeqUtils.hpp>
#include <pbdata/utils.hpp>

template <typename T_Sequence>
int OnlyACTG(T_Sequence &seq)
{
    DNALength p;
    for (p = 0; p < seq.length; p++) {
        if (ThreeBit[seq.seq[p]] > 3) return 0;
    }
    return 1;
}

template <typename T_Sequence>
DNALength CountMasked(T_Sequence &seq)
{
    DNALength p;
    DNALength nMasked = 0;
    for (p = 0; p < seq.length; p++) {
        if ((seq.seq[p] >= 'a' and seq.seq[p] <= 'z') or seq.seq[p] == 'N') {
            nMasked++;
        }
    }
    return nMasked;
}

template <typename T_Sequence>
int CountNotMasked(T_Sequence &seq)
{
    int nUnmasked = 0;
    for (unsigned p = 0; p < seq.length; p++) {
        if (seq.seq[p] == 'A' or seq.seq[p] == 'C' or seq.seq[p] == 'G' or seq.seq[p] == 'T') {
            nUnmasked++;
        }
    }

    return nUnmasked;
}
