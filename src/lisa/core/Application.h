//
// Created by kinami on 1/26/26.
//

#ifndef LISA_VULKAN_APPLICATION_H
#define LISA_VULKAN_APPLICATION_H


class Application
{
public:
    Application();
    ~Application();

    int run();

private:
    static void init_libraries();
};


#endif //LISA_VULKAN_APPLICATION_H