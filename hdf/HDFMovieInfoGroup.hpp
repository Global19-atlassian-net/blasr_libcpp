#ifndef HDF_MOVIE_INFO_GROUP_H_
#define HDF_MOVIE_INFO_GROUP_H_

#include "../pbdata/saf/MovieInfo.hpp"
#include "HDFArray.hpp"
#include "HDFGroup.hpp"

class HDFMovieInfoGroup
{
public:
    HDFGroup movieInfoGroup;
    HDFArray<UInt> idArray;
    HDFStringArray nameArray;

    HDFStringArray whenStartedArray;
    HDFArray<float> frameRateArray;

    ~HDFMovieInfoGroup() { movieInfoGroup.Close(); }

    bool Create(HDFGroup &parentGroup)
    {
        parentGroup.AddGroup("MovieInfo");
        if (movieInfoGroup.Initialize(parentGroup.group, "MovieInfo") == 0) {
            return 0;
        }
        idArray.Create(movieInfoGroup, "ID");
        nameArray.Create(movieInfoGroup, "Name");
        return true;
    }

    int Initialize(HDFGroup &parentGroup)
    {
        if (movieInfoGroup.Initialize(parentGroup.group, "MovieInfo") == 0) {
            return 0;
        }
        if (idArray.Initialize(movieInfoGroup, "ID") == 0) {
            return 0;
        }
        if (nameArray.Initialize(movieInfoGroup, "Name") == 0) {
            return 0;
        }
        return 1;
    }

    void Read(MovieInfo &movieInfo)
    {
        int nId = idArray.arrayLength;
        movieInfo.id.resize(nId);
        idArray.Read(0, nId, &movieInfo.id[0]);

        int nName = nameArray.arrayLength;
        movieInfo.name.resize(nName);
        int i;
        for (i = 0; i < nName; i++) {
            nameArray.Read(i, i + 1, &movieInfo.name[i]);
        }
    }

    int AddMovie(std::string &movieName)
    {
        nameArray.Write(&movieName, 1);
        unsigned int id = nameArray.size();
        idArray.Write(&id, 1);
        return id;
    }

    void StoreFrameRate(int movieIndex, float frameRate)
    {
        if (movieIndex < 0) {
            std::cout << "ERROR. Invalid movie index " << movieIndex << std::endl;
            std::exit(EXIT_FAILURE);
        }

        if (!frameRateArray.IsInitialized()) {
            if (!movieInfoGroup.ContainsObject("FrameRate")) {
                frameRateArray.Create(movieInfoGroup, "FrameRate");
            } else {
                frameRateArray.Initialize(movieInfoGroup, "FrameRate");
            }
        }
        frameRateArray.WriteToPos(&frameRate, 1, movieIndex);
    }
};

#endif
