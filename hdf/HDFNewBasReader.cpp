#include "HDFNewBasReader.hpp"

int HDFNewBasReader::Initialize(const std::string &hdfBasFileName)
{
    //
    // Initialize access to the HDF file.  For reading bas files, this
    // involves:
    //   - Opening the h5 file, and initializing the rootGroup.
    //
    try {
        H5::Exception::dontPrint();
        hdfBasFile.openFile(hdfBasFileName.c_str(), H5F_ACC_RDONLY);
    } catch (H5::Exception &e) {
        std::cout << "ERROR, could not open bas.h5 file" << hdfBasFileName << ", exiting."
                  << std::endl;
        return 0;
    }

    if (rootGroup.Initialize(hdfBasFile, "/") == 0) {
        return 0;
    }
    // If /MultiPart/Parts exists, initialize and return 1;
    // otherwise, return 0
    if (rootGroup.ContainsObject("MultiPart") and
        multiPartGroup.Initialize(rootGroup.group, "MultiPart") != 0) {
        if (multiPartGroup.ContainsObject("Parts") and
            partsArray.InitializeForReading(multiPartGroup, "Parts") != 0) {
            basFileName = hdfBasFileName;
            return 1;  // Success
        }
    }
    return 0;  // Fail
}

std::vector<std::string> HDFNewBasReader::GetBaxMovieNames()
{
    // Read bax.h5 names from /MultiPart/Parts, not including path prefix.
    // e.g., m...._s1_p0.1.bax.h5
    std::vector<std::string> baxNames;
    baxNames.resize(BAXPERBAS);  // 3 bax files per bas file.
    for (int i = 0; i < BAXPERBAS; i++) {
        partsArray.Read(i, i + 1, &baxNames[i]);
    }
    return baxNames;
}

std::vector<std::string> HDFNewBasReader::GetBaxFileNames()
{
    // Get full paths to bax.h5 files,
    // e.g., /mnt/data3/vol53/2450598/0001/Analysis_Results/m..._s1_p0.1.bax.h5
    // Assumption: bax.h5 files are in the same directory as bas.h5.
    std::string prefix = "";
    size_t slashPos = basFileName.rfind("/");
    if (slashPos != std::string::npos) {
        prefix = basFileName.substr(0, slashPos + 1);
    }
    std::vector<std::string> baxNames = GetBaxMovieNames();
    for (int i = 0; i < int(baxNames.size()); i++) {
        baxNames[i] = prefix + baxNames[i];
    }
    return baxNames;
}

void HDFNewBasReader::Close()
{
    partsArray.Close();
    multiPartGroup.Close();
    rootGroup.Close();
    hdfBasFile.close();
    basFileName = "";
}
