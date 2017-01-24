#ifndef _BLASR_SENTINAL_FILE_HPP_
#define _BLASR_SENTINAL_FILE_HPP_

#include "HDF2DArray.h"
#include "HDFArray.h"

class HDFSentinal
{
public:
    HDFStringArray parts;
    HDF2DArray<unsigned int> holeLookup;
    HDFGroup multiPartGroup;

    void Initialize(HDFGroup &rootGroup)
    {
        multiPartGroup.Initialize(rootGroup, "MultiPart");
        parts.Initialize(multiPartGroup, "Parts");
        holeLookup.Initialize(multiPartGroup, "HoleLookup");
    }
};

#endif
