#include <hdf/HDFAlnInfoGroup.hpp>

int HDFAlnInfoGroup::InitializeNumPasses()
{
    numPasses.Initialize(alnInfoGroup, "NumPasses");
    return 1;
}

void HDFAlnInfoGroup::InitializeDefaultColumnNames(std::vector<std::string> &defaultColumnNames)
{
    defaultColumnNames.push_back("AlnID");
    defaultColumnNames.push_back("AlnGroupID");
    defaultColumnNames.push_back("MovieID");
    defaultColumnNames.push_back("RefGroupID");
    defaultColumnNames.push_back("tStart");
    defaultColumnNames.push_back("tEnd");
    defaultColumnNames.push_back("RCRefStrand");
    defaultColumnNames.push_back("HoleNumber");
    defaultColumnNames.push_back("SetNumber");
    defaultColumnNames.push_back("StrobeNumber");
    defaultColumnNames.push_back("MoleculeID");
    defaultColumnNames.push_back("rStart");
    defaultColumnNames.push_back("rEnd");
    defaultColumnNames.push_back("MapQV");
    defaultColumnNames.push_back("nM");
    defaultColumnNames.push_back("nMM");
    defaultColumnNames.push_back("nIns");
    defaultColumnNames.push_back("nDel");
    defaultColumnNames.push_back("Offset_begin");
    defaultColumnNames.push_back("Offset_end");
    defaultColumnNames.push_back("nBackRead");
    defaultColumnNames.push_back("nReadOverlap");
}

bool HDFAlnInfoGroup::Create(HDFGroup &parent)
{
    parent.AddGroup("AlnInfo");
    // Make sure it was created, and intialize this group to reference the newly created one.
    if (alnInfoGroup.Initialize(parent.group, "AlnInfo") == 0) {
        return 0;
    }
    std::vector<std::string> defaultColumnNames;
    InitializeDefaultColumnNames(defaultColumnNames);
    columnNames.Create(alnInfoGroup.group, "ColumnNames", defaultColumnNames);

    alnIndexArray.Create(&alnInfoGroup.group, "AlnIndex", defaultColumnNames.size());
    return true;
}

int HDFAlnInfoGroup::Initialize(HDFGroup &rootGroup)
{
    if (alnInfoGroup.Initialize(rootGroup.group, "AlnInfo") == 0) {
        return 0;
    }
    if (alnIndexArray.Initialize(alnInfoGroup, "AlnIndex") == 0) {
        return 0;
    }
    /*
     * This functionality should go into the python.
     if (!alnIndexArray.ContainsAttribute("ColumnNames")) {
     try {
     std::vector<std::string> defaultColumnNames;
     InitializeDefaultColumnNames(defaultColumnNames);
     columnNames.Create(alnIndexArray.dataset, "ColumnNames", defaultColumnNames);
     }
     catch(Execption e) {
    //
    // If the dataset is not writable
    }
    }
    */
    return 1;
}

HDFAlnInfoGroup::~HDFAlnInfoGroup() { alnInfoGroup.Close(); }

// Return size of /AlnInfo/AlnIndex in KB
UInt HDFAlnInfoGroup::GetAlnIndexSize()
{
    return alnIndexArray.GetNRows() / 1024 * sizeof(unsigned int) * NCols;
}

void HDFAlnInfoGroup::Read(AlnInfo &alnInfo)
{

    UInt nAlignments = alnIndexArray.GetNRows();
    alnInfo.alignments.resize(nAlignments);
    UInt alignmentIndex;
    UInt alignmentRow[NCols];
    for (alignmentIndex = 0; alignmentIndex < nAlignments; alignmentIndex++) {
        // Input the values.
        alnIndexArray.Read(alignmentIndex, alignmentIndex + 1, alignmentRow);
        alnInfo.alignments[alignmentIndex].StoreAlignmentIndex(alignmentRow, NCols);
    }
}

int HDFAlnInfoGroup::GetNAlignments() { return alnIndexArray.GetNRows(); }

unsigned int HDFAlnInfoGroup::WriteAlnIndex(std::vector<unsigned int> &aln)
{
    alnIndexArray.WriteRow(&aln[0], aln.size());
    return alnIndexArray.GetNRows();
}

void HDFAlnInfoGroup::ReadCmpAlignment(UInt alignmentIndex, CmpAlignment &cmpAlignment)
{
    UInt alignmentRow[NCols];
    alnIndexArray.Read(alignmentIndex, alignmentIndex + 1, alignmentRow);
    cmpAlignment.StoreAlignmentIndex(alignmentRow, NCols);
}
