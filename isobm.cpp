/*
 * Copyright 2019 by Philip N. Garner
 *
 * See the file COPYING for the licence associated with this software.
 *
 * Author(s):
 *   Phil Garner, January 2019
 */

#include <ostream>
#include <ctime>
#include <iomanip>
#include "isobm.h"

using namespace std;

ISOBM::ISOBM()
{
}

ISOBM::ISOBM(var iFileName) : ISOBM()
{
    load(iFileName);
}

ISOBM::~ISOBM()
{
}


void ISOBM::load(var iFileName)
{
    try
    {
        // We don't care about the actual movie data
        mParser.AddOption(ISOBMFF::Parser::Options::SkipMDATData);
        mParser.Parse(iFileName.str());
    }
    catch(...)
    {
        // If it throws just ignore it; valid() should return false
    }
}

static var format(int iNumber, int iWidth)
{
    varstream vs;
    vs << std::setw(iWidth) << std::setfill('0');
    vs << iNumber;
    return var(vs);
}

var ISOBM::date()
{
    // Start by getting the major brand from ftyp box
    shared_ptr<ISOBMFF::File> file = mParser.GetFile();
    shared_ptr<ISOBMFF::FTYP> ftyp = file->GetTypedBox<ISOBMFF::FTYP>("ftyp");
    var major = ftyp->GetMajorBrand().c_str();

    // The date method then depends on the brand
    if (major == "isom")
    {
        // For ISOM, it's a specific field in a MVHD box
        shared_ptr<ISOBMFF::ContainerBox> moov =
            file->GetTypedBox<ISOBMFF::ContainerBox>("moov");
        shared_ptr<ISOBMFF::MVHD> mvhd =
            moov->GetTypedBox<ISOBMFF::MVHD>("mvhd");

        // I hope it's gmtime() rather than localtime()
        time_t ct = mvhd->GetCreationTime();
        struct tm *tm = gmtime(&ct);
        var ret;
        ret[0] = format(tm->tm_year + 1900, 4);
        ret[1] = format(tm->tm_mon + 1, 2);
        ret[2] = format(tm->tm_mday, 2);
        ret[3] = format(tm->tm_hour, 2);
        ret[4] = format(tm->tm_min, 2);
        ret[5] = format(tm->tm_sec, 2);
        return ret;
    }

    // Didn't find a brand we understand
    return lube::nil;
}

void ISOBM::dump()
{
    cout << "Dumping ISO:" << endl;
    shared_ptr<ISOBMFF::File> file = mParser.GetFile();
    shared_ptr<ISOBMFF::FTYP> ftyp = file->GetTypedBox<ISOBMFF::FTYP>("ftyp");
    var major = ftyp->GetMajorBrand().c_str();
    cout << " Major brand: " << major << endl;
    cout << " Minor version: " << ftyp->GetMinorVersion() << endl;
    cout << " Compatible brands:";
    vector<string> cb = ftyp->GetCompatibleBrands();
    for (int i=0; i<cb.size(); i++)
        cout << " " << cb[i];
    cout << endl;

    if (major == "isom")
    {
        cout << "Looking for moov box containing mvhd box" << endl;
        shared_ptr<ISOBMFF::ContainerBox> moov =
            file->GetTypedBox<ISOBMFF::ContainerBox>("moov");
        shared_ptr<ISOBMFF::MVHD> mvhd =
            moov->GetTypedBox<ISOBMFF::MVHD>("mvhd");
        int v = mvhd->GetVersion();
        time_t ct = mvhd->GetCreationTime();
        time_t mt = mvhd->GetModificationTime();
        cout << " Version: " << v << endl;
        cout << " Creation time: " << asctime(gmtime(&ct));
        cout << " Modification time: " << asctime(gmtime(&mt));
        return;
    }
}
