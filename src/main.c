//
//  main.c
//  xcodeAssgn1
//
//  Created by Jason Eddy on 2020-01-15.
//  Copyright © 2020 Jason Eddy. All rights reserved.
//

#include "SVGParser_A2.h"

int main(int argc, const char *argv[])
{
    // insert code here...
    SVGimage *image = NULL;
   image = createValidSVGimage("Emoji_poo.svg", "svg.xsd");
    if (image == NULL)
    {
        printf("NULL!!");
    }
    else
    {

    }

            deleteSVGimage(image);
    
  

            
    return 0;
}
