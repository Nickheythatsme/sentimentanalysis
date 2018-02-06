#include "src/support.h"
#include "src/count_words.h"
#include "src/window.h"
#include "src/make_window.h"

using std::cout;
using std::endl;

/* Testing functions */
void test_window(const std::vector<string> &files);
void test_count(const std::vector<string> &files);
void test_make_windows(const std::vector<string> &files);


int main(int argc, char *argv[])
{
    auto f = glob_files(argv[1]);

    cout << "Files read: " << f.size() << endl;

    //test_window(f);
    //test_count(f);
    test_make_windows(f);
    return 0;
}


void test_window(const std::vector<string> &files)
{
}


void test_count(const std::vector<string> &files)
{
}

void test_make_windows(const std::vector<string> &files)
{
    cout << "First file: " << files[0] << endl;
    make_window maker(files);
    for(auto const & a: maker)
        cout << "window: " << a << endl;
}
