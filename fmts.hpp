#include "fmt/core.h"
#include "tokenizer/tokenizer.h"
#include "analyser/analyser.h"

namespace fmt {
	template<>
	struct formatter<miniplc0::ErrorCode> {
		template <typename ParseContext>
		constexpr auto parse(ParseContext &ctx) { return ctx.begin(); }

		template <typename FormatContext>
		auto format(const miniplc0::ErrorCode &p, FormatContext &ctx) {
			std::string name;
			switch (p) {
			case miniplc0::ErrNoError:
				name = "No error.";
				break;
			case miniplc0::ErrStreamError:
				name = "Stream error.";
				break;
			case miniplc0::ErrEOF:
				name = "EOF";
				break;
			case miniplc0::ErrInvalidInput:
				name = "The input is invalid.";
				break;
			case miniplc0::ErrInvalidIdentifier:
				name = "Identifier is invalid";
				break;
			case miniplc0::ErrIntegerOverflow:
				name = "The integer is too big(int64_t).";
				break;
			case miniplc0::ErrNoBegin:
				name = "The program should start with 'begin'.";
				break;
			case miniplc0::ErrNoEnd:
				name = "The program should end with 'end'.";
				break;
			case miniplc0::ErrNeedIdentifier:
				name = "Need an identifier here.";
				break;
			case miniplc0::ErrConstantNeedValue:
				name = "The constant need a value to initialize.";
				break;
			case miniplc0::ErrNoSemicolon:
				name = "Zai? Wei shen me bu xie fen hao.";
				break;
			case miniplc0::ErrInvalidVariableDeclaration:
				name = "The declaration is invalid.";
				break;
			case miniplc0::ErrIncompleteExpression:
				name = "The expression is incomplete.";
				break;
			case miniplc0::ErrNotDeclared:
				name = "The variable or constant must be declared before being used.";
				break;
			case miniplc0::ErrAssignToConstant:
				name = "Trying to assign value to a constant.";
				break;
			case miniplc0::ErrDuplicateDeclaration:
				name = "The variable or constant has been declared.";
				break;
			case miniplc0::ErrNotInitialized:
				name = "The variable has not been initialized.";
				break;
			case miniplc0::ErrInvalidAssignment:
				name = "The assignment statement is invalid.";
				break;
			case miniplc0::ErrInvalidPrint:
				name = "The output statement is invalid.";
				break;
			}
			return format_to(ctx.out(), name);
		}
	};

	template<>
	struct formatter<miniplc0::CompilationError> {
		template <typename ParseContext>
		constexpr auto parse(ParseContext &ctx) { return ctx.begin(); }

		template <typename FormatContext>
		auto format(const miniplc0::CompilationError &p, FormatContext &ctx) {
			return format_to(ctx.out(), "Line: {} Column: {} Error: {}", p.GetPos().first, p.GetPos().second, p.GetCode());
		}
	};
}

namespace fmt {
	template<>
	struct formatter<miniplc0::Token> {
		template <typename ParseContext>
		constexpr auto parse(ParseContext &ctx) { return ctx.begin(); }

		template <typename FormatContext>
		auto format(const miniplc0::Token &p, FormatContext &ctx) {
			return format_to(ctx.out(),
				"Line: {} Column: {} Type: {} Value: {}",
				p.GetStartPos().first, p.GetStartPos().second, p.GetType(), p.GetValueString());
		}
	};

	template<>
	struct formatter<miniplc0::TokenType> {
		template <typename ParseContext>
		constexpr auto parse(ParseContext &ctx) { return ctx.begin(); }

		template <typename FormatContext>
		auto format(const miniplc0::TokenType &p, FormatContext &ctx) {
			std::string name;
			switch (p) {
			case miniplc0::NULL_TOKEN:
				name = "NullToken";
				break;
			case miniplc0::INT:
				name = "UnsignedInteger";
				break;
			case miniplc0::IDENTIFIER:
				name = "Identifier";
				break;
			case miniplc0::VOID:
				name = "Void";
				break;
			case miniplc0::ELSE:
				name = "Else";
				break;
			case miniplc0::CHAR:
				name = "Char";
				break;
			case miniplc0::CONST:
				name = "Const";
				break;
			case miniplc0::PRINT:
				name = "Print";
				break;
			case miniplc0::PLUS_SIGN:
				name = "PlusSign";
				break;
			case miniplc0::MINUS_SIGN:
				name = "MinusSign";
				break;
			case miniplc0::MULTIPLICATION_SIGN:
				name = "MultiplicationSign";
				break;
			case miniplc0::DIVISION_SIGN:
				name = "DivisionSign";
				break;
			case miniplc0::EQUAL_SIGN:
				name = "EqualSign";
				break;
			case miniplc0::SEMICOLON:
				name = "Semicolon";
				break;
			case miniplc0::LEFT_BRACKET:
				name = "LeftBracket";
				break;
			case miniplc0::RIGHT_BRACKET:
				name = "RightBracket";
				break;
                case miniplc0::DOUBLE_EQUAL_SIGN:
                    name="DoubleEqual";
                    break;
			    case miniplc0::HEX_INTEGER:
			        name="HexInteger";
                    break;
                case miniplc0::LESS_THAN_SIGN:
                    name="LessThanSign";
                    break;
                case miniplc0::COMMA:
                    name="Comma";
                    break;


			}
			return format_to(ctx.out(), name);
		}
	};
}

namespace fmt {
	template<>
	struct formatter<miniplc0::Operation> {
		template <typename ParseContext>
		constexpr auto parse(ParseContext &ctx) { return ctx.begin(); }

		template <typename FormatContext>
		auto format(const miniplc0::Operation &p, FormatContext &ctx) {
			std::string name;
			switch (p) {
			case miniplc0::loada:
				name = "loada";
				break;
			case miniplc0::iadd:
				name = "iadd";
				break;
			case miniplc0::isub:
				name = "isub";
				break;
			case miniplc0::imul:
				name = "imul";
				break;
			case miniplc0::idiv:
				name = "idiv";
				break;
			case miniplc0::iprint:
				name = "iprint";
				break;
			case miniplc0::iload:
				name = "iload";
				break;
			case miniplc0::loadc:
				name = "loadc";
				break;
			case miniplc0::istore:
				name = "istore";
				break;
			    case miniplc0::icmp:
                    name = "icmp";
                    break;
                case miniplc0::jge:
                    name = "jge";
                    break;
                case miniplc0::jmp:
                    name = "jmp";
                    break;
                case miniplc0::ipush:
                    name="ipush";
                    break;
                case miniplc0::call:
                    name="call";
                    break;
                case miniplc0::iscan:
                    name="iscan";
                    break;
                case miniplc0::printl:
                    name="printl";
                    break;
                case miniplc0::iret:
                    name="iret";
                    break;
                case miniplc0::cprint:
                    name="cprint";
                    break;
                case miniplc0::ret:
                    name="ret";
                    break;
                case miniplc0::jl:
                    name="jl";
                    break;
                case miniplc0::jle:
                    name="jle";
                    break;
                case miniplc0::jg:
                    name="jd";
                    break;
                case miniplc0::je:
                    name="je";
                    break;
                case miniplc0::ineg:
                    name="ineg";
                    break;
                case miniplc0::jne:
                    name="jne";
                    break;

			}
			return format_to(ctx.out(), name);
		}
	};
	template<>
	struct formatter<miniplc0::Instruction> {
		template <typename ParseContext>
		constexpr auto parse(ParseContext &ctx) { return ctx.begin(); }

		template <typename FormatContext>
		auto format(const miniplc0::Instruction &p, FormatContext &ctx) {
			std::string name;
			switch (p.GetOperation())
			{
			case miniplc0::iload:
			case miniplc0::iadd:
			case miniplc0::isub:
			case miniplc0::imul:
			case miniplc0::idiv:
            case miniplc0::icmp:
            case miniplc0::ineg:
			case miniplc0::iprint:
            case miniplc0::cprint:
			case miniplc0::iscan:
            case miniplc0::printl:
            case miniplc0::istore:
            case miniplc0::iret:
            case miniplc0::ret:
				return format_to(ctx.out(), "{}", p.GetOperation());
			case miniplc0::jl:
            case miniplc0::jg:
            case miniplc0::jle:
            case miniplc0::jge:
            case miniplc0::je:
            case miniplc0::jne:
            case miniplc0::jmp:
            case miniplc0::jCOND:
            case miniplc0::ipush:
			case miniplc0::loadc:
			case miniplc0::call:
				return format_to(ctx.out(), "{} {}", p.GetOperation(), p.GetX());

                case miniplc0::loada:
                    return format_to(ctx.out(), "{} {},{}", p.GetOperation(), p.GetX(),p.GetY());

			}
			return format_to(ctx.out(), "ILL");
		}
	};
}