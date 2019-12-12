#ifndef PARSE_ARGS
#define PARSE_ARGS

#include <getopt.h>
#include <iostream>

using std::string;

#define NO_ARG no_argument
#define REQ_ARG required_argument
#define OPT_ARG optional_argument

#define MEMBER(a,b) a.b
#define PMEMBER(a,b) a->b

void storeAt(string & v, string s) { v = s; }
void storeAt(int & v,    string s) { v = std::stoi(s); }
void storeAt(double & v, string s) { v = std::stod(s); }
void storeAt(bool & v,   string s) { v = true; (void)s;}

    // Create structure
#define T(l,o,t,h,a) t o;
struct Args { ARGS_OPTIONS_LIST };
#undef T
bool parseArgs(int argc, char * argv[], Args & a)
{
    // Help string
#define T(l,o,t,h,a) "[ -" + string(1, l) + " | --" #o " <" #t "> ] ",
    string helpCmdLineOpts[] = { ARGS_OPTIONS_LIST };
#undef T
    string helpCmdLine(ARGS_CMDNAME " ");
    for (auto s: helpCmdLineOpts) { helpCmdLine += s; }

#define T(l,o,t,h,a) "\t-" + string(1, l) + " | --" #o " <" #t ">: \t" h "\n",
    string helpStringLines[] = { ARGS_OPTIONS_LIST };
#undef T
    string helpString(ARGS_CMDNAME " - " ARGS_DESC "\n\n"
                      "Usage:\n\t" + helpCmdLine + "\nwhere:\n");
    for (auto s: helpStringLines) { helpString += s; }

    // Short opts. string
#define T(l,o,t,h,a) string(1, l) + (a == REQ_ARG ? ":" : ""),
    string shortOpts[] = { ARGS_OPTIONS_LIST };
#undef T
    string shortOptsString;
    for (auto s: shortOpts) { shortOptsString += s; }
    const char* const short_opts = shortOptsString.c_str();


    // Options array
#define T(l,o,t,h,a) {#o, a, nullptr, l},
    const option long_opts[] = { ARGS_OPTIONS_LIST
                                 {nullptr, no_argument, nullptr, 0}};
#undef T

    // Loop on command line options
    while (true)
    {
        const auto opt = getopt_long(argc, argv,
                                     short_opts, long_opts, nullptr);

        if (-1 == opt) break;

        Args * arg = &a;

        switch (opt) {

#define T(l,o,t,h,a) case l: storeAt( PMEMBER(arg,o) , optarg ); break;
            ARGS_OPTIONS_LIST;
#undef T

        case '?': // Unrecognized option
        default:
            std::cerr << helpString << '\n';
            return false;
        }
    }

    return true;
}

#endif
