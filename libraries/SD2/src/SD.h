/* 
    SD2 ESP32 SD card library designed for performance
    Uses standard code designed for optimal ESP32 performance as per example code provided by Espressif.
    
    License: 
        GNU General Public License V3 

    Authored by Gordon MacPherson <gordon@gordonite.tech>
    
    (c) Copyright GORDONITE LTD
*/

#ifndef __SD2_H__
#define __SD2_H__

#include <Arduino.h>
#include <string.h>

namespace SD2 {

    enum card_connection_mode {
        DEFAULT_PIN_MAPPING = 0, // GPIOs 15, 2, 4, 12, 13 should have external 10k pull-ups.
        INTERNAL_PULLUP_MAPPING = 1, // Enables internal pullup resistors, not sufficient but makes difference on some boards.
        SPI_MAPPING = 2, // GPIOs MISO(2), MOSI(15), CLK(14), CS(13)
        SINGLE_LINE_SDMODE = 3
    };


    class SD
    {
    public:
        // Instruct ESP32 to begin communication with the SD CARD
        boolean begin( card_connection_mode mode = DEFAULT_PIN_MAPPING, boolean auto_format_if_mount_failed = false);        

        // Shutdown all ESP32 SD card communication
        boolean end(); // custom non standard!

        // Not implemented yet        
        void exists();
        void mkdir();
        void open();
        void remove();
        void rmdir();
    private:
        bool status = false;
    };

    class OFile {
        void name();
        void available();
        void close();
        void flush();
        void peek();
        void position();
        void print();
        void println();
        void seek();
        void size();
        void read();
        void write();
        void isDirectory();
        void openNextFile();
        void rewindDirectory();
    };


    class File {
    File()
    {};

    virtual ~File()
    {
        if(filePtr != NULL)
        {
            fclose(filePtr);
        }
    }

    // Open a file
    inline FILE* Open(String filePath, String mode )
    {
        filePtr = fopen(filePath.c_str(), mode.c_str());
        this->filePath = filePath;
    }

    // Close a file
    inline FILE* Close()
    {
        fclose(filePtr);
        filePtr = NULL;
    }

    // Is the file open?
    inline bool IsOpen(void)
    {
        return filePtr != NULL;
    }

    // Return the file pointer for use in custom serialisers.
    inline FILE* GetFilePtr(void)
    {
        return this->filePtr;
    }

    inline String GetFilePath(void)
    {
        return this->filePath;
    }


    private:
        String filePath = "";
        FILE * filePtr = NULL;

        // prevent copying file instances should always be used as a pointer if you need to pass it around, prevents multiple handles being open or pointless reallocations too
        File( const File&);
        File& operator=(const File&);
    };




}


#endif // __SD2_H__