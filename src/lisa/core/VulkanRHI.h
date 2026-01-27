//
// Created by kinami on 1/27/26.
//

#ifndef LISA_VULKANRHI_H
#define LISA_VULKANRHI_H

#include <string>

class VulkanRHI
{
public:
    VulkanRHI();
    ~VulkanRHI();

    void init(std::string name, unsigned int device_index);
private:

};


#endif //LISA_VULKANRHI_H