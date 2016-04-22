//////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                                          //
//  The MIT License (MIT)                                                                                   //
//  Copyright (c) 2016 Benjamin Reeves                                                                      //
//                                                                                                          //
//  Permission is hereby granted, free of charge, to any person obtaining a copy of this software           //
//  and associated documentation files (the "Software"), to deal in the Software without restriction,       //
//  including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense,   //
//  and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so,   //
//  subject to the following conditions:                                                                    //
//                                                                                                          //
//  The above copyright notice and this permission notice shall be included in all copies or substantial    //
//  portions of the Software.                                                                               //
//                                                                                                          //
//  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT   //
//  LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.     //
//  IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, //
//  WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE     //
//  SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.                                                  //
//                                                                                                          //
//////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef TOKEN_H
#define TOKEN_H

#include <QDebug>
#include <QHash>
#include <QMetaType>
#include <QRegExp>
#include <QString>

#include "intellisense_global.h"

struct Token
{
    enum TokenType
    {
        Comment,
        Instruction,
        Include,
        IncludeFile,
        Label,
        IntLiteral,
        CharLiteral,
        Newline,
        Whitespace,
        Unrecognized,
        NUM_TOKEN_TYPES
    };

    static INTELLISENSE_EXPORT const QRegExp REGEX[NUM_TOKEN_TYPES - 1];
    static INTELLISENSE_EXPORT const QString TYPE_NAMES[NUM_TOKEN_TYPES];

    QString value;
    TokenType type;
};

Q_DECLARE_METATYPE(Token)

inline bool operator==(const Token& a, const Token& b)
{
    return a.value == b.value &&
            a.type == b.type;
}

inline bool operator!=(const Token& a, const Token& b)
{
    return a.value != b.value &&
            a.type != b.type;
}

inline uint qHash(const Token& key, uint seed)
{
    return qHash(key.value, seed) ^ key.type;
}

inline QDebug operator<<(QDebug dbg, const Token& token)
{
    dbg << Token::TYPE_NAMES[token.type] << ":" << token.value;

    return dbg;
}

#endif // TOKEN_H
