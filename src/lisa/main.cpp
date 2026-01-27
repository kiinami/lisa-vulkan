//
// Created by kinami on 1/26/26.
//

#include "core/Application.h"

int main(int argc, char** argv)
{
    auto app = Application("lisa");
    app.init(1);
    return app.run();
}
