#ifndef _BLASR_SUFFIX_ARRAY_TYPES_HPP_
#define _BLASR_SUFFIX_ARRAY_TYPES_HPP_

#include <vector>

#include <alignment/suffixarray/SharedSuffixArray.hpp>
#include <alignment/suffixarray/SuffixArray.hpp>
#include <pbdata/Compare4BitCompressed.hpp>
#include <pbdata/CompressedSequence.hpp>
#include <pbdata/FASTASequence.hpp>

typedef SuffixArray<Nucleotide, std::vector<int> > DNASuffixArray;
typedef SuffixArray<Nucleotide, std::vector<int>, Compare4BitCompressed<Nucleotide>,
                    CompressedDNATuple<FASTASequence> >
    CompressedDNASuffixArray;

#endif  // _BLASR_SUFFIX_ARRAY_TYPES_HPP_
