#include "analyser.h"

#include <climits>
#include <iostream>
#include <sstream>
namespace miniplc0 {
	std::pair<std::vector<struct Function>, std::optional<CompilationError>> Analyser::Analyse() {
		auto err = analyseProgram();
		if (err.has_value())
			return std::make_pair(std::vector<Function>(), err);
		else{
            return std::make_pair(functions_list, std::optional<CompilationError>());
		}
	}
    std::optional<CompilationError> Analyser::analyseProgram() {
        Symboltables.push_back(Symboltable{});
        Function f =Function{};
        f.fun_name="init";
        f.paramnum=0;
        f.return_flag=false;
        f.type=TokenType ::VOID;
        f.instructions=std::vector<Instruction>{};
        functions_list.push_back(f);
        auto err = analyseVarialble(false);
        if (err.has_value())
            return err;
        while(true){


            auto tk=nextToken();
            if(!tk.has_value())
                return {};
            else
                unreadToken();
            err=analyseFunction();
            if(err.has_value())
                return err;
        }
    }
    // <variable-declaration>
    /*
    <variable-declaration> ::=[<const-qualifier>]<type-specifier><init-declarator-list>';'
    <init-declarator-list> ::=<init-declarator>{','<init-declarator>}
    <init-declarator> ::=<identifier>[<initializer>]
    <initializer> ::= '='<expression>
     */
    std::optional<CompilationError> Analyser::analyseVarialble(bool in_fun) {
        bool Varialbe_flag=true;
        while(true) {
           auto tk = nextToken();
           if (tk.value().GetType() != TokenType::CONST) {

               tk = nextToken();   //标识符
               if (!tk.has_value())
                   return std::make_optional<CompilationError>(tk.value().GetStartPos(), ErrorCode::ErrNoBegin);
               tk = nextToken();   //区分（ 与 = , ;
               if (!tk.has_value())
                   return std::make_optional<CompilationError>(tk.value().GetStartPos(), ErrorCode::ErrNoBegin);
               if (tk.value().GetType() != TokenType::EQUAL_SIGN&&tk.value().GetType() != TokenType::COMMA&&tk.value().GetType() != TokenType::SEMICOLON)
                   Varialbe_flag = false;
               unreadToken();
               unreadToken();
           }
           unreadToken();

           if (Varialbe_flag) {
               auto err = variable_declaration(in_fun);
               if (err.has_value())
                   return err;
           }
           else {
               return {};
           }
       }
    }

    std::optional<CompilationError> Analyser::analyseFunction(){
            Symboltables.push_back(Symboltable{});   //压入函数符号表
            auto err=function_definition();
            if(err.has_value())
                return err;
            err=compound_statement();
            if(err.has_value())
                return err;
            return {};
        }



    std::optional<CompilationError> Analyser::variable_declaration(bool in_fun) {
		bool const_flag=true;
        auto tk = nextToken();
		// ['const']
		if(!tk.has_value())
            return std::make_optional<CompilationError>(tk.value().GetStartPos(), ErrorCode::ErrNoBegin);
		if(tk.value().GetType()!=TokenType::CONST){
		    const_flag=false;
		    unreadToken();
		}
        //<type-specifier>
		tk=nextToken();
        if(!tk.has_value())
            return std::make_optional<CompilationError>(tk.value().GetStartPos(), ErrorCode::ErrNoBegin);
        if(tk.value().GetType()!=TokenType::INT ){
            return std::make_optional<CompilationError>(tk.value().GetStartPos(), ErrorCode::ErrInvalidVariableDeclaration);
        }

        //<init-declarator-list>
        tk=nextToken();
        if(!tk.has_value())
            return std::make_optional<CompilationError>(tk.value().GetStartPos(), ErrorCode::ErrEOF);
        else
            unreadToken();

        do {
            tk=nextToken();
            if (tk.value().GetType() != TokenType::IDENTIFIER) {   //分析identifier
                return std::make_optional<CompilationError>(tk.value().GetStartPos(), ErrorCode::ErrEOF);
            }
            if(isDeclared(tk.value().GetValueString() )&& (in_fun&&find_symbol(tk.value().GetValueString()).level!=1)){
                return std::make_optional<CompilationError>(tk.value().GetStartPos(), ErrorCode::ErrDuplicateDeclaration);
            }
            addVariable(tk.value(),const_flag);//插入符号表
            tk = nextToken();
            if (tk.value().GetType() == TokenType::EQUAL_SIGN) {
                auto err=analyseExpression();
                if(err.has_value())
                    return std::make_optional<CompilationError>(tk.value().GetStartPos(), ErrorCode::ErrInvalidVariableDeclaration);
            }
            else {
                if(const_flag)
                    return std::make_optional<CompilationError>(tk.value().GetStartPos(), ErrorCode::ErrConstantNeedValue);
                unreadToken();
                functions_list.back().instructions.push_back(Instruction(Operation ::ipush,0));
            }
            tk=nextToken();

        }while(tk.value().GetType()==TokenType::COMMA);

        if(!tk.has_value()||tk.value().GetType()!=TokenType::SEMICOLON){
            return std::make_optional<CompilationError>(tk.value().GetStartPos(), ErrorCode::ErrNoSemicolon);
            }

        return {};
        }
/*<function-definition> ::=
    <type-specifier><identifier><parameter-clause><compound-statement>

<parameter-clause> ::=
    '(' [<parameter-declaration-list>] ')'
<parameter-declaration-list> ::=
    <parameter-declaration>{','<parameter-declaration>}
<parameter-declaration> ::=
    [<const-qualifier>]<type-specifier><identifier>*/

    std::optional<CompilationError> Analyser::function_definition() {

        auto tk=nextToken();

        if(!tk.has_value())
            return std::make_optional<CompilationError>(tk.value().GetStartPos(), ErrorCode::ErrEOF);

        if(tk.value().GetType()!=TokenType::INT && tk.value().GetType()!=TokenType::VOID){
            return std::make_optional<CompilationError>(tk.value().GetStartPos(), ErrorCode::ErrInvalidVariableDeclaration);
        }
        TokenType tp=tk.value().GetType();
        tk=nextToken();
        if(!tk.has_value()||tk.value().GetType()!=TokenType::IDENTIFIER)
            return std::make_optional<CompilationError>(tk.value().GetStartPos(), ErrorCode::ErrInvalidInput);

        for(int i=0;i<functions_list.size();i++ ){
            if(functions_list[i].fun_name.compare(tk.value().GetValueString())==0)
                return std::make_optional<CompilationError>(tk.value().GetStartPos(), ErrorCode::ErrDuplicateDeclaration);
        }
        //记录表格

        Function f =Function{};
        f.fun_name=tk.value().GetValueString();
        f.paramnum=0;
        f.return_flag=false;
        f.type=tp;
        f.instructions=std::vector<Instruction>{};
        functions_list.push_back(f); //压入函数表

        Constants.insert(make_pair(tk.value().GetValueString(),Constants.size()));

        tk=nextToken();
        if(!tk.has_value()||tk.value().GetType()!=TokenType::LEFT_BRACKET)
            return std::make_optional<CompilationError>(tk.value().GetStartPos(), ErrorCode::ErrNotDeclared);

        tk=nextToken();
        if(tk.value().GetType()==TokenType::RIGHT_BRACKET){
            unreadToken();
        }
        else {
            while (true) {
                if (!tk.has_value() ||
                    (tk.value().GetType() != TokenType::INT ))
                    return std::make_optional<CompilationError>(tk.value().GetStartPos(),
                                                                ErrorCode::ErrAssignToConstant);
                functions_list.back().paramnum++;
                tk = nextToken();
                if (!tk.has_value() || tk.value().GetType() != TokenType::IDENTIFIER)
                    return std::make_optional<CompilationError>(tk.value().GetStartPos(), ErrorCode::ErrIntegerOverflow);
                if(!isDeclared(tk.value().GetValueString())||find_symbol(tk.value().GetValueString()).level==1)
                    addVariable(tk.value(),false);
                else
                    return std::make_optional<CompilationError>(tk.value().GetStartPos(), ErrorCode::ErrDuplicateDeclaration);
                tk = nextToken();
                if (!tk.has_value())
                    return std::make_optional<CompilationError>(tk.value().GetStartPos(),
                                                                ErrorCode::ErrIntegerOverflow);
                if (tk.value().GetType() == TokenType::RIGHT_BRACKET) {
                    unreadToken();
                    break;
                }
                else if (tk.value().GetType() == TokenType::COMMA) {
                    tk=nextToken();
                    continue;
                }
            }
        }
        tk=nextToken();
        if(!tk.has_value()||tk.value().GetType()!=TokenType::RIGHT_BRACKET)
            return std::make_optional<CompilationError>(tk.value().GetStartPos(), ErrorCode::ErrNoSemicolon);

        return {};
    }


	std::optional<CompilationError> Analyser::compound_statement() {
        auto tk = nextToken();

        if (!tk.has_value())
            return std::make_optional<CompilationError>(tk.value().GetStartPos(), ErrorCode::ErrNotDeclared);
        if (tk.value().GetType() != TokenType::LEFT_BRACES)
            return std::make_optional<CompilationError>(tk.value().GetStartPos(), ErrorCode::ErrNoBegin);
        tk = nextToken();
        if (tk.value().GetType() == TokenType::CONST || tk.value().GetType() == TokenType::INT) {
            unreadToken();
            auto err = analyseVarialble(true);
            if (err.has_value())
                return err;
        }
        else
            unreadToken();

        tk=nextToken();
        if(tk.value().GetType()==TokenType::RIGHT_BRACES){
            return {};
        }
        else{
            unreadToken();
        }

        do{
            auto err=statement_seq();

            if(err.has_value())
                return err;
            tk=nextToken();

            if(tk.value().GetType()!=TokenType ::RIGHT_BRACES){
                unreadToken();
                continue;
            }

            else
                break;
        }while(true);

        return {};
    }

    std::optional<CompilationError> Analyser::statement_seq(){
        auto tk=nextToken();
        auto err=std::make_optional<CompilationError>(tk.value().GetStartPos(), ErrorCode::ErrNoBegin);
        TokenType tp=tk.value().GetType();
        if(tp!=TokenType::IF&&tp!=TokenType::WHILE&&tp!=TokenType::SCAN&&tp!=TokenType::PRINT&&tp!=TokenType::IDENTIFIER&&tp!=TokenType::RETURN&&tp!=TokenType::SEMICOLON){
            if(tp==RIGHT_BRACES)
                return {};
            else
                return std::make_optional<CompilationError>(tk.value().GetStartPos(), ErrorCode::ErrNoBegin);
        }
        switch(tp){
            case TokenType::IF:
                unreadToken();
                err=condition_statement();
                if(err.has_value())
                    return err;
                break;
            case TokenType::WHILE:
                unreadToken();
                err=loop_statement();
                if(err.has_value())
                    return err;
                break;
            case TokenType::SCAN:
                unreadToken();
                err=scan_statement();
                if(err.has_value())
                    return err;
                break;
            case TokenType::PRINT:
                unreadToken();
                err=print_statement();
                if(err.has_value())
                    return err;
                break;
            case TokenType::IDENTIFIER:

                tk=nextToken();
                unreadToken();
                unreadToken();
                if(tk.value().GetType()==TokenType::EQUAL_SIGN){
                    err=analyseAssignmentStatement();
                    if(err.has_value())
                        return err;
                }
                else{
                    err=function_call();
                    if(err.has_value())
                        return err;
                }
                break;
            case TokenType ::RETURN:
                unreadToken();
                err=return_seq();
                if(err.has_value())
                    return err;
                break;
            case TokenType::SEMICOLON:
                break;

        }
        return {};
    }

    std::optional<CompilationError> Analyser::condition_statement() {
        auto tk=nextToken();
        if(!tk.has_value()||tk.value().GetType()!=TokenType::IF)
            return std::make_optional<CompilationError>(tk.value().GetStartPos(), ErrorCode::ErrNoBegin);
        tk=nextToken();
        if(!tk.has_value()||tk.value().GetType()!=TokenType::LEFT_BRACKET)
            return std::make_optional<CompilationError>(tk.value().GetStartPos(), ErrorCode::ErrNoBegin);

        auto err=condition();
        if(err.has_value())
            return err;

        tk=nextToken();
        if(!tk.has_value()||tk.value().GetType()!=TokenType::RIGHT_BRACKET)
            return std::make_optional<CompilationError>(tk.value().GetStartPos(), ErrorCode::ErrNoBegin);

        err=statement_seq();
        if (err.has_value())
            return err;

        tk=nextToken();
        if(tk.value().GetType()==TokenType::ELSE){

            int offset=seq_id.top();
            seq_id.pop();

            functions_list.back().instructions.at(offset).SetX(functions_list.back().instructions.size()+1);
            functions_list.back().instructions.push_back(Instruction(Operation ::jmp));
            seq_id.push(functions_list.back().instructions.size()-1);

            err=statement_seq();
            if (err.has_value())
                return err;

            offset=seq_id.top();
            seq_id.pop();
            functions_list.back().instructions.at(offset).SetX(functions_list.back().instructions.size());
        }
        else{
            unreadToken();
            int offset=seq_id.top();
            seq_id.pop();
            functions_list.back().instructions.at(offset).SetX(functions_list.back().instructions.size());
            return {};
        }
        return {};
    }
    std::optional<CompilationError> Analyser::loop_statement() {
        auto tk=nextToken();
        if(!tk.has_value()||tk.value().GetType()!=TokenType::WHILE)
            return std::make_optional<CompilationError>(tk.value().GetStartPos(), ErrorCode::ErrNoBegin);
        tk=nextToken();
        if(!tk.has_value()||tk.value().GetType()!=TokenType::LEFT_BRACKET)
            return std::make_optional<CompilationError>(tk.value().GetStartPos(), ErrorCode::ErrNoBegin);
        seq_id.push(functions_list.back().instructions.size());
        auto err=condition();
        if(err.has_value())
            return err;

        tk=nextToken();
        if(!tk.has_value()||tk.value().GetType()!=TokenType::RIGHT_BRACKET)
            return std::make_optional<CompilationError>(tk.value().GetStartPos(), ErrorCode::ErrNoBegin);

        err=statement_seq();
        if (err.has_value())
            return err;

        int off=seq_id.top();
        seq_id.pop();
        functions_list.back().instructions.at(off).SetX(functions_list.back().instructions.size()+1);
        off=seq_id.top();
        seq_id.pop();
        functions_list.back().instructions.push_back(Instruction(Operation ::jmp,off));


        return {};
    }
    std::optional<CompilationError> Analyser::print_statement() {
        auto tk=nextToken();
        if(!tk.has_value()||tk.value().GetType()!=TokenType::PRINT)
            return std::make_optional<CompilationError>(tk.value().GetStartPos(), ErrorCode::ErrDuplicateDeclaration);
        tk=nextToken();
        if(!tk.has_value()||tk.value().GetType()!=TokenType::LEFT_BRACKET)
            return std::make_optional<CompilationError>(tk.value().GetStartPos(), ErrorCode::ErrNotDeclared);
        while (true){
                auto err=analyseExpression();
                if(err.has_value())
                    return err;
                functions_list.back().instructions.push_back(Instruction(Operation ::iprint));
                tk=nextToken();
                if(!tk.has_value())
                    return std::make_optional<CompilationError>(tk.value().GetStartPos(), ErrorCode::ErrNoBegin);
                if(tk.value().GetType()==TokenType::COMMA){
                    functions_list.back().instructions.push_back(Instruction(Operation ::bipush,32));
                    functions_list.back().instructions.push_back(Instruction(Operation ::cprint));
                    continue;
                }
                else
                    {
                    unreadToken();
                        break;
                    }

        }

        tk=nextToken();
        if(!tk.has_value()||tk.value().GetType()!=TokenType::RIGHT_BRACKET)
            return std::make_optional<CompilationError>(tk.value().GetStartPos(), ErrorCode::ErrNoBegin);
        tk=nextToken();
        if(!tk.has_value()||tk.value().GetType()!=TokenType::SEMICOLON)
            return std::make_optional<CompilationError>(tk.value().GetStartPos(), ErrorCode::ErrNoBegin);
        functions_list.back().instructions.push_back(Instruction(Operation ::printl));
        return {};
    }
    std::optional<CompilationError> Analyser::scan_statement() {
        auto tk=nextToken();
        if(!tk.has_value()||tk.value().GetType()!=TokenType::SCAN)
            return std::make_optional<CompilationError>(tk.value().GetStartPos(), ErrorCode::ErrNoBegin);
        tk=nextToken();
        if(!tk.has_value()||tk.value().GetType()!=TokenType::LEFT_BRACKET)
            return std::make_optional<CompilationError>(tk.value().GetStartPos(), ErrorCode::ErrNoBegin);

        tk=nextToken();
        if(!tk.has_value()||tk.value().GetType()!=TokenType::IDENTIFIER)
            return std::make_optional<CompilationError>(tk.value().GetStartPos(), ErrorCode::ErrNoBegin);
        if((!isDeclared(tk.value().GetValueString()))||isConstant(tk.value().GetValueString()))
            return std::make_optional<CompilationError>(tk.value().GetStartPos(), ErrorCode::ErrDuplicateDeclaration);
        symbol sym =find_symbol(tk.value().GetValueString());
        if(sym.tp==TokenType::VOID)
            return std::make_optional<CompilationError>(tk.value().GetStartPos(), ErrorCode::ErrNoBegin);


        tk=nextToken();
        if(!tk.has_value()||tk.value().GetType()!=TokenType::RIGHT_BRACKET)
            return std::make_optional<CompilationError>(tk.value().GetStartPos(), ErrorCode::ErrNoBegin);
        tk=nextToken();
        if(!tk.has_value()||tk.value().GetType()!=TokenType::SEMICOLON)
            return std::make_optional<CompilationError>(tk.value().GetStartPos(), ErrorCode::ErrNoBegin);
        if(sym.level==1)
            functions_list.back().instructions.push_back(Instruction(Operation ::loada,1,sym.offset));
        else
            functions_list.back().instructions.push_back(Instruction(Operation ::loada,0,sym.offset));
        functions_list.back().instructions.push_back(Instruction(Operation ::iscan));
        functions_list.back().instructions.push_back(Instruction(Operation ::istore));
        return {};
    }
    std::optional<CompilationError> Analyser::assignment_expression() {
        return {};
    }
    std::optional<CompilationError> Analyser::function_call() {
        int param_num = 0;
        auto tk = nextToken();
        if (!tk.has_value() || tk.value().GetType() != IDENTIFIER)
            return std::make_optional<CompilationError>(tk.value().GetStartPos(), ErrorCode::ErrNoBegin);
        Function fun = find_function(tk.value().GetValueString());
        if (fun.paramnum == -1) {
            return std::make_optional<CompilationError>(tk.value().GetStartPos(), ErrorCode::ErrNoSemicolon);
        }
        param_num = fun.paramnum;
        int fun_diff = -1;
        for (int i = 0; i < functions_list.size(); i++) {
            if (functions_list.at(i).fun_name.compare(tk.value().GetValueString()) == 0) {
                fun_diff = i - 1;

            }
        }
        if (_offset == -1)
            return std::make_optional<CompilationError>(tk.value().GetStartPos(), ErrorCode::ErrNotDeclared);
        tk = nextToken();
        if (!tk.has_value() || tk.value().GetType() != TokenType::LEFT_BRACKET)
            return std::make_optional<CompilationError>(tk.value().GetStartPos(), ErrorCode::ErrNoBegin);
        if (param_num != 0) {
        while (true) {
            auto err = analyseExpression();
            if (err.has_value())
                return err;
            tk = nextToken();
            param_num--;
            if (!tk.has_value())
                return std::make_optional<CompilationError>(tk.value().GetStartPos(), ErrorCode::ErrNoBegin);
            if (tk.value().GetType() == TokenType::COMMA) {
                continue;
            } else {
                unreadToken();
                break;
            }


            }
            if(param_num!=0)
                return std::make_optional<CompilationError>(tk.value().GetStartPos(), ErrorCode::ErrNoBegin);
        }
        tk=nextToken();
        if(!tk.has_value()||tk.value().GetType()!=TokenType::RIGHT_BRACKET)
            return std::make_optional<CompilationError>(tk.value().GetStartPos(), ErrorCode::ErrNoBegin);
        functions_list.back().instructions.push_back(Instruction(Operation ::call,fun_diff));
        return {};
    }

    std::optional<CompilationError> Analyser::condition() {
        auto err=analyseExpression();
        if(err.has_value())
            return err;
        auto tk=nextToken();
        if(!tk.has_value())
            return {};
        if(tk.value().GetType()==TokenType ::DOUBLE_EQUAL_SIGN||tk.value().GetType()==TokenType ::NOT_EQUAL_SIGN||tk.value().GetType()==TokenType ::LESS_THAN_SIGN||tk.value().GetType()==TokenType ::MORE_THAN_SIGN||tk.value().GetType()==TokenType ::NOT_LESS_THAN_SIGN||tk.value().GetType()==TokenType ::NOT_MORE_THAN_SIGN){
            TokenType t=tk.value().GetType();
            err=analyseExpression();
            if(err.has_value())
                return err;
            functions_list.back().instructions.push_back(Instruction(Operation ::icmp));
            switch (t){
                case DOUBLE_EQUAL_SIGN:
                    functions_list.back().instructions.push_back(Instruction(Operation ::jne));
                    break;
                case NOT_EQUAL_SIGN:
                    functions_list.back().instructions.push_back(Instruction(Operation ::je));
                    break;
                case LESS_THAN_SIGN:
                    functions_list.back().instructions.push_back(Instruction(Operation ::jge));
                    break;
                case MORE_THAN_SIGN:
                    functions_list.back().instructions.push_back(Instruction(Operation ::jl));
                    break;
                case NOT_LESS_THAN_SIGN:
                    functions_list.back().instructions.push_back(Instruction(Operation ::jl));
                    break;
                case NOT_MORE_THAN_SIGN:
                    functions_list.back().instructions.push_back(Instruction(Operation ::jg));
                    break;
            }
            seq_id.push(functions_list.back().instructions.size()-1);
        }
        else{
            functions_list.back().instructions.push_back(Instruction(Operation ::je));
            seq_id.push(functions_list.back().instructions.size()-1);
            unreadToken();
        }
        return {};
    }

    std::optional<CompilationError> Analyser::return_seq() {
        auto tk=nextToken();
        if(!tk.has_value()||tk.value().GetType()!=TokenType::RETURN)
            return std::make_optional<CompilationError>(tk.value().GetStartPos(), ErrorCode::ErrNeedIdentifier);
        if(functions_list.back().type==INT){
            auto err=analyseExpression();
            if(err.has_value())
                return err;
            tk=nextToken();
            if(!tk.has_value()||tk.value().GetType()!=TokenType::SEMICOLON)
                return std::make_optional<CompilationError>(tk.value().GetStartPos(), ErrorCode::ErrNeedIdentifier);
            functions_list.back().instructions.push_back(Instruction(iret));
            return {};
        }
        else{
            tk=nextToken();
            if(!tk.has_value()||tk.value().GetType()!=TokenType::SEMICOLON)
                return std::make_optional<CompilationError>(tk.value().GetStartPos(), ErrorCode::ErrNeedIdentifier);
            functions_list.back().instructions.push_back(Instruction(Operation::ret));
            return {};
        }
    }




	// <表达式> ::= <项>{<加法型运算符><项>}
	std::optional<CompilationError> Analyser::analyseExpression() {
		// <项>
		auto err = analyseItem();
		if (err.has_value())
			return err;

		// {<加法型运算符><项>}
		while (true) {
			// 预读
			auto next = nextToken();
			if (!next.has_value())
				return {};
			auto type = next.value().GetType();

			if (type != TokenType::PLUS_SIGN && type != TokenType::MINUS_SIGN) {
				unreadToken();

				return {};
			}

			// <项>

			err = analyseItem();
			if (err.has_value())
				return err;

			// 根据结果生成指令
			if (type == TokenType::PLUS_SIGN)
                functions_list.back().instructions.push_back(Instruction(Operation ::iadd));
			else if (type == TokenType::MINUS_SIGN)
                functions_list.back().instructions.push_back(Instruction(Operation ::isub));
		}
		return {};
	}

	// <赋值语句> ::= <标识符>'='<表达式>';'
	// 需要补全
	std::optional<CompilationError> Analyser::analyseAssignmentStatement() {
		// 这里除了语法分析以外还要留意
        auto next = nextToken();
        if (!next.has_value() || next.value().GetType() != TokenType::IDENTIFIER)
            return std::make_optional<CompilationError>(next.value().GetStartPos(), ErrNoBegin);

        if (!isDeclared(next.value().GetValueString()))// 标识符声明过吗？
            return std::make_optional<CompilationError>(next.value().GetStartPos(), ErrNotDeclared);
        if (isConstant(next.value().GetValueString()))// 标识符是常量吗？
            return std::make_optional<CompilationError>(next.value().GetStartPos(), ErrAssignToConstant);
        symbol sym=find_symbol(next.value().GetValueString());
        if(sym.level==1)
            functions_list.back().instructions.push_back(Instruction(Operation ::loada,1,sym.offset));
        else
            functions_list.back().instructions.push_back(Instruction(Operation ::loada,0,sym.offset));

        //auto id = next.value();
        next = nextToken();
        if (!next.has_value() || next.value().GetType() != TokenType::EQUAL_SIGN)
            return std::make_optional<CompilationError>(next.value().GetStartPos(), ErrInvalidAssignment);

        auto err = analyseExpression();
        if (err.has_value())
            return err;

        next=nextToken();
        if(next.value().GetType()!=TokenType::SEMICOLON)
            return std::make_optional<CompilationError>(next.value().GetStartPos(), ErrNoSemicolon);
        functions_list.back().instructions.push_back(Instruction(Operation ::istore));
		return {};
	}



	// <项> :: = <因子>{ <乘法型运算符><因子> }
	// 需要补全
	std::optional<CompilationError> Analyser::analyseItem() {
		// 可以参考 <表达式> 实现
        auto err = analyseFactor();

        if (err.has_value())
            return err;

        while (true) {
            auto next = nextToken();
            if (!next.has_value())
                return {};
            auto type = next.value().GetType();
            if (!(type == TokenType::MULTIPLICATION_SIGN ||
                  type == TokenType::DIVISION_SIGN)){
                unreadToken();
                return {};
            }


            auto err = analyseFactor();
            if (err.has_value())
                return err;

            // 根据结果生成指令
            if (type == TokenType::MULTIPLICATION_SIGN)
                functions_list.back().instructions.push_back(Instruction(Operation ::imul));
            else if (type == TokenType::DIVISION_SIGN)
                functions_list.back().instructions.push_back(Instruction(Operation ::idiv));
        }

		return {};
	}

	// <因子> ::= [<符号>]( <标识符> | <无符号整数> | '('<表达式>')' )
	// 需要补全
	std::optional<CompilationError> Analyser::analyseFactor() {
		// [<符号>]
		auto next = nextToken();
		auto prefix = 1;
		if (!next.has_value())
			return std::make_optional<CompilationError>(next.value().GetStartPos(), ErrorCode::ErrIncompleteExpression);
		if (next.value().GetType() == TokenType::PLUS_SIGN)
			prefix = 1;
		else if (next.value().GetType() == TokenType::MINUS_SIGN) {
			prefix = -1;
		}
		else
			unreadToken();

		// 预读
		next = nextToken();
        auto tk=next;
        int32_t v;
        auto err =std::make_optional<CompilationError>(_current_pos, ErrorCode::ErrIncompleteExpression);
		if (!next.has_value())
			return std::make_optional<CompilationError>(next.value().GetStartPos(), ErrorCode::ErrIncompleteExpression);
		switch (next.value().GetType()) {
			// 这里和 <语句序列> 类似，需要根据预读结果调用不同的子程序
			// 但是要注意 default 返回的是一个编译错误
            case IDENTIFIER:
                tk=nextToken();
                if(tk.value().GetType()==TokenType::LEFT_BRACKET){
                    //参数列表
                    unreadToken();
                    unreadToken();


                    tk=nextToken();
                    unreadToken();
                    if(find_function(tk.value().GetValueString().c_str()).type==TokenType ::VOID)
                        return std::make_optional<CompilationError>(next.value().GetStartPos(), ErrorCode::ErrNotInitialized);
                    err=function_call();
                    if(err.has_value())
                        return err;

                    break;
                }
                else{
                    unreadToken();
                    unreadToken();
                    tk=nextToken();
                    symbol sym = find_symbol(tk.value().GetValueString());
                    if(sym.offset==-99)
                        return std::make_optional<CompilationError>(next.value().GetStartPos(), ErrorCode::ErrNotInitialized);
                    if(sym.level==1)
                        functions_list.back().instructions.push_back(Instruction(Operation ::loada,1,sym.offset));
                    else{
                        functions_list.back().instructions.push_back(Instruction(Operation ::loada,0,sym.offset));
                    }
                    functions_list.back().instructions.push_back(Instruction(Operation ::iload));
                }

                break;
            case INT:
                v=stoi(next.value().GetValueString());
                //_instructions.emplace_back(Operation::loada, v);
                functions_list.back().instructions.push_back(Instruction(Operation ::ipush,v));
                break;
            case HEX_INTEGER:
                v=hex_to_ten(next.value().GetValueString());
                //_instructions.emplace_back(Operation::loada, v);
                functions_list.back().instructions.push_back(Instruction(Operation ::ipush,v));
                break;
            case LEFT_BRACKET:
                err = analyseExpression();
                if (err.has_value())
                    return err;
                next=nextToken();
                if (!next.has_value() || next.value().GetType() != RIGHT_BRACKET)
                    return std::make_optional<CompilationError>(next.value().GetStartPos(), ErrorCode::ErrIncompleteExpression);

                break;
            default:
			return std::make_optional<CompilationError>(next.value().GetStartPos(), ErrorCode::ErrIncompleteExpression);
		}

		// 取负
		if (prefix == -1)
			functions_list.back().instructions.push_back(Instruction(Operation::ineg));
		return {};
	}

	std::optional<Token> Analyser::nextToken() {
		if (_offset == _tokens.size())
			return {};
		// 考虑到 _tokens[0..._offset-1] 已经被分析过了
		// 所以我们选择 _tokens[0..._offset-1] 的 EndPos 作为当前位置
		_current_pos = _tokens[_offset].GetEndPos();
		return _tokens[_offset++];
	}

	void Analyser::unreadToken() {
		if (_offset == 0)
			DieAndPrint("analyser unreads token from the begining.");
		_current_pos = _tokens[_offset - 1].GetEndPos();
		_offset--;
	}



	void Analyser::addVariable(const Token& tk,bool const_flag) {
		//symbol sym = {const_flag,tk.GetType(),tk.GetValueString(),static_cast<int>(Symboltables.back().table.size()),static_cast<int>(Symboltables.size())};
        symbol sym ={};
		sym.const_flag=const_flag;
        sym.tp=tk.GetType();
        sym.val=tk.GetValueString();
        sym.offset=static_cast<int>(Symboltables.back().table.size());
        sym.level=static_cast<int>(Symboltables.size());
		Symboltables.back().table.insert(make_pair(tk.GetValueString(),sym));
	}




	bool Analyser::isDeclared(const std::string& s) {
        return find_symbol(s).offset==-99? false: true;
		//return isConstant(s) || isUninitializedVariable(s) || isInitializedVariable(s);
	}



	bool Analyser::isConstant(const std::string&s) {
        return find_symbol(s).const_flag;

	}

	int32_t Analyser::hex_to_ten(std::string s){
	    int32_t ans=0;
        std::stringstream ss;
        ss.str(s);
        ss>>std::hex>>ans;
        return ans;
    }

    symbol Analyser::find_symbol(std::string str) {
        if(Symboltables.back().table.find(str)!=Symboltables.back().table.end())
            return Symboltables.back().table[str];
	    if(Symboltables.at(0).table.find(str)!=Symboltables.at(0).table.end())
                return Symboltables.at(0).table[str];

        return symbol{};
	}
    Function Analyser::find_function(std::string s){
	    for(int i=0;i<functions_list.size();i++){
            if(functions_list.at(i).fun_name.compare(s)==0)
                return functions_list.at(i);
        }
        return Function{};
	}
}