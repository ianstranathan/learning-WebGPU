#include <fstream>
#include <sstream>
#include <string>
#include <iostream>


std::string load_shader_from_file(const char* filepath)
{
	// NOTE: throws std::runtime_error if the file cannot be opened.

    std::ifstream file(filepath);
    
    if (!file.is_open())
	{
        std::cerr << "Error: Could not open shader file at " << filepath << std::endl;
        return "";
    }

    std::stringstream buffer; //Read the entire file content into a stringstream
    buffer << file.rdbuf();

    file.close();

    return buffer.str();
}
