//
// Created by zeliboba on 2/18/24.
//

#ifndef DIPLOMA_DEFS_HPP
#define DIPLOMA_DEFS_HPP

// Binary alphabet size + 2 for special symbols
static const int BinaryAlphabetSize = 258;
using Token = int;
static const Token PPMEndOfFileToken = BinaryAlphabetSize - 1;
// todo: make const
static /*const*/ Token BWTEndOfFileToken = BinaryAlphabetSize - 2;

#endif //DIPLOMA_DEFS_HPP
