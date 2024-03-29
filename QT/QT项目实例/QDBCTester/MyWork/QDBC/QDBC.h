#ifndef QDBC_H
#define QDBC_H

#include "QDBC_global.h"
#include "IDBC.h"

class QDBC_EXPORT QDBC
{
public:
    QDBC();

public:
    static IDBC* GetDBClient();


};

#endif // QDBC_H
