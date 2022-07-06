#include <fstream>
#include <filesystem>

int main( int argc, const char *argv[] )
{
    if ( argc == 1 ) return 0;

    const char *dirpath = std::filesystem::current_path().c_str();

    for ( int i = 1; i < argc; ++i )
    {
        char path[ 2048 ] { 0 };
        char out[ 2048 ] { 0 };

        sprintf( path, "%s/source/%s", dirpath, argv[ i ] );
        const char *filename = std::filesystem::path{ path }.filename().c_str();
        sprintf( out, "%s/h/%s.h", dirpath, filename );

        std::ifstream png { path };
        std::ofstream h { out };


        //skip first 8 bits of png
        png.ignore( 8 );

        while ( png.peek() != EOF )
        {
            unsigned int len = 0;
            for ( int iter = 0; iter < 4; ++iter )
            {
                unsigned char byte = 0;
                png >> byte;
                len = ( len << 8 ) | byte;
            }


            h << "g_iWidth = ";
        }
    }
}