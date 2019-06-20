#include <cmath>
#include <cstdlib>
#include <fstream>
#include <iomanip>
#include <iostream>

using namespace std;

void consoleToFile(istream & in, ostream & out)
{
    int numEntries = 0;

    if (in == cin)
        cout << "Enter number of entries.";

    in >> numEntries;
    out << numEntries << endl;

    double consoleValue1 = 0, consoleValue2 = 0;

    for (int count = 1; count <= numEntries; count++)
    {
        if (in == cin)
            cout << "Enter location of item #" << count << ". (x, y) ";

        in >> consoleValue1 >> consoleValue2;
        out << consoleValue1 << " " << consoleValue2 << endl;
    }
}

int main()
{
    ofstream fout("locations.txt");

    consoleToFile(cin, fout);

    system("PAUSE");
    return EXIT_SUCCESS;
}
