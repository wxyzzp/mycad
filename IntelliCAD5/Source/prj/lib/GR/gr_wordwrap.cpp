/*------------------------------------------------------------------------
$Workfile  gr_wordwrap.cpp$
$Archive$
$Revision: 1.1.10.1 $
$Date: 2003/08/29 01:34:03 $
$Author    Vitaly Spirin$

$Contents: Functions to process CJK text wrapping$
------------------------------------------------------------------------*/


#include "gr_wordwrap.h"

#include "dwgcodepage.h"

char*
cjk_wordwrap(                       // R: new string with slashes in the places where it can be wrapped
unsigned char*      str_origin,     // I: string that has to be wrapped
db_drawing*         pDrawing        // I: drawing (to get dwgcodepage)
)
{
    char*   str_with_slashes;
    
    str_with_slashes = new char[strlen((char*)str_origin) * 2 + 1];

    UINT    dwg_code_page;

    dwg_code_page = get_dwgcodepage(pDrawing);

    if (dwg_code_page == 932 || dwg_code_page == 936 || dwg_code_page == 950)
    {
        //Japanese and Chinese only. Korean text is processed like English
        int     i = 0;
        int     j = 0;
        while (str_origin[i] != 0)
        {
            if (_ismbblead( ((unsigned char)str_origin[i]) ) != 0)
            {
                str_with_slashes[j] = str_origin[i];
                str_with_slashes[j + 1] = str_origin[i + 1];
                j = j + 2;
                i = i + 2;
                if (str_origin[i] != 0 && str_origin[i] > 0x7f &&
                    cjk_wordwrap_exceptions(dwg_code_page, str_origin+i-2) != LEADING_CHARACTER && 
                    cjk_wordwrap_exceptions(dwg_code_page, str_origin+i) != FOLLOWING_CHARACTER)
                {
                    str_with_slashes[j] = '\\';
                    j++;
                }; // if
            }else
            {
                if (str_origin[i] == '\\' && str_origin[i+1] == 'f')
                {
                    int save_i = i, save_j = j;
                    do
                    {
                        // -[ ebatech(cnbr) Bugzilla78576 In case of format code does not complete. 2003/7/6
                        if( str_origin[i] == 0 )
                        {
                            i = save_i;
                            j = save_j;
                            break;
                        }
                        // ]- ebatech(cnbr)
                        if (_ismbblead( ((unsigned char)str_origin[i]) ) != 0)
                        {
                            str_with_slashes[j] = str_origin[i];
                            str_with_slashes[j + 1] = str_origin[i + 1];
                            j = j + 2;
                            i = i + 2;
                        }else
                        {
                            str_with_slashes[j] = str_origin[i];
                            j++;
                            i++;
                        }; // if
                    } while (str_origin[i] != ';');
                }; // if

                str_with_slashes[j] = str_origin[i];
                j++;
                i++;
                if (_ismbbkana(str_origin[i-1]) != 0 && dwg_code_page == 932)
                {
                    if (str_origin[i] != 0 && str_origin[i] > 0x7f &&
                        cjk_wordwrap_exceptions(dwg_code_page, str_origin+i-2) != LEADING_CHARACTER && 
                        cjk_wordwrap_exceptions(dwg_code_page, str_origin+i) != FOLLOWING_CHARACTER)
                    {
                        str_with_slashes[j] = '\\';
                        j++;
                    }; // if
                }; //if
            }; // if
        }; // while
        str_with_slashes[j] = str_origin[i];
    }else
    {
        strcpy(str_with_slashes, (char*)str_origin);
    }; // if

    return str_with_slashes;
} // cjk_wordwrap()


int
cjk_wordwrap_exceptions(        // R: ORDINARY_CHARACTER or LEADING_CHARACTER or FOLLOWING_CHARACTER
    UINT            code_page,  // I: code page for processing
    unsigned char*  mb_string   // I: multibyte code
)
{

    static struct {
        #define NUMBER_OF_JAPANESE_LEADING_CHARACTER 20
        #define NUMBER_OF_JAPANESE_FOLLOWING_CHARACTER 81

        unsigned int    leading_characters[ NUMBER_OF_JAPANESE_LEADING_CHARACTER ];
        unsigned int    following_characters[ NUMBER_OF_JAPANESE_FOLLOWING_CHARACTER ];
    } kinsoku =
    {
        {
            0x24, 0xA2,   0x816D, 0x8177,
            0x28, 0x8165, 0x816F, 0x8179,
            0x5B, 0x8167, 0x8171, 0x818F,
            0x5C, 0x8169, 0x8173, 0x8190,
            0x7B, 0x816B, 0x8175, 0x8192
        },
        {
            0x21, 0xB0,   0x8168, 0x82A7,
            0x25, 0xDE,   0x816A, 0x82C1,
            0x29, 0xDF,   0x816C, 0x82E1,
            0x2C, 0x8141, 0x816E, 0x82E3,
            0x2E, 0x8142, 0x8170, 0x82E5,
            0x3F, 0x8143, 0x8172, 0x82EC,
            0x5D, 0x8144, 0x8174, 0x8340,
            0x7D, 0x8145, 0x8176, 0x8342,
            0xA1, 0x8146, 0x8178, 0x8344,
            0xA3, 0x8147, 0x817A, 0x8346,
            0xA4, 0x8148, 0x818B, 0x8348,
            0xA5, 0x8149, 0x818C, 0x8362,
            0xA7, 0x814A, 0x818D, 0x8383,
            0xA8, 0x814B, 0x818E, 0x8385,
            0xA9, 0x8152, 0x8191, 0x8387,
            0xAA, 0x8153, 0x8193, 0x838E,
            0xAB, 0x8154, 0x81F1, 0x8395,
            0xAC, 0x8155, 0x829F, 0x8396,
            0xAD, 0x8158, 0x82A1,
            0xAE, 0x815B, 0x82A3,
            0xAF, 0x8166, 0x82A5,
        }
    }; // struct


    static struct {
        #define NUMBER_OF_TRADITIONAL_CHINESE_LEADING_CHARACTER 26
        #define NUMBER_OF_TRADITIONAL_CHINESE_FOLLOWING_CHARACTER 60

        unsigned int    leading_characters[ NUMBER_OF_TRADITIONAL_CHINESE_LEADING_CHARACTER ];
        unsigned int    following_characters[ NUMBER_OF_TRADITIONAL_CHINESE_FOLLOWING_CHARACTER ];
    } traditional_chinese =
    {
        {
            0x0028, 0xA165, 0xA173, 0xA1A3,
            0x005B, 0xA167, 0xA175, 0xA1A5,
            0x007B, 0xA169, 0xA177, 0xA1A7,
            0xA15D, 0xA16B, 0xA179, 0xA1A9,
            0xA15F, 0xA16D, 0xA17B, 0xA1AB,
            0xA161, 0xA16F, 0xA17D,
            0xA163, 0xA171, 0xA1A1,
        },
        {
            0x0021, 0xA147, 0xA156, 0xA16E,
            0x0029, 0xA148, 0xA157, 0xA170,
            0x002C, 0xA149, 0xA158, 0xA172,
            0x002E, 0xA14A, 0xA159, 0xA174,
            0x003A, 0xA14B, 0xA15A, 0xA176,
            0x003B, 0xA14C, 0xA15B, 0xA178,
            0x003F, 0xA14D, 0xA15C, 0xA17A,
            0x005D, 0xA14E, 0xA15E, 0xA17C,
            0x007D, 0xA14F, 0xA160, 0xA17E,
            0xA141, 0xA150, 0xA162, 0xA1A2,
            0xA142, 0xA151, 0xA164, 0xA1A4,
            0xA143, 0xA152, 0xA166, 0xA1A6,
            0xA144, 0xA153, 0xA168, 0xA1A8,
            0xA145, 0xA154, 0xA16A, 0xA1AA,
            0xA146, 0xA155, 0xA16C, 0xA1AC
        }
    }; // struct

    static struct {
        #define NUMBER_OF_SIMPLIFIED_CHINESE_LEADING_CHARACTER 26
        #define NUMBER_OF_SIMPLIFIED_CHINESE_FOLLOWING_CHARACTER 44

        unsigned int    leading_characters[ NUMBER_OF_SIMPLIFIED_CHINESE_LEADING_CHARACTER ];
        unsigned int    following_characters[ NUMBER_OF_SIMPLIFIED_CHINESE_FOLLOWING_CHARACTER ];
    } simplified_chinese =
    {
        {
            0x0028, 0xA1B6, 0xA3B0, 0xA3B7,
            0x005B, 0xA1B8, 0xA3B1, 0xA3B8,
            0x007B, 0xA1BA, 0xA3B2, 0xA3B9,
            0xA1AE, 0xA1BC, 0xA3B3, 0xA3DB,
            0xA1B0, 0xA1BE, 0xA3B4, 0xA3FB,
            0xA1B2, 0xA3A8, 0xA3B5,
            0xA1B4, 0xA3AE, 0xA3B6
        },
        {
            0x0021, 0xA1A4, 0xA1B1, 0xA3A7,
            0x0029, 0xA1A5, 0xA1B3, 0xA3A9,
            0x002C, 0xA1A6, 0xA1B5, 0xA3AC,
            0x002E, 0xA1A7, 0xA1B7, 0xA3AE,
            0x003A, 0xA1A8, 0xA1B9, 0xA3BA,
            0x003B, 0xA1A9, 0xA1BB, 0xA3BB,
            0x003F, 0xA1AA, 0xA1BD, 0xA3BF,
            0x005D, 0xA1AB, 0xA1BF, 0xA3DD,
            0x007D, 0xA1AC, 0xA1C3, 0xA3E0,
            0xA1A2, 0xA1AD, 0xA3A1, 0xA3FC,
            0xA1A3, 0xA1AF, 0xA3A2, 0xA3FD,
        }
    }; // struct


    unsigned short  mb_code = 0;

    if (_ismbblead(mb_string[0]) != 0)
    {
        mb_code = mb_string[0]*0x100+mb_string[1];
    }else
    {
        mb_code = mb_string[0];
    }; // if

    if (code_page == 932)
    {
        for (int i = 0; i <= NUMBER_OF_JAPANESE_LEADING_CHARACTER; i++)
        {
            if (mb_code == kinsoku.leading_characters[i])
            {
                return LEADING_CHARACTER;
            }; // if
        }; // for i

        for (i = 0; i <= NUMBER_OF_JAPANESE_FOLLOWING_CHARACTER; i++)
        {
            if (mb_code == kinsoku.following_characters[i])
            {
                return FOLLOWING_CHARACTER;
            }; // if
        }; // for i
    }; // if

    if (code_page == 950)
    {
        for (int i = 0; i <= NUMBER_OF_TRADITIONAL_CHINESE_LEADING_CHARACTER; i++)
        {
            if (mb_code == traditional_chinese.leading_characters[i])
            {
                return LEADING_CHARACTER;
            }; // if
        }; // for i

        for (i = 0; i <= NUMBER_OF_TRADITIONAL_CHINESE_FOLLOWING_CHARACTER; i++)
        {
            if (mb_code == traditional_chinese.following_characters[i])
            {
                return FOLLOWING_CHARACTER;
            }; // if
        }; // for i
    }; // if

    if (code_page == 936)
    {
        for (int i = 0; i <= NUMBER_OF_SIMPLIFIED_CHINESE_LEADING_CHARACTER; i++)
        {
            if (mb_code == simplified_chinese.leading_characters[i])
            {
                return LEADING_CHARACTER;
            }; // if
        }; // for i

        for (i = 0; i <= NUMBER_OF_SIMPLIFIED_CHINESE_FOLLOWING_CHARACTER; i++)
        {
            if (mb_code == simplified_chinese.following_characters[i])
            {
                return FOLLOWING_CHARACTER;
            }; // if
        }; // for i
    }; // if

    return ORDINARY_CHARACTER;

} // cjk_wordwrap_exceptions()
