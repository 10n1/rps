//
//  Shader.fsh
//  rps
//
//  Created by Kyle Weicht on 7/10/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

varying lowp vec4 colorVarying;

void main()
{
    gl_FragColor = colorVarying;
}
