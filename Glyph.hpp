#include <vector>

#include "CookbookSampleFramework.h"

class Glyph{

public:
    Glyph()
    {        
       
    };
    
    
    std::vector<unsigned char> LoadFromFile(const char* filename)
    {
        if( !VulkanCookbook::LoadTextureDataFromFile( filename, 4, image_data, &width, &height ) ) {
            throw std::runtime_error( "Failed to load texture data from file!" );
        }

        return image_data;
    }

    std::vector<unsigned char> const& getData()
    {
        return image_data;
    }   
    
    
    int width = 1;
    int height = 1;

private:   
    

    std::vector<unsigned char> image_data;
};