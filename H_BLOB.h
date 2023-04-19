#pragma once

#define EMPTY_BLOB(stBlob) (stBlob.cbSize == 0 || stBlob.pBlobData == NULL)

#define INIT_BLOB(stBlob) {stBlob.pBlobData = NULL; stBlob.cbSize = 0;}

#define ALOC_BLOB(stBlob, size) {stBlob.cbSize = size; stBlob.pBlobData = new BYTE[size];}

#define RESET_BLOB(stBlob, value) memset(stBlob.pBlobData, value, stBlob.cbSize)

#define COPY_BLOB(stDstBlob, stSrcBlob) {stDstBlob.cbSize = stSrcBlob.cbSize; memcpy(stDstBlob.pBlobData, stSrcBlob.pBlobData, stSrcBlob.cbSize);}

#define RELEASE_BLOB(stBlob) if (stBlob.pBlobData) { \
							delete[] stBlob.pBlobData; \
							stBlob.pBlobData = NULL; \
							stBlob.cbSize = 0; }

#define REALOC_BLOB(stBlob, size) {RELEASE_BLOB(stBlob); ALOC_BLOB(stBlob, size);}