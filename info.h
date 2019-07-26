#pragma once

#define __MAKE_INFO__(file, line)	file":"#line
#define __INFO__			__MAKE_INFO__(__FILE__, __LINE__)
