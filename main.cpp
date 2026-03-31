#include <iostream>
#include <string>
#include <cctype>
#include <stdexcept>

using namespace std;

// ---------------- TOKEN TYPES ----------------
enum TokenType {
    NUMBER, PLUS, MINUS, MUL, DIV, LPAREN, RPAREN, END
};

struct Token {
    TokenType type;
    double value;
};

// ---------------- LEXER ----------------
class Lexer {
    string text;
    int pos;

public:
    Lexer(string input) : text(input), pos(0) {}

    char currentChar() {
        if (pos >= text.size()) return '\0';
        return text[pos];
    }

    void advance() {
        pos++;
    }

    void skipWhitespace() {
        while (isspace(currentChar())) advance();
    }

    double number() {
        string result;
        while (isdigit(currentChar()) || currentChar() == '.') {
            result += currentChar();
            advance();
        }
        return stod(result);
    }

    Token getNextToken() {
        while (currentChar() != '\0') {

            if (isspace(currentChar())) {
                skipWhitespace();
                continue;
            }

            if (isdigit(currentChar()) || currentChar() == '.') {
                return {NUMBER, number()};
            }

            if (currentChar() == '+') {
                advance();
                return {PLUS, 0};
            }

            if (currentChar() == '-') {
                advance();
                return {MINUS, 0};
            }

            if (currentChar() == '*') {
                advance();
                return {MUL, 0};
            }

            if (currentChar() == '/') {
                advance();
                return {DIV, 0};
            }

            if (currentChar() == '(') {
                advance();
                return {LPAREN, 0};
            }

            if (currentChar() == ')') {
                advance();
                return {RPAREN, 0};
            }

            throw runtime_error("Invalid character");
        }

        return {END, 0};
    }
};

// ---------------- PARSER / INTERPRETER ----------------
class Parser {
    Lexer lexer;
    Token currentToken;

public:
    Parser(Lexer l) : lexer(l) {
        currentToken = lexer.getNextToken();
    }

    void eat(TokenType type) {
        if (currentToken.type == type) {
            currentToken = lexer.getNextToken();
        } else {
            throw runtime_error("Syntax error");
        }
    }

    // factor : NUMBER | (expr)
    double factor() {
        Token token = currentToken;

        if (token.type == NUMBER) {
            eat(NUMBER);
            return token.value;
        }
        else if (token.type == LPAREN) {
            eat(LPAREN);
            double result = expr();
            eat(RPAREN);
            return result;
        }
        else if (token.type == MINUS) { // unary minus
            eat(MINUS);
            return -factor();
        }

        throw runtime_error("Invalid factor");
    }

    // term : factor ((MUL | DIV) factor)*
    double term() {
        double result = factor();

        while (currentToken.type == MUL || currentToken.type == DIV) {
            Token token = currentToken;

            if (token.type == MUL) {
                eat(MUL);
                result *= factor();
            } else if (token.type == DIV) {
                eat(DIV);
                double divisor = factor();
                if (divisor == 0) throw runtime_error("Division by zero");
                result /= divisor;
            }
        }

        return result;
    }

    // expr : term ((PLUS | MINUS) term)*
    double expr() {
        double result = term();

        while (currentToken.type == PLUS || currentToken.type == MINUS) {
            Token token = currentToken;

            if (token.type == PLUS) {
                eat(PLUS);
                result += term();
            } else if (token.type == MINUS) {
                eat(MINUS);
                result -= term();
            }
        }

        return result;
    }
};

// ---------------- MAIN ----------------
int main() {
    cout << "Simple Arithmetic Compiler (type 'exit' to quit)\n";

    while (true) {
        cout << "\nEnter expression: ";
        string input;
        getline(cin, input);

        if (input == "exit") break;

        try {
            Lexer lexer(input);
            Parser parser(lexer);
            double result = parser.expr();

            cout << "Result: " << result << endl;
        } catch (exception &e) {
            cout << "Error: " << e.what() << endl;
        }
    }

    return 0;
}
