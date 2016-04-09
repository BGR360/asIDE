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
