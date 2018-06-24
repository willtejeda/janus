#ifndef MEM_ISTREAM_H
#define MEM_ISTREAM_H

#include "ImfInt64.h"
#include "ImfIO.h"
#include "ImfNamespace.h"
#include "ImfExport.h"

#ifdef __linux__
#include <string.h>
#endif

OPENEXR_IMF_INTERNAL_NAMESPACE_HEADER_ENTER

class Mem_IStream : public IStream
{
public:
	Mem_IStream(unsigned char *exrbuf, int exrsize) : 
		IStream("dummy"), 
		_exrpos(0), 
		_exrsize(exrsize) 
	{
		_exrbuf = exrbuf;
	}

	virtual bool read(char c[], int n);
	virtual Int64 tellg();
	virtual void seekg(Int64 pos);
	virtual void clear();

private:
	Int64 _exrpos;
	Int64 _exrsize;
	unsigned char *_exrbuf;
};

OPENEXR_IMF_INTERNAL_NAMESPACE_HEADER_EXIT

#endif // MEM_ISTREAM_H
