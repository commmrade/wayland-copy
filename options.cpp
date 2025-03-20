#include "options.hpp"

void parse_options(int argc, char** argv, App& app) {
    struct option long_options[] = {
        {"foreground", no_argument, 0, 'f'},
        {"paste-once", no_argument, 0, 'p'},
        {"help", no_argument, 0, 'h'},
        {0, 0, 0, 0}
    };
    while (true) {
        int option_index{0};
        const char* opts = "fph";
        int c = getopt_long(argc, argv, opts, long_options, &option_index);
        if (c == -1) {
            break;
        }
        if (c == 0) {
            c = long_options[option_index].val;
        }
        std::cout << char(c) << std::endl;
        switch (c) {
            case 'h': {
                print_usage();
                exit(0);
            }
            case 'f': {
                app.foreground = true;
                std::cout << "fuck\n";
                break;
            }
            case 'p': {
                app.paste_once = true;
                break;
            }
            default: {
                print_usage();
                exit(1);
            }
        }
    }
}


void print_usage() {
    fprintf(
        stdout,
        "Usage:\n"
        "\twlcopy [options] text to copy\n"
        "\twlcopy [options] < file-to-copy\n\n"
        "Copy content to the Wayland clipboard.\n\n"
        "Options:\n"
        "\t-o, --paste-once\tOnly serve one paste request and then exit.\n"
        "\t-f, --foreground\tStay in the foreground instead of forking.\n"
        "\t-h, --help\t\tDisplay this message.\n"
        "Mandatory arguments to long options are mandatory"
        " for short options too.\n\n"
    );
}
