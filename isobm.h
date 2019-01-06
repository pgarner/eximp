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
#include "ISOBMFF.hpp"

class ISOBM
{
public:
    ISOBM();
    ISOBM(var iFileName);
    ~ISOBM();
    bool valid() { return bool(mParser.GetFile()); };
    void dump();
    var date();
private:
    ISOBMFF::Parser mParser;
    void load(var iFileName);
};

#endif // ISOBM_H
