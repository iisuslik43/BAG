#include "headers.h"
#include "config.h"
#include "writing.h"
using namespace std;

int main(int argc, char* argv[])
{
    Config conf(argc, argv);
    Bag_header bh;
    vector<Chunk> chunks;
    ifstream in(conf.in_filename, ios::binary);
    ofstream out(conf.out_filename_correct, ios::binary);

    read_BAG(conf,chunks,bh,in,out);
    cout << "\n=================================\nStart Writing" << endl;
    cout << "=================================\n" << endl;
    cout << "Writing correct messages:\n\n";
    cout << chunks[0].header_len << endl;
    Bag_header bh_other = bh;
    vector<Chunk> chunks_other = chunks;

    conf.correct_conditions = true;
    conf.parseBag(chunks, bh, in);
    cout << chunks[0].header_len << endl;
    writingBag(chunks, bh, conf, in, out);

    cout << "\n=================================\n" << endl;
    cout << "Writing other messages:\n\n";
    conf.correct_conditions = false;
    conf.parseBag(chunks_other, bh_other, in);
    out = ofstream(conf.out_filename_other, ios::binary);
    writingBag(chunks_other, bh_other, conf, in, out);
    system("find . -name \"chunk*\" -exec rm {} \\;");
    return 0;
}
