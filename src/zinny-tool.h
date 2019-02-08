#ifndef ZINNY_TOOL_H
#define ZINNY_TOOL_H

#include <string>
#include <iostream>

static auto const check_validate(int const in)
{
    if( in <= 1 ) return false;
    return true;
}

static auto const show_help()
{
    std::cout << "-p {out_name} {files}     \t | Packaged files to out_name" << std::endl;
    std::cout << "-u {file} {path}          \t | un-packaging file to path" << std::endl;
    std::cout << "-v {file}                 \t | show file in-side" << std::endl;
    std::cout << "-c {files}                \t | checking vaildated pot files" << std::endl;
    std::cout << "-a                        \t | checking vaildated trigger project" << std::endl;
}

#endif