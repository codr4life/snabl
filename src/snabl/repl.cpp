#include "snabl/env.hpp"
#include "snabl/repl.hpp"
#include "snabl/stack.hpp"

namespace snabl {
	void repl(Env &env, istream &in, ostream &out) {
		out << "Snabl v";
		char sep(0);
	
		for (auto v: version) {
			if (sep) { out << sep; }
			out << v;
			sep = '.';
		}

		out << endl << endl
				<< "Press Return on empty line to eval."
				<< endl << endl;
		
		stringstream buf;
		std::string l;
		cout << "   ";
	
		while (std::getline(in, l)) {
			if (l == "") {
				try {
					env.run(buf.str());
					out << env.stack() << endl;
				} catch (const exception &e) {
					out << e.what() << endl;
				}
				
				stringstream().swap(buf);
				cout << "   ";
			} else {
				buf << l << endl;
				cout << ".. ";
			}
		} 
	}
}
