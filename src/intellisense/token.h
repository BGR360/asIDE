#ifndef TOKEN_H
#define TOKEN_H

#include <QHash>
#include <QString>

struct Token
{
    enum TokenType
    {
        Keyword,
        Comment,
        Label,
        Literal,
        Whitespace,
        Newline
    };

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
