#include "argparse.hpp"
#include "fmt/core.h"

#include "tokenizer/tokenizer.h"
#include "analyser/analyser.h"
#include "fmts.hpp"

#include <iostream>
#include <fstream>
std::uint8_t find_op(miniplc0::Operation x);
int paramnum_(miniplc0::Operation x);
std::vector<miniplc0::Token> _tokenize(std::istream& input) {
	miniplc0::Tokenizer tkz(input);
	auto p = tkz.AllTokens();
	if (p.second.has_value()) {
		fmt::print(stderr, "Tokenization error: {}\n", p.second.value());
		exit(2);
	}
	return p.first;
}

void Tokenize(std::istream& input, std::ostream& output) {
	auto v = _tokenize(input);
	for (auto& it : v)
		output << fmt::format("{}\n", it);
	return;
}

void Analyse_l(std::istream& input, std::ostream& output){
	auto tks = _tokenize(input);
	miniplc0::Analyser analyser(tks);
	auto p = analyser.Analyse();
	if (p.second.has_value()) {
		fmt::print(stderr, "Syntactic analysis error: {}\n", p.second.value());
		exit(2);
	}
	auto v = p.first;

    output<<".constants:\n";
	for(int i=2;i<p.first.size()+1;i++){
	    output<<i-2;
        output<<" S \"";
        output<<p.first.at(i-1).fun_name.c_str();
        output<<"\"\n";
	}
    output<<".start:\n";
    for(int i=0;i<p.first.at(0).instructions.size();i++){
        output<<i;
        output<<" ";
        output<<fmt::format("{}\n", p.first.at(0).instructions.at(i));
    }
    output<<".functions:\n";
    for(int i=1;i<p.first.size();i++){
        output<<i-1;
        output<<" ";
        output<<i-1;
        output<<" ";
        output<<p.first.at(i).paramnum;
        output<<" 1\n";
    }
    for(int i=1;i<p.first.size();i++){
        output<<".F";
        output<<i-1;
        output<<": #";
        output<<p.first.at(i).fun_name.c_str();
        output<<"\n";
        for(int j=0;j<p.first.at(i).instructions.size();j++){
            output<<j;
            output<<" ";
            output<<fmt::format("{}\n", p.first.at(i).instructions.at(j));
        }
    }
}
void Analyse_c(std::istream& input, std::ostream& output){
    auto tks = _tokenize(input);
    miniplc0::Analyser analyser(tks);
    auto p = analyser.Analyse();
    if (p.second.has_value()) {
        fmt::print(stderr, "Syntactic analysis error: {}\n", p.second.value());
        exit(2);
    }
    auto v = p.first;

    char bytes[8];
    const auto writeNBytes = [&](void* addr, int count) {
        assert(0 < count && count <= 8);
        char* p = reinterpret_cast<char*>(addr) + (count-1);
        for (int i = 0; i < count; ++i) {
            bytes[i] = *p--;
        }
        output.write(bytes, count);
    };

    output.write("\x43\x30\x3A\x29", 4);
    // version
    output.write("\x00\x00\x00\x01", 4);
     std::uint16_t constants_count = p.first.size()-1;
     writeNBytes(&constants_count, 2);

     for(int i=1;i<p.first.size();i++){
         output.write("\x00", 1);
         std::string v = p.first.at(i).fun_name;
         std::uint16_t len = v.length();
         writeNBytes(&len, 2);
         output.write(v.c_str(), len);
     }

    std::uint16_t instructions_count=p.first.at(0).instructions.size();
    writeNBytes(&instructions_count, 2);

    for(int i=0;i<p.first.at(0).instructions.size();i++){
        uint8_t op=find_op(p.first.at(0).instructions.at(i).GetOperation());
        writeNBytes(&op, 1);
        if(paramnum_(p.first.at(0).instructions.at(i).GetOperation())==1){
            uint8_t a=p.first.at(0).instructions.at(i).GetX();
            writeNBytes(&a,1);
        }
        else if(paramnum_(p.first.at(0).instructions.at(i).GetOperation())==2){
            uint16_t b=p.first.at(0).instructions.at(i).GetX();
            writeNBytes(&b,2);
        }
        else if(paramnum_(p.first.at(0).instructions.at(i).GetOperation())==4){
            uint32_t c=p.first.at(0).instructions.at(i).GetX();
            writeNBytes(&c,4);
        }
        else if(paramnum_(p.first.at(0).instructions.at(i).GetOperation())==6){
            uint16_t b=p.first.at(0).instructions.at(i).GetX();
            writeNBytes(&b,2);
            uint32_t c=p.first.at(0).instructions.at(i).GetY();
            writeNBytes(&c,4);
        }

    }

    uint16_t fun_num=p.first.size()-1;
    writeNBytes(&fun_num,2);

    for(int i=1;i<p.first.size();i++){
        uint16_t name=i-1;
        writeNBytes(&name,2);
        uint16_t para=p.first.at(i).paramnum;
        writeNBytes(&para,2);
        para=1;
        writeNBytes(&para, 2);
        uint16_t count=p.first.at(i).instructions.size();
        writeNBytes(&count, 2);
        for(int j=0;j<p.first.at(i).instructions.size();j++){
            std::uint8_t op=find_op(p.first.at(i).instructions.at(j).GetOperation());
            writeNBytes(&op, 1);
            if(paramnum_(p.first.at(i).instructions.at(j).GetOperation())==1){
                uint8_t a=p.first.at(i).instructions.at(j).GetX();
                writeNBytes(&a,1);
            }
            else if(paramnum_(p.first.at(i).instructions.at(j).GetOperation())==2){
                uint16_t b=p.first.at(i).instructions.at(j).GetX();
                writeNBytes(&b,2);
            }
            else if(paramnum_(p.first.at(i).instructions.at(j).GetOperation())==4){
                uint32_t c=p.first.at(i).instructions.at(j).GetX();
                writeNBytes(&c,4);
            }
            else if(paramnum_(p.first.at(i).instructions.at(j).GetOperation())==6){
                uint16_t b=p.first.at(i).instructions.at(j).GetX();
                writeNBytes(&b,2);
                uint32_t c=p.first.at(i).instructions.at(j).GetY();
                writeNBytes(&c,4);
            }

        }
    }



}
int main(int argc, char** argv) {
	argparse::ArgumentParser program("miniplc0");
	program.add_argument("input")
		.help("speicify the file to be compiled.");
	program.add_argument("-t")
		.default_value(false)
		.implicit_value(true)
		.help("perform tokenization for the input file.");
	program.add_argument("-l")
            .default_value(false)
            .implicit_value(true)
            .help("perform syntactic analysis for the input file.");
    program.add_argument("-c")
            .default_value(false)
            .implicit_value(true)
            .help("perform binary analysis for the input file.");
	program.add_argument("-o", "--output")
		.required()
		.default_value(std::string("-"))
		.help("specify the output file.");

	try {
		program.parse_args(argc, argv);
	}
	catch (const std::runtime_error& err) {
		fmt::print(stderr, "{}\n\n", err.what());
		program.print_help();
		exit(2);
	}

	auto input_file = program.get<std::string>("input");
	auto output_file = program.get<std::string>("--output");
	std::istream* input;
	std::ostream* output;
	std::ifstream inf;
	std::ofstream outf;
	if (input_file != "-") {
		inf.open(input_file, std::ios::binary|std::ios::in);
		if (!inf) {
			fmt::print(stderr, "Fail to open {} for reading.\n", input_file);
			exit(2);
		}
		input = &inf;
	}
	else
		input = &std::cin;
	if (output_file != "-") {
		outf.open(output_file, std::ios::binary|std::ios::out | std::ios::trunc);
		if (!outf) {
			fmt::print(stderr, "Fail to open {} for writing.\n", output_file);
			exit(2);
		}
		output = &outf;
	}
	else
		output = &std::cout;
	if (program["-t"] == true && program["-l"] == true) {
		fmt::print(stderr, "You can only perform tokenization or syntactic analysis at one time.");
		exit(2);
	}
	if (program["-t"] == true) {
		Tokenize(*input, *output);
	}
	else if (program["-l"] == true) {
		Analyse_l(*input, *output);
	}
	else if(program["-c"]==true){
        Analyse_c(*input, *output);
	}
	else {
		fmt::print(stderr, "You must choose tokenization or syntactic analysis.");
		exit(2);
	}
	return 0;
}
    std::uint8_t find_op(miniplc0::Operation x) {
        switch (x) {
            case miniplc0::bipush:
                return 0x01;
            case miniplc0::ipush:
                return 0x02;
            case miniplc0::loada:
                return 0x0a;
            case miniplc0::iload:
                return 0x10;
            case miniplc0::iaload:
                return 0x18;
            case miniplc0::istore:
                return 0x20;
            case miniplc0::iadd:
                return 0x30;
            case miniplc0::isub:
                return 0x34;
            case miniplc0::imul:
                return 0x38;
            case miniplc0::idiv:
                return 0x3c;
            case miniplc0::ineg:
                return 0x40;
            case miniplc0::icmp:
                return 0x44;
            case miniplc0::jmp:
                return 0x70;
            case miniplc0::je:
                return 0x71;
            case miniplc0::jne:
                return 0x72;
            case miniplc0::jl:
                return 0x73;
            case miniplc0::jge:
                return 0x74;
            case miniplc0::jg:
                return 0x75;
            case miniplc0::jle:
                return 0x76;
            case miniplc0::call:
                return 0x80;
            case miniplc0::ret:
                return 0x88;
            case miniplc0::iret:
                return 0x89;
            case miniplc0::iprint:
                return 0xa0;
            case miniplc0::cprint:
                return 0xa2;
            case miniplc0::printl:
                return 0xaf;
            case miniplc0::iscan:
                return 0xb0;
        }
    }

    int paramnum_(miniplc0::Operation x){
        switch (x){
            case miniplc0::iload:
            case miniplc0::iadd:
            case miniplc0::isub:
            case miniplc0::imul:
            case miniplc0::idiv:
            case miniplc0::ineg:
            case miniplc0::icmp:
            case miniplc0::iprint:
            case miniplc0::cprint:
            case miniplc0::iscan:
            case miniplc0::printl:
            case miniplc0::istore:
            case miniplc0::iret:
            case miniplc0::ret:
                return 0;
            case miniplc0::bipush:
                return 1;
            case miniplc0::jl:
            case miniplc0::jg:
            case miniplc0::jle:
            case miniplc0::jge:
            case miniplc0::je:
            case miniplc0::jne:
            case miniplc0::jmp:
            case miniplc0::jCOND:
            case miniplc0::call:
                return 2;
            case miniplc0::ipush:
                return 4;
            case miniplc0::loada:
                return 6;
        }
}

