#include "QDBC.h"
#include "DBC.h"

QDBC::QDBC()
{
}

///////////////////////////////////////////////////////////////////////
IDBC* QDBC::GetDBClient()
{
    IDBC* pDBClient = new DBC();
    if (pDBClient==NULL)
        return NULL;

    return pDBClient;
}
