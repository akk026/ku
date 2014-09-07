// crt.cpp

#ifndef _DEBUG

#include <windows.h>

HANDLE hHeap;

int __argc;

#ifdef UNICODE

wchar_t ** __wargv;

void wWinMainCRTStartup()
{
	int ret;

	hHeap = GetProcessHeap();

	__wargv = CommandLineToArgvW(GetCommandLineW(), &__argc);

	ret = wWinMain(NULL, NULL, NULL, 0);

	LocalFree(__wargv);

	ExitProcess(ret);
}

#endif

void * malloc(size_t size)
{
	return HeapAlloc(hHeap, HEAP_GENERATE_EXCEPTIONS | HEAP_ZERO_MEMORY, size ? size : 1);
}

void free(void *ptr)
{
	if (ptr)
		HeapFree(hHeap, 0, ptr);
}

const wchar_t * wcsstr(const wchar_t *s1, const wchar_t *s2)
{
	register const wchar_t *p1;
	register wchar_t cc;

	if (!s1 || !*s1)
	{
		return NULL;
	}

	if (!s2 || !*s2)
	{
		return s1;
	}

	p1 = s1;
	cc = *s2;

	while (*p1++)
	{
		if (!(*p1 ^ cc))
		{
			const wchar_t *p0, *p2;

			p0 = p1;
			p1++;
			p2 = (wchar_t *) s2 + 1;

			while (*p1 && !(*p1 ^ *p2))
			{
				p1++, p2++;
			}

			if (!*p2)
			{
				return p0;
			}

			p1 = p0;
		}
	}

	return NULL;
}

#endif
