#include "mem_istream.h"

OPENEXR_IMF_INTERNAL_NAMESPACE_HEADER_ENTER

bool Mem_IStream::read(char c[], int n)
{
	if (n + _exrpos <= _exrsize)
	{
		memcpy(c, (void *)(_exrbuf + _exrpos), n);
		_exrpos += n;
		return true;
	}
	else
	{
		return false;
	}
}

Int64 Mem_IStream::tellg()
{
	return _exrpos;
}

void Mem_IStream::seekg(Int64 pos)
{
	_exrpos = pos;
}

void Mem_IStream::clear()
{
	
}

OPENEXR_IMF_INTERNAL_NAMESPACE_HEADER_EXIT