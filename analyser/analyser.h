#pragma once

#include "error/error.h"
#include "instruction/instruction.h"
#include "tokenizer/token.h"

#include <vector>
#include <optional>
#include <utility>
#include <map>
#include <cstdint>
#include <cstddef> // for std::size_t
#include <stack>

namespace miniplc0 {
    struct Fun_record{
        std::map<std::string, int32_t> _uninitialized_vars;
        std::map<std::string, int32_t> _vars;
        std::map<std::string, int32_t> _consts;
    };
    struct Symboltable{
        std::map<std::string,struct symbol> table;
    };
    struct Function{
        std::string fun_name;
        int paramnum=-1;
        TokenType type;
        std::vector<Instruction> instructions;
        bool return_flag;
    };
    struct symbol{
        bool const_flag=false;
        TokenType tp;
        std::string val;
        int offset=-99;
        int level;
    };
	class Analyser final {
	private:
		using uint64_t = std::uint64_t;
		using int64_t = std::int64_t;
		using uint32_t = std::uint32_t;
		using int32_t = std::int32_t;
	public:
		Analyser(std::vector<Token> v)
			: _tokens(std::move(v)), _offset(0), _instructions({}), _current_pos(0, 0),
			_uninitialized_vars({}), _vars({}), _consts({}), _nextTokenIndex(0) {}
		Analyser(Analyser&&) = delete;
		Analyser(const Analyser&) = delete;
		Analyser& operator=(Analyser) = delete;

		// 唯一接口
		std::pair<std::vector<struct Function>, std::optional<CompilationError>> Analyse();
	private:
		// 所有的递归子程序

		// <程序>
		std::optional<CompilationError> analyseProgram();
		// <主过程>
		std::optional<CompilationError> analyseMain();
		//
        std::optional<CompilationError> variable_declaration(bool in_fun);
        //
        std::optional<CompilationError> analyseVarialble(bool in_fun);
        //
        int32_t hex_to_ten(std::string);
        std::optional<CompilationError> return_seq();
        //
        std::optional<CompilationError> function_definition();
		// <常量声明>
		std::optional<CompilationError> analyseConstantDeclaration();
		//loop-statement
        std::optional<CompilationError> loop_statement();
        //
        std::optional<CompilationError> print_statement();
        //
        std::optional<CompilationError> scan_statement();
        //
        std::optional<CompilationError> assignment_expression();
        //
        std::optional<CompilationError> function_call();
		//
        symbol find_symbol(std::string);
        //
        Function find_function(std::string);
        //
        std::optional<CompilationError> analyseFunction();
        //
        std::optional<CompilationError> condition_statement();
		//
        std::optional<CompilationError> condition();
		//
        std::optional<CompilationError> compound_statement();
        //
        std::optional<CompilationError> statement_seq();
        // <变量声明>
		std::optional<CompilationError> analyseVariableDeclaration();
		// <语句序列>
		std::optional<CompilationError> analyseStatementSequence();
		// <常表达式>
		// 这里的 out 是常表达式的值
		std::optional<CompilationError> analyseConstantExpression(int32_t& out);
		// <表达式>
		std::optional<CompilationError> analyseExpression();
		// <赋值语句>
		std::optional<CompilationError> analyseAssignmentStatement();
		// <输出语句>
		std::optional<CompilationError> analyseOutputStatement();
		// <项>
		std::optional<CompilationError> analyseItem();
		// <因子>
		std::optional<CompilationError> analyseFactor();

		// Token 缓冲区相关操作

		// 返回下一个 token
		std::optional<Token> nextToken();
		// 回退一个 token
		void unreadToken();

		// 下面是符号表相关操作

		// helper function
		void _add(const Token&, std::map<std::string, int32_t>&);
		//
        void _add_function(std::string fun_name, struct fun_record mp, std::map<std::string, struct Fun_record> &_functions_list);
		// 添加变量、常量、未初始化的变量
		void addVariable(const Token&,bool const_flag);
		void addConstant(const Token&);
		void addUninitializedVariable(const Token&);
		// 是否被声明过
		bool isDeclared(const std::string&);
		// 是否是未初始化的变量
		bool isUninitializedVariable(const std::string&);
		// 是否是已初始化的变量
		bool isInitializedVariable(const std::string&);
		//
        bool is_Var_or_Fun();
		// 是否是常量
		bool isConstant(const std::string&);
		// 获得 {变量，常量} 在栈上的偏移
		int32_t getIndex(const std::string&);
	private:
		std::vector<Token> _tokens;
		std::size_t _offset;
		std::vector<Instruction> _instructions;
		std::pair<uint64_t, uint64_t> _current_pos;

		// 为了简单处理，我们直接把符号表耦合在语法分析里
		// 变量                   示例
		// _uninitialized_vars    var a;
		// _vars                  var a=1;
		// _consts                const a=1;
		std::map<std::string, int32_t> _uninitialized_vars;
		std::map<std::string, int32_t> _vars;
		std::map<std::string, int32_t> _consts;
        std::vector<struct Function> functions_list;
		// 下一个 token 在栈的偏移
		int32_t _nextTokenIndex;
        std::vector<struct Symboltable> Symboltables;
        std::map<std::string,int> Constants;
        std::stack<int> seq_id={};
    };
}