#ifndef TOKEN_H
#define TOKEN_H

#include <QHash>
#include <QString>

struct Token
{
    struct Location
    {
        int lineNumber;
        int columnNumber;
    };

    enum TokenType
    {
        Keyword,
        Comment,
        Identifier,
        Literal
    };

    QString value;
    TokenType type;
    Location location;
};

inline bool operator==(const Token::Location& a, const Token::Location b)
{
    return a.lineNumber == b.lineNumber &&
            a.columnNumber == b.columnNumber;
}

inline bool operator!=(const Token::Location& a, const Token::Location& b)
{
    return a.lineNumber != b.lineNumber ||
            a.columnNumber != b.columnNumber;
}

inline bool operator==(const Token& a, const Token& b)
{
    return a.value == b.value &&
            a.type == b.type &&
            a.location == b.location;
}

inline bool operator!=(const Token& a, const Token& b)
{
    return a.value != b.value &&
            a.type != b.type &&
            a.location != b.location;
}

inline uint qHash(const Token::Location& key, uint seed)
{
    // Put the bits of the lineNumber in the first half of the hash,
    // and the bits of the columnNumber in the second half of the hash
    uint hash = key.lineNumber;
    hash <<= (sizeof(uint) / 2);
    hash |= key.columnNumber;

    // XOR the seed into it, cause that should do something
    hash = hash ^ seed;

    return hash;
}

inline uint qHash(const Token& key, uint seed)
{
    return qHash(key.value, seed) ^ qHash(key.location, seed);
}

#endif // TOKEN_H
