#pragma once

#include <cstdint>
#include <utility>

namespace miniplc0 {

	enum Operation {
		loada = 0,
		loadc,
		ipush,
		iload,
		istore,
		iaload,
		iastore,
		printl,
		bipush,
		snew,
		pop,
		dup,
		iadd,
		isub,
		imul,
		idiv,
		ineg,
		icmp,
		jCOND,
		jmp,
		call,
		ret,
		iret,
		iprint,
		cprint,
		iscan,
		je,
		jne,
		jg,
		jl,
		jge,
		jle
	};
	
	class Instruction final {
	private:
		using int32_t = std::int32_t;
	public:
		friend void swap(Instruction& lhs, Instruction& rhs);
	public:
		Instruction(Operation opr, int32_t x) : _opr(opr), _x(x) {}
        Instruction(Operation opr, int32_t x,int32_t y) : _opr(opr), _x(x) ,_y(y) {}
        Instruction(Operation opr) { _opr=opr; }

		Instruction& operator=(Instruction i) { swap(*this, i); return *this; }
		bool operator==(const Instruction& i) const { return _opr == i._opr && _x == i._x; }

		Operation GetOperation() const { return _opr; }
		int32_t GetX() const { return _x; }
		void SetX(int32_t x){
		    _x=x;
		}
        int32_t GetY() const { return _y; }
        void SetY(int32_t y){
            _y=y;
        }
	private:
		Operation _opr;
		int32_t _x;
		int32_t _y;
	};

	inline void swap(Instruction& lhs, Instruction& rhs) {
		using std::swap;
		swap(lhs._opr, rhs._opr);
		swap(lhs._x, rhs._x);
	}
}