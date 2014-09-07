// str.cpp

#include "str.h"

void fullpath_to_dir(const wchar_t *path)
{
	wchar_t *tmp = (wchar_t *) path, *tmp2 = 0;

	for (; *tmp != 0; tmp++)
	{
		if (*tmp == L'/' || *tmp == L'\\')
		{
			tmp2 = ++tmp;
		}
	}

	if (tmp2)
	{
		*tmp2 = L'\0';
	}
}
