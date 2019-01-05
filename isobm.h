/*
 * Copyright 2019 by Philip N. Garner
 *
 * See the file COPYING for the licence associated with this software.
 *
 * Author(s):
 *   Phil Garner, January 2019
 */
#ifndef ISOBM_H
#define ISOBM_H

#include <lube.h>
#include "ISOMovies.h"

class ISOBM
{
public:
    ISOBM();
    ISOBM(var iFileName);
    ~ISOBM();
    bool valid() { return mData; };
    void dump();
    //var date();
private:
    ISOMovie mData;
    void load(var iFileName);
};

#endif // ISOBM_H
