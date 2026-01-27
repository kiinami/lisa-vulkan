//
// Created by kinami on 1/26/26.
//

#include "core/Application.h"

int main(int argc, char** argv)
{
    auto app = Application();
    app.init("Lisa", 800, 600, 1);
    return app.run();
}
