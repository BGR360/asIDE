#ifndef TOKEN_H
#define TOKEN_H

#include <QHash>
#include <QMetaType>
#include <QRegExp>
#include <QString>

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

    static const QRegExp REGEX[NUM_TOKEN_TYPES - 1];

    QString value;
    TokenType type;
};

Q_DECLARE_METATYPE(Token);

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

#endif // TOKEN_H
