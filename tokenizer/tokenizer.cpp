#include "tokenizer/tokenizer.h"

#include <cctype>
#include <sstream>

namespace miniplc0 {

	std::pair<std::optional<Token>, std::optional<CompilationError>> Tokenizer::NextToken() {
		if (!_initialized)
			readAll();
		if (_rdr.bad())
			return std::make_pair(std::optional<Token>(), std::make_optional<CompilationError>(0, 0, ErrorCode::ErrStreamError));
		if (isEOF())
			return std::make_pair(std::optional<Token>(), std::make_optional<CompilationError>(0, 0, ErrorCode::ErrEOF));
		auto p = nextToken();
		if (p.second.has_value())
			return std::make_pair(p.first, p.second);
		auto err = checkToken(p.first.value());
		if (err.has_value())
			return std::make_pair(p.first, err.value());
		return std::make_pair(p.first, std::optional<CompilationError>());
	}

	std::pair<std::vector<Token>, std::optional<CompilationError>> Tokenizer::AllTokens() {
		std::vector<Token> result;
		while (true) {
			auto p = NextToken();
			if (p.second.has_value()) {
				if (p.second.value().GetCode() == ErrorCode::ErrEOF)
					return std::make_pair(result, std::optional<CompilationError>());
				else
					return std::make_pair(std::vector<Token>(), p.second);
			}
			result.emplace_back(p.first.value());
		}
	}

	// 注意：这里的返回值中 Token 和 CompilationError 只能返回一个，不能同时返回。
	std::pair<std::optional<Token>, std::optional<CompilationError>> Tokenizer::nextToken() {
		// 用于存储已经读到的组成当前token字符
		std::stringstream ss;
		// 分析token的结果，作为此函数的返回值
		std::pair<std::optional<Token>, std::optional<CompilationError>> result;
		// <行号，列号>，表示当前token的第一个字符在源代码中的位置
		std::pair<int64_t, int64_t> pos;
		// 记录当前自动机的状态，进入此函数时是初始状态
		DFAState current_state = DFAState::INITIAL_STATE;
		// 这是一个死循环，除非主动跳出
		// 每一次执行while内的代码，都可能导致状态的变更
		while (true) {
			// 读一个字符，请注意auto推导得出的类型是std::optional<char>
			// 这里其实有两种写法
			// 1. 每次循环前立即读入一个 char
			// 2. 只有在可能会转移的状态读入一个 char
			// 因为我们实现了 unread，为了省事我们选择第一种
			auto current_char = nextChar();
			// 针对当前的状态进行不同的操作
			switch (current_state) {

				// 初始状态
				// 这个 case 我们给出了核心逻辑，但是后面的 case 不用照搬。
			case INITIAL_STATE: {
				// 已经读到了文件尾
				if (!current_char.has_value())
					// 返回一个空的token，和编译错误ErrEOF：遇到了文件尾
					return std::make_pair(std::optional<Token>(), std::make_optional<CompilationError>(0, 0, ErrEOF));

				// 获取读到的字符的值，注意auto推导出的类型是char
				auto ch = current_char.value();
				// 标记是否读到了不合法的字符，初始化为否
				auto invalid = false;

				// 使用了自己封装的判断字符类型的函数，定义于 tokenizer/utils.hpp
				// see https://en.cppreference.com/w/cpp/string/byte/isblank
				if (miniplc0::isspace(ch)) // 读到的字符是空白字符（空格、换行、制表符等）
					current_state = DFAState::INITIAL_STATE; // 保留当前状态为初始状态，此处直接break也是可以的
				else if (!miniplc0::isprint(ch)) // control codes and backspace
					invalid = true;
                else if (miniplc0::isdigit(ch)) { // 读到的字符是数字
                    current_state = DFAState::UNSIGNED_INTEGER_STATE;
                    if(ch=='0') {
                        current_char = nextChar();
                        if (!current_char.has_value()) {
                            unreadLast();
                        }
                        else {
                            char ch_tmp = current_char.value();
                            if (ch_tmp=='x'||ch_tmp=='X'){
                                ss<<ch;
                                ch=ch_tmp;
                                current_char = nextChar();
                                if (!current_char.has_value()) {
                                    return std::make_pair(std::optional<Token>(), std::make_optional<CompilationError>(pos, ErrorCode::ErrInvalidInput));
                                }
                                else {
                                    ch_tmp = current_char.value();
                                    if (isdigit(ch_tmp) || (ch_tmp <= 'F' && ch_tmp >= 'A') ||
                                        (ch_tmp <= 'f' && ch_tmp >= 'a')) {
                                        current_state = DFAState::HEX_INTEGER_STATE;
                                        ss << ch;
                                        ch = ch_tmp;
                                    }
                                    else
                                        return std::make_pair(std::optional<Token>(), std::make_optional<CompilationError>(pos, ErrorCode::ErrInvalidInput));
                                }
                            }
                            else
                                unreadLast();
                        }
                    }
                }
				else if (miniplc0::isalpha(ch)) // 读到的字符是英文字母
					current_state = DFAState::IDENTIFIER_STATE; // 切换到标识符的状态
				else {
					switch (ch) {
					case '=': // 如果读到的字符是`=`，则切换到等于号的状态
                        current_state=DFAState ::EQUAL_STATE;
                         break;

					    case '<':
                            current_state=DFAState ::LESS_STATE;
                            break;

					    case '>':
                            current_state=DFAState ::MORE_STATE;
                            break;

					    case '\\':
                            current_state=DFAState ::BACKSLASH_STATE;
                            break;

					    case '!':
					        current_state=DFAState ::EXCLAMATION_STATE;
					        break;

					case '-':
                        return std::make_pair(std::make_optional<Token>(TokenType::MINUS_SIGN, '-', previousPos(), currentPos()),
                                              std::optional<CompilationError>());
					case '+':
                        return std::make_pair(std::make_optional<Token>(TokenType::PLUS_SIGN, '+', previousPos(), currentPos()),
                                              std::optional<CompilationError>());
					case '*':
                        return std::make_pair(std::make_optional<Token>(TokenType::MULTIPLICATION_SIGN, '*', previousPos(), currentPos()),
                                              std::optional<CompilationError>());
					case '/':
                        return std::make_pair(std::make_optional<Token>(TokenType::DIVISION_SIGN, '/', previousPos(), currentPos()),
                                              std::optional<CompilationError>());
                        case ')':
                            return std::make_pair(std::make_optional<Token>(TokenType::RIGHT_BRACKET, ')', previousPos(), currentPos()),
                                                  std::optional<CompilationError>());

                        case '(':
                            return std::make_pair(std::make_optional<Token>(TokenType::LEFT_BRACKET, '(', previousPos(), currentPos()),
                                                  std::optional<CompilationError>());
                        case ';':
                            return std::make_pair(std::make_optional<Token>(TokenType::SEMICOLON, ';', previousPos(), currentPos()),
                                                  std::optional<CompilationError>());
					    case '{':
                            return std::make_pair(std::make_optional<Token>(TokenType::LEFT_BRACES, '{', previousPos(), currentPos()),
                                                  std::optional<CompilationError>());
                        case '}':
                            return std::make_pair(std::make_optional<Token>(TokenType::RIGHT_BRACES, '}', previousPos(), currentPos()),
                                                  std::optional<CompilationError>());
                        case ',':
                            return std::make_pair(std::make_optional<Token>(TokenType::COMMA, ',', previousPos(), currentPos()),
                                                  std::optional<CompilationError>());
					///// 请填空：
					///// 对于其他的可接受字符
					///// 切换到对应的状态

					// 不接受的字符导致的不合法的状态
					default:
						invalid = true;
						break;
					}
				}
				// 如果读到的字符导致了状态的转移，说明它是一个token的第一个字符
				if (current_state != DFAState::INITIAL_STATE)
					pos = previousPos(); // 记录该字符的的位置为token的开始位置
				// 读到了不合法的字符
				if (invalid) {
                    unreadLast();// 回退这个字符
					// 返回编译错误：非法的输入
					return std::make_pair(std::optional<Token>(), std::make_optional<CompilationError>(pos, ErrorCode::ErrInvalidInput));
				}
				// 如果读到的字符导致了状态的转移，说明它是一个token的第一个字符
				if (current_state != DFAState::INITIAL_STATE) // ignore white spaces
					ss << ch; // 存储读到的字符
				break;
			}

								// 当前状态是无符号整数
			case UNSIGNED_INTEGER_STATE: {
				// 请填空：
                if (!current_char.has_value()) { // 如果当前已经读到了文件尾，则解析已经读到的字符串为整数
                    std::string val;
                    ss >> val;
                    Token x = Token(TokenType::INT, val, pos.first, pos.second, _ptr.first, _ptr.second);
                    if(x.GetValueString().length()>10||(x.GetValueString().length()==10&&(x.GetValueString().compare("2147483647")<0)))
                        return std::make_pair(std::optional<Token>(), std::make_optional<CompilationError>(pos,ErrIntegerOverflow));//     解析成功则返回无符号整数类型的token，否则返回编译错误
                    else
                        return std::make_pair(std::optional<Token>(x), std::optional<CompilationError>());
                }
                char ch = current_char.value();
                if (isdigit(ch))// 如果读到的字符是数字，则存储读到的字符
                    ss << ch;
                else if (isalpha(ch)) {// 如果读到的是字母，则存储读到的字符，并切换状态到标识符
                    current_state = DFAState::IDENTIFIER_STATE;
                    ss << ch;
                }
                else if(ch=='.'||ch=='e'||ch=='E'){
                    current_state=DFAState::DOUBLE_STATE;
                    ss<<ch;
                }
                else {
                    unreadLast();
                    std::string val;
                    ss >> val;
                    Token x = Token(TokenType::INT, val, pos.first, pos.second, _ptr.first, _ptr.second);
                    if(x.GetValueString().length()>10||(x.GetValueString().length()==10&&(x.GetValueString().compare("2147483647")>0)))
                        return std::make_pair(std::optional<Token>(), std::make_optional<CompilationError>(pos,ErrIntegerOverflow));//     解析成功则返回无符号整数类型的token，否则返回编译错误
                    else
                        return std::make_pair(std::optional<Token>(x), std::optional<CompilationError>());
                }
				// 如果读到的字符不是上述情况之一，则回退读到的字符，并解析已经读到的字符串为整数
				//     解析成功则返回无符号整数类型的token，否则返回编译错误
				break;
			}
                case HEX_INTEGER_STATE: {
                    // 请填空：
                    if (!current_char.has_value()) { // 如果当前已经读到了文件尾，则解析已经读到的字符串为整数
                        std::string val;
                        ss >> val;
                        Token x = Token(TokenType::HEX_INTEGER, val, pos.first, pos.second, _ptr.first, _ptr.second);
                        //if(x.GetValueString().length()>10||(x.GetValueString().length()==10&&(x.GetValueString().compare("2147483647")<0)))
                        //return std::make_pair(std::optional<Token>(), std::make_optional<CompilationError>(pos,ErrIntegerOverflow));//     解析成功则返回无符号整数类型的token，否则返回编译错误
                        // else
                        return std::make_pair(std::optional<Token>(x), std::optional<CompilationError>());
                    }
                    char ch = current_char.value();
                    if (isdigit(ch)||(ch<='F'&&ch>='A')||(ch<='f'&&ch>='a'))// 如果读到的字符是合法16进制，则存储读到的字符
                        ss << ch;
                    else {
                        unreadLast();
                        std::string val;
                        ss >> val;
                        Token x = Token(TokenType::HEX_INTEGER, val, pos.first, pos.second, _ptr.first, _ptr.second);
                        //if(x.GetValueString().length()>10||(x.GetValueString().length()==10&&(x.GetValueString().compare("2147483647")>0)))
                        //return std::make_pair(std::optional<Token>(), std::make_optional<CompilationError>(pos,ErrIntegerOverflow));//     解析成功则返回无符号整数类型的token，否则返回编译错误
                        //else
                        return std::make_pair(std::optional<Token>(x), std::optional<CompilationError>());
                    }
                    // 如果读到的字符不是上述情况之一，则回退读到的字符，并解析已经读到的字符串为整数
                    //     解析成功则返回无符号整数类型的token，否则返回编译错误
                    break;
                }

			case IDENTIFIER_STATE: {
                // 请填空：
                if (!current_char.has_value()) {
                    std::string val;// 如果当前已经读到了文件尾，则解析已经读到的字符串
                    ss >> val;//              如果解析结果是关键字，那么返回对应关键字的token，否则返回标识符的token
                    Token x = Token(check_keyword(val).value(), val, pos, _ptr);
                    return std::make_pair(std::optional<Token>(x), std::optional<CompilationError>());
                }
                    // 如果读到的是字符或字母，则存储读到的字符
                else if (isalpha(current_char.value()) || isdigit(current_char.value()))
                    ss << current_char.value();
                else {
                    unreadLast();// 如果读到的字符不是上述情况之一，则回退读到的字符，并解析已经读到的字符串
                    std::string val;//     如果解析结果是关键字，那么返回对应关键字的token，否则返回标识符的token
                    ss >> val;
                    Token x = Token(check_keyword(val).value(), val, pos.first, pos.second, _ptr.first, _ptr.second);
                    return std::make_pair(std::optional<Token>(x), std::optional<CompilationError>());
                }
                break;
            }

            case EQUAL_STATE:{
                if (!current_char.has_value())// 返回一个空的token，和编译错误ErrEOF：遇到了文件尾
                    return std::make_pair(std::make_optional<Token>(TokenType::EQUAL_SIGN, '=', pos,currentPos()),
                                          std::optional<CompilationError>());
                char ch=current_char.value();
                if(ch!='='){
                    unreadLast();
                    return std::make_pair(std::make_optional<Token>(TokenType::EQUAL_SIGN, '=', pos,currentPos()),
                                          std::optional<CompilationError>());
                }
                else{
                    ss<<ch;
                    std::string val;
                    ss>>val;
                    return std::make_pair(std::make_optional<Token>(TokenType::DOUBLE_EQUAL_SIGN, val, pos, currentPos()),
                                          std::optional<CompilationError>());
                }
                }

                case BACKSLASH_STATE:{
                    break;
                }

                case LESS_STATE:{
                    if (!current_char.has_value())// 返回一个空的token，和编译错误ErrEOF：遇到了文件尾
                        return std::make_pair(std::make_optional<Token>(TokenType::LESS_THAN_SIGN, '<', pos, currentPos()),
                                              std::optional<CompilationError>());
                    char ch=current_char.value();
                    if(ch!='='){
                        unreadLast();
                        return std::make_pair(std::make_optional<Token>(TokenType::LESS_THAN_SIGN, '<', pos, currentPos()),
                                              std::optional<CompilationError>());
                    }
                    else if(current_char=='='){
                        ss<<ch;
                        std::string val;
                        ss>>val;
                        return std::make_pair(std::make_optional<Token>(TokenType::NOT_MORE_THAN_SIGN, val ,pos, currentPos()),
                                              std::optional<CompilationError>());
                    }
                    break;
                }
                case MORE_STATE:{
                    if (!current_char.has_value())// 返回一个空的token，和编译错误ErrEOF：遇到了文件尾
                        return std::make_pair(std::make_optional<Token>(TokenType::MORE_THAN_SIGN, '>', pos, currentPos()),
                                              std::optional<CompilationError>());
                    char ch=current_char.value();
                    if(ch!='='){
                        unreadLast();
                        return std::make_pair(std::make_optional<Token>(TokenType::MORE_THAN_SIGN, '>', pos, currentPos()),
                                              std::optional<CompilationError>());
                    }
                    else if(ch=='='){
                        ss<<ch;
                        std::string val;
                        ss>>val;
                        return std::make_pair(std::make_optional<Token>(TokenType::NOT_LESS_THAN_SIGN, val, pos, currentPos()),
                                              std::optional<CompilationError>());
                    }
                    break;
                }

                case EXCLAMATION_STATE:{
                    printf("%d\n",current_char.value());
                    if (!current_char.has_value())// 返回一个空的token，和编译错误ErrEOF：遇到了文件尾
                        return std::make_pair(std::optional<Token>(), std::make_optional<CompilationError>(pos, ErrEOF));
                    char ch=current_char.value();
                    if(ch!='='){
                        unreadLast();
                        return std::make_pair(std::optional<Token>(), std::make_optional<CompilationError>(pos, ErrInvalidInput));
                    }
                    else if(ch=='='){
                        ss<<ch;
                        std::string val;
                        ss>>val;
                        return std::make_pair(std::make_optional<Token>(TokenType::NOT_EQUAL_SIGN, val, pos, currentPos()),
                                              std::optional<CompilationError>());
                    }
                    break;
                }

			default:
			    printf("thh\n");
				DieAndPrint("unhandled state.");
				break;
			}
		}
		// 预料之外的状态，如果执行到了这里，说明程序异常
		return std::make_pair(std::optional<Token>(), std::optional<CompilationError>());
	}

	std::optional<CompilationError> Tokenizer::checkToken(const Token& t) {
		switch (t.GetType()) {
			case IDENTIFIER: {
				auto val = t.GetValueString();
				if (miniplc0::isdigit(val[0]))
					return std::make_optional<CompilationError>(t.GetStartPos().first, t.GetStartPos().second, ErrorCode::ErrInvalidIdentifier);
				break;
			}
		default:
			break;
		}
		return {};
	}

	void Tokenizer::readAll() {
		if (_initialized)
			return;
		for (std::string tp; std::getline(_rdr, tp);)
			_lines_buffer.emplace_back(std::move(tp + "\n"));
		_initialized = true;
		_ptr = std::make_pair<int64_t, int64_t>(0, 0);
		return;
	}

	// Note: We allow this function to return a postion which is out of bound according to the design like std::vector::end().
	std::pair<uint64_t, uint64_t> Tokenizer::nextPos() {
		if (_ptr.first >= _lines_buffer.size())
			DieAndPrint("advance after EOF");
		if (_ptr.second == _lines_buffer[_ptr.first].size() - 1)
			return std::make_pair(_ptr.first + 1, 0);
		else
			return std::make_pair(_ptr.first, _ptr.second + 1);
	}

	std::pair<uint64_t, uint64_t> Tokenizer::currentPos() {
		return _ptr;
	}

	std::pair<uint64_t, uint64_t> Tokenizer::previousPos() {
		if (_ptr.first == 0 && _ptr.second == 0)
			DieAndPrint("previous position from beginning");
		if (_ptr.second == 0)
			return std::make_pair(_ptr.first - 1, _lines_buffer[_ptr.first - 1].size() - 1);
		else
			return std::make_pair(_ptr.first, _ptr.second - 1);
	}

	std::optional<char> Tokenizer::nextChar() {
		if (isEOF())
			return {}; // EOF
		auto result = _lines_buffer[_ptr.first][_ptr.second];
		_ptr = nextPos();
		return result;
	}

	bool Tokenizer::isEOF() {
		return _ptr.first >= _lines_buffer.size();
	}

	// Note: Is it evil to unread a buffer?
	void Tokenizer::unreadLast() {
		_ptr = previousPos();
	}


    std::optional<TokenType> Tokenizer::check_keyword(std::string val) {
        if (val.compare("void")==0)
            return TokenType::VOID;
        else if (val.compare("const")==0)
            return TokenType::CONST;
        else if (val.compare("struct")==0)
            return TokenType::STRUCT;
        else if (val.compare("char")==0)
            return TokenType::CHAR;
        else if (val.compare("int")==0)
            return TokenType::INT;
        else if (val.compare("double")==0)
            return TokenType::DOUBLE;
        else if (val.compare("if")==0)
            return TokenType::IF;
        else if (val.compare("else")==0)
            return TokenType::ELSE;
        else if (val.compare("char")==0)
            return TokenType::CHAR;
        else if (val.compare("switch")==0)
            return TokenType::SWITCH;
        else if (val.compare("case")==0)
            return TokenType::CASE;
        else if (val.compare("default")==0)
            return TokenType::DEFAULT;
        else if (val.compare("while")==0)
            return TokenType::WHILE;
        else if (val.compare("for")==0)
            return TokenType::FOR;
        else if (val.compare("do")==0)
            return TokenType::DO;
        else if (val.compare("return")==0)
            return TokenType::RETURN;
        else if (val.compare("break")==0)
            return TokenType::BREAK;
        else if (val.compare("continue")==0)
            return TokenType::CONTINUE;
        else if (val.compare("scan")==0)
            return TokenType::SCAN;
        else if (val.compare("print")==0)
            return TokenType::PRINT;
        else
            return TokenType::IDENTIFIER;
    }
}