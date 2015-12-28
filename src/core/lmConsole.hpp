#pragma once

#include <iostream>

#define ANSI_RESET "\x1b[0m"

#define ANSI_R "\x1b[31m"
#define ANSI_G "\x1b[32m"
#define ANSI_B "\x1b[34m"
#define ANSI_C "\x1b[36m"
#define ANSI_M "\x1b[35m"
#define ANSI_Y "\x1b[33m"

#define lmpmt(s) std::cout << ANSI_G << "LIGHTMAN: " << s  << " " << ANSI_RESET

#define lmout(s) std::cout << s << std::endl << std::flush

#define lmmsg(s) std::cout << ANSI_B << "LIGHTMAN: " << ANSI_RESET << s << std::endl << std::flush
#define lmwrn(s) std::cout << ANSI_Y << "WARNING: "  << ANSI_RESET << s << std::endl << std::flush
#define lmerr(s) std::cout << ANSI_R << "ERROR: "    << ANSI_RESET << s << std::endl << std::flush

#define lmins(s) std::getline(std::cin,s)

#define lmprog(s,num,sum) std::cout << s << num << "/" << sum << "\r" << std::flush
