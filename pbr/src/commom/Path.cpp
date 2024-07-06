#include "Path.h"
float gMaxAnisotropy = 1.0f;					// 最大各向异性

#include <filesystem>
void InitPath()
{
    //std::filesystem::path filePath = "pbr.frag";
    //// 获取当前路径
    //std::filesystem::path currentPath = std::filesystem::current_path();
    //std::cout << "Current path: " << currentPath << std::endl;

    //// 绝对路径
    //std::filesystem::path absolutePath = std::filesystem::absolute(filePath);
    //std::cout << "Absolute path: " << absolutePath << std::endl;

    //// 文件名
    //std::filesystem::path filename = absolutePath.filename();
    //std::cout << "Filename: " << filename << std::endl;

    //// 父路径
    //std::filesystem::path parentPath = absolutePath.parent_path();
    //std::cout << "Parent path: " << parentPath << std::endl;

    //// 检查文件是否存在
    //if (std::filesystem::exists(absolutePath)) {
    //    std::cout << "The file exists." << std::endl;
    //}
    //else {
    //    std::cout << "The file does not exist." << std::endl;
    //}
}
