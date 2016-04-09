#include "token.h"

const QRegExp Token::REGEX[Token::NUM_TOKEN_TYPES - 1] = {
        QRegExp("//[^\n]*"),            // Comment
        QRegExp("halt|add|sub|mult|div|cp|and|or|not|"
                "sl|sr|cpfa|cpta|be|bne|blt|call|ret",
                Qt::CaseInsensitive),   // Instruction
        QRegExp("#include"),            // Include
        QRegExp("[\\./\\w]+\\.e"),      // IncludeFile
        QRegExp("[A-Za-z]\\w*"),        // Label
        QRegExp("0x[0-9a-fA-F]+|-?[0-9]+"), // IntLiteral
        QRegExp("'.'"),                     // CharLiteral
        QRegExp("\\n"),                 // Newline
        QRegExp("\\s+")                 // Whitespace
    };
