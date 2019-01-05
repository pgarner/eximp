/*
 * Copyright 2019 by Philip N. Garner
 *
 * See the file COPYING for the licence associated with this software.
 *
 * Author(s):
 *   Phil Garner, January 2019
 */

#include <ostream>
#include "isobm.h"

using namespace std;

ISOBM::ISOBM()
{
    mData = 0;
}

ISOBM::ISOBM(var iFileName) : ISOBM()
{
    load(iFileName);
}

ISOBM::~ISOBM()
{
    if (mData)
    {
        ISODisposeMovie(mData);
        mData = 0;
    }
}


void ISOBM::load(var iFileName)
{
    ISOErr err = ISOOpenMovieFile(&mData, iFileName.str(), MP4OpenMovieNormal);
    if (err)
        cout << "ISO err: " << err << endl;
}


void ISOBM::dump()
{
    cout << "Dumping ISO:" << endl;
}
