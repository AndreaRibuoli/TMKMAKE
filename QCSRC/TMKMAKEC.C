# 15 "TMKMAKEC.C"
#include <stdio.h>
#include <stdlib.h>

#include "tmkbase.h"
#include "tmkutil.h"
#include "tmkfile.h"
#include "tmkparse.h"
#include "tmkbuilt.h"
#include "tmkmaker.h"
#include "tmkdict.h"
#include "tmkoptio.h"


int main( int argc, Char **argv ) {
        Int32 makefile_count;
        Char *makefile_name;


        process_options( argc, argv );


        setup_parser();

        makefile_count = no_of_makefile_mbr();
        while( makefile_count-- ) {

            setup_dict();



            setup_builtin_structures();


            setup_parser_structures();


            setup_command_macro();



            read_qmaksrc_builtin();


            parse_makefile( next_makefile_mbr(), PARSE_MAKEFILE );



            apply_inference_rules();


            process_makefile();
        }


        delete_usrspc();

        return( TMK_EXIT_SUCCESS );
}
