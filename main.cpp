#include <iostream>
#include "KMeansImageCompress.h"

int main(int argc, char **argv) {

    KMeansImageCompress kIC(argv[1], atoi(argv[3]), atoi(argv[4]));
    kIC.initialize();
    kIC.run();
    kIC.calculateError();
    kIC.write(argv[2]);

    std::cout << "Error: " << kIC.getError() << '\n';

    return 0;

}